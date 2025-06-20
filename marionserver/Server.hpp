#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include "Client.hpp"

#define RESET       "\033[0m"
#define GREEN       "\033[32m"
#define RED         "\033[31m"
#define BLUE        "\033[34m"
#define YELLOW      "\033[33m"
#define PURPLE     "\033[35m"


class Server 
{
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();

	void start();
private:
    int port;
    std::string password;
    int serverSocket;
    std::vector<pollfd> pollFds;
    std::map<int, Client*> clients; 

    void acceptNewClient();
    void handleMessage(size_t i);
    void removeClient(size_t i);
}