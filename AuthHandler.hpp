#ifndef AUTHHANDLER_HPP
#define AUTHHANDLER_HPP

#include "Client.hpp"
#include <string>
#include <vector>
#include <map>

class Server; // Forward declaration
class ChannelManager; // Forward declaration

class AuthHandler {
private:
    std::string _serverPassword;
    std::map<int, Client*> *_clients;
    Server *_server;
    
    // Validation
    bool isValidNickname(const std::string& nick) const;
    bool isNicknameInUse(const std::string& nick) const;
    
    // Réponses IRC
    void sendWelcome(Client* client);
    void sendError(Client* client, const std::string& error);

public:
    AuthHandler(const std::string& password, std::map<int, Client*> *clients, Server *server);
    ~AuthHandler();
    
    // Utilitaires publics pour comparaison insensible à la casse
    static std::string toLowerCase(const std::string& str);
    static bool compareNicknames(const std::string& nick1, const std::string& nick2);
    
    // Commandes d'authentification
    bool handlePass(Client* client, const std::vector<std::string>& params);
    bool handleNick(Client* client, const std::vector<std::string>& params);
    bool handleUser(Client* client, const std::vector<std::string>& params);
    
    // Utilitaires
    void checkRegistration(Client* client);
    bool isClientRegistered(Client* client) const;
    void handleTimeout(Client* client);
    
    // Méthode publique pour envoyer des réponses numériques
    void sendNumericReply(Client* client, int code, const std::string& message);
    
    // Constantes de réponse IRC
    static const int RPL_WELCOME = 001;
    static const int RPL_YOURHOST = 002;
    static const int RPL_CREATED = 003;
    static const int RPL_MYINFO = 004;
    static const int ERR_NONICKNAMEGIVEN = 431;
    static const int ERR_ERRONEOUSNICKNAME = 432;
    static const int ERR_NICKNAMEINUSE = 433;
    static const int ERR_NEEDMOREPARAMS = 461;
    static const int ERR_ALREADYREGISTERED = 462;
    static const int ERR_PASSWDMISMATCH = 464;
};

#endif