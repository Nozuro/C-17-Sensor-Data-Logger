#pragma once
#include <fstream>
#include <string>

class Logger {
public:
	Logger(const std::string& filename);
	~Logger();
	void log(long long timestamp, double value);

private:
	std::ofstream file;
};