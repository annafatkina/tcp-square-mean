#include "logger.h"

Logger::Logger(const std::string &filename)
    : filename_(filename) {
}

Logger::~Logger() {
    stop();
}

void Logger::run() {
    if (!outFile_.is_open()) {
        outFile_.open(filename_, std::ios::app);
    }
}

void Logger::stop() {
    if (outFile_.is_open()) {
        outFile_.close();
    }
}

void Logger::print(const std::string &str) {
    std::cout << str << std::endl;
    outFile_ << str << std::endl;
}