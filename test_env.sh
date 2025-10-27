#!/bin/bash
echo "🧪 Testing Environment Variables..."
echo "INFURA_API_KEY: ${INFURA_API_KEY:0:8}***"
echo "LOG_LEVEL: $LOG_LEVEL"

if [ -z "$INFURA_API_KEY" ]; then
    echo "❌ INFURA_API_KEY not set"
else
    echo "✅ INFURA_API_KEY is set"
fi
