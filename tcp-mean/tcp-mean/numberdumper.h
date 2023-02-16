#pragma once
#include <boost/asio.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>

class NumberDumper : public std::enable_shared_from_this<NumberDumper> {
    std::condition_variable     dumpCV_;
    std::mutex                  numbersMutex_;
    std::string                 dumpFilename_;
    std::ofstream               outFile_;
    std::set<uint32_t>          numbers_;
    std::atomic_bool            shutdown_;
    std::thread                 dumpThread_;
    int                         dumpPeriodSeconds_;
    boost::asio::deadline_timer dumpTimer_;

    void dump() {
        for (auto num : numbers_) {
            outFile_ << num << " ";
        }
        outFile_ << std::endl;
    }

    void work() {
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

    void sheduleDumpRecurring() {
        initNumberDumper();
        auto self = shared_from_this();
        dumpTimer_.expires_at(dumpTimer_.expires_at() +
                              boost::posix_time::seconds(dumpPeriodSeconds_));
        dumpTimer_.async_wait([self](const boost::system::error_code &) {
            self->sheduleDumpRecurring();
        });
    }

  public:
    NumberDumper(boost::asio::io_context &io_context,
                 const std::string &dumpFilename, int dumpPeriodSeconds)
        : dumpFilename_(dumpFilename)
        , dumpPeriodSeconds_(dumpPeriodSeconds)
        , dumpTimer_(io_context,
                     boost::posix_time::seconds(dumpPeriodSeconds)) {
    }

    void start() {
        if (!outFile_.is_open()) {
            outFile_.open(dumpFilename_, std::ios::app | std::ios::binary);
        }

        auto self = shared_from_this();
        dumpTimer_.async_wait([self](const boost::system::error_code &) {
            self->sheduleDumpRecurring();
        });
        dumpThread_ = std::thread([self]() { self->work(); });
    }

    void stop() {
        dumpCV_.notify_one();
        dumpTimer_.cancel();
        dumpThread_.join();
        if (outFile_.is_open()) {
            outFile_.close();
        }
    }

    void push(uint32_t number) {
        std::unique_lock<std::mutex> locker(numbersMutex_);
        numbers_.insert(number);
    }

    void initNumberDumper() {
        dumpCV_.notify_one();
    }
};