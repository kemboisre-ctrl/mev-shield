#!/bin/bash
echo "🚀 MEV Shield Quick Start"
echo "========================="

# Check prerequisites
if ! command -v cmake &> /dev/null; then
    echo "Installing dependencies..."
    sudo apt update
    sudo apt install -y build-essential cmake libcurl4-openssl-dev libssl-dev
fi

# Build
echo "Building MEV Shield..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "✅ Build complete!"
echo "🚀 Starting MEV Shield..."
./mev_shield --api --port 8765
