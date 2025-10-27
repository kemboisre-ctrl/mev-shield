#!/bin/bash
echo "🔨 Building MEV Shield..."

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo "✅ Build complete! Binary: ./build/mev_shield"
