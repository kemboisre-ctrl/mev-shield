#!/bin/bash

echo "🔍 Verifying MEV Shield Configuration"
echo "===================================="

# Check if config file exists and has correct structure
if [ ! -f "config/config.yaml" ]; then
    echo "❌ config.yaml not found"
    exit 1
fi

echo "✅ config.yaml exists"

# Check for required sections
if grep -q "primary_provider:" config/config.yaml && grep -q "websocket_url:" config/config.yaml; then
    echo "✅ Configuration structure is correct"
else
    echo "❌ Configuration structure is missing required sections"
    exit 1
fi

# Check if WebSocket URL is properly set
WEBSOCKET_URL=$(grep "websocket_url:" config/config.yaml | head -1 | cut -d'"' -f2)
if [[ $WEBSOCKET_URL == wss://* ]]; then
    echo "✅ WebSocket URL is secure: $WEBSOCKET_URL"
else
    echo "❌ WebSocket URL is not secure: $WEBSOCKET_URL"
    exit 1
fi

# Check if API key is set (not placeholder)
if grep -q "c4ac605f5f484c73b8135c6f2ad20ec0" config/config.yaml; then
    echo "✅ API key is properly set"
else
    echo "❌ API key is not set or uses placeholder"
    exit 1
fi

echo ""
echo "🎉 Configuration verification passed!"
echo "🚀 Starting MEV Shield..."

# Set environment
export INFURA_API_KEY=c4ac605f5f484c73b8135c6f2ad20ec0

# Run the application
./build/mev_shield_final
