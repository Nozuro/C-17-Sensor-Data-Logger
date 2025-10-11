#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "SensorSimulator.h"
#include "Logger.h"

#include <numeric>
#include <stdexcept>

#include "../experiments/phase1/queue_demo.cpp";

using namespace std;
using namespace std::chrono;

atomic<bool> StopRequested(false);
atomic<bool> FakeLag(false);
atomic<bool> DataActivity(false);

queue<double> QSensorToProcessor;
queue<double> QProcessorToLogger;
mutex Mutex1;
mutex Mutex2;
condition_variable CVar1;
condition_variable CVar2;


void SensorTask(SensorSimulator& sensor) {
	while (!StopRequested) {
		if (FakeLag) continue;

		double value = sensor.readValue();
		{
			lock_guard<mutex> lock(Mutex1);
			QSensorToProcessor.push(value);
		}
		DataActivity = true;
		CVar1.notify_one();
		this_thread::sleep_for(milliseconds(100));
	}
}


void ProcessorTask() {
	while (!StopRequested) {
		unique_lock<mutex> lock(Mutex1);
		CVar1.wait(lock, [] {return !QSensorToProcessor.empty() || StopRequested;});

		// --- Human-Readable Version ---
		//auto callableFunction = [] {
			//bool hasData = !QSensorToProcessor.empty();
			//return hasData || StopRequested;
			//};
		//CVar1.wait(lock, callableFunction);
		// -----

		while (!QSensorToProcessor.empty()) {
			double raw = QSensorToProcessor.front();
			QSensorToProcessor.pop();
			lock.unlock();

			// --- Filtering ---
			thread_local vector<double> window;
			window.push_back(raw); // Similar to C# List<T>.Add()
			if (window.size() > 5) window.erase(window.begin());
			double sum = 0;
			for (auto v : window)
				sum += v;
			double avg = sum / window.size();
			double avgOtherWay = accumulate(window.begin(), window.end(), 0.0) / window.size();
			if (avg != avgOtherWay) throw runtime_error("This sht doesnt work!");

			{
				lock_guard<mutex> lock2(Mutex2);
				QProcessorToLogger.push(avg);
			}
			CVar2.notify_one();

			lock.lock();
		}
	}
}


void LoggerTask(Logger& logger) {
	while (!StopRequested) {
		unique_lock<mutex> lock(Mutex2);
		CVar2.wait(lock, [] {return !QProcessorToLogger.empty() || StopRequested;});

		while (!QProcessorToLogger.empty()) {
			double val = QProcessorToLogger.front();
			QProcessorToLogger.pop();
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
		else if (duration_cast<milliseconds>(steady_clock::now() - last).count() > 500) {
			cout << "[Watchdog] Warning: no new data for 500 ms\n";
			last = steady_clock::now();
		}
	}
}

int main() {
	SensorSimulator sensor(20.0, 30.0);
	Logger logger("phase2-data.csv");

	thread threadSensor(SensorTask, ref(sensor));
	thread threadProcessor(ProcessorTask);
	thread threadLogger(LoggerTask, ref(logger));
	thread threadWatchdog(WatchdogTask);

	this_thread::sleep_for(seconds(2));
	FakeLag.store(true);
	this_thread::sleep_for(seconds(1));
	StopRequested = true;
	CVar1.notify_all();
	CVar2.notify_all();

	threadSensor.join();
	threadProcessor.join();
	threadLogger.join();
	threadWatchdog.join();

	cout << "Phase 2 complete - data.csv generated.\n";
	return 0;
}
