#include "client.hpp"

int set_nonblock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;            
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Client::Client()
{
    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(PORT);
    sock_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    set_nonblock(client_socket);
    connect(client_socket, (const sockaddr*)&sock_addr, sizeof(sock_addr));
}

Client::~Client()
{
    shutdown(client_socket, SHUT_RDWR);
}

void Client::run()
{
    while (true) {
        FD_ZERO(&set);
        FD_SET(0, &set);
        FD_SET(client_socket, &set);
        
        if (select(client_socket + 1, &set, NULL, NULL, NULL) == -1) {
            break;
        }
   
        if (FD_ISSET(0, &set)) {
            std::string line;
            std::getline(std::cin, line);
            send_to(client_socket, line + "\n");
        }        

        if (FD_ISSET(client_socket, &set)) {
            std::string data = read_from(client_socket);
            std::cout << data << std::endl;
        }
    }
}

void Client::send_to(int fd, const std::string& data)
{
    send(fd, data.data(), data.length(), MSG_NOSIGNAL);
}

std::string Client::read_from(int fd)
{
    std::string data;
    char buf[512] = {0};

    int r;
    while ((r = recv(fd, buf, sizeof(buf), 0)) > 0) {
        data += std::string(buf, buf + r);
    }

    return data;
}
