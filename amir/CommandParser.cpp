#include "CommandParser.hpp"
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
CommandParser::CommandParser(AuthHandler *authHandler, std::map<int, Client*> *clients)
    : _authHandler(authHandler), _clients(clients) {}

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
        std::string reason = "Client Quit";
        if (!msg.params.empty())
            reason = msg.params[0];
        
        // Notifier la déconnexion (sera géré par le serveur)
        std::cout << "Client " << client->getNickname() << " quit: " << reason << std::endl;
        return false; // Indique que le client doit être déconnecté
    }
    else if (msg.command == "WHO") {
        // Commande WHO basique (peut être étendue)
        _authHandler->sendNumericReply(client, 315, "End of WHO list");
        return true;
    }
    else if (msg.command == "MODE") {
        // Commande MODE basique pour l'utilisateur
        if (!msg.params.empty() && msg.params[0] == client->getNickname()) {
            _authHandler->sendNumericReply(client, 221, "+");
        }
        return true;
    }
    
    // Commande non reconnue
    _authHandler->sendNumericReply(client, 421, msg.command + " :Unknown command");
    return true;
}

// Traiter le buffer d'un client
void CommandParser::processClientBuffer(Client* client) {
    std::string message;
    while (!(message = client->extractMessage()).empty()) {
        if (!processMessage(client, message)) {
            // Si processMessage retourne false, le client doit être déconnecté
            break;
        }
    }
}

// Convertir en majuscules
std::string CommandParser::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}