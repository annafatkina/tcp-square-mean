#ifndef SESSION_H
#define SESSION_H

#include "icomputer.h"
#include "isession.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <memory>

class Session : public ISession {
    // This class represents a single client session.

    // TYPES
    using Tcp     = boost::asio::ip::tcp;
    using Context = boost::asio::io_context;

    // DATA
    std::shared_ptr<IComputer> computer_;
    Context::strand            rwStrand_;
    Tcp::socket                socket_;
    boost::asio::streambuf     rBuffer_;
    boost::asio::streambuf     wBuffer_;

  protected:
    // Return data read from buffer.
    std::string readBuffer();

    // Write the specified 'str' to buffer.
    void writeToBuffer(const std::string &str);

    // Read data from the socket and schedule the callback to be called after
    // we get data.
    void do_read() override;

    // Write data to the socket.
    void do_write() override;

    // Process the received data.
    void handle() override;

  public:
    // NOT IMPLEMENTED
    Session(const Session &) = delete;
    Session(Session &&)      = delete;

    // Create 'Session' object with the specified 'io_context', 'socket',
    // 'sessionId', 'logger', and 'computer'.
    Session(Context &io_context, Tcp::socket socket, int sessionId,
            std::shared_ptr<Logger>    logger,
            std::shared_ptr<IComputer> computer);

    // Destroy this object.
    virtual ~Session();

    // Start receiving data.
    void start() override;
};

#endif // SESSION_H
