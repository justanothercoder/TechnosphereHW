#include "proxy.hpp"
#include "server_connection.hpp"
#include <iostream>

Proxy::Proxy(boost::asio::io_service& io_service, Port port, std::vector<std::pair<IP, Port>> servers) 
    : acceptor(io_service, tcp::endpoint(tcp::v4(), port))
    , servers(servers)
{

}

void Proxy::run()
{
    auto connection = std::shared_ptr<ServerConnection>(new ServerConnection(acceptor.get_io_service()));
    acceptor.async_accept(connection->socket(), [this, connection = std::move(connection)] (const auto& error) {

                std::cerr << "Async accept: " << error << std::endl;

                if (!error) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, servers.size() - 1);
                    int n = dis(gen);

                    connection->start(servers[n]);
                }

                this->run();
            });
}
