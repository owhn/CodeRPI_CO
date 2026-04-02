# borne_cpp — Project snapshot

This file aggregates the contents of the `borne_cpp` folder (source files, headers, CSVs, and Makefile).

## Files

- [badges_autorises.csv](badges_autorises.csv)
- [bcm2835.h](bcm2835.h)
- [borne](borne)
- [Borne.cpp](Borne.cpp)
- [Borne.h](Borne.h)
- [data.csv](data.csv)
- [GPS.cpp](GPS.cpp)
- [GPS.h](GPS.h)
- [LecteurRFID.cpp](LecteurRFID.cpp)
- [LecteurRFID.h](LecteurRFID.h)
- [main.cpp](main.cpp)
- [Makefile](Makefile)
- [MFRC522.cpp](MFRC522.cpp)
- [MFRC522.h](MFRC522.h)

---

## Notes

- `borne` is a compiled executable (binary) and is not included below.
- CSV files contain runtime data and authorized badges.
- Makefile builds the `borne` target using `g++` and links `wiringPi` and `bcm2835`.

---

## File contents

### badges_autorises.csv

```
F9:32:9A:E2
```

### data.csv

```
26-03-2026 10:56:36,F9:32:9A:E2,45.829688,1.251190
26-03-2026 11:02:07,F9:32:9A:E2,N/A,N/A
26-03-2026 11:02:21,F9:32:9A:E2,N/A,N/A
26-03-2026 11:02:23,F9:32:9A:E2,N/A,N/A
26-03-2026 11:03:43,F9:32:9A:E2,N/A,N/A
26-03-2026 11:04:40,F9:32:9A:E2,N/A,N/A
26-03-2026 11:04:52,F9:32:9A:E2,45.829003,1.249302
26-03-2026 11:04:55,F9:32:9A:E2,45.829003,1.249302
26-03-2026 11:04:56,F9:32:9A:E2,45.829003,1.249302
26-03-2026 11:04:58,F9:32:9A:E2,45.829003,1.249302
26-03-2026 11:05:54,F9:32:9A:E2,45.828995,1.248717
26-03-2026 11:05:59,F9:32:9A:E2,45.828937,1.248427
26-03-2026 11:06:00,F9:32:9A:E2,45.828920,1.248380
26-03-2026 11:06:02,F9:32:9A:E2,45.828920,1.248382
26-03-2026 11:06:03,F9:32:9A:E2,45.828915,1.248352
26-03-2026 11:13:06,F9:32:9A:E2,45.829108,1.248457
26-03-2026 11:13:07,F9:32:9A:E2,45.829107,1.248445
26-03-2026 11:13:08,F9:32:9A:E2,45.829098,1.248425
26-03-2026 11:19:17,F9:32:9A:E2,45.829283,1.248735
26-03-2026 11:19:58,F9:32:9A:E2,45.829287,1.248775
26-03-2026 11:20:58,F9:32:9A:E2,45.829227,1.248502
26-03-2026 11:21:45,F9:32:9A:E2,45.829088,1.248198
26-03-2026 11:22:50,F9:32:9A:E2,45.829088,1.248198
26-03-2026 11:34:17,F9:32:9A:E2,N/A,N/A
26-03-2026 11:37:58,F9:32:9A:E2,N/A,N/A
31-03-2026 15:11:07,F9:32:9A:E2,N/A,N/A
31-03-2026 15:11:08,F9:32:9A:E2,N/A,N/A
31-03-2026 15:18:59,F9:32:9A:E2,N/A,N/A
31-03-2026 15:19:01,F9:32:9A:E2,N/A,N/A
31-03-2026 15:19:50,F9:32:9A:E2,N/A,N/A
31-03-2026 15:23:05,F9:32:9A:E2,N/A,N/A
31-03-2026 15:23:10,F9:32:9A:E2,N/A,N/A
31-03-2026 15:23:11,F9:32:9A:E2,N/A,N/A
31-03-2026 17:21:57,F9:32:9A:E2,N/A,N/A
31-03-2026 17:26:09,F9:32:9A:E2,N/A,N/A
31-03-2026 17:26:26,F9:32:9A:E2,N/A,N/A
02-04-2026 08:17:24,F9:32:9A:E2,N/A,N/A
```

### Makefile

```
CXX     = g++
CXXFLAGS = -std=c++11 -Wall -pthread
LIBS    = -lwiringPi -lbcm2835

TARGET  = borne
SRCS    = main.cpp Borne.cpp LecteurRFID.cpp GPS.cpp MFRC522.cpp
OBJS    = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
```

### Borne.h

```
#pragma once
#include "LecteurRFID.h"
#include "GPS.h"
#include <string>

// Pins GPIO (BCM)
#define PIN_BUZZER 24
#define PIN_LED_V  17
#define PIN_LED_R  27

class Borne {
public:
    Borne();
    ~Borne();

    bool initialiser();   // Init GPIO + RFID + GPS
    void boucle();        // Boucle principale (bloquante)
    void arreter();       // Arrêt propre

private:
    LecteurRFID _rfid;
    GPS         _gps;
    bool        _running;

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
```

### Borne.cpp (excerpt)

```
[contents omitted for brevity — full file included in repository]
```

### GPS.h / GPS.cpp (excerpt)

```
[GPS reader: serial port reading NMEA $GPRMC sentences, parses lat/lon and exposes thread-safe getters]
```

### LecteurRFID.h / LecteurRFID.cpp (excerpt)

```
[Wrapper around MFRC522: initializes RC522, reads UID, loads authorized badges from CSV.]
```

### MFRC522.h / MFRC522.cpp

```
[Full MFRC522 library ported for Raspberry Pi (SPI + bcm2835). See files for implementation.]
```

---

If you want the README to include full verbatim content for every C/C++ source and header instead of the short excerpts above, tell me and I'll expand each section to the full file contents in this markdown.
