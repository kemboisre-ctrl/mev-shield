#!/bin/bash

echo "Installing MEV Shield dependencies..."

# Update package manager
sudo apt-get update

# Install build tools
sudo apt-get install -y build-essential cmake pkg-config

# Install dependencies
sudo apt-get install -y libcurl4-openssl-dev libssl-dev

echo "Dependencies installed successfully!"