#include "tcpserver.h"
#include "logger.h"
#include <boost/bind.hpp>
#include <functional>
#include <iostream>
#include <thread>

void TcpServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, Tcp::socket socket) {
            if (!ec) {
                try {
                    sessionFactory_(context_, std::move(socket),
                                    sessionCounter_, logger_, computer_)
                        ->start();
                    sessionCounter_++;
                } catch (const std::exception &e) {
                    logger_->printError(
                        "Failed to create session with a given session ",
                        "factory method, error: ", e.what());
                }
            } else {
                logger_->printError("Failed to accept connection, error: ",
                                    ec.message());
            }

            do_accept();
        });
}

void TcpServer::run() {
    // Create a pool of threads to run all of the io_contexts.
    threads_.reserve(num_threads_);
    logger_->print("Starting ", num_threads_, " threads...");

    for (std::size_t i = 0; i < num_threads_; ++i) {
        auto t = std::make_shared<std::thread>(
            boost::bind(&boost::asio::io_context::run, &context_));
        threads_.emplace_back(std::move(t));
    }
    isRunning_ = true;
}

TcpServer::TcpServer(short port, const std::string &logFilename,
                     const std::string &numbersDumpFilename, int maxNumber,
                     SessionFactoryFunc sessionFactoryFunc)
    : context_()
    , signals_(context_)
    , acceptor_(context_, Tcp::endpoint(Tcp::v4(), port))
    , threads_()
    , sessionCounter_(0)
    , sessionFactory_(sessionFactoryFunc)
    , numberDumper_(
          std::make_shared<NumberDumper>(context_, numbersDumpFilename, 2))
    , num_threads_(std::thread::hardware_concurrency() > 2
                       ? std::thread::hardware_concurrency() - 2
                       : 1)
    , logger_(std::make_shared<Logger>(context_, logFilename, 1))
    , computer_(createMeanComputer(numberDumper_, logger_, maxNumber)), isRunning_(false) {

    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through Asio.
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    signals_.async_wait(boost::bind(&TcpServer::stop, this));

    loggerThread_ =
        std::make_shared<std::thread>(boost::bind(&Logger::run, logger_.get()));
    numberDumper_->start();
    do_accept();
}

TcpServer::~TcpServer() {
    if (isRunning_)
        stop();
}

void TcpServer::stop() {
    isRunning_ = false;
    signals_.cancel();
    signals_.clear();

    if (!context_.stopped()) {
        context_.stop();
        logger_->print("Tcp Server stopped.");
    }

    numberDumper_->stop();
    logger_->stop();
    
    if (acceptor_.is_open()) {
        acceptor_.cancel();
        acceptor_.close();
    }
}

void TcpServer::waitForStop() {
    for (std::size_t i = 0; i < threads_.size(); ++i)
        threads_[i]->join();
    loggerThread_->join();
    threads_.clear();
}
