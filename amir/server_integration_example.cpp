// server_integration_example.cpp
// Exemple d'intégration de la partie 2 avec le serveur principal

#include "ClientManager.hpp"
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
// Include atoi 
#include <cstdlib>

class Server {
private:
    int _serverFd;
    int _port;
    std::string _password;
    ClientManager *_clientManager;
    fd_set _readFds;
    fd_set _writeFds;
    int _maxFd;
    
public:
    Server(int port, const std::string& password) : _port(port), _password(password) {
        _clientManager = new ClientManager(this, password);
        _maxFd = 0;
    }
    
    ~Server() {
        delete _clientManager;
        if (_serverFd > 0)
            close(_serverFd);
    }
    
    bool start() {
        // Créer le socket serveur
        _serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (_serverFd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            return false;
        }
        
        // Réutiliser l'adresse
        int opt = 1;
        setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // Configurer l'adresse
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(_port);
        
        // Bind
        if (bind(_serverFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            return false;
        }
        
        // Listen
        if (listen(_serverFd, 10) < 0) {
            std::cerr << "Error listening on socket" << std::endl;
            return false;
        }
        
        // Mettre en mode non-bloquant
        fcntl(_serverFd, F_SETFL, O_NONBLOCK);
        _maxFd = _serverFd;
        
        std::cout << "Server started on port " << _port << std::endl;
        return true;
    }
    
    void run() {
        while (true) {
            setupFdSets();
            
            // Select avec timeout de 1 seconde pour vérifier les timeouts
            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            
            int activity = select(_maxFd + 1, &_readFds, NULL, NULL, &timeout);
            
            if (activity < 0) {
                std::cerr << "Select error" << std::endl;
                break;
            }
            
            // Nouvelle connexion
            if (FD_ISSET(_serverFd, &_readFds)) {
                acceptNewConnection();
            }
            
            // Données des clients existants
            handleClientData();
            
            // Vérifier les timeouts périodiquement
            static time_t lastTimeoutCheck = time(NULL);
            if (time(NULL) - lastTimeoutCheck > 30) {
                _clientManager->checkTimeouts();
                lastTimeoutCheck = time(NULL);
            }
        }
    }
    
private:
    void setupFdSets() {
        FD_ZERO(&_readFds);
        FD_SET(_serverFd, &_readFds);
        _maxFd = _serverFd;
        
        // Ajouter tous les clients
        const std::map<int, Client*>& clients = _clientManager->getClients();
        for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            FD_SET(it->first, &_readFds);
            if (it->first > _maxFd)
                _maxFd = it->first;
        }
    }
    
    void acceptNewConnection() {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd < 0) {
            std::cerr << "Error accepting connection" << std::endl;
            return;
        }
        
        // Mettre en mode non-bloquant
        fcntl(clientFd, F_SETFL, O_NONBLOCK);
        
        // Ajouter le client au gestionnaire
        _clientManager->addClient(clientFd);
        
        std::cout << "New connection accepted (fd: " << clientFd << ")" << std::endl;
    }
    
    void handleClientData() {
        const std::map<int, Client*>& clients = _clientManager->getClients();
        std::vector<int> toRemove;
        
        for (std::map<int, Client*>::const_iterator it = clients.begin(); it != clients.end(); ++it) {
            int fd = it->first;
            
            if (FD_ISSET(fd, &_readFds)) {
                char buffer[1024];
                ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytesRead <= 0) {
                    // Connexion fermée ou erreur
                    toRemove.push_back(fd);
                } else {
                    buffer[bytesRead] = '\0';
                    _clientManager->handleClientData(fd, std::string(buffer));
                }
            }
        }
        
        // Supprimer les clients déconnectés
        for (std::vector<int>::iterator it = toRemove.begin(); it != toRemove.end(); ++it) {
            _clientManager->removeClient(*it);
        }
    }
};

// Point d'entrée principal
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    
    int port = atoi(argv[1]);
    std::string password = argv[2];
    
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }
    
    if (password.empty()) {
        std::cerr << "Password cannot be empty" << std::endl;
        return 1;
    }
    
    Server server(port, password);
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    std::cout << "ft_irc server running..." << std::endl;
    std::cout << "Use Ctrl+C to stop" << std::endl;
    
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}