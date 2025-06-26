#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <string>
#include <vector>
#include <map>
#include "Client.hpp"
#include "AuthHandler.hpp"

// Forward declarations
class CommandParser;
class Channel;
class ChannelManager;

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
    ChannelManager *_channelManager; // AJOUT NÉCESSAIRE
    
    // Parsing
    IRCMessage parseMessage(const std::string& raw);
    
    // Gestion des commandes
    bool handleAuthCommand(Client* client, const IRCMessage& msg);
    bool handleGeneralCommand(Client* client, const IRCMessage& msg);
    
    // NOUVELLES COMMANDES OBLIGATOIRES
    bool handleJoin(Client* client, const std::vector<std::string>& params);
    bool handlePart(Client* client, const std::vector<std::string>& params);
    bool handlePrivmsg(Client* client, const std::vector<std::string>& params);
    bool handleKick(Client* client, const std::vector<std::string>& params);
    bool handleInvite(Client* client, const std::vector<std::string>& params);
    bool handleTopic(Client* client, const std::vector<std::string>& params);
    bool handleMode(Client* client, const std::vector<std::string>& params);

public:
    CommandParser(AuthHandler *authHandler, std::map<int, Client*> *clients, ChannelManager *channelManager);
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