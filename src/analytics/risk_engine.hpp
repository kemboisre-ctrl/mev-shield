#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <rapidjson/document.h>
#include "common/config.hpp"
#include "common/logger.hpp"
#include <chrono> 
#include <algorithm>

namespace mev_shield {

struct TransactionAnalysis {
    enum RiskLevel { LOW, MEDIUM, HIGH };
    
    RiskLevel risk_level = LOW;
    double estimated_mev_profit_eth = 0.0;
    double slippage_percent = 0.0;
    std::string risk_reason;
    std::vector<std::string> risk_factors;
    bool is_dex_swap = false;
    int64_t analysis_time_ms = 0;
};

class RiskEngine {
public:
    // Add the constructor that your tests expect
    RiskEngine(double min_profit_threshold = 0.01, double high_risk_slippage = 3.0)
        : min_profit_threshold_(min_profit_threshold)
        , high_risk_slippage_(high_risk_slippage) {
        initialize_dex_routers();
        initialize_tokens();
    }
    
    // Keep your default constructor for backward compatibility
    RiskEngine() {
        initialize_dex_routers();
        initialize_tokens();
    }
    
    // Add the method that your tests expect
    bool analyze_opportunity(double potential_profit_eth, double slippage_percent) {
        return potential_profit_eth >= min_profit_threshold_ && 
               slippage_percent <= high_risk_slippage_;
    }
    
    TransactionAnalysis analyze_transaction(const rapidjson::Document& tx_data) {
        auto start_time = std::chrono::steady_clock::now();
        TransactionAnalysis analysis;
        
        try {
            auto tx_info = extract_transaction_info(tx_data);
            analysis.is_dex_swap = is_dex_transaction(tx_info.to);
            
            if (analysis.is_dex_swap) {
                analyze_dex_risk(tx_info, analysis);
            } else {
                analysis.risk_level = TransactionAnalysis::LOW;
                analysis.risk_reason = "Non-DEX transaction - low MEV risk";
            }
            
        } catch (const std::exception& e) {
            analysis.risk_level = TransactionAnalysis::LOW;
            analysis.risk_reason = "Analysis error: " + std::string(e.what());
        }
        
        auto end_time = std::chrono::steady_clock::now();
        analysis.analysis_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time).count();
            
        return analysis;
    }

private:
    struct TransactionInfo {
        std::string hash;
        std::string from;
        std::string to;
        std::string input_data;
        std::string value;
        double eth_value = 0.0;
    };
    
    std::unordered_map<std::string, std::string> dex_routers_;
    std::unordered_map<std::string, std::string> token_addresses_;
    double min_profit_threshold_ = 0.01;
    double high_risk_slippage_ = 3.0;
    
    void initialize_dex_routers() {
        dex_routers_ = {
            {"uniswap_v2", "0x7a250d5630B4cF539739dF2C5dAcb4c659F2488D"},
            {"sushiswap", "0xd9e1cE17f2641f24aE83637ab66a2cca9C378B9F"}
        };
    }
    
    void initialize_tokens() {
        token_addresses_ = {
            {"WETH", "0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2"},
            {"DAI", "0x6B175474E89094C44Da98b954EedeAC495271d0F"},
            {"USDC", "0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48"},
            {"USDT", "0xdAC17F958D2ee523a2206206994597C13D831ec7"}
        };
    }
    
    TransactionInfo extract_transaction_info(const rapidjson::Document& tx_data) {
        TransactionInfo info;
        
        if (tx_data.HasMember("result") && tx_data["result"].IsObject()) {
            const auto& result = tx_data["result"];
            
            if (result.HasMember("hash") && result["hash"].IsString()) {
                info.hash = result["hash"].GetString();
            }
            if (result.HasMember("from") && result["from"].IsString()) {
                info.from = result["from"].GetString();
            }
            if (result.HasMember("to") && result["to"].IsString()) {
                info.to = normalize_address(result["to"].GetString());
            }
            if (result.HasMember("input") && result["input"].IsString()) {
                info.input_data = result["input"].GetString();
            }
            if (result.HasMember("value") && result["value"].IsString()) {
                info.value = result["value"].GetString();
                info.eth_value = hex_to_eth(info.value);
            }
        }
        
        return info;
    }
    
    bool is_dex_transaction(const std::string& to_address) {
        for (const auto& [name, address] : dex_routers_) {
            if (to_address == normalize_address(address)) {
                return true;
            }
        }
        return false;
    }
    
    void analyze_dex_risk(const TransactionInfo& tx_info, TransactionAnalysis& analysis) {
        // Basic risk analysis for open source version
        analysis.estimated_mev_profit_eth = estimate_basic_profit(tx_info);
        analysis.slippage_percent = estimate_slippage(tx_info);
        
        if (analysis.estimated_mev_profit_eth > 0.05) {
            analysis.risk_level = TransactionAnalysis::HIGH;
            analysis.risk_reason = "High MEV profit opportunity detected";
        } else if (analysis.estimated_mev_profit_eth > 0.01) {
            analysis.risk_level = TransactionAnalysis::MEDIUM;
            analysis.risk_reason = "Medium MEV risk";
        } else {
            analysis.risk_level = TransactionAnalysis::LOW;
            analysis.risk_reason = "Low MEV risk";
        }
        
        analysis.risk_factors.push_back("DEX swap detected");
        if (analysis.slippage_percent > 3.0) {
            analysis.risk_factors.push_back("High slippage: " + 
                std::to_string(analysis.slippage_percent) + "%");
        }
    }
    
    double estimate_basic_profit(const TransactionInfo& tx_info) {
        // Basic profit estimation for open source
        // Advanced arbitrage detection kept for commercial version
        if (tx_info.eth_value > 10.0) {
            return tx_info.eth_value * 0.02; // 2% estimated profit for large trades
        }
        return tx_info.eth_value * 0.005; // 0.5% for smaller trades
    }
    
    double estimate_slippage(const TransactionInfo& tx_info) {
        // Basic slippage estimation
        return std::min(tx_info.eth_value * 0.1, 10.0); // Max 10% slippage
    }
    
    std::string normalize_address(const std::string& addr) {
        std::string normalized = addr;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
        return normalized;
    }
    
    double hex_to_eth(const std::string& hex) {
        if (hex.empty() || hex == "0x0" || hex == "0x") return 0.0;
        try {
            return std::stoull(hex.substr(2), nullptr, 16) / 1e18;
        } catch (...) {
            return 0.0;
        }
    }
};

} // namespace mev_shield
