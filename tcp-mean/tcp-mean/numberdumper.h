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

    // Dump data to file specified in ctor.
    void dump();

    // Wait for dump notification and dump number to file whet it happens.
    void work();

    // Schedule recurring dump once per period specified in ctor.
    void sheduleDumpRecurring();

  public:
      // NOT IMPLEMENTED
    NumberDumper(const NumberDumper &) = delete;
    NumberDumper(NumberDumper &&)      = delete;

    // Create 'NumberDumper' object with the specified 'io_context',
    // 'dumpFilename' and 'dumpPeriodSeconds'.
    NumberDumper(boost::asio::io_context &io_context,
                 const std::string &dumpFilename, int dumpPeriodSeconds);

    // Destroy this object.
    ~NumberDumper();

    // Start running dumper. Schedule dump task once per period specified in
    // ctor.
    void start();

    // Stor thid dumper.
    void stop();

    // Add the specified 'number' to a set of numbers received by server.
    void push(uint32_t number);

    // Initialize dump process.
    void initNumberDumper();
};