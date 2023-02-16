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
    std::string             dumpFilename_;
    std::thread             dumpThread_;
    std::mutex              dumpMsgQueueMutex_;
    std::condition_variable dumpCV_;
    std::deque<std::string> dumpMsgQueue_;
    std::atomic_bool        shutdown_;

    // If dumper is not shutdown and there is any logs stored, dump logs to the
    // file specifed in ctor.
    void work();

    // Dump data stored in the specified 'dumpQueue' to a file specified in
    // ctor.
    void dump(std::deque<std::string> &dumpQueue);

  public:
    // NOT IMPLEMENTED
    Dumper(const Dumper &) = delete;
    Dumper(Dumper &&)      = delete;

    // Create 'Dumper' onject with the specified dump file 'filename'.
    Dumper(const std::string &filename);

    // Push the specified 'string' into dump queue.
    void push(const std::string &string);

    // Start dumper.
    void start();

    // Initialize dump process.
    void initDump();

    // Stop dumper.
    void stop();
};