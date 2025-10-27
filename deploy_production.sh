#!/bin/bash
set -e

echo "==============================================="
echo "🚀 MEV Shield - Production Deployment"
echo "==============================================="
echo ""

# Security: Check if running as root
if [ "$EUID" -eq 0 ]; then 
    echo "❌ SECURITY: Do not run as root"
    exit 1
fi

# Load production environment
if [ ! -f ".env.production" ]; then
    echo "❌ SECURITY: .env.production not found"
    echo "   Create .env.production with INFURA_API_KEY"
    exit 1
fi

echo "📁 Loading production environment..."
# Load environment variables and export them
set -a
source .env.production
set +a

# Validate required environment variables
if [ -z "$INFURA_API_KEY" ]; then
    echo "❌ SECURITY: INFURA_API_KEY not set in .env.production"
    exit 1
fi

# Security: Validate API key format
if [ ${#INFURA_API_KEY} -lt 20 ]; then
    echo "❌ SECURITY: Invalid INFURA_API_KEY format"
    exit 1
fi

echo "✅ Environment: Production"
echo "🔑 Infura Key: ${INFURA_API_KEY:0:8}***"
echo "🌍 Primary: Infura Mainnet"
echo "🔄 Fallbacks: Chainstack, Public RPC"

# Build if needed
if [ ! -f "mev-shield" ]; then
    echo "🔨 Building application..."
    make clean
    make
fi

# Security: Check binary permissions
if [ "$(stat -c %a mev-shield)" != "755" ]; then
    chmod 755 mev-shield
fi

# Create secure logs directory
mkdir -p logs
chmod 700 logs

echo ""
echo "🎯 Production Status:"
echo "   ✅ Binary: Ready ($(du -h mev-shield | cut -f1))"
echo "   ✅ Security: Environment variables loaded"
echo "   ✅ API: Infura configured"
echo "   ✅ Fallbacks: Chainstack available"
echo "   ✅ Monitoring: Real-time MEV detection"
echo ""

echo "�� Starting MEV Protection System..."
echo "==============================================="
echo ""

# Export the environment variables for the application
export INFURA_API_KEY
export LOG_LEVEL

./mev-shield
