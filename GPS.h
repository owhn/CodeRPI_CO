#pragma once
#include <string>
#include <thread>
#include <mutex>

class GPS {
public:
    GPS(const std::string& port = "/dev/ttyUSB0", int baudrate = 4800);
    ~GPS();

    void demarrer();               // Lance le thread de lecture GPS
    void arreter();                // Arrête le thread proprement

    double getLatitude() const;
    double getLongitude() const;
    bool   isValide() const;       // True si on a un fix GPS

private:
    std::string  _port;
    int          _baudrate;
    int          _fd;              // File descriptor port série

    double       _latitude;
    double       _longitude;
    bool         _valide;

    bool         _running;
    std::thread  _thread;
    mutable std::mutex _mutex;

    void         _lireGPS();       // Fonction du thread
    bool         _parserGPRMC(const std::string& ligne);
    double       _nmeaVersDecimal(double valeur, char direction);
};
