#include "server_connection.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <chrono>
#include <thread>
    
ServerConnection::ServerConnection(boost::asio::io_service& io_service) 
    : client_socket(io_service)
    , server_socket(io_service)
{ 
    temp_from_client_buf.resize(1024);   
    temp_to_client_buf.resize(1024);
}
tcp::socket& ServerConnection::socket() { return client_socket; }

void ServerConnection::start(std::pair<IP, Port> ip_port)
{
    tcp::endpoint server_endpoint(ip_port.first, ip_port.second);

    server_socket.async_connect(server_endpoint, 
                                boost::bind(&ServerConnection::handle_connect, 
                                            shared_from_this(), 
                                            boost::asio::placeholders::error));
}

void ServerConnection::handle_connect(const boost::system::error_code& error)
{
    std::cerr << "Async connect: " << error << std::endl;
    if (!error) {
        client_socket.async_read_some(boost::asio::buffer(temp_from_client_buf.data(), temp_from_client_buf.size()), 
                                      boost::bind(&ServerConnection::read_from_client_handler, 
                                                  shared_from_this(), 
                                                  boost::asio::placeholders::error, 
                                                  boost::asio::placeholders::bytes_transferred));
/*        
        async_write(server_socket, 
                    boost::asio::buffer(buf_from_client), 
                    boost::bind(&ServerConnection::write_to_server_handler, 
                                shared_from_this(), 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred));
*/        
        server_socket.async_read_some(boost::asio::buffer(temp_to_client_buf.data(), temp_to_client_buf.size()), 
                                      boost::bind(&ServerConnection::read_from_server_handler, 
                                                  shared_from_this(), 
                                                  boost::asio::placeholders::error, 
                                                  boost::asio::placeholders::bytes_transferred));
/*        
        async_write(client_socket, 
                    boost::asio::buffer(buf_to_client), 
                    boost::bind(&ServerConnection::write_to_client_handler, 
                                shared_from_this(), 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred));
*/                                
    } else {
        server_socket.close();
        client_socket.close();
    }    
}

void ServerConnection::read_from_client_handler(const boost::system::error_code& error, std::size_t bytes_transferred) 
{ 
//    std::cerr << "Read from client. Writing to server\nError: " << error << std::endl;
//    std::cerr << "Read bytes: " << bytes_transferred << std::endl;
  
    std::cerr << "Error on reading from client: " << error << std::endl;
    std::cerr << "Is EOF: " << (error == boost::asio::error::eof) << std::endl;

    if (error == boost::asio::error::eof || bytes_transferred == 0) {
        tmp_from = buf_from_client;
        buf_from_client = "";

        async_write(server_socket, 
                    boost::asio::buffer(tmp_from), 
                    boost::bind(&ServerConnection::write_to_server_handler, 
                                shared_from_this(), 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred));
    }
    else if (!error) {
        buf_from_client.append(temp_from_client_buf.begin(), temp_from_client_buf.begin() + bytes_transferred);
               
        tmp_from = buf_from_client;
        buf_from_client = "";
        client_socket.async_read_some(boost::asio::buffer(temp_from_client_buf),
                                      boost::bind(&ServerConnection::read_from_client_handler,
                                                  shared_from_this(),
                                                  boost::asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);

        async_write(server_socket, 
                    boost::asio::buffer(tmp_from), 
                    boost::bind(&ServerConnection::write_to_server_handler, 
                                shared_from_this(), 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred));
        
    } else {
        server_socket.close();
        client_socket.close();
    } 
}

void ServerConnection::write_to_server_handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{ 
    std::cerr << "Wrote to server. Reading from server\nError: " << error << std::endl;
    std::cerr << "Wrote bytes: " << bytes_transferred << std::endl;
    if (!error) {
        tmp_from = buf_from_client;
        buf_from_client = "";
/*        async_write(server_socket, 
                    boost::asio::buffer(tmp_from), 
                    boost::bind(&ServerConnection::write_to_server_handler, 
                                shared_from_this(), 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred));*/
    } else {
        server_socket.close();
        client_socket.close();
    } 
}

void ServerConnection::read_from_server_handler(const boost::system::error_code& error, std::size_t bytes_transferred) 
{ 
    std::cerr << "Read from server. Writing to client\nError: " << error << std::endl;
    std::cerr << "Read bytes: " << bytes_transferred << std::endl;

    if (error == boost::asio::error::eof || bytes_transferred == 0) {
        tmp_to = buf_to_client;
        buf_to_client = "";
        async_write(client_socket,
                    boost::asio::buffer(tmp_to),
                    boost::bind(&ServerConnection::write_to_client_handler,
                                shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));                                
  
    }
    else if (!error) {
        buf_to_client.append(temp_to_client_buf.begin(), temp_to_client_buf.begin() + bytes_transferred);
        
        tmp_to = buf_to_client;
        buf_to_client = "";
        async_write(client_socket,
                    boost::asio::buffer(tmp_to),
                    boost::bind(&ServerConnection::write_to_client_handler,
                                shared_from_this(),
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));                                
        
        server_socket.async_read_some(boost::asio::buffer(temp_to_client_buf, temp_to_client_buf.size()), 
                                      boost::bind(&ServerConnection::read_from_server_handler, 
                                                  shared_from_this(), 
                                                  boost::asio::placeholders::error, 
                                                  boost::asio::placeholders::bytes_transferred));
        
    } else {
        server_socket.close();
        client_socket.close();
    }    
}

void ServerConnection::write_to_client_handler(const boost::system::error_code& error, std::size_t bytes_transferred) 
{ 
    std::cerr << "Wrote to client. Reading from client\nError: " << error << std::endl;
    std::cerr << "Wrote bytes: " << bytes_transferred << std::endl;
    if (!error) {
        tmp_to = buf_to_client;
        buf_to_client = "";
/*
        async_write(client_socket, 
                    boost::asio::buffer(tmp_to), 
                    boost::bind(&ServerConnection::write_to_client_handler, 
                                shared_from_this(), 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred));
                                */
    } else {
        server_socket.close();
        client_socket.close();
    } 
}
