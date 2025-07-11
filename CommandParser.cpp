#include "CommandParser.hpp"
#include "ChannelManager.hpp"
#include "AuthHandler.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>

// Constructeur IRCMessage
IRCMessage::IRCMessage(const std::string& raw) {
    std::istringstream iss(raw);
    std::string token;
    
    // Vérifier s'il y a un préfixe
    if (raw[0] == ':') {
        iss >> prefix;
        prefix = prefix.substr(1); // Enlever le ':'
    }
    
    // Lire la commande
    if (iss >> command) {
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);
    }
    
    // Lire les paramètres
    std::string remaining;
    std::getline(iss, remaining);
    if (!remaining.empty() && remaining[0] == ' ')
        remaining = remaining.substr(1);
    
    if (!remaining.empty()) {
        params = CommandParser::splitParams(remaining);
    }
}

// Constructeur CommandParser
CommandParser::CommandParser(AuthHandler *authHandler, std::map<int, Client*> *clients, ChannelManager *channelManager)
    : _authHandler(authHandler), _clients(clients), _channelManager(channelManager) {}

// Destructeur
CommandParser::~CommandParser() {}

// Parser un message IRC
IRCMessage CommandParser::parseMessage(const std::string& raw) {
    return IRCMessage(raw);
}

// Séparer les paramètres (méthode statique)
std::vector<std::string> CommandParser::splitParams(const std::string& params) {
    std::vector<std::string> result;
    std::istringstream iss(params);
    std::string token;
    
    while (iss >> token) {
        if (token[0] == ':') {
            // Paramètre final (trailing parameter)
            std::string trailing = token.substr(1);
            std::string remaining;
            std::getline(iss, remaining);
            if (!remaining.empty())
                trailing += remaining;
            result.push_back(trailing);
            break;
        } else {
            result.push_back(token);
        }
    }
    
    return result;
}

// Traiter un message
bool CommandParser::processMessage(Client* client, const std::string& message) {
    if (message.empty())
        return true;
    
    IRCMessage msg = parseMessage(message);
    
    if (msg.command.empty())
        return true;
    
    client->updateLastActivity();
    
    // Commandes d'authentification
    if (msg.command == "PASS" || msg.command == "NICK" || msg.command == "USER") {
        return handleAuthCommand(client, msg);
    }
    
    // Vérifier si le client est enregistré pour les autres commandes
    if (!client->isRegistered()) {
        _authHandler->sendNumericReply(client, 451, "You have not registered");
        return false;
    }
    
    // Autres commandes (PING, QUIT, etc.)
    return handleGeneralCommand(client, msg);
}

// Gérer les commandes d'authentification
bool CommandParser::handleAuthCommand(Client* client, const IRCMessage& msg) {
    if (msg.command == "PASS") {
        return _authHandler->handlePass(client, msg.params);
    }
    else if (msg.command == "NICK") {
        return _authHandler->handleNick(client, msg.params);
    }
    else if (msg.command == "USER") {
        return _authHandler->handleUser(client, msg.params);
    }
    
    return false;
}

// Gérer les commandes générales
bool CommandParser::handleGeneralCommand(Client* client, const IRCMessage& msg) {
    if (msg.command == "PING") {
        std::string response = "PONG :ft_irc.42.fr";
        if (!msg.params.empty())
            response = "PONG :" + msg.params[0];
        client->sendMessage(response);
        return true;
    }
    else if (msg.command == "QUIT") {
        return handleQuit(client, msg.params);
    }
    else if (msg.command == "JOIN") {
        return handleJoin(client, msg.params);
    }
    else if (msg.command == "PART") {
        return handlePart(client, msg.params);
    }
    else if (msg.command == "PRIVMSG") {
        return handlePrivmsg(client, msg.params);
    }
    else if (msg.command == "KICK") {
        return handleKick(client, msg.params);
    }
    else if (msg.command == "INVITE") {
        return handleInvite(client, msg.params);
    }
    else if (msg.command == "TOPIC") {
        return handleTopic(client, msg.params);
    }
    else if (msg.command == "MODE") {
        return handleMode(client, msg.params);
    }
    else if (msg.command == "WHO") {
        _authHandler->sendNumericReply(client, 315, "End of WHO list");
        return true;
    }
    
    // Commande non reconnue
    _authHandler->sendNumericReply(client, 421, msg.command + " :Unknown command");
    return true;
}

// Commande JOIN
bool CommandParser::handleJoin(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        _authHandler->sendNumericReply(client, 461, "JOIN :Not enough parameters");
        return false;
    }
    
    std::string channelName = params[0];
    std::string key = (params.size() > 1) ? params[1] : "";
    
    return _channelManager->joinChannel(client, channelName, key);
}

