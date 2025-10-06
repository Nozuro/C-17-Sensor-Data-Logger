#include <iostream>
#include <thread>
#include <chrono>

class threads_demo {
public:
	void sensorTask();
	void run();
};

void threads_demo::sensorTask() {
	for (int i = 0; i < 5; i++) {
		std::cout << "Sensor reading " << i << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void threads_demo::run() {
	std::thread sensor(&threads_demo::sensorTask, this);
	sensor.join();
	std::cout << "Main done.\n";
}