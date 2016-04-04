#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

using IP = boost::asio::ip::address;
using Port = std::size_t;

class ServerConnection : public std::enable_shared_from_this<ServerConnection>
{
public:
    using pointer = std::shared_ptr<ServerConnection>;

    ServerConnection(boost::asio::io_service& io_service);
    
    tcp::socket& socket();

    void start(std::pair<IP, Port> ip_port);

    void read_from_client_handler(const boost::system::error_code& error, std::size_t bytes_transferred);
    void write_to_client_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    void read_from_server_handler(const boost::system::error_code& error, std::size_t bytes_transferred);
    void write_to_server_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    void handle_connect(const boost::system::error_code& error);

private:
    tcp::socket client_socket;
    tcp::socket server_socket;

    std::string buf_from_client;
    std::string buf_to_client;

    std::vector<char> temp_from_client_buf;
    std::vector<char> temp_to_client_buf;

    std::string tmp_to, tmp_from;
};
