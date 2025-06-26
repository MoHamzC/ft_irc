#include "Channel.hpp"
#include <algorithm>
#include <sstream>

Channel::Channel(const std::string& name) 
    : _name(name), _userLimit(0), _creationTime(time(NULL)) {
    // Le créateur devient automatiquement opérateur
}

Channel::~Channel() {}

// Getters
const std::string& Channel::getName() const { return _name; }
const std::string& Channel::getTopic() const { return _topic; }
const std::string& Channel::getKey() const { return _key; }
size_t Channel::getUserLimit() const { return _userLimit; }
size_t Channel::getMemberCount() const { return _members.size(); }

bool Channel::hasMode(char mode) const {
    return _modes.find(mode) != _modes.end();
}

// Membres
bool Channel::addMember(Client* client) {
    if (!client) return false;
    
    if (_members.find(client) != _members.end())
        return false; // Déjà membre
    
    _members.insert(client);
    return true;
}

void Channel::removeMember(Client* client) {
    if (!client) return;
    
    _members.erase(client);
    _operators.erase(client);
    _inviteList.erase(client);
}

bool Channel::isMember(Client* client) const {
    return _members.find(client) != _members.end();
}

bool Channel::isOperator(Client* client) const {
    return _operators.find(client) != _operators.end();
}

void Channel::addOperator(Client* client) {
    if (client && isMember(client)) {
        _operators.insert(client);
    }
}

void Channel::removeOperator(Client* client) {
    _operators.erase(client);
}

// Modes
void Channel::setMode(char mode, bool set) {
    if (set) {
        _modes.insert(mode);
    } else {
        _modes.erase(mode);
    }
}

void Channel::setKey(const std::string& key) {
    _key = key;
    if (key.empty()) {
        setMode('k', false);
    } else {
        setMode('k', true);
    }
}

void Channel::setUserLimit(size_t limit) {
    _userLimit = limit;
    if (limit == 0) {
        setMode('l', false);
    } else {
        setMode('l', true);
    }
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

// Invitations
void Channel::addInvite(Client* client) {
    if (client) {
        _inviteList.insert(client);
    }
}

void Channel::removeInvite(Client* client) {
    _inviteList.erase(client);
}

bool Channel::isInvited(Client* client) const {
    return _inviteList.find(client) != _inviteList.end();
}

// Broadcast
void Channel::broadcast(const std::string& message, Client* sender) {
    for (std::set<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it != sender) { // Ne pas renvoyer à l'expéditeur
            (*it)->sendMessage(message);
        }
    }
}

void Channel::broadcastToOperators(const std::string& message) {
    for (std::set<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) {
        (*it)->sendMessage(message);
    }
}

// Validation
bool Channel::canJoin(Client* client, const std::string& key) const {
    if (!client) return false;
    
    // Vérifier la limite d'utilisateurs
    if (hasMode('l') && _members.size() >= _userLimit) {
        return false;
    }
    
    // Vérifier le mode invite-only
    if (hasMode('i') && !isInvited(client)) {
        return false;
    }
    
    // Vérifier la clé
    if (hasMode('k') && key != _key) {
        return false;
    }
    
    return true;
}

bool Channel::canSpeak(Client* client) const {
    return isMember(client);
}

bool Channel::canChangeTopic(Client* client) const {
    if (!isMember(client)) return false;
    
    // Si le mode +t est activé, seuls les opérateurs peuvent changer le topic
    if (hasMode('t')) {
        return isOperator(client);
    }
    
    return true;
}

// Utilitaires
std::string Channel::getModeString() const {
    std::string modes = "+";
    
    for (std::set<char>::const_iterator it = _modes.begin(); it != _modes.end(); ++it) {
        modes += *it;
    }
    
    if (modes == "+") {
        return "";
    }
    
    return modes;
}

std::string Channel::getMembersList() const {
    std::string list;
    
    for (std::set<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
        if (!list.empty()) list += " ";
        
        if (isOperator(*it)) {
            list += "@";
        }
        list += (*it)->getNickname();
    }
    
    return list;
}

std::string Channel::getChannelInfo() const {
    std::ostringstream oss;
    oss << "Channel: " << _name;
    oss << " | Members: " << _members.size();
    oss << " | Modes: " << getModeString();
    if (!_topic.empty()) {
        oss << " | Topic: " << _topic;
    }
    return oss.str();
}
