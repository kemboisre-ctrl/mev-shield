#include <iostream>
#include "analytics/risk_engine.hpp"

int main() {
    std::cout << "🧪 Testing Risk Engine..." << std::endl;
    
    // Test with different profit scenarios
    mev_shield::RiskEngine engine(0.01, 3.0); // 0.01 ETH min profit, 3% slippage
    
    // Test case 1: Below threshold
    bool result1 = engine.analyze_opportunity(0.005, 2.0);
    std::cout << "Profit: 0.005 ETH, Slippage: 2% -> " << (result1 ? "ACCEPT" : "REJECT") << std::endl;
    
    // Test case 2: Above threshold, low slippage
    bool result2 = engine.analyze_opportunity(0.02, 1.5);
    std::cout << "Profit: 0.02 ETH, Slippage: 1.5% -> " << (result2 ? "ACCEPT" : "REJECT") << std::endl;
    
    // Test case 3: Above threshold, high slippage
    bool result3 = engine.analyze_opportunity(0.02, 4.0);
    std::cout << "Profit: 0.02 ETH, Slippage: 4% -> " << (result3 ? "ACCEPT" : "REJECT") << std::endl;
    
    if (!result1 && result2 && !result3) {
        std::cout << "✅ Risk engine logic working correctly" << std::endl;
    } else {
        std::cout << "❌ Risk engine logic test failed" << std::endl;
    }
    
    return 0;
}
