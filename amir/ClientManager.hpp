#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include "Client.hpp"
#include "AuthHandler.hpp"
#include "CommandParser.hpp"
#include <map>
#include <vector>

class Server; // Forward declaration

class ClientManager {
private:
    std::map<int, Client*> _clients;
    AuthHandler *_authHandler;
    CommandParser *_commandParser;
    Server *_server;
    int _timeout;
    
public:
    ClientManager(Server *server, const std::string& password, int timeout = 300);
    ~ClientManager();
    
    // Gestion des clients
    void addClient(int fd);
    void removeClient(int fd);
    Client* getClient(int fd);
    
    // Traitement des données
    void handleClientData(int fd, const std::string& data);
    void processClientMessages(int fd);
    
    // Maintenance
    void checkTimeouts();
    void disconnectClient(int fd, const std::string& reason = "");
    
    // Statistiques
    size_t getClientCount() const;
    std::vector<Client*> getRegisteredClients() const;
    std::vector<Client*> getAllClients() const;
    
    // Utilitaires pour le serveur
    bool isValidFd(int fd) const;
    void broadcastToAll(const std::string& message);
    void sendToNick(const std::string& nickname, const std::string& message);
    
    // Getters
    const std::map<int, Client*>& getClients() const;
};

#endif