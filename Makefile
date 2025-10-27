CXX = g++
CXXFLAGS = -std=c++17 -Isrc -O2 -Wall -Wextra
LDFLAGS = -lspdlog -lfmt -lboost_system -lboost_thread -lcurl -lssl -lcrypto -lpthread -lstdc++fs -lyaml-cpp
TARGET = mev-shield

SOURCES = src/main.cpp src/common/config_loader.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
