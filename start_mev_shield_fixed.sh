#!/bin/bash

echo "🛡️ MEV Shield - Fixed Configuration"
echo "==================================="

# Check if config files exist and are properly configured
if [ ! -f "config/config.yaml" ]; then
    echo "❌ config.yaml not found. Creating default config..."
    cat > config/config.yaml << 'CONFIGEOF'
infura_api_key: "c4ac605f5f484c73b8135c6f2ad20ec0"
websocket_url: "wss://mainnet.infura.io/ws/v3/c4ac605f5f484c73b8135c6f2ad20ec0"
min_profit_threshold: 0.01
max_slippage_percent: 3.0
log_level: "info"
CONFIGEOF
fi

# Verify the config has actual API key (not placeholder)
if grep -q "YOUR_INFURA_API_KEY_HERE" config/config.yaml || grep -q "\${INFURA_API_KEY}" config/config.yaml; then
    echo "❌ Configuration contains placeholders. Fixing..."
    sed -i 's/YOUR_INFURA_API_KEY_HERE/c4ac605f5f484c73b8135c6f2ad20ec0/g' config/config.yaml
    sed -i 's/\${INFURA_API_KEY}/c4ac605f5f484c73b8135c6f2ad20ec0/g' config/config.yaml
fi

# Set environment
export INFURA_API_KEY=c4ac605f5f484c73b8135c6f2ad20ec0

echo "✅ Configuration verified"
echo "�� Using secure WebSocket endpoint"
echo "🚀 Starting MEV Shield..."

./build/mev_shield
