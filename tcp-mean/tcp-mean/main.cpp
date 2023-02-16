#include "tcpserver.h"
#include <iostream>

int
main(int argc, char *argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: tcp-mean-server.exe <port> <logFilename> <numbersDumpFilename> <maxNumber>\n";
            return 1;
        }

        TcpServer server(std::atoi(argv[1]), argv[2], argv[3], std::atoi(argv[4]));

        server.run();
        server.waitForStop();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
