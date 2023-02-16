#include "logdumper.h"

void LogDumper::work() {
    while (true) {
        auto self = shared_from_this();

        std::deque<std::string> tempQueue;
        {
            std::unique_lock<std::mutex> locker(dumpMsgQueueMutex_);
            dumpCV_.wait(locker, [self]() {
                return self->shutdown_ || !self->dumpMsgQueue_.empty();
            });
            std::swap(dumpMsgQueue_, tempQueue);
        }
        dump(tempQueue);
        if (shutdown_) {
            break;
        }
    }
}

void LogDumper::dump(std::deque<std::string> &dumpQueue) {
    if (dumpQueue.empty()) {
        return;
    }

    while (!dumpQueue.empty()) {
        outFile_ << dumpQueue.front() << std::endl;
        dumpQueue.pop_front();
    }

}

// public
LogDumper::LogDumper(const std::string &filename)
    : dumpFilename_(filename)
    , shutdown_(false) {
}

void LogDumper::push(const std::string &string) {
    std::unique_lock<std::mutex> locker(dumpMsgQueueMutex_);
    dumpMsgQueue_.emplace_back(string);
}

void LogDumper::start() {
    if (!outFile_.is_open()) {
        outFile_.open(dumpFilename_, std::ios::app);
    }
    auto self   = shared_from_this();
    dumpThread_ = std::thread([self]() { self->work(); });
}

void LogDumper::initDump() {
    dumpCV_.notify_one();
}

void LogDumper::stop() {
    shutdown_ = true;
    dumpCV_.notify_one();
    dumpThread_.join();
    if (outFile_.is_open()) {
        outFile_.close();
    }
}