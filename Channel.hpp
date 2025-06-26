#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <string>
#include <set>
#include <map>

class Channel {
private:
    std::string _name;
    std::string _topic;
    std::string _key;           // Mode +k
    std::set<Client*> _members;
    std::set<Client*> _operators;
    std::set<char> _modes;      // i, t, k, o, l
    size_t _userLimit;          // Mode +l
    std::set<Client*> _inviteList; // Mode +i
    time_t _creationTime;

public:
    Channel(const std::string& name);
    ~Channel();
    
    // Getters
    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::string& getKey() const;
    size_t getUserLimit() const;
    size_t getMemberCount() const;
    bool hasMode(char mode) const;
    
    // Membres
    bool addMember(Client* client);
    void removeMember(Client* client);
    bool isMember(Client* client) const;
    bool isOperator(Client* client) const;
    void addOperator(Client* client);
    void removeOperator(Client* client);
    
    // Modes
    void setMode(char mode, bool set);
    void setKey(const std::string& key);
    void setUserLimit(size_t limit);
    void setTopic(const std::string& topic);
    
    // Invitations (mode +i)
    void addInvite(Client* client);
    void removeInvite(Client* client);
    bool isInvited(Client* client) const;
    
    // Broadcast
    void broadcast(const std::string& message, Client* sender = NULL);
    void broadcastToOperators(const std::string& message);
    
    // Validation
    bool canJoin(Client* client, const std::string& key = "") const;
    bool canSpeak(Client* client) const;
    bool canChangeTopic(Client* client) const;
    
    // Utilitaires
    std::string getModeString() const;
    std::string getMembersList() const;
    std::string getChannelInfo() const;
};

#endif
