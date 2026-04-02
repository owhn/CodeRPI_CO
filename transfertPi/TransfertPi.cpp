#include "TransfertPi.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

// Constructeur : initialise les attributs protégés
TransfertPi::TransfertPi(const std::string& ip, int port)
    : sock(-1), port(port), ip(ip) {}  

TransfertPi::~TransfertPi() {
    fermer();
}

bool TransfertPi::connecter() {
    // Crée un socket TCP (SOCK_STREAM) IPv4 (AF_INET)
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "[ERREUR] Création socket échouée" << std::endl;
        return false;
    }

    // Structure qui décrit l'adresse du serveur (PC)
    sockaddr_in adresse{};
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(port);           // htons : convertit le port en big-endian réseau
    inet_pton(AF_INET, ip.c_str(), &adresse.sin_addr); // convertit l'IP texte en binaire

    if (connect(sock, (sockaddr*)&adresse, sizeof(adresse)) < 0) {
        std::cerr << "[ERREUR] Connexion au PC échouée" << std::endl;
        return false;
    }

    std::cout << "[INFO] Connecté au PC " << ip << ":" << port << std::endl;
    return true;
}

void TransfertPi::fermer() {
    if (sock >= 0) {
        close(sock);
        sock = -1;
    }
}