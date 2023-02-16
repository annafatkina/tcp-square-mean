#include "client.h"

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
    , isRunnting_(false) {
    logger_.run();
}

void Client::run() {
    boost::asio::connect(socket_, resolver_.resolve(host_, port_));
    logger_.run();
    isRunnting_ = true;
    while (isRunnting_) {
        std::srand(std::time(0));
        int randNum = std::rand() % maxNumber_;
        send(randNum);
        recv();
    }
}

void Client::stop() {
    isRunnting_ = false;
    logger_.stop();
}