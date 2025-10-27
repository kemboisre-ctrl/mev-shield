#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <filesystem>
#include "common/logger.hpp"
#include "analytics/risk_engine.hpp"
#include "network/mempool_monitor.hpp"
#include "common/config_loader.hpp"

std::atomic<bool> running{true};

void signal_handler(int) {
    std::cout << "Shutting down MEV Shield..." << std::endl;
    running = false;
}

std::string load_websocket_url() {
    // Try multiple config locations and formats
    std::vector<std::string> config_paths = {
        "config/config.yaml",
        "../config/config.yaml", 
        "/home/ubuntu/mev-shield/config/config.yaml",
        "config.yaml"
    };
    
    for (const auto& path : config_paths) {
        if (std::filesystem::exists(path)) {
            try {
                std::cout << "Trying config: " << path << std::endl;
                auto config = mev_shield::AppConfig::load_from_file(path);
                if (!config.primary_provider.websocket_url.empty()) {
                    std::cout << "✅ Config loaded from: " << path << std::endl;
                    return config.primary_provider.websocket_url;
                }
            } catch (const std::exception& e) {
                std::cout << "❌ Config error in " << path << ": " << e.what() << std::endl;
            }
        }
    }
    
    // Fallback: Try to extract from your original YAML format
    for (const auto& path : config_paths) {
        if (std::filesystem::exists(path)) {
            try {
                std::cout << "Trying alternative YAML parsing for: " << path << std::endl;
                // Simple manual extraction for your format
                std::ifstream file(path);
                std::string line;
                while (std::getline(file, line)) {
                    if (line.find("websocket_url:") != std::string::npos && 
                        line.find("wss://") != std::string::npos) {
                        size_t start = line.find("wss://");
                        size_t end = line.find("\"", start);
                        if (end == std::string::npos) end = line.length();
                        std::string url = line.substr(start, end - start);
                        std::cout << "✅ Extracted WebSocket URL: " << url << std::endl;
                        return url;
                    }
                }
            } catch (...) {
                // Continue to next attempt
            }
        }
    }
    
    // Final fallback
    std::cout << "⚠️  Using hardcoded WebSocket URL" << std::endl;
    return "wss://mainnet.infura.io/ws/v3/c4ac605f5f484c73b8135c6f2ad20ec0";
}

int main() {
    std::cout << R"(

    ███    ███  ███████ ██    ██     ███████ ██   ██ ██  ██████ ██ ██████  ███████ 
    ████  ████ ██       ██    ██     ██       ██ ██  ██ ██      ██ ██   ██ ██      
    ██ ████ ██ ███████  ██    ██     █████     ███   ██ ██      ██ ██   ██ █████   
    ██  ██  ██      ██  ██    ██     ██       ██ ██  ██ ██      ██ ██   ██ ██      
    ██      ██ ███████   ██████      ███████ ██   ██ ██  ██████ ██ ██████  ███████ 
    
    )" << std::endl;

    std::cout << "MEV Shield Core v1.0.0 - UNIVERSAL" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Initialize logger
    if (!mev_shield::Logger::get_instance().initialize("mev_shield")) {
        std::cerr << "Failed to initialize logger" << std::endl;
        return 1;
    }

    LOG_INFO("Starting MEV Shield Core...");
    
    // Set up signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    
    // Load WebSocket URL with multiple fallbacks
    std::string websocket_url = load_websocket_url();
    
    LOG_INFO("Final WebSocket URL: {}", websocket_url);
    
    try {
        // Initialize components
        auto risk_engine = std::make_shared<mev_shield::RiskEngine>();
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
        LOG_INFO("Monitoring Ethereum mempool for MEV opportunities...");
        LOG_INFO("Press Ctrl+C to stop");
        std::cout << std::endl;
        
        // Start monitoring
        mempool_monitor->run();
        
    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        return 1;
    }
    
    LOG_INFO("MEV Shield Core stopped gracefully");
    return 0;
}
