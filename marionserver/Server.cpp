#include "Server.hpp"

Server::Server(int port, const std::string& password)
    : port(port), password(password), serverSocket(-1)
{
}

Server::~Server() 
{
    close(serverSocket);
    for (size_t i = 0; i < pollFds.size(); ++i)
        close(pollFds[i].fd);
    std::map<int, Client*>::iterator it = clients.begin();
    for (; it != clients.end(); ++it)
        delete it->second;
}


void Server::start()
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        throw std::runtime_error("socket failed");
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
	int opt = 1; //option activee 
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");

    if (listen(serverSocket, SOMAXCONN) < 0)
        throw std::runtime_error("listen failed");

    pollfd pfd;
    pfd.fd = serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pollFds.push_back(pfd);
}


void Server::run()
 {
    while (true) 
	{
        int pollCount = poll(&pollFds[0], pollFds.size(), -1);
        if (pollCount < 0) //ifpollcountdiffde0thenfor
            continue;

        for (size_t i = 0; i < pollFds.size(); ++i) {
            if (pollFds[i].revents & POLLIN) 
			{
                if (pollFds[i].fd == serverSocket)
                    acceptNewClient();
                else
                    handleMessage(i);
            }
        }
    }
}


void Server::acceptNewClient() 
{
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &len);
    if (clientSocket < 0)
        return;
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    pollfd clientPfd;
    clientPfd.fd = clientSocket;
    clientPfd.events = POLLIN;
    clientPfd.revents = 0;
    pollFds.push_back(clientPfd);
    clients[clientSocket] = new Client(clientSocket);
    std::cout << "New client connected (fd " << clientSocket << ")" << std::endl;
}

void Server::removeClient(size_t i) 
{
	int clientSocket = pollFds[i].fd;
	close(clientSocket);
	delete clients[clientSocket];
	clients.erase(clientSocket);
	pollFds.erase(pollFds.begin() + i);
	std::cout << "Client disconnected (fd " << clientSocket << ")" << std::endl;
}

void Server::handleMessage(size_t i)
 {
    char buffer[1024];
    int clientSocket = pollFds[i].fd;
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) 
	{
        removeClient(i);
        return;
    }
    buffer[bytesRead] = '\0';
    Client* client = clients[clientSocket];
    client->getBuffer() += buffer;
    size_t pos;
    while ((pos = client->getBuffer().find("\r\n")) != std::string::npos) 
	{
        std::string msg = client->getBuffer().substr(0, pos);
        client->getBuffer().erase(0, pos + 2);
        std::cout << "received: [" << msg << "]" << std::endl;
        std::string reply = "You said: " + msg + "\r\n";
        send(clientSocket, reply.c_str(), reply.length(), 0);
    }
}



