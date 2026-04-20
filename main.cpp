#include "Borne.h"
#include "transfertPi/EmetteurCSV.h"
#include <iostream>
#include <csignal>
#include <thread>
#include <mutex>

Borne* g_borne = nullptr;
std::mutex mutexCSV;

//arrêt propre à la réception d'un signal d'interruption (SIGINT(Ctrl+C)) ou de terminaison (SIGTERM)
void gestionSignal(int signal) {
    std::cout << "\n[Main] Arrêt demandé..." << std::endl;
    if (g_borne)
        g_borne->arreter();
}

int main() {
    std::cout << "=== Borne Course d'Orientation ===" << std::endl;

    Borne borne(mutexCSV);
    g_borne = &borne;
    std::signal(SIGINT, gestionSignal);
    std::signal(SIGTERM, gestionSignal);

    if (!borne.initialiser()) {
        std::cerr << "[Main] Échec initialisation, arrêt." << std::endl;
        return 1;
    }

    // Lance le transfert WiFi en arrière-plan
    EmetteurCSV emetteur("192.168.137.1", 8080, "borne_1", "/home/pi/borne_cpp/data.csv", mutexCSV);
    std::thread threadTransfert([&emetteur]() {
        while (!emetteur.connecter()) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        emetteur.attendreEtEnvoyer();
    });
    threadTransfert.detach();

    borne.boucle(); // Bloquant — thread principal

    return 0;
}
