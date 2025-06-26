#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <ctime>

enum ClientState {
    CONNECTING,
    PASS_OK,
    NICK_OK,
    USER_OK,
    REGISTERED
};

class Client {
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    std::string _buffer;
    ClientState _state;
    bool _passwordOk;
    time_t _lastActivity;
    time_t _connectionTime;
    std::vector<std::string> _channels;

public:
    // Constructeurs et destructeur
    Client(int fd);
    ~Client();
    
    // Getters
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    const std::string& getBuffer() const;
    ClientState getState() const;
    bool isPasswordOk() const;
    bool isRegistered() const;
    time_t getLastActivity() const;
    time_t getConnectionTime() const;
    const std::vector<std::string>& getChannels() const;
    
    // Setters
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    void setState(ClientState state);
    void setPasswordOk(bool ok);
    void updateLastActivity();
    
    // Gestion du buffer
    void appendToBuffer(const std::string& data);
    std::string extractMessage();
    void clearBuffer();
    
    // Gestion des canaux
    void joinChannel(const std::string& channel);
    void leaveChannel(const std::string& channel);
    bool isInChannel(const std::string& channel) const;
    
    // Utilitaires
    std::string getPrefix() const; // :nick!user@host
    bool isTimedOut(int timeout) const;
    void sendMessage(const std::string& message);
};

#endif