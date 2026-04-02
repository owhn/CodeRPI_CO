#pragma once
#include "LecteurRFID.h"
#include "GPS.h"
#include <string>
#include <mutex>

// Pins GPIO (BCM)
#define PIN_BUZZER 24
#define PIN_LED_V  17
#define PIN_LED_R  27

class Borne {
public:
    Borne(std::mutex& mutexCSV);
    ~Borne();

    bool initialiser();   // Init GPIO + RFID + GPS
    void boucle();        // Boucle principale (bloquante)
    void arreter();       // Arrêt propre

private:
    LecteurRFID _rfid;
    GPS         _gps;
    bool        _running;
    
    std::mutex& _mutexCSV;
    
    // GPIO
    void _initGPIO();
    void _bip(int nb, int dureeMs = 100);
    void _led(int pin, bool etat);

    // Enregistrement
    void _enregistrerPassage(const std::string& uid, double lat, double lon);
    void _logErreur(const std::string& msg);

    // Signaux visuels/sonores
    void _signalAutorise();
    void _signalRefuse();
};
