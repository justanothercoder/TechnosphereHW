#include "proxy.hpp"

int main()
{
    const int PORT = 10000;

    boost::asio::io_service io_service;

    Proxy proxy(io_service, PORT, {{boost::asio::ip::address::from_string("127.0.0.1"), 3100}});
    proxy.run();

    io_service.run();
}
