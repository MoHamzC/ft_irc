#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include <map>
#include <string>
#include <vector>

class Server; // Forward declaration

class ChannelManager {
private:
    std::map<std::string, Channel*> _channels;
    Server *_server;
    
public:
    ChannelManager(Server *server);
    ~ChannelManager();
    
    // Gestion des canaux
    Channel* createChannel(const std::string& name, Client* creator);
    Channel* getChannel(const std::string& name);
    void removeChannel(const std::string& name);
    bool channelExists(const std::string& name) const;
    
    // Validation
    bool isValidChannelName(const std::string& name) const;
    
    // Commandes IRC
    bool joinChannel(Client* client, const std::string& channelName, const std::string& key = "");
    bool partChannel(Client* client, const std::string& channelName, const std::string& reason = "");
    bool kickFromChannel(Client* kicker, const std::string& channelName, const std::string& target, const std::string& reason = "");
    bool inviteToChannel(Client* inviter, const std::string& channelName, const std::string& target);
    bool setChannelTopic(Client* client, const std::string& channelName, const std::string& topic);
    bool setChannelMode(Client* client, const std::string& channelName, const std::string& modeString, const std::vector<std::string>& params);
    
    // Messages
    bool sendToChannel(const std::string& channelName, const std::string& message, Client* sender);
    void broadcastQuit(Client* client, const std::string& reason);
    void broadcastNickChange(Client* client, const std::string& oldNick, const std::string& newNick);
    
    // Statistiques
    size_t getChannelCount() const;
    std::vector<std::string> getChannelList() const;
    std::vector<Channel*> getClientChannels(Client* client) const;
    
    // Nettoyage
    void removeClientFromAllChannels(Client* client);
    void cleanupEmptyChannels();
    
    // Getters
    const std::map<std::string, Channel*>& getChannels() const;
};

#endif
