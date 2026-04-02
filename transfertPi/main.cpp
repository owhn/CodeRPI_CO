#include "TransfertPi/EmetteurCSV.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    const std::string IP_PC    = "192.168.137.1";
    const int         PORT     = 8080;
    const std::string ID_BORNE = "borne_1";
    const std::string CSV_PATH = "/home/pi/data.csv";

    EmetteurCSV borne(IP_PC, PORT, ID_BORNE, CSV_PATH);

    // Tentatives de connexion en boucle (le PC peut ne pas être encore prêt)
    while (!borne.connecter()) {
        std::cout << "[INFO] Nouvelle tentative dans 5 secondes..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    // Boucle bloquante : attend SEND_DATA et envoie le CSV
    borne.attendreEtEnvoyer();

    return 0;
}