// Commande PART
bool CommandParser::handlePart(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        _authHandler->sendNumericReply(client, 461, "PART :Not enough parameters");
        return false;
    }
    
    std::string channelName = params[0];
    std::string reason = (params.size() > 1) ? params[1] : "";
    
    return _channelManager->partChannel(client, channelName, reason);
}

// Commande PRIVMSG
bool CommandParser::handlePrivmsg(Client* client, const std::vector<std::string>& params) {
    if (params.size() < 2) {
        _authHandler->sendNumericReply(client, 461, "PRIVMSG :Not enough parameters");
        return false;
    }
    
    std::string target = params[0];
    std::string message = params[1];
    
    // Message vers un canal
    if (target[0] == '#' || target[0] == '&') {
        std::string fullMsg = client->getPrefix() + " PRIVMSG " + target + " :" + message;
        return _channelManager->sendToChannel(target, fullMsg, client);
    }
    
    // Message privé vers un utilisateur
    Client* targetClient = NULL;
    for (std::map<int, Client*>::iterator it = _clients->begin(); it != _clients->end(); ++it) {
        if (AuthHandler::compareNicknames(it->second->getNickname(), target) && it->second->isRegistered()) {
            targetClient = it->second;
            break;
        }
    }
    
    if (!targetClient) {
        _authHandler->sendNumericReply(client, 401, target + " :No such nick/channel");
        return false;
    }
    
    // Envoyer le message privé
    std::string fullMsg = client->getPrefix() + " PRIVMSG " + target + " :" + message;
    targetClient->sendMessage(fullMsg);
    return true;
}

// Commande KICK
bool CommandParser::handleKick(Client* client, const std::vector<std::string>& params) {
    if (params.size() < 2) {
        _authHandler->sendNumericReply(client, 461, "KICK :Not enough parameters");
        return false;
    }
    
    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = (params.size() > 2) ? params[2] : client->getNickname();
    
    return _channelManager->kickFromChannel(client, channelName, targetNick, reason);
}

bool CommandParser::handleInvite(Client* client, const std::vector<std::string>& params) {
    if (params.size() < 2) {
        _authHandler->sendNumericReply(client, 461, "INVITE :Not enough parameters");
        return false;
    }
    
    std::string targetNick = params[0];
    std::string channelName = params[1];
    
    return _channelManager->inviteToChannel(client, channelName, targetNick);
}

bool CommandParser::handleTopic(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        _authHandler->sendNumericReply(client, 461, "TOPIC :Not enough parameters");
        return false;
    }
    
    std::string channelName = params[0];
    std::string topic = (params.size() > 1) ? params[1] : "";
    
    return _channelManager->setChannelTopic(client, channelName, topic);
}

bool CommandParser::handleMode(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        _authHandler->sendNumericReply(client, 461, "MODE :Not enough parameters");
        return false;
    }
    
    std::string target = params[0];
    
    // Mode utilisateur
    if (AuthHandler::compareNicknames(target, client->getNickname())) {
        _authHandler->sendNumericReply(client, 221, "+");
        return true;
    }
    
    // Mode canal
    if (target[0] == '#' || target[0] == '&') {
        std::string modeString = (params.size() > 1) ? params[1] : "";
        std::vector<std::string> modeParams;
        
        // Récupérer les paramètres supplémentaires
        for (size_t i = 2; i < params.size(); ++i) {
            modeParams.push_back(params[i]);
        }
        
        return _channelManager->setChannelMode(client, target, modeString, modeParams);
    }
    
    _authHandler->sendNumericReply(client, 401, target + " :No such nick/channel");
    return false;
}

// Traiter le buffer d'un client
bool CommandParser::processClientBuffer(Client* client) {
    std::string message;
    while (!(message = client->extractMessage()).empty()) {
        if (!processMessage(client, message)) {
            // Si processMessage retourne false, le client doit être déconnecté
			if (message == "QUIT") {
					return false; // Le client doit être déconnecté
				}
            return true;
        }
    }
    return true; // Le client peut continuer
}

// Convertir en majuscules
std::string CommandParser::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Commande QUIT
bool CommandParser::handleQuit(Client* client, const std::vector<std::string>& params) {
    if (!client)
        return false;
    
    // Extraire la raison du QUIT
    std::string reason = "Client Quit";
    if (!params.empty())
        reason = params[0];
    
    std::cout << "Client " << client->getNickname() << " quit: " << reason << std::endl;
    
    // Si le client est enregistré, notifier les canaux
    if (client->isRegistered() && _channelManager) {
        _channelManager->broadcastQuit(client, reason);
        _channelManager->removeClientFromAllChannels(client);
    }
    
    // Retourner false pour indiquer que le client doit être déconnecté
    return false;
}