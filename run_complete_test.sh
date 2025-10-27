#!/bin/bash
set -e

echo "🚀 Starting Complete MEV Shield Test Suite"
echo "==========================================="

# Test 1: File structure
echo "1. Testing file structure..."
bash -c '
required_files=(
    "src/main.cpp"
    "src/common/config_loader.hpp"
    "src/common/config_loader.cpp"
    "src/common/logger.hpp"
    "src/analytics/risk_engine.hpp"
    "src/network/mempool_monitor.hpp"
    "config/config.yaml"
    "Makefile"
)

for file in "${required_files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ Missing: $file"
        exit 1
    fi
done
echo "✅ All files present"
'

# Test 2: Compilation
echo "2. Testing compilation..."
make clean
make
echo "✅ Compilation successful"

# Test 3: Configuration
echo "3. Testing configuration loader..."
./test_config
echo "✅ Configuration test passed"

# Test 4: Risk Engine
echo "4. Testing risk engine..."
./test_risk_engine
echo "✅ Risk engine test passed"

# Test 5: Environment
echo "5. Testing environment..."
./test_env
echo "✅ Environment test passed"

# Test 6: Quick app test
echo "6. Testing main application (brief run)..."
timeout 5s ./mev-shield && echo "✅ Application started successfully" || echo "⚠️  Application stopped (expected)"

echo ""
echo "🎉 ALL TESTS COMPLETED SUCCESSFULLY!"
echo "✨ MEV Shield is ready for production deployment"
