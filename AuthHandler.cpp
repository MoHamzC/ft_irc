#include "AuthHandler.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

// Forward declaration pour éviter l'include
class Server;
class ChannelManager;

// Constructeur
AuthHandler::AuthHandler(const std::string& password, std::map<int, Client*> *clients, Server *server) 
    : _serverPassword(password), _clients(clients), _server(server) {}

// Destructeur
AuthHandler::~AuthHandler() {}

// Validation du nickname selon RFC 2812
bool AuthHandler::isValidNickname(const std::string& nick) const {
    if (nick.empty() || nick.length() > 9) // Limite de 9 caractères
        return false;
    
    // Premier caractère doit être une lettre ou un caractère spécial
    char first = nick[0];
    if (!std::isalpha(first) && first != '[' && first != ']' && first != '{' && 
        first != '}' && first != '\\' && first != '|' && first != '_')
        return false;
    
    // Caractères suivants peuvent inclure des chiffres et tirets
    for (size_t i = 1; i < nick.length(); i++) {
        char c = nick[i];
        if (!std::isalnum(c) && c != '[' && c != ']' && c != '{' && c != '}' && 
            c != '\\' && c != '|' && c != '_' && c != '-')
            return false;
    }
    
    return true;
}

// Convertir en minuscules pour comparaison insensible à la casse
std::string AuthHandler::toLowerCase(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + ('a' - 'A');
        }
    }
    return result;
}

// Comparer deux nicknames en ignorant la casse
bool AuthHandler::compareNicknames(const std::string& nick1, const std::string& nick2) {
    return toLowerCase(nick1) == toLowerCase(nick2);
}

// Vérifier si le nickname est déjà utilisé
bool AuthHandler::isNicknameInUse(const std::string& nick) const {
    for (std::map<int, Client*>::iterator it = _clients->begin(); it != _clients->end(); ++it) {
        if (compareNicknames(it->second->getNickname(), nick))
            return true;
    }
    return false;
}

// Envoyer message de bienvenue
void AuthHandler::sendWelcome(Client* client) {
    std::string serverName = "ft_irc.42.fr"; // À adapter
    
    sendNumericReply(client, RPL_WELCOME, 
        "Welcome to the Internet Relay Network " + client->getPrefix());
    
    sendNumericReply(client, RPL_YOURHOST, 
        "Your host is " + serverName + ", running version ft_irc-1.0");
    
    sendNumericReply(client, RPL_CREATED, 
        "This server was created by 42 students");
    
    sendNumericReply(client, RPL_MYINFO, 
        serverName + " ft_irc-1.0 o o");
}

// Envoyer erreur
void AuthHandler::sendError(Client* client, const std::string& error) {
    client->sendMessage("ERROR :" + error);
}

// Envoyer réponse numérique IRC
void AuthHandler::sendNumericReply(Client* client, int code, const std::string& message) {
    std::ostringstream oss;
    oss << ":ft_irc.42.fr ";
    if (code < 100) oss << "0";
    if (code < 10) oss << "0";
    oss << code << " ";
    
    if (!client->getNickname().empty())
        oss << client->getNickname();
    else
        oss << "*";
    
    oss << " :" << message;
    client->sendMessage(oss.str());
}

// Commande PASS
bool AuthHandler::handlePass(Client* client, const std::vector<std::string>& params) {
    if (client->isRegistered()) {
        sendNumericReply(client, ERR_ALREADYREGISTERED, "You may not reregister");
        return false;
    }
    
    if (params.empty()) {
        sendNumericReply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
        return false;
    }
    
    std::string password = params[0];
    // Enlever le ':' au début si présent
    if (!password.empty() && password[0] == ':')
        password = password.substr(1);
    
    if (password == _serverPassword) {
        client->setPasswordOk(true);
        if (client->getState() == CONNECTING)
            client->setState(PASS_OK);
        return true;
    } else {
        sendNumericReply(client, ERR_PASSWDMISMATCH, "Password incorrect");
        return false;
    }
}

// Commande NICK
bool AuthHandler::handleNick(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        sendNumericReply(client, ERR_NONICKNAMEGIVEN, "No nickname given");
        return false;
    }
    
    std::string newNick = params[0];
    
    // Validation du nickname
    if (!isValidNickname(newNick)) {    
        sendNumericReply(client, ERR_ERRONEOUSNICKNAME, newNick + " :Erroneous nickname");
            return false;
    }
    
    // Vérifier si le nick est déjà utilisé
    if (isNicknameInUse(newNick)) {
        sendNumericReply(client, ERR_NICKNAMEINUSE, newNick + " :Nickname is already in use");
        return false;
    }
    
    // Si le client a déjà un nickname, notifier le changement
    if (!client->getNickname().empty() && client->isRegistered()) {
        std::string oldNick = client->getNickname();
        client->setNickname(newNick);
        
        // Notifier tous les canaux du changement de nick
        // (sera géré par le serveur principal lors du traitement)
        std::string nickMsg = ":" + oldNick + "!" + client->getUsername() + "@" + client->getHostname() + " NICK :" + newNick;
        client->sendMessage(nickMsg);
    } else {
        client->setNickname(newNick);
        
        // Mettre à jour l'état
        if (client->getState() == CONNECTING && client->isPasswordOk())
            client->setState(PASS_OK);
        if (client->getState() == PASS_OK || client->getState() == USER_OK)
            client->setState(NICK_OK);
    }
    
    checkRegistration(client);
    return true;
}

// Commande USER
bool AuthHandler::handleUser(Client* client, const std::vector<std::string>& params) {
    if (client->isRegistered()) {
        sendNumericReply(client, ERR_ALREADYREGISTERED, "You may not reregister");
        return false;
    }
    
    if (params.size() < 4) {
        sendNumericReply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
        return false;
    }
    
    std::string username = params[0];
    std::string hostname = params[1]; // Généralement ignoré
    std::string servername = params[2]; // Généralement ignoré
    std::string realname = params[3];
    
    // Enlever le ':' au début du realname si présent
    if (!realname.empty() && realname[0] == ':')
        realname = realname.substr(1);
    
    client->setUsername(username);
    client->setRealname(realname);
    
    // Mettre à jour l'état
    if (client->getState() == CONNECTING && client->isPasswordOk())
        client->setState(PASS_OK);
    if (client->getState() == PASS_OK || client->getState() == NICK_OK)
        client->setState(USER_OK);
    
    checkRegistration(client);
    return true;
}

// Vérifier si le client peut être enregistré
void AuthHandler::checkRegistration(Client* client) {
    if (!client->isPasswordOk()) {
        return; // Mot de passe requis d'abord
    }
    
    if (client->getNickname().empty() || client->getUsername().empty()) {
        return; // NICK et USER requis
    }
    
    if (!client->isRegistered()) {
        client->setState(REGISTERED);
        client->updateLastActivity();
        sendWelcome(client);
    }
}

// Vérifier si le client est enregistré
bool AuthHandler::isClientRegistered(Client* client) const {
    return client->isRegistered();
}

// Gérer timeout
void AuthHandler::handleTimeout(Client* client) {
    sendError(client, "Ping timeout");
    // Le serveur devra fermer la connexion
}