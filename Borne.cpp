#include "Borne.h"
#include <wiringPi.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>

Borne::Borne(std::mutex& mutexCSV, std::atomic<bool>& running) : _mutexCSV(mutexCSV), _running(running) {}

Borne::~Borne() {
    arreter();
}

bool Borne::initialiser() {
    // Init wiringPi
    if (wiringPiSetupGpio() < 0) {
        std::cerr << "[Borne] Erreur init GPIO" << std::endl;
        return false;
    }
    _initGPIO();

    // Init RFID
    if (!_rfid.initialiser()) return false;
    _rfid.chargerBadgesAutorises("badges_autorises.csv");

    // Init GPS
    _gps.demarrer();

    std::cout << "[Borne] Prête." << std::endl;
    return true;
}

void Borne::boucle() {
    while (_running) {
        try {
            std::string uid = _rfid.lireTag();

            if (!uid.empty()) {
                double lat = _gps.isValide() ? _gps.getLatitude()  : 0.0;
                double lon = _gps.isValide() ? _gps.getLongitude() : 0.0;

                if (_rfid.estAutorise(uid)) {
                    _enregistrerPassage(uid, lat, lon);
                    _signalAutorise();
                   std::cout << "[Borne] Passage enregistré : " << uid << " | GPS: " << lat << ", " << lon << std::endl;
                } else {
                    _signalRefuse();
                    std::cout << "[Borne] Badge refusé : " << uid << std::endl;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));

        } catch (const std::exception& e) {
            _logErreur(e.what());
            _signalRefuse();
        }
    }
}

void Borne::arreter() {
    _running = false;
    _gps.arreter();
}

// GPIO 

void Borne::_initGPIO() {
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_LED_V,  OUTPUT);
    pinMode(PIN_LED_R,  OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_LED_V,  LOW);
    digitalWrite(PIN_LED_R,  LOW);
}

void Borne::_bip(int nb, int dureeMs) {
    for (int i = 0; i < nb; i++) {
        digitalWrite(PIN_BUZZER, HIGH);
        std::this_thread::sleep_for(std::chrono::milliseconds(dureeMs));
        digitalWrite(PIN_BUZZER, LOW);
        if (i < nb - 1)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Borne::_led(int pin, bool etat) {
    digitalWrite(pin, etat ? HIGH : LOW);
}

// Signaux

void Borne::_signalAutorise() {
    _led(PIN_LED_V, true);
    _bip(1, 100);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    _led(PIN_LED_V, false);
}

void Borne::_signalRefuse() {
    _led(PIN_LED_R, true);
    _bip(3, 100);
    _led(PIN_LED_R, false);
}

// Enregistrement 

void Borne::_enregistrerPassage(const std::string& uid, double lat, double lon) {
    // Horodatage
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", std::localtime(&t));

    // Coordonnées GPS : "N/A" si pas de fix
    std::string sLat = (lat == 0.0 && lon == 0.0 && !_gps.isValide()) ? "N/A" : std::to_string(lat);
    std::string sLon = (lat == 0.0 && lon == 0.0 && !_gps.isValide()) ? "N/A" : std::to_string(lon);

    // Écriture CSV
    std::lock_guard<std::mutex> verrou(_mutexCSV);
    std::ofstream f("data.csv", std::ios::app);
    if (f.is_open()) {
        f << buf << "," << uid << "," << sLat << "," << sLon << "\n";
    } else {
        _logErreur("Impossible d'ouvrir data.csv");
    }
}

void Borne::_logErreur(const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", std::localtime(&t));

    std::ofstream f("logs.txt", std::ios::app);
    if (f.is_open())
        f << buf << " - Erreur : " << msg << "\n";
}
