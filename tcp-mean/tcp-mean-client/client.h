#pragma once

#include "logger.h"
#include <atomic>
#include <boost/asio.hpp>
#include <string>

class Client {
    // This class implements a simple single-thread client.

    // TYPES
    using tcp = boost::asio::ip::tcp;

    // DATA
    boost::asio::io_context io_context_;
    tcp::socket             socket_;
    tcp::resolver           resolver_;
    std::string             host_;
    std::string             port_;
    boost::asio::streambuf  buffer_;
    Logger                  logger_;
    int                     maxNumber_;
    boost::asio::signal_set signals_;
    std::atomic_bool        isRunnting_;

    // Send the specified 'number' to the server.
    void send(int number);

    // Read the answer received from the server.
    void recv();

  public:
    // Create 'Client' object with the specified 'host', 'port', 'logFilename'
    // and 'maxNumber'.
    Client(const std::string &host, const std::string &port,
           const std::string logFilename, int maxNumber);

    // Run the client, connect it to the server with hostname and port specified
    // in ctor. Start sending random numbers to the server, wait for the answer
    // and send new number again, etc.
    void run();

    // Stop the client.
    void stop();
};
