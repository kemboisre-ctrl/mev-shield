#include "config_loader.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <regex>

namespace mev_shield {

std::string replace_env_variables(const std::string& input) {
    std::string result = input;
    std::regex env_regex("\\$\\{([^}]+)\\}");
    std::smatch match;
    
    while (std::regex_search(result, match, env_regex)) {
        std::string env_var = match[1].str();
        const char* env_value = std::getenv(env_var.c_str());
        
        // Validate environment variable
        if (env_value == nullptr || std::string(env_value).empty()) {
            std::cout << "❌ Environment variable '" << env_var << "' is not set" << std::endl;
            std::cout << "💡 Make sure .env.production exists and contains INFURA_API_KEY" << std::endl;
            throw std::runtime_error("Required environment variable not set: " + env_var);
        }
        
        // Security: Validate API key format
        if (env_var == "INFURA_API_KEY") {
            std::string key = env_value;
            if (key.length() < 20) {
                throw std::runtime_error("Invalid INFURA_API_KEY format - key too short");
            }
        }
        
        result.replace(match.position(0), match.length(0), env_value);
    }
    
    return result;
}

AppConfig AppConfig::load_from_file(const std::string& config_path) {
    AppConfig config;
    
    try {
        YAML::Node yaml_config = YAML::LoadFile(config_path);
        
        // Primary RPC Configuration (Infura)
        if (yaml_config["rpc"] && yaml_config["rpc"]["ethereum"]) {
            auto eth_node = yaml_config["rpc"]["ethereum"];
            config.primary_provider.name = "infura";
            config.primary_provider.websocket_url = replace_env_variables(
                eth_node["websocket_url"].as<std::string>());
            config.primary_provider.http_url = replace_env_variables(
                eth_node["http_url"].as<std::string>());
            config.primary_provider.timeout_ms = eth_node["timeout_ms"].as<int>();
        }
        
        // Fallback providers
        if (yaml_config["rpc"] && yaml_config["rpc"]["fallback_providers"]) {
            for (const auto& provider_node : yaml_config["rpc"]["fallback_providers"]) {
                RPCProvider provider;
                provider.name = provider_node["name"].as<std::string>();
                provider.websocket_url = provider_node["websocket_url"].as<std::string>();
                provider.http_url = provider_node["http_url"].as<std::string>();
                
                if (provider_node["timeout_ms"]) {
                    provider.timeout_ms = provider_node["timeout_ms"].as<int>();
                }
                
                config.fallback_providers.push_back(provider);
            }
        }
        
        // Risk Engine Configuration
        if (yaml_config["analytics"] && yaml_config["analytics"]["risk_engine"]) {
            auto risk_node = yaml_config["analytics"]["risk_engine"];
            config.risk_engine.min_profit_threshold_eth = 
                risk_node["min_profit_threshold_eth"].as<double>();
            config.risk_engine.high_risk_slippage_percent = 
                risk_node["high_risk_slippage_percent"].as<double>();
            config.risk_engine.max_simulation_time_ms = 
                risk_node["max_simulation_time_ms"].as<int>();
            
            if (risk_node["max_gas_price_gwei"]) {
                config.risk_engine.max_gas_price_gwei = 
                    risk_node["max_gas_price_gwei"].as<int>();
            }
        }
        
        // API Configuration
        if (yaml_config["api"]) {
            config.api.port = yaml_config["api"]["port"].as<int>();
            config.api.max_connections = yaml_config["api"]["max_connections"].as<int>();
        }
        
        // DEX Configuration
        if (yaml_config["dex"] && yaml_config["dex"]["routers"]) {
            auto routers_node = yaml_config["dex"]["routers"];
            config.dex_routers.uniswap_v2 = routers_node["uniswap_v2"].as<std::string>();
            config.dex_routers.uniswap_v3 = routers_node["uniswap_v3"].as<std::string>();
            config.dex_routers.sushiswap = routers_node["sushiswap"].as<std::string>();
        }
        
        // Tokens Configuration
        if (yaml_config["tokens"]) {
            config.tokens.weth = yaml_config["tokens"]["weth"].as<std::string>();
            config.tokens.dai = yaml_config["tokens"]["dai"].as<std::string>();
            config.tokens.usdc = yaml_config["tokens"]["usdc"].as<std::string>();
            config.tokens.usdt = yaml_config["tokens"]["usdt"].as<std::string>();
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ Configuration error: " << e.what() << std::endl;
        throw;
    }
    
    return config;
}

} // namespace mev_shield
