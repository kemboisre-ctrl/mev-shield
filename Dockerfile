FROM ubuntu:22.04

# Install dependencies
RUN apt update && apt install -y \
    build-essential \
    libssl-dev \
    libyaml-cpp-dev \
    libboost-system-dev \
    g++ \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy only the necessary files
COPY mev_shield_single.cpp .
COPY config/ config/

# Compile directly without CMake
RUN g++ -std=c++17 -O2 -o mev_shield mev_shield_single.cpp \
    -lyaml-cpp -lssl -lcrypto -lboost_system -lpthread

CMD ["./mev_shield"]
