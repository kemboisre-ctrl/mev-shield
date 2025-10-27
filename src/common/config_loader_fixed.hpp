#pragma once

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace mev_shield {

struct ProviderConfig {
    std::string name;
    std::string websocket_url;
    int priority;
};

struct RiskSettings {
    double min_profit_eth = 0.01;
    double high_risk_slippage = 3.0;
    int max_simulation_time_ms = 1000;
    int max_gas_price_gwei = 150;
};

struct AppConfig {
    ProviderConfig primary_provider;
    std::vector<ProviderConfig> fallback_providers;
    RiskSettings risk_settings;
    std::vector<std::string> supported_dexs;
    std::vector<std::string> supported_tokens;
    int api_port = 8765;
    int max_connections = 100;
    std::string log_level = "info";

    static AppConfig load_from_file(const std::string& config_path);
};

} // namespace mev_shield
