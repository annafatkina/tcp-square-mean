#pragma once

#include <fstream>
#include <iostream>
#include <string>

class Logger {
    // This class implements a simple single-thread logger.

    std::string   filename_;
    std::ofstream outFile_;

  public:
    // Create 'Logger' object with the specified log 'filename'.
    Logger(const std::string &filename);

    // Run the logger, open log file.
    void run();

    // Stop the logger, close log file.
    void stop();

    // Print the specified 'str' to the log file and to command line.
    void print(const std::string &str);
};