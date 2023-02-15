#pragma once
#include <boost/asio.hpp>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

class Dumper : public std::enable_shared_from_this<Dumper> {
    std::mutex dumpMsgQueueMutex_;
    std::condition_variable dumpCV_;
    std::deque<std::string> dumpMsgQueue_;
    std::string dumpFilename_;
    std::thread dumpThread_;
    std::atomic_bool shutdown_ = false;

    void work();

    void dump(std::deque<std::string>& dumpQueue);

public:
    // NOT IMPLEMENTED
    Dumper(const Dumper&) = delete;
    Dumper(Dumper&&) = delete;

    Dumper(const std::string& filename);

    void push(const std::string& string);

    void start();

    void initDump();

    void stop();
};