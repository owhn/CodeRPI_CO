#include "LecteurRFID.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

LecteurRFID::LecteurRFID() : _initialise(false) {}

LecteurRFID::~LecteurRFID() {}

bool LecteurRFID::initialiser() {
    _mfrc.PCD_Init();
    _initialise = true;
    std::cout << "[RFID] RC522 initialisé" << std::endl;
    return true;
}

std::string LecteurRFID::lireTag() {
    if (!_initialise) return "";

    // Vérifie si un tag est présent, non bloquant
    if (!_mfrc.PICC_IsNewCardPresent()) return "";
    if (!_mfrc.PICC_ReadCardSerial())   return "";

    return _uidVersString();
}

void LecteurRFID::chargerBadgesAutorises(const std::string& fichier) {
    _badgesAutorises.clear();

    std::ifstream f(fichier);
    if (!f.is_open()) {
        std::cerr << "[RFID] Fichier badges introuvable : " << fichier << std::endl;
        return;
    }

    std::string ligne;
    while (std::getline(f, ligne)) {
        std::stringstream ss(ligne);
        std::string uid;
        std::getline(ss, uid, ',');
        if (!uid.empty())
            _badgesAutorises.insert(uid);
    }

    std::cout << "[RFID] " << _badgesAutorises.size() << " badge(s) autorisé(s) chargé(s)" << std::endl;
}

bool LecteurRFID::estAutorise(const std::string& uid) const {
    return _badgesAutorises.count(uid) > 0;
}

std::string LecteurRFID::_uidVersString() {
    std::ostringstream oss;
    for (byte i = 0; i < _mfrc.uid.size; i++) {
        if (i > 0) oss << ":";
        oss << std::uppercase << std::hex
            << std::setw(2) << std::setfill('0')
            << (int)_mfrc.uid.uidByte[i];
    }
    return oss.str();
}
