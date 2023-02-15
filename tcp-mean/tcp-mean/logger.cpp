#include "logger.h"

void Logger::push(const std::string& msg)
{
    std::lock_guard<std::mutex> guard(logMsgQueueMutex_);
    logMsgQueue_.emplace_back(msg);
}

void Logger::processLogQueue()
{
    // To be processed in a logger thread
    std::string currentLog;

    {
        std::lock_guard<std::mutex> guard(logMsgQueueMutex_);
        if (logMsgQueue_.empty()) {
            return;
        }
        currentLog = logMsgQueue_.front();
        logMsgQueue_.pop_front();
    }

    dumper_->push(currentLog);
    std::cout << currentLog << std::endl;
}

void Logger::sheduleDumpRecurring()
{
    dumper_->initDump();
    dumpTimer_.expires_at(dumpTimer_.expires_at() + boost::posix_time::seconds(dumpPeriodSeconds_));
    dumpTimer_.async_wait(std::bind(&Logger::sheduleDumpRecurring, this));
}

// public
Logger::Logger(boost::asio::io_context& io_context, const std::string& filename, int dumpPeriodSeconds, Level level)
    : dumper_(std::make_shared<Dumper>(filename))
    , logLevel_(level)
    , isRunning_(false)
    , dumpPeriodSeconds_(dumpPeriodSeconds)
    , dumpTimer_(io_context, boost::posix_time::seconds(dumpPeriodSeconds))
{
    dumper_->start();
}

void Logger::run()
{
    isRunning_ = true;

    dumpTimer_.async_wait(std::bind(&Logger::sheduleDumpRecurring, this));

    while (isRunning_) {
        try {
            processLogQueue();
        } catch (const std::exception& e) {
            std::cout << "Error while loggng: " << e.what();
        }
    }
}