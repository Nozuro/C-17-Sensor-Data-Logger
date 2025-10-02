#include "SensorSimulator.h"
#include <chrono>

SensorSimulator::SensorSimulator(double min, double max)
	: eng(std::random_device{}()), dist(min, max) {
}

double SensorSimulator::readValue() {
	return dist(eng);
}