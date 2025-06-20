#include "Client.hpp"
#include <sys/socket.h>
#include <algorithm>
#include <iostream>

// Constructeur
Client::Client(int fd) : _fd(fd), _state(CONNECTING), _passwordOk(false) {
    _connectionTime = time(NULL);
    _lastActivity = _connectionTime;
    _hostname = "localhost"; // À adapter selon votre configuration
}

// Destructeur
Client::~Client() {}

// Getters
int Client::getFd() const { return _fd; }
const std::string& Client::getNickname() const { return _nickname; }
const std::string& Client::getUsername() const { return _username; }
const std::string& Client::getRealname() const { return _realname; }
const std::string& Client::getHostname() const { return _hostname; }
const std::string& Client::getBuffer() const { return _buffer; }
ClientState Client::getState() const { return _state; }
bool Client::isPasswordOk() const { return _passwordOk; }
time_t Client::getLastActivity() const { return _lastActivity; }
time_t Client::getConnectionTime() const { return _connectionTime; }
const std::vector<std::string>& Client::getChannels() const { return _channels; }

bool Client::isRegistered() const {
    return _state == REGISTERED;
}

// Setters
void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}

void Client::setRealname(const std::string& realname) {
    _realname = realname;
}

void Client::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void Client::setState(ClientState state) {
    _state = state;
}

void Client::setPasswordOk(bool ok) {
    _passwordOk = ok;
}

void Client::updateLastActivity() {
    _lastActivity = time(NULL);
}

// Gestion du buffer
void Client::appendToBuffer(const std::string& data) {
    _buffer += data;
}

std::string Client::extractMessage() {
    size_t pos = _buffer.find("\r\n");
    if (pos == std::string::npos) {
        pos = _buffer.find("\n");
        if (pos == std::string::npos)
            return "";
    }
    
    std::string message = _buffer.substr(0, pos);
    _buffer.erase(0, pos + (pos < _buffer.length() - 1 && _buffer[pos + 1] == '\n' ? 2 : 1));
    return message;
}

void Client::clearBuffer() {
    _buffer.clear();
}

// Gestion des canaux
void Client::joinChannel(const std::string& channel) {
    if (!isInChannel(channel)) {
        _channels.push_back(channel);
    }
}

void Client::leaveChannel(const std::string& channel) {
    _channels.erase(std::remove(_channels.begin(), _channels.end(), channel), _channels.end());
}

bool Client::isInChannel(const std::string& channel) const {
    return std::find(_channels.begin(), _channels.end(), channel) != _channels.end();
}

// Utilitaires
std::string Client::getPrefix() const {
    return ":" + _nickname + "!" + _username + "@" + _hostname;
}

bool Client::isTimedOut(int timeout) const {
    return (time(NULL) - _lastActivity) > timeout;
}

void Client::sendMessage(const std::string& message) {
    std::string fullMessage = message + "\r\n";
    send(_fd, fullMessage.c_str(), fullMessage.length(), 0);
}