#pragma once
#include <string>

class Client {
public:
    Client(int fd);
    ~Client();

    int getFd() const;
    std::string& getBuffer();

    bool getIsDisconnected() const;
    void setIsDisconnected(bool value);

private:
    int _fd;
    bool isDisconnected;
    std::string _buffer;
};