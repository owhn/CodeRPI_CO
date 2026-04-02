#include "EmetteurCSV.h"
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <string>

EmetteurCSV::EmetteurCSV(const std::string& ip, int port,
                         const std::string& identifiant,
                         const std::string& cheminCSV
                        ,std::mutex& mutexCSV)
    : ClientTCP(ip, port, identifiant), 
      cheminCSV(cheminCSV),
      _mutexCSV(mutexCSV) {}

void EmetteurCSV::attendreEtEnvoyer() {
    char buffer[256];

    while (true) {
        // recv() est bloquant : attend des données du PC
        // Retourne 0 si la connexion est fermée, -1 si erreur
        ssize_t recu = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (recu <= 0) {
            std::cout << "[INFO] Connexion fermée par le PC" << std::endl;
            break;
        }

        buffer[recu] = '\0';
        std::string message(buffer);

        if (message.find("SEND_DATA") != std::string::npos) {
            std::cout << "[INFO] SEND_DATA reçu, envoi du CSV..." << std::endl;
            envoyerCSV();
        }
    }
}

bool EmetteurCSV::envoyerCSV() {
    std::lock_guard<std::mutex> verrou(_mutexCSV);
    // Ouvre le fichier CSV en lecture
    std::ifstream fichier(cheminCSV);
    if (!fichier.is_open()) {
        std::cerr << "[ERREUR] Impossible d'ouvrir " << cheminCSV << std::endl;
        return false;
    }

    // Lit et envoie le fichier ligne par ligne
    std::string ligne;
    while (std::getline(fichier, ligne)) {
        ligne += "\n";
        send(sock, ligne.c_str(), ligne.size(), 0);
    }
    fichier.close();

    // Marqueur de fin de transmission
    std::string fin = "END_DATA\n";
    send(sock, fin.c_str(), fin.size(), 0);
    std::cout << "[INFO] CSV envoyé avec succès" << std::endl;
    return true;
}