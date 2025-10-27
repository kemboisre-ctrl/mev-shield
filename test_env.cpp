#include <iostream>
#include <cstdlib>

int main() {
    std::cout << "🧪 Testing Environment Variables..." << std::endl;
    
    // Set test environment variables
    setenv("TEST_VAR", "test_value_123", 1);
    setenv("ALCHEMY_API_KEY", "test_alchemy_key", 1);
    
    const char* test_var = std::getenv("TEST_VAR");
    const char* alchemy_key = std::getenv("ALCHEMY_API_KEY");
    
    std::cout << "TEST_VAR: " << (test_var ? test_var : "NOT SET") << std::endl;
    std::cout << "ALCHEMY_API_KEY: " << (alchemy_key ? alchemy_key : "NOT SET") << std::endl;
    std::cout << "INFURA_API_KEY: " << (std::getenv("INFURA_API_KEY") ? "SET" : "NOT SET") << std::endl;
    
    if (test_var && std::string(test_var) == "test_value_123") {
        std::cout << "✅ Environment variables working correctly" << std::endl;
    } else {
        std::cout << "❌ Environment variables test failed" << std::endl;
    }
    
    return 0;
}
