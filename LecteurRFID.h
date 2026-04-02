#pragma once
#include "MFRC522.h"
#include <string>
#include <set>

class LecteurRFID {
public:
    LecteurRFID();
    ~LecteurRFID();

    bool        initialiser();                        // Init SPI + RC522
    std::string lireTag();                            // Retourne l'UID lu, "" si rien

    void        chargerBadgesAutorises(const std::string& fichier = "badges_autorises.csv");
    bool        estAutorise(const std::string& uid) const;

private:
    MFRC522               _mfrc;
    std::set<std::string> _badgesAutorises;
    bool                  _initialise;

    std::string           _uidVersString();
};
