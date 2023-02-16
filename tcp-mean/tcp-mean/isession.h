#ifndef ISESSION_H
#define ISESSION_H

#include "icomputer.h"
#include "logger.h"
#include <boost/asio.hpp>
#include <memory>

class ISession : public std::enable_shared_from_this<ISession> {
    // This class specifies an interface for session types.

    // TYPES
    using Tcp     = boost::asio::ip::tcp;
    using Context = boost::asio::io_context;

  protected:
    int                     sessionId_;
    std::shared_ptr<Logger> logger_;

    // Read data from the socket and schedule the callback to be called after
    // we get data.
    virtual void do_read() = 0;

    // Write data to the socket.
    virtual void do_write() = 0;

    // Process the received data.
    virtual void handle() = 0;

  public:
    // Create object with the specified 'sessionId'.
    ISession(int sessionId, std::shared_ptr<Logger> logger)
        : sessionId_(sessionId)
        , logger_(logger) {
    }

    // Destroy this object.
    virtual ~ISession() = default;

    // Start receiving data.
    virtual void start() = 0;
};

// Create session with the specified 'io_context', 'socket' and 'sessionId'.
// Return shared ptr to the created session.
std::shared_ptr<ISession> createSession(boost::asio::io_context &    io_context,
                                        boost::asio::ip::tcp::socket socket,
                                        int                          sessionId,
                                        std::shared_ptr<Logger>      logger,
                                        std::shared_ptr<IComputer>   computer);

#endif // ISESSION_H
