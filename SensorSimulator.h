#pragma once
#include <random>

class SensorSimulator {
public:
	SensorSimulator(double min, double max);
	double readValue();


private:
	std::default_random_engine eng;
	std::uniform_real_distribution<double> dist;
};