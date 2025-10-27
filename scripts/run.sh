#!/bin/bash

echo "Starting MEV Shield..."

# Check if binary exists
if [ ! -f "./build/mev_shield" ]; then
    echo "Error: Binary not found. Run ./scripts/build.sh first."
    exit 1
fi

# Create logs directory
mkdir -p logs

# Run MEV Shield
./build/mev_shield