#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "icomputer.h"
#include "isession.h"
#include "logger.h"
#include "numberdumper.h"
#include <boost/asio.hpp>

class TcpServer {
    // This class implements the server which accepts connections, run multiple
    // threads and stores sessions.

    // TYPES
    using Context            = boost::asio::io_context;
    using Signals            = boost::asio::signal_set;
    using Tcp                = boost::asio::ip::tcp;
    using Acceptor           = Tcp::acceptor;
    using SessionFactoryFunc = std::function<std::shared_ptr<ISession>(
        Context &io_context, Tcp::socket socket, int sessionId,
        std::shared_ptr<Logger> logger, std::shared_ptr<IComputer> computer)>;

    // DATA
    Context                                   context_;
    Signals                                   signals_;
    Acceptor                                  acceptor_;
    SessionFactoryFunc                        sessionFactory_;
    std::shared_ptr<NumberDumper>             numberDumper_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    int                                       sessionCounter_;
    unsigned int                              num_threads_;
    std::shared_ptr<Logger>                   logger_;
    std::shared_ptr<std::thread>              loggerThread_;
    std::shared_ptr<IComputer>                computer_;

    // Accept new connection
    void do_accept();

  public:
    // Create 'TcpServer' object with the specified 'port', 'logFilename',
    // 'numbersDumpFilename' and 'sessionFactoryFunc'. Use 'Session' object
    // creation function by default.
    TcpServer(short port, const std::string &logFilename,
              const std::string &numbersDumpFilename, int maxNumber = 1023,
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

#endif // TCPSERVER_H
