#pragma once
#include <deque>
#include <mutex>
#include <string>
#include <iostream>
#include <fstream>

class Logger {
    enum class Level {
        ALL = 0, // print all logs: errors, warnings, info and debug
        INFO,    // print error, warnings, and info logs only
        WARNING, // print errors and warnings only
        ERR      // print errors only
    };

    std::mutex              logMsgQueueMutex_;
    std::mutex              dumpMsgQueueMutex_;

    std::deque<std::string> logMsgQueue_;
    std::deque<std::string> dumpMsgQueue_;
    std::string             dumpFilename_;
    Level                   logLevel_;
    std::atomic_bool        isRunning_;

    void push(const std::string& msg)
    {
        std::lock_guard<std::mutex> guard(logMsgQueueMutex_);
        logMsgQueue_.emplace_back(msg);
    }

    void processLogQueue() {
        // To be processed in logger thread
        std::string currentLog;

        {
            std::lock_guard<std::mutex> guard(logMsgQueueMutex_);
            if (logMsgQueue_.empty()) {
                return;
            }
            currentLog = logMsgQueue_.front();
            logMsgQueue_.pop_front();
        }

        {
            std::lock_guard<std::mutex> guard(dumpMsgQueueMutex_);
            dumpMsgQueue_.emplace_back(currentLog);
        }

        std::cout << currentLog << std::endl;
    }

    std::string toString(const std::string& str)
    {
        return str;
    }

    std::string toString(const char* arr)
    {
        return std::string(arr);
    }

    template <typename T>
    std::string toString(T value)
    {
        return std::to_string(value);
    }

    template <typename T, typename... Types>
    std::string toString(T arg, Types... args)
    {
        std::string str;
        if constexpr (sizeof...(args) > 0) {
            str += toString(arg) + toString(args...);
        }
        return str;
    }

public:
    // NOT IMPLEMENTED
    Logger(const Logger&) = delete;
    Logger(Logger&&)      = delete;

    //
    Logger(const std::string& filename, Level level = Level::ALL)
        : dumpFilename_(filename)
        , logLevel_(level)
        , isRunning_(false)
    {
        std::cout << "Logger created" << std::endl;
    }

    void run() {
        isRunning_ = true;

        while (isRunning_) {
            try {
                processLogQueue();
            } catch (const std::exception& e) {
                std::cout << "Error while loggng: " << e.what();
            }
        }
    }

    void stop() {
        isRunning_ = false;
    }

    // Add the specified 'msg' info level log to output. If log level is more
    // strict than 'INFO', ighnore it.
    template <typename... Types>
    void print(Types... args)
    {
        if (logLevel_ > Level::INFO) {
            return;
        }
        std::string msg = toString(args...);
        push(msg);
    }

    // Add the specified 'msg' error level log to output.
    //void printError(const std::string& msg) { push("\033[1;31mERROR: " + msg + "\033[0m"); }
    
    

    template <typename... Types>
    void printError(Types... args) {
        std::string msg = toString(args...);
        push("\033[1;31mERROR: " + msg + "\033[0m");
    }


    // Add the specified 'msg' warning level log to output. If log level is more
    // strict than 'WARNING', ighnore it.
    template <typename... Types>
    void printWarning(Types... args)
    {
        if (logLevel_ > Level::WARNING) {
            return;
        }
        std::string msg = toString(args...);
        push("\033[1;33mWARNING: " + msg + "\033[0m");
    }

    // Add the specified 'msg' debug level log to output. If log level is more
    // strict than 'ALL', ighnore it.
    template <typename... Types>
    void printDebug(Types... args)
    {
        if (logLevel_ > Level::ALL) {
            return;
        }

        std::string msg = toString(args...);
        push("\033[1;34mDEBUG: " + msg + "\033[0m");
    }

    // Dump all wtitten logs to the file specified in ctor.
    void dump()
    {
        std::ofstream outfile;
        outfile.open(dumpFilename_, std::ios::app);

        {
            std::lock_guard<std::mutex> guard(dumpMsgQueueMutex_);
            while (!dumpMsgQueue_.empty()) {
                outfile << dumpMsgQueue_.front() << std::endl;
                dumpMsgQueue_.pop_front();
            }
        }

        outfile.close();
    }
};
