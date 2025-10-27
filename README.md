# MEV Shield Core

Open source MEV protection system that monitors Ethereum mempool for front-running and sandwich attacks.

## Features

- 🔍 **Real-time monitoring** of Ethereum mempool
- ⚡ **Low-latency detection** of MEV opportunities  
- 🛡️ **Risk scoring** for transactions
- 📊 **Basic profit estimation** for arbitrage
- 🔧 **Easy configuration** with YAML

## Quick Start

### Prerequisites
- C++17 compiler
- CMake 3.15+
- libcurl, OpenSSL

### Installation

```bash
# 1. Install dependencies
./scripts/install_deps.sh

# 2. Build project
./scripts/build.sh

# 3. Configure (edit config/config.yaml)
cp config/config.yaml.example config/config.yaml

# 4. Run
./scripts/run.sh