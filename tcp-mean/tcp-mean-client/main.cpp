#include "client.h"
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
    try {
        if (argc != 5) {
            std::cerr << "Usage: tcp-mean-client.exe <host> <port> "
                         "<logFilename> <maxNumber>\n";
            return 1;
        }

        Client client(argv[1], argv[2], argv[3], std::atoi(argv[4]));
        client.run();

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
