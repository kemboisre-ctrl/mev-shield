#include <iostream>
#include "common/config_loader.hpp"

int main() {
    try {
        std::cout << "🧪 Testing Configuration Loader..." << std::endl;
        
        auto config = mev_shield::AppConfig::load_from_file("config/config.yaml");
        
        std::cout << "✅ Config loaded successfully!" << std::endl;
        std::cout << "📊 Primary RPC: " << config.primary_provider.websocket_url << std::endl;
        std::cout << "📊 Fallback providers: " << config.fallback_providers.size() << std::endl;
        std::cout << "📊 Min Profit Threshold: " << config.risk_engine.min_profit_threshold_eth << " ETH" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "❌ Config test failed: " << e.what() << std::endl;
        return 1;
    }
}
