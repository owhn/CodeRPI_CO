#pragma once
#include <string>
#include <mutex>

class TransfertPi {
protected:
    int sock;
    int port;
    std::string ip;

public:
    TransfertPi(const std::string& ip, int port);
    virtual ~TransfertPi();

    virtual bool connecter();
    void fermer();
};