#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "SensorSimulator.h"
#include "Logger.h"


#include "../experiments/phase1/queue_demo.cpp";

using namespace std;
using namespace std::chrono;

queue<double> SensorQueue;
mutex Mutex;
condition_variable ConditionVariable;
atomic<bool> StopRequested(false);
atomic<bool> DataActivity(false);

void SensorTask(SensorSimulator& sensor) {
	while (!StopRequested) {
		double value = sensor.readValue();
		{
			lock_guard<mutex> lock(Mutex);
			SensorQueue.push(value);
		}
		DataActivity = true;
		ConditionVariable.notify_one();
		this_thread::sleep_for(milliseconds(100));
	}
}

void LoggerTask(Logger& logger) {
	while (!StopRequested) {
		unique_lock<mutex> lock(Mutex);
		ConditionVariable.wait(lock, [] {return !SensorQueue.empty() || StopRequested;});

		while (!SensorQueue.empty()) {
			double val = SensorQueue.front();
			SensorQueue.pop();
			lock.unlock();

			logger.log(
				duration_cast<milliseconds> (system_clock::now().time_since_epoch()).count(),
				val);

			cout << "[Logger] Logged: " << val << "\n";
			lock.lock();
		}
	}
}


void WatchdogTask() {
	auto last = steady_clock::now();
	while (!StopRequested) {
		this_thread::sleep_for(milliseconds(100));
		if (DataActivity.exchange(false))
			last = steady_clock::now();
		else if (duration_cast<milliseconds>(steady_clock::now() - last).count() > 500)
			cout << "[Watchdog] Warning: no new data for 500 ms\n";
	}
}

int main() {
	SensorSimulator sensor(20.0, 30.0);
	Logger logger("data.csv");

	//thread threadSensor(SensorTask, &sensor);
	//thread threadLogger(LoggerTask, &logger);
	thread threadSensor(SensorTask, ref(sensor));
	thread threadLogger(LoggerTask, ref(logger));
	thread threadWatchdog(WatchdogTask);

	this_thread::sleep_for(seconds(10));
	StopRequested = true;
	ConditionVariable.notify_all();

	threadSensor.join();
	threadLogger.join();
	threadWatchdog.join();

	cout << "Phase 1 complete - data.csv generated.\n";
	return 0;
}
