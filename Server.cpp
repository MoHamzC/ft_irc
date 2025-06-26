#include "Server.hpp"
#include "ChannelManager.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cerrno>

Server::Server(int port, const std::string& password) 
    : _port(port), _password(password), _serverSocket(-1) {
    _clientManager = new ClientManager(this, password);
    _channelManager = new ChannelManager(this);
    
    // Initialiser le CommandParser avec le ChannelManager
    _clientManager->initializeCommandParser(_channelManager);
}

Server::~Server() {
    if (_serverSocket != -1)
        close(_serverSocket);
    
    // Fermer tous les fds de poll
    for (size_t i = 1; i < _pollFds.size(); ++i) {
        close(_pollFds[i].fd);
    }
    
    delete _clientManager;
    delete _channelManager;
}

void Server::setupSocket() {
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == -1)
        throw std::runtime_error("Socket creation failed");
    
    // Non-blocking
    fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
    
    // Réutiliser l'adresse
    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Configuration de l'adresse
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(_serverSocket, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Bind failed");
    
    if (listen(_serverSocket, SOMAXCONN) < 0)
        throw std::runtime_error("Listen failed");
    
    // Ajouter le serveur socket au poll
    pollfd serverPfd;
    serverPfd.fd = _serverSocket;
    serverPfd.events = POLLIN;
    serverPfd.revents = 0;
    _pollFds.push_back(serverPfd);
}

void Server::start() {
    setupSocket();
    std::cout << GREEN << "Server started on port " << _port << RESET << std::endl;
}

void Server::run() {
    while (true) {
        int pollCount = poll(&_pollFds[0], _pollFds.size(), 1000); // Timeout 1s
        
        if (pollCount < 0) {
            if (errno == EINTR) {
                // Signal reçu, arrêter proprement
                std::cout << YELLOW << "Signal received, shutting down..." << RESET << std::endl;
                break;
            }
            std::cerr << RED << "Poll error: " << strerror(errno) << RESET << std::endl;
            break;
        }
        
        // Vérifier les événements
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                if (_pollFds[i].fd == _serverSocket) {
                    acceptNewClient();
                } else {
                    handleClientData(i);
                }
            }
        }
        
        // Maintenance périodique
        static time_t lastMaintenance = time(NULL);
        if (time(NULL) - lastMaintenance > 30) {
            _clientManager->checkTimeouts();
            _channelManager->cleanupEmptyChannels();
            cleanupDisconnectedClients();
            lastMaintenance = time(NULL);
        }
    }
    
    std::cout << GREEN << "Server stopped cleanly." << RESET << std::endl;
}

void Server::acceptNewClient() {
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int clientSocket = accept(_serverSocket, (sockaddr*)&clientAddr, &len);
    
    if (clientSocket < 0)
        return;
    
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    
    // Ajouter au poll
    pollfd clientPfd;
    clientPfd.fd = clientSocket;
    clientPfd.events = POLLIN;
    clientPfd.revents = 0;
    _pollFds.push_back(clientPfd);
    
    // Ajouter au gestionnaire de clients
    _clientManager->addClient(clientSocket);
    
    std::cout << GREEN << "New client connected (fd " << clientSocket << ")" << RESET << std::endl;
}

void Server::handleClientData(size_t i) {
    char buffer[1024];
    int clientSocket = _pollFds[i].fd;
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesRead <= 0) {
        removeClient(i);
        return;
    }
    
    buffer[bytesRead] = '\0';
    
    // Traiter les données via le ClientManager
    _clientManager->handleClientData(clientSocket, std::string(buffer));
}

void Server::removeClient(size_t i) {
    int clientSocket = _pollFds[i].fd;
    
    // Supprimer du gestionnaire de clients (qui gère les canaux aussi)
    Client* client = _clientManager->getClient(clientSocket);
    if (client) {
        _channelManager->removeClientFromAllChannels(client);
        _clientManager->removeClient(clientSocket);
    }
    
    // Supprimer du poll
    _pollFds.erase(_pollFds.begin() + i);
    
    std::cout << YELLOW << "Client disconnected (fd " << clientSocket << ")" << RESET << std::endl;
}

void Server::cleanupDisconnectedClients() {
    // Nettoyer les clients déconnectés qui ne sont plus dans les pollFds
    std::vector<int> validFds;
    for (size_t i = 1; i < _pollFds.size(); ++i) {
        validFds.push_back(_pollFds[i].fd);
    }
    
    // Vérifier les clients du ClientManager
    const std::map<int, Client*>& clients = _clientManager->getClients();
    std::vector<int> toRemove;
    
    for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
        bool found = false;
        for (size_t j = 0; j < validFds.size(); ++j) {
            if (validFds[j] == it->first) {
                found = true;
                break;
            }
        }
        if (!found) {
            toRemove.push_back(it->first);
        }
    }
    
    // Supprimer les clients orphelins
    for (size_t i = 0; i < toRemove.size(); ++i) {
        _clientManager->removeClient(toRemove[i]);
    }
}

// Getters
ClientManager* Server::getClientManager() const {
    return _clientManager;
}

ChannelManager* Server::getChannelManager() const {
    return _channelManager;
}

// Utilitaires
void Server::broadcast(const std::string& message) {
    _clientManager->broadcastToAll(message);
}

void Server::sendToChannel(const std::string& channelName, const std::string& message, Client* sender) {
    _channelManager->sendToChannel(channelName, message, sender);
}
