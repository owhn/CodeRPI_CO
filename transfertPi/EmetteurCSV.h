#pragma once
#include "ClientTCP.h"

class EmetteurCSV : public ClientTCP {
protected:
    std::string cheminCSV;

public:
    EmetteurCSV(const std::string& ip, int port,
                const std::string& identifiant,
                const std::string& cheminCSV
                ,std::mutex& mutexCSV);

    void attendreEtEnvoyer();   // boucle principale : attend SEND_DATA puis envoie
private:
    std::mutex& _mutexCSV;
    bool envoyerCSV();
};