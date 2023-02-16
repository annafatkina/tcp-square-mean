#include "tcpserver.h"
#include <iostream>

int
main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        auto server = std::make_unique<TcpServer>(std::atoi(argv[1]));

        server->run();
        server->waitForStop();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
