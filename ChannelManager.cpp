#include "ChannelManager.hpp"
#include "Server.hpp"
#include <algorithm>
#include <cstdlib>
#include <sstream>

ChannelManager::ChannelManager(Server *server) : _server(server) {}

ChannelManager::~ChannelManager() {
    // Nettoyer tous les canaux
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
}

// Validation des noms de canaux
bool ChannelManager::isValidChannelName(const std::string& name) const {
    if (name.empty() || name.length() > 50)
        return false;
    
    // Doit commencer par # ou &
    if (name[0] != '#' && name[0] != '&')
        return false;
    
    // Pas d'espaces, virgules, ou caractères de contrôle
    for (size_t i = 1; i < name.length(); i++) {
        char c = name[i];
        if (c <= 32 || c == ',' || c == '\x07') // espace, virgule, bell
            return false;
    }
    
    return true;
}

// Gestion des canaux
Channel* ChannelManager::createChannel(const std::string& name, Client* creator) {
    if (!isValidChannelName(name) || !creator)
        return NULL;
    
    if (channelExists(name))
        return getChannel(name);
    
    Channel* channel = new Channel(name);
    _channels[name] = channel;
    
    // Le créateur devient membre et opérateur
    channel->addMember(creator);
    channel->addOperator(creator);
    
    return channel;
}

Channel* ChannelManager::getChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    return (it != _channels.end()) ? it->second : NULL;
}

void ChannelManager::removeChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
    }
}

bool ChannelManager::channelExists(const std::string& name) const {
    return _channels.find(name) != _channels.end();
}

