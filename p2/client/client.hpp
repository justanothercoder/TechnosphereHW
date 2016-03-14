#pragma once

#include <iostream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <arpa/inet.h>

class Client
{
    static const int PORT = 3100;
public:
    Client();
    ~Client();

    std::string read_from(int fd);
    void send_to(int fd, const std::string& data);

    void run();

private:
    int client_socket;
    fd_set set;
};
