#include "Client.hpp"
#include <unistd.h>

Client::Client(int fd) : _fd(fd), isDisconnected(false) {}

Client::~Client() 
{
    close(_fd);
}

int Client::getFd() const 
{
    return _fd;
}

std::string& Client::getBuffer() 
{
    return _buffer;
}

bool Client::getIsDisconnected() const 
{
    return isDisconnected;
}

void Client::setIsDisconnected(bool value) 
{
    isDisconnected = value;
}