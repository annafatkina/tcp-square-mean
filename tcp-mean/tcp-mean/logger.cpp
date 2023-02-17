#include "logger.h"

void Logger::push(const std::string &msg) {
    std::lock_guard<std::mutex> guard(logMsgQueueMutex_);
    logMsgQueue_.emplace_back(msg);
}

void Logger::processLogQueue() {
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

void Logger::sheduleDumpRecurring() {
    dumper_->initDump();
    auto self = shared_from_this();
    dumpTimer_.expires_at(dumpTimer_.expires_at() +
                          boost::posix_time::seconds(dumpPeriodSeconds_));
    dumpTimer_.async_wait([self](const boost::system::error_code &) {
        self->sheduleDumpRecurring();
    });
}

// public
Logger::Logger(boost::asio::io_context &io_context, const std::string &filename,
               int dumpPeriodSeconds, Level level)
    : dumper_(std::make_shared<LogDumper>(filename))
    , logLevel_(level)
    , isRunning_(false)
    , dumpPeriodSeconds_(dumpPeriodSeconds)
    , dumpTimer_(io_context, boost::posix_time::seconds(dumpPeriodSeconds)) {
    dumper_->start();
}

Logger::~Logger() {
    stop();
}

void Logger::run() {
    isRunning_ = true;

    auto self = shared_from_this();
    dumpTimer_.async_wait([self](const boost::system::error_code &) {
        self->sheduleDumpRecurring();
    });

    while (isRunning_) {
        try {
            processLogQueue();
        } catch (const std::exception &e) {
            // Write it right to cout as logger suppesed to be non-working in
            // this case.
            std::cout << "Error while loggng: " << e.what();
        }
    }
}

void Logger::stop() {
    if (~isRunning_) {
        return;
    }
    isRunning_ = false;
    dumpTimer_.cancel();
    dumper_->stop();
}