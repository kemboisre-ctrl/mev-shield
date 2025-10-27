#pragma once
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace mev_shield {

struct RPCProvider {
    std::string name;
    std::string websocket_url;
    std::string http_url;
    int timeout_ms = 5000;
};

struct RiskEngineConfig {
    double min_profit_threshold_eth = 0.01;
    double high_risk_slippage_percent = 3.0;
    int max_simulation_time_ms = 1000;
    int max_gas_price_gwei = 150;
};

struct APIConfig {
    int port = 8765;
    int max_connections = 100;
};

struct DEXRouters {
    std::string uniswap_v2;
    std::string uniswap_v3;
    std::string sushiswap;
};

struct TokenAddresses {
    std::string weth;
    std::string dai;
    std::string usdc;
    std::string usdt;
};

struct AppConfig {
    RPCProvider primary_provider;
    std::vector<RPCProvider> fallback_providers;
    RiskEngineConfig risk_engine;
    APIConfig api;
    DEXRouters dex_routers;
    TokenAddresses tokens;

    static AppConfig load_from_file(const std::string& config_path = "config/config.yaml");
};

} // namespace mev_shield
