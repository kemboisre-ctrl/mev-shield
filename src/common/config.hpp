#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

struct Config {
    std::string infura_api_key;
    std::string alchemy_api_key;
    std::string quicknode_api_key;
    std::string ethereum_rpc_url;
    double min_profit_threshold;
    double max_slippage_percent;
    int max_gas_price_gwei;
    bool enable_monitoring;
    int websocket_timeout_ms;
};

Config load_config();
bool validate_config(const Config& config);

#endif
