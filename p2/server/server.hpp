#pragma once

/*
 * Архитектура: Linux
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include "buffer.hpp"

class Server
{
public:
    Server();
    ~Server();

    void run();

private:
    std::string read_from(int fd);
    void send_to(int fd, std::string data);

private:
    int master_socket;
    int epoll_fd;
    std::vector<epoll_event> events;
    epoll_event ev;

    int max_events = 10;

    std::vector<int> sockets;
    std::map<int, Buffer> buffers;
};
