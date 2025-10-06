#include "Logger.h"

Logger::Logger(const std::string& filename) {
	file.open(filename);
	if (file.is_open())
		file << "timestamp,value\n";
}

Logger::~Logger() {
	if (file.is_open())
		file.close();
}

void Logger::log(long long timestamp, double value) {
	if (file.is_open())
		file << timestamp << "," << value << "\n";
}