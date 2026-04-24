#include "Borne.h"
#include "transfertPi/EmetteurCSV.h"
#include <iostream>
#include <csignal>
#include <thread>
#include <mutex>
#include <atomic>

std::atomic<bool> running(true); // Indique si la borne doit continuer à fonctionner
std::mutex mutexCSV;

//arrêt propre à la réception d'un signal d'interruption (SIGINT(Ctrl+C)) ou de terminaison (SIGTERM)
void gestionSignal(int signal) {
    std::cout << "\n[Main] Arrêt demandé..." << std::endl;
    running = false;
}

int main() {
    std::cout << "=== Borne Course d'Orientation ===" << std::endl;

    Borne borne(mutexCSV, running);
    std::signal(SIGINT, gestionSignal);
    std::signal(SIGTERM, gestionSignal);

    if (!borne.initialiser()) {
        std::cerr << "[Main] Échec initialisation, arrêt." << std::endl;
        return 1;
    }

    // Lance le transfert WiFi en arrière-plan
    EmetteurCSV emetteur("192.168.137.1", 8080, "borne_1", "/home/pi/borne_cpp/data.csv", mutexCSV);
    std::thread threadTransfert([&emetteur]() {
        while (running) {
            if (emetteur.connecter()) {
                emetteur.attendreEtEnvoyer();
                emetteur.fermer(); // Ferme la connexion après l'envoi pour libérer le file descriptor
            }
            else {
                std::this_thread::sleep_for(std::chrono::seconds(5)); // Attendre avant de réessayer
            }
        }
        emetteur.fermer(); // Assure la fermeture du socket à l'arrêt (au cas où)
    });
    threadTransfert.detach();

    borne.boucle(); // Bloquant — thread principal (sort quand running = false)

    threadTransfert.join(); // attend que le thread transfert se termine proprement
    return 0;
}
