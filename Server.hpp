#ifndef SERVER_HPP
#define SERVER_HPP

#include "ClientManager.hpp"
#include "ChannelManager.hpp"
#include <poll.h>
#include <vector>
#include <netinet/in.h>

#define RESET       "\033[0m"
#define GREEN       "\033[32m"
#define RED         "\033[31m"
#define BLUE        "\033[34m"
#define YELLOW      "\033[33m"
#define PURPLE      "\033[35m"

class Server {
private:
    int _port;
    std::string _password;
    int _serverSocket;
    std::vector<pollfd> _pollFds;
    
    // Gestionnaires (architecture d'Amir)
    ClientManager *_clientManager;
    ChannelManager *_channelManager;
    
    // Méthodes privées
    void setupSocket();
    void acceptNewClient();
    void handleClientData(size_t i);
    void removeClient(size_t i);
    void cleanupDisconnectedClients();

public:
    Server(int port, const std::string& password);
    ~Server();
    
    void start();
    void run();
    
    // Getters pour les gestionnaires
    ClientManager* getClientManager() const;
    ChannelManager* getChannelManager() const;
    
    // Utilitaires
    void broadcast(const std::string& message);
    void sendToChannel(const std::string& channelName, const std::string& message, Client* sender = NULL);
};

#endif
