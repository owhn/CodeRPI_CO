#include "ClientTCP.h"
#include <sys/socket.h>
#include <iostream>

ClientTCP::ClientTCP(const std::string& ip, int port, const std::string& identifiant)
    : TransfertPi(ip, port), identifiant(identifiant) {}

bool ClientTCP::connecter() {
    // Appel de la méthode parente pour créer le socket et se connecter
    if (!TransfertPi::connecter()) return false;
    return envoyerIdentifiant();
}

bool ClientTCP::envoyerIdentifiant() {
    // On ajoute '\n' pour que le PC puisse délimiter la fin de l'identifiant
    std::string msg = identifiant + "\n";
    ssize_t envoye = send(sock, msg.c_str(), msg.size(), 0);
    if (envoye < 0) {
        std::cerr << "[ERREUR] Envoi identifiant échoué" << std::endl;
        return false;
    }
    std::cout << "[INFO] Identifiant envoyé : " << identifiant << std::endl;
    return true;
}