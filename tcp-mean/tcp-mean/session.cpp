#include "session.h"
#include <boost/bind.hpp>
#include <iostream>

std::string Session::readBuffer() {
    std::istream iss(&rBuffer_);
    std::string  s;
    std::getline(iss, s);
    return s;
}

void Session::writeToBuffer(const std::string &str) {
    std::ostream output(&wBuffer_);
    output << str << "\n";
    do_write();
}

void Session::do_read() {
    auto self(shared_from_this());
    auto callback = [this, self](boost::system::error_code ec, int size) {
        if (!ec) {
            handle();
            return;
        }
        if (ec == boost::asio::error::eof) {
            // do nothing, session will be closed.
            return;
        }

        logger_->printError("Error while reading data from client:\n\t",
                            ec.message(), "\nfor session with session id ",
                            sessionId_, ". Closing this session.");
    };

    boost::asio::async_read_until(
        socket_, rBuffer_, '\n',
        boost::asio::bind_executor(rwStrand_, callback));
}

void Session::do_write() {
    auto self(shared_from_this());
    auto callback = [this, self](boost::system::error_code ec, int size) {
        if (!ec) {
            try {
                do_read();
            } catch (const std::exception &e) {
                logger_->printError(
                    "Error while calling 'Session::do_read()': ", e.what());
            }
        } else {
            logger_->printError("Error while writing data to client:\n\t",
                                std::string(ec.message()),
                                "\nfor session with session id ", sessionId_,
                                ". Closing this session.");
        }
    };

    boost::asio::async_write(socket_, wBuffer_,
                             boost::asio::bind_executor(rwStrand_, callback));
}

void Session::handle() {
    std::string s = readBuffer();
    if (s.size() == 0) {
        logger_->printWarning("No data received and no line break reched.");
    }
    logger_->printDebug("Received data: ", s);

    double res;
    try {
        // compute
        res = computer_->updateAndCompute(std::stoi(s));
    } catch (const std::exception &e) {
        logger_->printError("Error while computing: ", e.what());
    }

    writeToBuffer(std::to_string(res));
}

// public
Session::Session(Context &io_context, Tcp::socket socket, int sessionId,
                 std::shared_ptr<Logger>    logger,
                 std::shared_ptr<IComputer> computer)
    : ISession(sessionId, logger)
    , computer_(computer)
    , rwStrand_(io_context)
    , socket_(std::move(socket))
    , rBuffer_()
    , wBuffer_() {
}

Session::~Session() {
    logger_->print("Session with session id ", sessionId_, " closed.");
}

void Session::start() {
    logger_->print("Session with session id ", sessionId_, " started.");
    do_read();
}

std::shared_ptr<ISession> createSession(boost::asio::io_context &    io_context,
                                        boost::asio::ip::tcp::socket socket,
                                        int                          sessionId,
                                        std::shared_ptr<Logger>      logger,
                                        std::shared_ptr<IComputer>   computer) {
    try {
        return std::make_shared<Session>(io_context, std::move(socket),
                                         sessionId, logger, computer);
    } catch (const std::exception &e) {
        logger->printError("Error while create session: ", e.what());
    }
    return nullptr;
}
