#include "ClientManager.hpp"
#include "Server.hpp"
#include "AuthHandler.hpp"
#include <iostream>
#include <algorithm>
#include <unistd.h>

// Constructeur
ClientManager::ClientManager(Server *server, const std::string& password, int timeout)
    : _server(server), _timeout(timeout) {
    _authHandler = new AuthHandler(password, &_clients, server);
    // Note: _commandParser sera initialisé après la création du ChannelManager
    _commandParser = NULL;
}

// Destructeur
ClientManager::~ClientManager() {
    // Nettoyer tous les clients
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    _clients.clear();
    
    delete _authHandler;
    delete _commandParser;
}

// Ajouter un nouveau client
void ClientManager::addClient(int fd) {
    if (_clients.find(fd) != _clients.end()) {
        std::cerr << "Warning: Client with fd " << fd << " already exists" << std::endl;
        return;
    }
    
    Client* newClient = new Client(fd);
    _clients[fd] = newClient;
    
    std::cout << "New client connected (fd: " << fd << ")" << std::endl;
    
    // Envoyer un message de notification de connexion
    newClient->sendMessage("NOTICE AUTH :*** Looking up your hostname...");
    newClient->sendMessage("NOTICE AUTH :*** Found your hostname");
}

// Supprimer un client
void ClientManager::removeClient(int fd) {
    std::map<int, Client*>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    
    Client* client = it->second;
    
    if (client->isRegistered()) {
        std::cout << "Client " << client->getNickname() << " disconnected (fd: " << fd << ")" << std::endl;
        
        // Notifier les canaux de la déconnexion
        if (_server && _server->getChannelManager()) {
            _server->getChannelManager()->broadcastQuit(client, "Client disconnected");
            _server->getChannelManager()->removeClientFromAllChannels(client);
        }
    } else {
        std::cout << "Unregistered client disconnected (fd: " << fd << ")" << std::endl;
    }
    
    delete client;
    _clients.erase(it);
    close(fd);
}

// Récupérer un client par fd
Client* ClientManager::getClient(int fd) {
    std::map<int, Client*>::iterator it = _clients.find(fd);
    return (it != _clients.end()) ? it->second : NULL;
}

// Traiter les données reçues d'un client
void ClientManager::handleClientData(int fd, const std::string& data) {
    Client* client = getClient(fd);
    if (!client)
        return;
    
    // Ajouter les données au buffer
    client->appendToBuffer(data);
    
    // Traiter les messages complets
    processClientMessages(fd);
}

// Traiter tous les messages dans le buffer d'un client
void ClientManager::processClientMessages(int fd) {
    Client* client = getClient(fd);
    if (!client)
        return;
    
    _commandParser->processClientBuffer(client);
}

// Vérifier les timeouts
void ClientManager::checkTimeouts() {
    std::vector<int> toDisconnect;
    
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        Client* client = it->second;
        
        // Timeout pour clients non-enregistrés (30 secondes)
        if (!client->isRegistered() && client->isTimedOut(30)) {
            toDisconnect.push_back(it->first);
            continue;
        }
        
        // Timeout pour clients enregistrés
        if (client->isRegistered() && client->isTimedOut(_timeout)) {
            _authHandler->handleTimeout(client);
            toDisconnect.push_back(it->first);
        }
    }
    
    // Déconnecter les clients en timeout
    for (std::vector<int>::iterator it = toDisconnect.begin(); it != toDisconnect.end(); ++it) {
        disconnectClient(*it, "Ping timeout");
    }
}

// Déconnecter un client avec une raison
void ClientManager::disconnectClient(int fd, const std::string& reason) {
    Client* client = getClient(fd);
    if (client && !reason.empty()) {
        client->sendMessage("ERROR :" + reason);
    }
    removeClient(fd);
}

// Obtenir le nombre de clients
size_t ClientManager::getClientCount() const {
    return _clients.size();
}

// Obtenir tous les clients enregistrés
std::vector<Client*> ClientManager::getRegisteredClients() const {
    std::vector<Client*> registered;
    
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->isRegistered()) {
            registered.push_back(it->second);
        }
    }
    
    return registered;
}

// Obtenir tous les clients
std::vector<Client*> ClientManager::getAllClients() const {
    std::vector<Client*> all;
    
    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
        all.push_back(it->second);
    }
    
    return all;
}

// Vérifier si un fd est valide
bool ClientManager::isValidFd(int fd) const {
    return _clients.find(fd) != _clients.end();
}

// Envoyer un message à tous les clients enregistrés
void ClientManager::broadcastToAll(const std::string& message) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->isRegistered()) {
            it->second->sendMessage(message);
        }
    }
}

// Envoyer un message à un client par nickname
void ClientManager::sendToNick(const std::string& nickname, const std::string& message) {
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (AuthHandler::compareNicknames(it->second->getNickname(), nickname) && it->second->isRegistered()) {
            it->second->sendMessage(message);
            return;
        }
    }
}

// Getter pour les clients
const std::map<int, Client*>& ClientManager::getClients() const {
    return _clients;
}

// Initialisation différée du CommandParser
void ClientManager::initializeCommandParser(ChannelManager *channelManager) {
    if (_commandParser) {
        delete _commandParser;
    }
    _commandParser = new CommandParser(_authHandler, &_clients, channelManager);
}