#pragma once
#include <boost/asio.hpp>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "dumper.h"

class Logger : public std::enable_shared_from_this<Logger> {
    enum class Level {
        ALL = 0, // print all logs: errors, warnings, info and debug
        INFO, // print error, warnings, and info logs only
        WARNING, // print errors and warnings only
        ERR // print errors only
    };

    std::shared_ptr<Dumper> dumper_;
    std::mutex logMsgQueueMutex_;
    std::deque<std::string> logMsgQueue_;
    Level logLevel_;
    std::atomic_bool isRunning_;
    int dumpPeriodSeconds_;
    boost::asio::deadline_timer dumpTimer_;

    // Push the specified 'msg' into log message queue.
    void push(const std::string& msg);

    // Process log messages queue. Print them into output and save to the dump queue.
    void processLogQueue();

    // Return the specified 'str' as is.
    std::string toString(const std::string& str);

    // Wrap the specified 'arr' into a string.
    std::string toString(const char* arr);

    // Cast the specifies 'value' of a template type to a string.
    template <typename T>
    std::string toString(T value);

    // Cast the specified 'args' to strings and concatenate them. Return the concatenated string.
    template <typename T, typename... Types>
    std::string toString(T arg, Types... args);

    // Schedule dump to log file once per period specified in ctor.
    void sheduleDumpRecurring();

public:
    // NOT IMPLEMENTED
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    // Create 'Logger' object with the 'dumpFilename' and log 'level'.
    Logger(boost::asio::io_context& io_context, const std::string& dumpFilename, int dumpPeriodSeconds, Level level = Level::ALL);

    // Start processing logger queue.
    void run();

    // Stop logger.
    void stop();

    // Add the specified 'msg' info level log to output. If log level is more
    // strict than 'INFO', ighnore it.
    template <typename... Types>
    void print(Types... args);

    // Add the specified 'msg' error level log to output.
    template <typename... Types>
    void printError(Types... args);

    // Add the specified 'msg' warning level log to output. If log level is more
    // strict than 'WARNING', ighnore it.
    template <typename... Types>
    void printWarning(Types... args);

    // Add the specified 'msg' debug level log to output. If log level is more
    // strict than 'ALL', ighnore it.
    template <typename... Types>
    void printDebug(Types... args);
};

// INLINE DEFINITIONS

inline std::string Logger::toString(const std::string& str)
{
    return str;
}

inline std::string Logger::toString(const char* arr)
{
    return std::string(arr);
}

template <typename T>
std::string Logger::toString(T value)
{
    return std::to_string(value);
}

template <typename T, typename... Types>
std::string Logger::toString(T arg, Types... args)
{
    std::string str;
    if constexpr (sizeof...(args) > 0) {
        str += toString(arg) + toString(args...);
    }
    return str;
}

template <typename... Types>
void Logger::print(Types... args)
{
    if (logLevel_ > Level::INFO) {
        return;
    }
    std::string msg = toString(args...);
    push(msg);
}

template <typename... Types>
void Logger::printError(Types... args)
{
    std::string msg = toString(args...);
    push("\033[1;31mERROR: " + msg + "\033[0m");
}

template <typename... Types>
void Logger::printWarning(Types... args)
{
    if (logLevel_ > Level::WARNING) {
        return;
    }
    std::string msg = toString(args...);
    push("\033[1;33mWARNING: " + msg + "\033[0m");
}

template <typename... Types>
void Logger::printDebug(Types... args)
{
    if (logLevel_ > Level::ALL) {
        return;
    }

    std::string msg = toString(args...);
    push("\033[1;34mDEBUG: " + msg + "\033[0m");
}