#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <thread>
#include "common/logger.hpp"
#include "common/config_loader.hpp"
#include "analytics/risk_engine.hpp"
#include "network/mempool_monitor.hpp"

std::atomic<bool> running{true};

void signal_handler(int signal) {
    LOG_INFO("Shutting down MEV Shield...");
    running = false;
}

void print_banner() {
    std::cout << R"(

    ███    ███  ███████ ██    ██     ███████ ██   ██ ██  ██████ ██ ██████  ███████ 
    ████  ████ ██       ██    ██     ██       ██ ██  ██ ██      ██ ██   ██ ██      
    ██ ████ ██ ███████  ██    ██     █████     ███   ██ ██      ██ ██   ██ █████   
    ██  ██  ██      ██  ██    ██     ██       ██ ██  ██ ██      ██ ██   ██ ██      
    ██      ██ ███████   ██████      ███████ ██   ██ ██  ██████ ██ ██████  ███████ 
    
    )" << std::endl;

    std::cout << "MEV Shield Core v1.0.0 - Open Source MEV Protection" << std::endl;
    std::cout << "=====================================================" << std::endl;
}

void print_config_summary(const mev_shield::AppConfig& config) {
    std::cout << "\n📋 Configuration Summary:" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // Primary provider (Infura)
    std::string primary_url = config.primary_provider.websocket_url;
    // Hide full API key in display
    size_t key_pos = primary_url.find("/v3/");
    if (key_pos != std::string::npos) {
        primary_url = primary_url.substr(0, key_pos + 4) + "***";
    }
    std::cout << "🔗 Primary RPC: " << config.primary_provider.name << std::endl;
    std::cout << "   WebSocket: " << primary_url << std::endl;
    
    // Fallback providers
    std::cout << "🔄 Fallback Providers (" << config.fallback_providers.size() << "):" << std::endl;
    for (const auto& provider : config.fallback_providers) {
        std::cout << "   • " << provider.name << std::endl;
    }
    
    // Risk settings
    std::cout << "🎯 Risk Settings:" << std::endl;
    std::cout << "   • Min Profit: " << config.risk_engine.min_profit_threshold_eth << " ETH" << std::endl;
    std::cout << "   • High Risk Slippage: " << config.risk_engine.high_risk_slippage_percent << "%" << std::endl;
    std::cout << "   • Max Simulation Time: " << config.risk_engine.max_simulation_time_ms << "ms" << std::endl;
    std::cout << "   • Max Gas Price: " << config.risk_engine.max_gas_price_gwei << " Gwei" << std::endl;
    
    // DEX & Tokens
    std::cout << "🏦 DEX Support: Uniswap V2/V3, Sushiswap" << std::endl;
    std::cout << "💎 Token Support: WETH, DAI, USDC, USDT" << std::endl;
    std::cout << "🌐 API Port: " << config.api.port << std::endl;
    std::cout << "==========================" << std::endl;
}

int main() {
    print_banner();
    
    // Set up signal handlers
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try {
        // Initialize logger
        if (!mev_shield::Logger::get_instance().initialize("mev_shield")) {
            std::cerr << "Failed to initialize logger" << std::endl;
            return 1;
        }

        // Load configuration
        auto config = mev_shield::AppConfig::load_from_file("config/config.yaml");
        print_config_summary(config);
        
        LOG_INFO("Starting MEV Shield Core with Infura...");
        
        // Log safely (hide API key)
        std::string safe_url = config.primary_provider.websocket_url;
        size_t key_pos = safe_url.find("/v3/");
        if (key_pos != std::string::npos) {
            safe_url = safe_url.substr(0, key_pos + 4) + "***";
        }
        LOG_INFO("Primary RPC: {}", safe_url);

        // Initialize risk engine
        auto risk_engine = std::make_shared<mev_shield::RiskEngine>(
            config.risk_engine.min_profit_threshold_eth,
            config.risk_engine.high_risk_slippage_percent
        );

        // Initialize mempool monitor
        LOG_INFO("Initializing mempool monitor...");
        auto mempool_monitor = std::make_shared<mev_shield::MempoolMonitor>(
            config.primary_provider.websocket_url, 
            risk_engine
        );
        
        // Set up risk handler
        mempool_monitor->set_risk_handler([](const mev_shield::TransactionAnalysis& analysis) {
            if (analysis.risk_level == mev_shield::TransactionAnalysis::HIGH) {
                LOG_WARN("🚨 HIGH RISK TRANSACTION DETECTED!");
                LOG_WARN("   Estimated MEV Profit: {:.4f} ETH", analysis.estimated_mev_profit_eth);
                LOG_WARN("   Slippage: {:.1f}%", analysis.slippage_percent);
                LOG_WARN("   Reason: {}", analysis.risk_reason);
            }
        });
        
        // Start monitoring in a separate thread
        std::thread monitor_thread([mempool_monitor]() {
            try {
                mempool_monitor->run();
            } catch (const std::exception& e) {
                LOG_ERROR("Monitor thread error: {}", e.what());
            }
        });

        LOG_INFO("MEV Shield Core initialized successfully");
        LOG_INFO("Monitoring Ethereum mempool for MEV opportunities...");
        LOG_INFO("Press Ctrl+C to stop");

        // Main loop
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Clean shutdown
        LOG_INFO("Stopping monitor...");
        mempool_monitor->stop();
        if (monitor_thread.joinable()) {
            monitor_thread.join();
        }

        LOG_INFO("MEV Shield Core stopped gracefully");

    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        std::cout << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
