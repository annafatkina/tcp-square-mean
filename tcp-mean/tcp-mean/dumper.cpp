#include "dumper.h"

void Dumper::work() {
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

void Dumper::dump(std::deque<std::string> &dumpQueue) {
    if (dumpQueue.empty()) {
        return;
    }

    std::ofstream outfile;
    outfile.open(dumpFilename_, std::ios::app);

    while (!dumpQueue.empty()) {
        outfile << dumpQueue.front() << std::endl;
        dumpQueue.pop_front();
    }

    outfile.close();
}

// public
Dumper::Dumper(const std::string &filename)
    : dumpFilename_(filename)
    , shutdown_(false) {
}

void Dumper::push(const std::string &string) {
    std::unique_lock<std::mutex> locker(dumpMsgQueueMutex_);
    dumpMsgQueue_.emplace_back(string);
}

void Dumper::start() {
    dumpThread_ = std::thread(std::bind(&Dumper::work, this));
}

void Dumper::initDump() {
    dumpCV_.notify_one();
}

void Dumper::stop() {
    shutdown_ = true;
    dumpCV_.notify_one();
    dumpThread_.join();
}