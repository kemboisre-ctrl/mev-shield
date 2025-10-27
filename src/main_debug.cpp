#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include "common/logger.hpp"
#include "analytics/risk_engine.hpp"
#include "network/mempool_monitor.hpp"
#include "common/config_loader.hpp"

std::atomic<bool> running{true};

void signal_handler(int signal) {
    std::cout << "Shutting down MEV Shield..." << std::endl;
    running = false;
}

int main() {
    std::cout << R"(

    ███    ███  ███████ ██    ██     ███████ ██   ██ ██  ██████ ██ ██████  ███████ 
    ████  ████ ██       ██    ██     ██       ██ ██  ██ ██      ██ ██   ██ ██      
    ██ ████ ██ ███████  ██    ██     █████     ███   ██ ██      ██ ██   ██ █████   
    ██  ██  ██      ██  ██    ██     ██       ██ ██  ██ ██      ██ ██   ██ ██      
    ██      ██ ███████   ██████      ███████ ██   ██ ██  ██████ ██ ██████  ███████ 
    
    )" << std::endl;

    std::cout << "MEV Shield Core v1.0.0 - PRODUCTION" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Initialize logger
    if (!mev_shield::Logger::get_instance().initialize("mev_shield")) {
        std::cerr << "Failed to initialize logger" << std::endl;
        return 1;
    }

    LOG_INFO("Starting MEV Shield Core...");
    
    // Set up signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    try {
        // DEBUG: Try to load config and see what happens
        std::cout << "DEBUG: Attempting to load config..." << std::endl;
        auto config = mev_shield::AppConfig::load_from_file("config/config.yaml");
        std::cout << "DEBUG: Config loaded successfully!" << std::endl;
        std::cout << "DEBUG: WebSocket URL: " << config.primary_provider.websocket_url << std::endl;
        std::cout << "DEBUG: Provider Name: " << config.primary_provider.name << std::endl;
        
        // Initialize components
        auto risk_engine = std::make_shared<mev_shield::RiskEngine>();
        
        // Use the loaded WebSocket URL
        std::string websocket_url = config.primary_provider.websocket_url;
        std::cout << "FINAL WebSocket URL: " << websocket_url << std::endl;
        
        auto mempool_monitor = std::make_shared<mev_shield::MempoolMonitor>(websocket_url, risk_engine);
        
        // Set up risk handler
        mempool_monitor->set_risk_handler([](const mev_shield::TransactionAnalysis& analysis) {
            if (analysis.risk_level == mev_shield::TransactionAnalysis::HIGH) {
                LOG_WARN("🚨 HIGH RISK TRANSACTION DETECTED!");
                LOG_WARN("   Estimated MEV Profit: {:.4f} ETH", analysis.estimated_mev_profit_eth);
                LOG_WARN("   Reason: {}", analysis.risk_reason);
            }
        });
        
        LOG_INFO("MEV Shield Core initialized successfully");
        LOG_INFO("WebSocket URL: {}", websocket_url);
        LOG_INFO("Monitoring Ethereum mempool for MEV opportunities...");
        LOG_INFO("Press Ctrl+C to stop");
        std::cout << std::endl;
        
        // Start monitoring
        mempool_monitor->run();
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        std::cerr << "CONFIG ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    LOG_INFO("MEV Shield Core stopped gracefully");
    return 0;
}
