#include "common/config_loader_fixed.hpp"
#include <fstream>
#include <iostream>

namespace mev_shield {

AppConfig AppConfig::load_from_file(const std::string& config_path) {
    std::ifstream config_file(config_path);
    if (!config_file) {
        throw std::runtime_error("Cannot open config file: " + config_path);
    }

    YAML::Node config = YAML::LoadFile(config_path);
    AppConfig app_config;

    // Load primary provider
    if (config["primary_provider"]) {
        auto primary = config["primary_provider"];
        app_config.primary_provider.name = primary["name"].as<std::string>();
        app_config.primary_provider.websocket_url = primary["websocket_url"].as<std::string>();
        app_config.primary_provider.priority = primary["priority"].as<int>();
    }

    // Load fallback providers
    if (config["fallback_providers"]) {
        for (const auto& provider : config["fallback_providers"]) {
            ProviderConfig fallback;
            fallback.name = provider["name"].as<std::string>();
            fallback.websocket_url = provider["websocket_url"].as<std::string>();
            fallback.priority = provider["priority"].as<int>();
            app_config.fallback_providers.push_back(fallback);
        }
    }

    // Load risk settings
    if (config["risk_settings"]) {
        auto risk = config["risk_settings"];
        app_config.risk_settings.min_profit_eth = risk["min_profit_eth"].as<double>();
        app_config.risk_settings.high_risk_slippage = risk["high_risk_slippage"].as<double>();
        app_config.risk_settings.max_simulation_time_ms = risk["max_simulation_time_ms"].as<int>();
        app_config.risk_settings.max_gas_price_gwei = risk["max_gas_price_gwei"].as<int>();
    }

    // Load API port
    if (config["api_port"]) {
        app_config.api_port = config["api_port"].as<int>();
    }

    return app_config;
}

} // namespace mev_shield
