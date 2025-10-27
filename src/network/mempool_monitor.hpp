#pragma once
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <functional>
#include <rapidjson/document.h>
#include "common/logger.hpp"
#include "analytics/risk_engine.hpp"

namespace mev_shield {

class MempoolMonitor {
public:
    MempoolMonitor(const std::string& websocket_url, 
                   std::shared_ptr<RiskEngine> risk_engine)
        : websocket_url_(websocket_url), risk_engine_(risk_engine) {
        
        client_.init_asio();
        client_.set_tls_init_handler([this](websocketpp::connection_hdl) {
            return create_tls_context();
        });
        
        client_.set_open_handler([this](auto hdl) { on_open(hdl); });
        client_.set_message_handler([this](auto hdl, auto msg) { on_message(hdl, msg); });
        client_.set_fail_handler([this](auto hdl) { on_fail(hdl); });
        client_.set_close_handler([this](auto hdl) { on_close(hdl); });
    }
    
    void run() {
        try {
            websocketpp::lib::error_code ec;
            auto con = client_.get_connection(websocket_url_, ec);
            
            if (ec) {
                LOG_ERROR("WebSocket connection error: {}", ec.message());
                return;
            }
            
            client_.connect(con);
            client_.run();
        } catch (const std::exception& e) {
            LOG_ERROR("Mempool monitor exception: {}", e.what());
        }
    }
    
    void stop() {
        client_.stop();
        LOG_INFO("Mempool monitor stopped");
    }
    
    void set_risk_handler(std::function<void(const TransactionAnalysis&)> handler) {
        risk_handler_ = handler;
    }

private:
    std::string websocket_url_;
    std::shared_ptr<RiskEngine> risk_engine_;
    websocketpp::client<websocketpp::config::asio_tls_client> client_;
    std::function<void(const TransactionAnalysis&)> risk_handler_;
    
    std::shared_ptr<boost::asio::ssl::context> create_tls_context() {
        auto ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
        try {
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                            boost::asio::ssl::context::no_sslv2 |
                            boost::asio::ssl::context::no_sslv3);
            ctx->set_verify_mode(boost::asio::ssl::verify_none);
        } catch (std::exception& e) {
            LOG_ERROR("TLS context error: {}", e.what());
        }
        return ctx;
    }
    
    void on_open(websocketpp::connection_hdl hdl) {
        LOG_INFO("✅ Connected to WebSocket, subscribing to mempool...");
        
        std::string subscribe_msg = R"({
            "jsonrpc": "2.0",
            "id": 1,
            "method": "eth_subscribe",
            "params": ["newPendingTransactions"]
        })";
        
        client_.send(hdl, subscribe_msg, websocketpp::frame::opcode::text);
    }
    
    void on_message(websocketpp::connection_hdl hdl, 
                   websocketpp::config::asio_tls_client::message_type::ptr msg) {
        
        std::string payload = msg->get_payload();
        process_websocket_message(payload);
    }
    
    void on_fail(websocketpp::connection_hdl hdl) {
        LOG_ERROR("❌ WebSocket connection failed");
        
        // Try to reconnect after 5 seconds
        LOG_INFO("🔄 Attempting to reconnect in 5 seconds...");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        run();
    }
    
    void on_close(websocketpp::connection_hdl hdl) {
        LOG_INFO("📡 WebSocket connection closed");
    }
    
    void process_websocket_message(const std::string& payload) {
        rapidjson::Document doc;
        doc.Parse(payload.c_str());
        
        if (doc.HasParseError()) {
            LOG_DEBUG("Failed to parse WebSocket message");
            return;
        }
        
        // Check if this is a new transaction notification
        if (doc.HasMember("params") && doc["params"].IsObject()) {
            const auto& params = doc["params"];
            if (params.HasMember("result") && params["result"].IsString()) {
                std::string tx_hash = params["result"].GetString();
                LOG_INFO("🔍 Detected transaction: {}", tx_hash.substr(0, 16) + "...");
                simulate_transaction_analysis(tx_hash);
            }
        }
        
        // Check if this is a subscription confirmation
        if (doc.HasMember("result") && doc["result"].IsString()) {
            LOG_INFO("✅ Subscription confirmed: {}", doc["result"].GetString());
        }
    }
    
    void simulate_transaction_analysis(const std::string& tx_hash) {
        // Create a mock transaction analysis
        TransactionAnalysis analysis;
        
        // Simulate random risk levels for demo
        int risk_roll = rand() % 100;
        if (risk_roll < 5) { // 5% high risk
            analysis.risk_level = TransactionAnalysis::HIGH;
            analysis.estimated_mev_profit_eth = 0.02 + (rand() % 100) / 1000.0;
            analysis.risk_reason = "High MEV arbitrage opportunity detected";
        } else if (risk_roll < 20) { // 15% medium risk
            analysis.risk_level = TransactionAnalysis::MEDIUM;
            analysis.estimated_mev_profit_eth = 0.005 + (rand() % 50) / 1000.0;
            analysis.risk_reason = "Medium risk transaction";
        } else { // 80% low risk
            analysis.risk_level = TransactionAnalysis::LOW;
            analysis.estimated_mev_profit_eth = (rand() % 20) / 1000.0;
            analysis.risk_reason = "Low risk transaction";
        }
        
        analysis.slippage_percent = (rand() % 50) / 10.0;
        analysis.is_dex_swap = (rand() % 3 == 0);
        analysis.analysis_time_ms = rand() % 100;
        
        // Add risk factors
        if (analysis.estimated_mev_profit_eth > 0.01) {
            analysis.risk_factors.push_back("High profit potential");
        }
        if (analysis.slippage_percent > 3.0) {
            analysis.risk_factors.push_back("High slippage");
        }
        if (analysis.is_dex_swap) {
            analysis.risk_factors.push_back("DEX swap detected");
        }
        
        // Call risk handler if set
        if (risk_handler_) {
            risk_handler_(analysis);
        }
        
        log_analysis_result(tx_hash, analysis);
    }
    
    void log_analysis_result(const std::string& tx_hash, const TransactionAnalysis& analysis) {
        std::string level_str;
        switch (analysis.risk_level) {
            case TransactionAnalysis::LOW: level_str = "LOW"; break;
            case TransactionAnalysis::MEDIUM: level_str = "MEDIUM"; break;
            case TransactionAnalysis::HIGH: level_str = "HIGH"; break;
        }
        
        if (analysis.risk_level == TransactionAnalysis::HIGH) {
            LOG_WARN("🚨 HIGH RISK - TX: {} | Risk: {} | Profit: {:.4f} ETH | Slippage: {:.1f}%", 
                    tx_hash.substr(0, 16), level_str, analysis.estimated_mev_profit_eth,
                    analysis.slippage_percent);
        } else if (analysis.risk_level == TransactionAnalysis::MEDIUM) {
            LOG_INFO("⚠️  MEDIUM RISK - TX: {} | Profit: {:.4f} ETH", 
                    tx_hash.substr(0, 16), analysis.estimated_mev_profit_eth);
        } else {
            LOG_DEBUG("TX: {} | Risk: {} | Profit: {:.4f} ETH", 
                     tx_hash.substr(0, 16), level_str, analysis.estimated_mev_profit_eth);
        }
    }
};

} // namespace mev_shield
