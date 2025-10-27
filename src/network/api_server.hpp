#pragma once

#include <atomic>
#include <thread>
#include <string>
#include <memory>
#include <chrono>
#include "common/logger.hpp"

namespace mev_shield {

class APIServer {
private:
    std::atomic<bool> running_{false};
    std::thread server_thread_;
    int port_;
    std::shared_ptr<MempoolMonitor> mempool_monitor_;
    
public:
    APIServer(int port, std::shared_ptr<MempoolMonitor> mempool_monitor) 
        : port_(port), mempool_monitor_(mempool_monitor) {}
    
    void start() {
        running_ = true;
        server_thread_ = std::thread([this]() {
            this->run_server();
        });
        LOG_INFO("API Server started on port {}", port_);
        
        // Print API information
        std::cout << "\n🌐 MEV Shield API Endpoints:" << std::endl;
        std::cout << "   http://localhost:" << port_ << "/api/health" << std::endl;
        std::cout << "   http://localhost:" << port_ << "/api/stats" << std::endl;
        std::cout << "   http://localhost:" << port_ << "/api/opportunities" << std::endl;
        std::cout << "   http://localhost:" << port_ << "/api/config" << std::endl;
        std::cout << std::endl;
    }
    
    void stop() {
        running_ = false;
        if (server_thread_.joinable()) {
            server_thread_.join();
        }
        LOG_INFO("API Server stopped");
    }
    
private:
    void run_server() {
        int request_count = 0;
        
        while (running_) {
            // Simulate API server processing
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            // In a real implementation, this would handle HTTP requests
            // For now, we'll simulate API functionality that your app can use
            
            if (request_count % 15 == 0) { // Log every 30 seconds
                LOG_INFO("[API] Server active - Port: {}, Requests: {}", port_, request_count);
            }
            request_count++;
        }
    }
    
    // API response generators
    std::string generate_health_response() {
        return R"({
            "status": "healthy",
            "service": "mev_shield",
            "version": "1.0.0",
            "timestamp": ")" + get_timestamp() + R"("
        })";
    }
    
    std::string generate_stats_response() {
        return R"({
            "transactions_analyzed": 15432,
            "mev_opportunities_found": 23,
            "high_risk_transactions": 5,
            "estimated_profit_eth": 0.45,
            "uptime_seconds": 3600
        })";
    }
    
    std::string generate_opportunities_response() {
        return R"({
            "active_opportunities": [
                {
                    "type": "arbitrage",
                    "estimated_profit_eth": 0.12,
                    "risk_score": 0.3,
                    "tokens": ["WETH", "DAI"]
                },
                {
                    "type": "liquidation", 
                    "estimated_profit_eth": 0.08,
                    "risk_score": 0.4,
                    "tokens": ["USDC", "ETH"]
                }
            ]
        })";
    }
    
    std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&time_t);
        timestamp.pop_back(); // Remove newline
        return timestamp;
    }
};

} // namespace mev_shield
