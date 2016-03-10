#include "server.hpp"

int set_nonblock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;            
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Server::Server()
{
    master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(3100);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int optval = 1;
    setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bind(master_socket, (struct sockaddr*)(&sock_addr), sizeof(sock_addr));
    set_nonblock(master_socket);
                
    listen(master_socket, SOMAXCONN);

    epoll_fd = epoll_create1(0);
    events.resize(max_events);

    ev.data.fd = master_socket;
    ev.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, master_socket, &ev);
}

Server::~Server()
{
    shutdown(master_socket, SHUT_RDWR);
}
    
std::string Server::read_from(int fd)
{
    std::string data;
    char buf[512] = {0};

    int r;
    while ((r = recv(fd, buf, sizeof(buf), 0)) > 0) {
        std::cout << r << std::endl;
        data += buf;
    }

    return data;
}

void Server::send_to(int fd, std::string data)
{
    const char* s = data.data();
    int len = data.length();

    const int msg_len = 1024;

    while (len > 0) {
        send(fd, s, std::min(msg_len, len), MSG_NOSIGNAL);
        len -= msg_len;
        s += msg_len;
    }
}

void Server::run()
{
    while (true) {
        int number_of_fd = epoll_wait(epoll_fd, events.data(), max_events, -1);

        for (int i = 0; i < number_of_fd; ++i) {
            if ( (events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ) {
                shutdown(events[i].data.fd, SHUT_RDWR);
                std::cout << "connection terminated" << std::endl;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
            } else if (events[i].data.fd == master_socket) {
                struct sockaddr_in in_addr;
                socklen_t len;                
                
                int fd = accept(master_socket, (struct sockaddr*)&in_addr, &len);
                set_nonblock(fd);

                sockets.push_back(fd);

                send_to(fd, "Welcome to chat\n");
                std::cout << "accepted connection" << std::endl;

                ev.data.fd = fd;
                ev.events = EPOLLIN;

                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
            } else {
                int fd = events[i].data.fd;

                std::string data = read_from(fd);
                std::cout << data << std::endl;

                data = "Message from <" + std::to_string(fd) + ">: " + data;

                for (auto sfd : sockets) {
                    send_to(sfd, data);
                }
            }
        }
    }
}
