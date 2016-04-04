#pragma once

#include <utility>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
    
using IP = boost::asio::ip::address;
using Port = std::size_t;

class Proxy
{
public:
    Proxy(boost::asio::io_service& io_service, Port port, std::vector<std::pair<IP, Port>> servers);

    void run();
    
private:
    tcp::acceptor acceptor;
    std::vector<std::pair<IP, Port>> servers;
};
