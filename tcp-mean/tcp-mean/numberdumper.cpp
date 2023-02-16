#include "numberdumper.h"

void NumberDumper::dump() {
    for (auto num : numbers_) {
        outFile_ << num << " ";
    }
    outFile_ << std::endl;
}

void NumberDumper::work() {
    while (true) {
        {
            std::unique_lock<std::mutex> locker(numbersMutex_);
            dumpCV_.wait(locker);
            dump();
        }
        if (shutdown_) {
            break;
        }
    }
}

void NumberDumper::sheduleDumpRecurring() {
    initNumberDumper();
    auto self = shared_from_this();
    dumpTimer_.expires_at(dumpTimer_.expires_at() +
                          boost::posix_time::seconds(dumpPeriodSeconds_));
    dumpTimer_.async_wait([self](const boost::system::error_code &) {
        self->sheduleDumpRecurring();
    });
}

// public
NumberDumper::NumberDumper(boost::asio::io_context &io_context,
                           const std::string &      dumpFilename,
                           int                      dumpPeriodSeconds)
    : dumpFilename_(dumpFilename)
    , dumpPeriodSeconds_(dumpPeriodSeconds)
    , dumpTimer_(io_context, boost::posix_time::seconds(dumpPeriodSeconds)) {
}

void NumberDumper::start() {
    if (!outFile_.is_open()) {
        outFile_.open(dumpFilename_, std::ios::app | std::ios::binary);
    }

    auto self = shared_from_this();
    dumpTimer_.async_wait([self](const boost::system::error_code &) {
        self->sheduleDumpRecurring();
    });
    dumpThread_ = std::thread([self]() { self->work(); });
}

void NumberDumper::stop() {
    dumpCV_.notify_one();
    dumpTimer_.cancel();
    dumpThread_.join();
    if (outFile_.is_open()) {
        outFile_.close();
    }
}

void NumberDumper::push(uint32_t number) {
    std::unique_lock<std::mutex> locker(numbersMutex_);
    numbers_.insert(number);
}

void NumberDumper::initNumberDumper() {
    dumpCV_.notify_one();
}