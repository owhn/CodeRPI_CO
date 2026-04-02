#pragma once
#include "TransfertPi.h"

class ClientTCP : public TransfertPi {
protected:
    std::string identifiant;

public:
    ClientTCP(const std::string& ip, int port, const std::string& identifiant);

    bool connecter() override;         // se connecte ET envoie l'identifiant
    bool envoyerIdentifiant();
};