#include "client.h"
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <string>

void Client::send(int number) {
    std::string randNum = std::to_string(number);

    randNum += "\n";
    size_t request_length = randNum.size();
    boost::asio::write(socket_,
                       boost::asio::buffer(randNum.c_str(), request_length));

    logger_.print("Send number: " + randNum);
}

void Client::recv() {
    boost::asio::read_until(socket_, buffer_, '\n');
    std::istream iss(&buffer_);
    std::string  s;
    std::getline(iss, s);
    logger_.print("Received number: " + s);
}

Client::Client(const std::string &host, const std::string &port,
               const std::string logFilename, int maxNumber)
    : io_context_()
    , socket_(io_context_)
    , resolver_(io_context_)
    , host_(host)
    , port_(port)
    , buffer_(32)
    , logger_(logFilename)
    , maxNumber_(maxNumber)
    , signals_(io_context_)
    , isRunnting_(false) {
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    signals_.async_wait(boost::bind(&Client::stop, this));
    logger_.run();
}

void Client::run() {
    logger_.print("Run the client.");
    boost::asio::connect(socket_, resolver_.resolve(host_, port_));
    logger_.run();
    isRunnting_ = true;
    while (isRunnting_) {
        boost::random::mt19937                    rng;
        boost::random::uniform_int_distribution<> gen(0, maxNumber_);
        int                                       randNum = gen(rng);
        try {
            send(randNum);
        } catch (const std::exception &e) {
            logger_.print("ERROR: can't write data, " + std::string(e.what()) +
                          " closing...");
            stop();
        }

        try {
            recv();
        } catch (const std::exception &e) {
            logger_.print("ERROR: can't read data, " + std::string(e.what()) +
                          " closing...");
            stop();
        }
    }
}

void Client::stop() {
    logger_.print("Shutdown the client.");
    isRunnting_ = false;
    logger_.stop();
}