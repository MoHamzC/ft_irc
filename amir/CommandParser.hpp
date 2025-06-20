#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <string>
#include <vector>
#include <map>
#include "Client.hpp"
#include "AuthHandler.hpp"

// Forward declaration pour pouvoir utiliser splitParams dans IRCMessage
class CommandParser;

struct IRCMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    
    IRCMessage() {}
    IRCMessage(const std::string& raw);
};

class CommandParser {
private:
    AuthHandler *_authHandler;
    std::map<int, Client*> *_clients;
    
    // Parsing
    IRCMessage parseMessage(const std::string& raw);
    
    // Gestion des commandes
    bool handleAuthCommand(Client* client, const IRCMessage& msg);
    bool handleGeneralCommand(Client* client, const IRCMessage& msg);
    
public:
    CommandParser(AuthHandler *authHandler, std::map<int, Client*> *clients);
    ~CommandParser();
    
    // Méthode statique pour être utilisée par IRCMessage
    static std::vector<std::string> splitParams(const std::string& params);
    
    // Traiter un message reçu d'un client
    bool processMessage(Client* client, const std::string& message);
    
    // Traiter tous les messages en buffer d'un client
    void processClientBuffer(Client* client);
    
    // Utilitaires
    std::string toUpper(const std::string& str);
};

#endif