CXX      = g++
CXXFLAGS = -std=c++17 -Wall -pthread
LIBS     = -lwiringPi -lbcm2835

TARGET = borne
SRCS   = main.cpp Borne.cpp LecteurRFID.cpp GPS.cpp MFRC522.cpp \
         transfertPi/TransfertPi.cpp transfertPi/ClientTCP.cpp transfertPi/EmetteurCSV.cpp
OBJS   = $(SRCS:.cpp=.o) MFRC522.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Règle générique — s'applique à tout sauf MFRC522.cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Règle spécifique APRÈS la générique — make préfère toujours la plus spécifique
# MFRC522.o: MFRC522.cpp
# 	$(CXX) -std=c++11 -Wall -pthread -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean