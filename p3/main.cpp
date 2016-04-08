#include <fstream>
#include "proxy.hpp"

int main(int argc, char** argv)
{
    std::ifstream in(argv[1]);
    std::vector<std::pair<IP, Port>> servers;
    
//    const int PORT = 10000;
    int PORT;
    in >> PORT;
    
    std::string ip;
    int port;
    while (in >> ip >> port) {
        servers.emplace_back(boost::asio::ip::address::from_string(ip), port);
    }

    boost::asio::io_service io_service;

//    Proxy proxy(io_service, PORT, {{boost::asio::ip::address::from_string("127.0.0.1"), 3100}});
    Proxy proxy(io_service, PORT, servers);
    proxy.run();

    io_service.run();
}
