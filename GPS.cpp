#include "GPS.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cmath>

GPS::GPS(const std::string& port, int baudrate)
    : _port(port), _baudrate(baudrate), _fd(-1),
      _latitude(0.0), _longitude(0.0), _valide(false), _running(false)
{}

GPS::~GPS() {
    arreter();
}

void GPS::demarrer() {
    // Ouverture du port série
    _fd = open(_port.c_str(), O_RDONLY | O_NOCTTY);
    if (_fd < 0) {
        std::cerr << "[GPS] Impossible d'ouvrir " << _port << std::endl;
        return;
    }

    // Configuration port série
    struct termios tty;
    tcgetattr(_fd, &tty);
    cfsetispeed(&tty, B4800);
    tty.c_cflag = CS8 | CREAD | CLOCAL;
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    tcsetattr(_fd, TCSANOW, &tty);

    _running = true;
    _thread = std::thread(&GPS::_lireGPS, this);
    std::cout << "[GPS] Thread démarré sur " << _port << std::endl;
}

void GPS::arreter() {
    _running = false;
    if (_thread.joinable())
        _thread.join();
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
}

double GPS::getLatitude() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _latitude;
}

double GPS::getLongitude() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _longitude;
}

bool GPS::isValide() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _valide;
}

void GPS::_lireGPS() {
    std::string ligne;
    char c;

    while (_running) {
        if (read(_fd, &c, 1) == 1) {
            if (c == '\n') {
                if (ligne.find("$GPRMC") == 0 || ligne.find("$GNRMC") == 0)
                    _parserGPRMC(ligne);
                ligne.clear();
            } else if (c != '\r') {
                ligne += c;
            }
        }
    }
}

bool GPS::_parserGPRMC(const std::string& ligne) {
    // Format: $GPRMC,hhmmss,A,lat,N/S,lon,E/W,...
    std::vector<std::string> champs;
    std::stringstream ss(ligne);
    std::string champ;

    while (std::getline(ss, champ, ','))
        champs.push_back(champ);

    if (champs.size() < 7) return false;
    if (champs[2] != "A")  return false;  // A = données valides

    try {
        double lat = std::stod(champs[3]);
        double lon = std::stod(champs[5]);
        char   dirLat = champs[4][0];
        char   dirLon = champs[6][0];

        double latDec = _nmeaVersDecimal(lat, dirLat);
        double lonDec = _nmeaVersDecimal(lon, dirLon);

        std::lock_guard<std::mutex> lock(_mutex);
        _latitude  = latDec;
        _longitude = lonDec;
        _valide    = true;
    } catch (...) {
        return false;
    }

    return true;
}

double GPS::_nmeaVersDecimal(double valeur, char direction) {
    // NMEA : DDDMM.MMMM → degrés décimaux
    int    degres  = (int)(valeur / 100);
    double minutes = valeur - (degres * 100);
    double decimal = degres + (minutes / 60.0);

    if (direction == 'S' || direction == 'W')
        decimal = -decimal;

    return decimal;
}