// Commandes IRC
bool ChannelManager::joinChannel(Client* client, const std::string& channelName, const std::string& key) {
    if (!client || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    
    // Créer le canal s'il n'existe pas
    if (!channel) {
        channel = createChannel(channelName, client);
        if (!channel) return false;
        
        // Envoyer confirmation de JOIN
        std::string joinMsg = client->getPrefix() + " JOIN :" + channelName;
        client->sendMessage(joinMsg);
        
        // Envoyer topic si défini
        if (!channel->getTopic().empty()) {
            std::string topicMsg = ":ft_irc.42.fr 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic();
            client->sendMessage(topicMsg);
        }
        
        // Envoyer liste des utilisateurs (NAMES)
        std::string namesList = channel->getMembersList();
        std::string namesMsg = ":ft_irc.42.fr 353 " + client->getNickname() + " = " + channelName + " :" + namesList;
        client->sendMessage(namesMsg);
        
        std::string endNamesMsg = ":ft_irc.42.fr 366 " + client->getNickname() + " " + channelName + " :End of NAMES list";
        client->sendMessage(endNamesMsg);
        
        return true;
    }
    
    // Vérifier si on peut rejoindre
    if (!channel->canJoin(client, key))
        return false;
    
    // Ajouter le client au canal
    if (!channel->addMember(client))
        return false;
    
    // Notifier tous les membres du JOIN
    std::string joinMsg = client->getPrefix() + " JOIN :" + channelName;
    channel->broadcast(joinMsg, NULL); // Envoyer à tous y compris le client
    client->sendMessage(joinMsg);
    
    // Envoyer topic si défini
    if (!channel->getTopic().empty()) {
        std::string topicMsg = ":ft_irc.42.fr 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic();
        client->sendMessage(topicMsg);
    }
    
    // Envoyer liste des utilisateurs (NAMES)
    std::string namesList = channel->getMembersList();
    std::string namesMsg = ":ft_irc.42.fr 353 " + client->getNickname() + " = " + channelName + " :" + namesList;
    client->sendMessage(namesMsg);
    
    std::string endNamesMsg = ":ft_irc.42.fr 366 " + client->getNickname() + " " + channelName + " :End of NAMES list";
    client->sendMessage(endNamesMsg);
    
    return true;
}

bool ChannelManager::partChannel(Client* client, const std::string& channelName, const std::string& reason) {
    if (!client || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    if (!channel || !channel->isMember(client))
        return false;
    
    // Notifier le PART
    std::string partMsg = client->getPrefix() + " PART " + channelName;
    if (!reason.empty()) {
        partMsg += " :" + reason;
    }
    
    channel->broadcast(partMsg, NULL); // À tous y compris le client
    client->sendMessage(partMsg);
    
    // Supprimer du canal
    channel->removeMember(client);
    
    // Supprimer le canal s'il est vide
    if (channel->getMemberCount() == 0) {
        removeChannel(channelName);
    }
    
    return true;
}

bool ChannelManager::sendToChannel(const std::string& channelName, const std::string& message, Client* sender) {
    if (!sender || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    if (!channel || !channel->canSpeak(sender))
        return false;
    
    channel->broadcast(message, sender);
    return true;
}

// Méthodes de base (stubs pour compilation)
bool ChannelManager::kickFromChannel(Client* kicker, const std::string& channelName, const std::string& targetNick, const std::string& reason) {
    if (!kicker || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    if (!channel)
        return false;
    
    // Vérifier que le kicker est opérateur
    if (!channel->isOperator(kicker))
        return false;
    
    // Trouver le client cible
    Client* target = NULL;
    for (std::map<int, Client*>::const_iterator it = _server->getClientManager()->getClients().begin(); 
         it != _server->getClientManager()->getClients().end(); ++it) {
        if (it->second->getNickname() == targetNick && it->second->isRegistered()) {
            target = it->second;
            break;
        }
    }
    
    if (!target || !channel->isMember(target))
        return false;
    
    // Envoyer le message KICK
    std::string kickMsg = kicker->getPrefix() + " KICK " + channelName + " " + targetNick + " :" + reason;
    channel->broadcast(kickMsg, NULL); // À tous y compris le target
    target->sendMessage(kickMsg);
    
    // Retirer du canal
    channel->removeMember(target);
    
    return true;
}

bool ChannelManager::inviteToChannel(Client* inviter, const std::string& channelName, const std::string& targetNick) {
    if (!inviter || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    if (!channel)
        return false;
    
    // Vérifier que l'inviter est dans le canal
    if (!channel->isMember(inviter))
        return false;
    
    // Si le canal est en mode +i, seuls les opérateurs peuvent inviter
    if (channel->hasMode('i') && !channel->isOperator(inviter))
        return false;
    
    // Trouver le client cible
    Client* target = NULL;
    for (std::map<int, Client*>::const_iterator it = _server->getClientManager()->getClients().begin(); 
         it != _server->getClientManager()->getClients().end(); ++it) {
        if (it->second->getNickname() == targetNick && it->second->isRegistered()) {
            target = it->second;
            break;
        }
    }
    
    if (!target)
        return false;
    
    // Vérifier que la cible n'est pas déjà dans le canal
    if (channel->isMember(target))
        return false;
    
    // Ajouter à la liste d'invitation
    channel->addInvite(target);
    
    // Notifier l'inviter
    std::string replyMsg = ":ft_irc.42.fr 341 " + inviter->getNickname() + " " + targetNick + " " + channelName;
    inviter->sendMessage(replyMsg);
    
    // Notifier le target
    std::string inviteMsg = inviter->getPrefix() + " INVITE " + targetNick + " :" + channelName;
    target->sendMessage(inviteMsg);
    
    return true;
}

bool ChannelManager::setChannelTopic(Client* client, const std::string& channelName, const std::string& topic) {
    if (!client || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    if (!channel)
        return false;
    
    // Vérifier que le client est dans le canal
    if (!channel->isMember(client))
        return false;
    
    // Si pas de topic fourni, afficher le topic actuel
    if (topic.empty()) {
        if (channel->getTopic().empty()) {
            std::string replyMsg = ":ft_irc.42.fr 331 " + client->getNickname() + " " + channelName + " :No topic is set";
            client->sendMessage(replyMsg);
        } else {
            std::string replyMsg = ":ft_irc.42.fr 332 " + client->getNickname() + " " + channelName + " :" + channel->getTopic();
            client->sendMessage(replyMsg);
        }
        return true;
    }
    
    // Vérifier les permissions pour changer le topic
    if (!channel->canChangeTopic(client))
        return false;
    
    // Changer le topic
    channel->setTopic(topic);
    
    // Notifier tous les membres
    std::string topicMsg = client->getPrefix() + " TOPIC " + channelName + " :" + topic;
    channel->broadcast(topicMsg, NULL); // À tous y compris le client
    client->sendMessage(topicMsg);
    
    return true;
}

bool ChannelManager::setChannelMode(Client* client, const std::string& channelName, const std::string& modeString, const std::vector<std::string>& params) {
    if (!client || !isValidChannelName(channelName))
        return false;
    
    Channel* channel = getChannel(channelName);
    if (!channel)
        return false;
    
    // Vérifier que le client est opérateur
    if (!channel->isOperator(client))
        return false;
    
    // Si pas de mode fourni, afficher les modes actuels
    if (modeString.empty()) {
        std::string modeStr = channel->getModeString();
        if (modeStr.empty()) modeStr = "+";
        std::string replyMsg = ":ft_irc.42.fr 324 " + client->getNickname() + " " + channelName + " " + modeStr;
        client->sendMessage(replyMsg);
        return true;
    }
    
    bool adding = true;
    size_t paramIndex = 0;
    std::string appliedModes;
    std::string appliedParams;
    
    for (size_t i = 0; i < modeString.length(); i++) {
        char c = modeString[i];
        
        if (c == '+') {
            adding = true;
            continue;
        } else if (c == '-') {
            adding = false;
            continue;
        }
        
        // Traiter les modes
        switch (c) {
            case 'i': // invite-only
                channel->setMode('i', adding);
                appliedModes += adding ? "+" : "-";
                appliedModes += "i";
                break;
                
            case 't': // topic restriction
                channel->setMode('t', adding);
                appliedModes += adding ? "+" : "-";
                appliedModes += "t";
                break;
                
            case 'k': // channel key
                if (adding && paramIndex < params.size()) {
                    channel->setKey(params[paramIndex++]);
                    appliedModes += "+k";
                    appliedParams += " " + channel->getKey();
                } else if (!adding) {
                    channel->setKey("");
                    appliedModes += "-k";
                }
                break;
                
            case 'l': // user limit
                if (adding && paramIndex < params.size()) {
                    int limit = std::atoi(params[paramIndex++].c_str());
                    if (limit > 0) {
                        channel->setUserLimit(limit);
                        appliedModes += "+l";
                        std::ostringstream oss;
                        oss << " " << limit;
                        appliedParams += oss.str();
                    }
                } else if (!adding) {
                    channel->setUserLimit(0);
                    appliedModes += "-l";
                }
                break;
                
            case 'o': // operator
                if (paramIndex < params.size()) {
                    std::string targetNick = params[paramIndex++];
                    Client* target = NULL;
                    
                    // Trouver le client cible
                    for (std::map<int, Client*>::const_iterator it = _server->getClientManager()->getClients().begin(); 
                         it != _server->getClientManager()->getClients().end(); ++it) {
                        if (it->second->getNickname() == targetNick && it->second->isRegistered()) {
                            target = it->second;
                            break;
                        }
                    }
                    
                    if (target && channel->isMember(target)) {
                        if (adding) {
                            channel->addOperator(target);
                        } else {
                            channel->removeOperator(target);
                        }
                        appliedModes += adding ? "+o" : "-o";
                        appliedParams += " " + targetNick;
                    }
                }
                break;
        }
    }
    
    // Notifier tous les membres du changement de mode
    if (!appliedModes.empty()) {
        std::string modeMsg = client->getPrefix() + " MODE " + channelName + " " + appliedModes + appliedParams;
        channel->broadcast(modeMsg, NULL);
        client->sendMessage(modeMsg);
    }
    
    return true;
}

void ChannelManager::broadcastQuit(Client* client, const std::string& reason) {
    if (!client) return;
    
    std::string quitMsg = client->getPrefix() + " QUIT :" + reason;
    
    // Envoyer le QUIT à tous les canaux où le client est membre
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->isMember(client)) {
            it->second->broadcast(quitMsg, client); // Ne pas renvoyer au client qui quit
        }
    }
}

void ChannelManager::broadcastNickChange(Client* client, const std::string& oldNick, const std::string& newNick) {
    if (!client) return;
    
    std::string nickMsg = ":" + oldNick + "!" + client->getUsername() + "@" + client->getHostname() + " NICK :" + newNick;
    
    // Envoyer le changement de nick à tous les canaux où le client est membre
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->isMember(client)) {
            it->second->broadcast(nickMsg, NULL); // À tous y compris le client
        }
    }
    
    // Envoyer aussi au client lui-même
    client->sendMessage(nickMsg);
}

// Statistiques et utilitaires
size_t ChannelManager::getChannelCount() const {
    return _channels.size();
}

std::vector<std::string> ChannelManager::getChannelList() const {
    std::vector<std::string> list;
    for (std::map<std::string, Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
        list.push_back(it->first);
    }
    return list;
}

std::vector<Channel*> ChannelManager::getClientChannels(Client* client) const {
    std::vector<Channel*> clientChannels;
    
    for (std::map<std::string, Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->isMember(client)) {
            clientChannels.push_back(it->second);
        }
    }
    
    return clientChannels;
}

void ChannelManager::removeClientFromAllChannels(Client* client) {
    if (!client) return;
    
    std::vector<std::string> toRemove;
    
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->isMember(client)) {
            it->second->removeMember(client);
            
            // Marquer pour suppression si vide
            if (it->second->getMemberCount() == 0) {
                toRemove.push_back(it->first);
            }
        }
    }
    
    // Supprimer les canaux vides
    for (size_t i = 0; i < toRemove.size(); ++i) {
        removeChannel(toRemove[i]);
    }
}

void ChannelManager::cleanupEmptyChannels() {
    std::vector<std::string> toRemove;
    
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->getMemberCount() == 0) {
            toRemove.push_back(it->first);
        }
    }
    
    for (size_t i = 0; i < toRemove.size(); ++i) {
        removeChannel(toRemove[i]);
    }
}

const std::map<std::string, Channel*>& ChannelManager::getChannels() const {
    return _channels;
}
