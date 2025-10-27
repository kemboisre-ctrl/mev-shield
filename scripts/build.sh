#!/bin/bash
echo "🛡️ Building MEV Shield..."
mkdir -p build
cd build
g++ -std=c++17 -Isrc -O2 -Wall -Wextra \
    ../src/main.cpp ../src/common/config_loader.cpp \
    -o mev-shield \
    -lspdlog -lfmt -lboost_system -lboost_thread \
    -lcurl -lssl -lcrypto -lpthread -lstdc++fs -lyaml-cpp
echo "✅ Build complete! Run with: ./build/mev-shield"
