CXX = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = Transfert
SRCS = main.cpp TransfertPi.cpp ClientTCP.cpp EmetteurCSV.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)