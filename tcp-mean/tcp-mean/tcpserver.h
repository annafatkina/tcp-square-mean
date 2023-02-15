#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "isession.h"
#include "icomputer.h"
#include "logger.h"
#include <boost/asio.hpp>

class TcpServer {
    using Context            = boost::asio::io_context;
    using Signals            = boost::asio::signal_set;
    using Tcp                = boost::asio::ip::tcp;
    using Acceptor           = Tcp::acceptor;
    using SessionFactoryFunc = std::function<std::shared_ptr<ISession>(
        Context& io_context, Tcp::socket socket, int sessionId, std::shared_ptr<Logger> logger, std::shared_ptr<IComputer> computer)>;

    Context                                   context_;
    Signals                                   signals_;
    Acceptor                                  acceptor_;
    SessionFactoryFunc                        sessionFactory_;
    std::shared_ptr<IComputer>                computer_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    int                                       sessionCounter_;
    unsigned int                              num_threads_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<std::thread> loggerThread_;

    // Accept new connection
    void do_accept();

  public:
    // Create 'TcpServer' object with the specified 'port' and
    // 'sessionFactoryFunc'. Use 'Session' object creation function by default.
    TcpServer(short              port,
              SessionFactoryFunc sessionFactoryFunc = &createSession);

    // Destroy this object.
    ~TcpServer();

    // Join all the threads ans stop this server.
    void stop();

    // Wait until server stopped.
    void waitForStop();

    // Run this server.
    void run();
};

#endif   // TCPSERVER_H
