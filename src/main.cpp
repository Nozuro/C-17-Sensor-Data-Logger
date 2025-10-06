#include <iostream>
#include <chrono>
#include <thread>
#include "SensorSimulator.h"
#include "Logger.h"
#include "../experiments/phase1/queue_demo.cpp";


int main() {
	run_queue_demo();
	return 0;

	SensorSimulator sensor(20.0, 30.0);
	Logger logger("data.csv");

	for (int i = 0; i < 50; i++) {
		auto now = std::chrono::system_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		double value = sensor.readValue();
		logger.log(ms, value);

		std::cout << "Logged: " << ms << " -> " << value << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "Data logging complete. Check data.csv\n";
	return 0;
}
