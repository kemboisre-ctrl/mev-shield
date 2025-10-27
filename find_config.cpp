#include <iostream>
#include <fstream>
#include <filesystem>

int main() {
    std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
    
    std::vector<std::string> test_paths = {
        "config/config.yaml",
        "../config/config.yaml",
        "../../config/config.yaml",
        "/home/ubuntu/mev-shield/config/config.yaml"
    };
    
    for (const auto& path : test_paths) {
        std::ifstream file(path);
        if (file) {
            std::cout << "✅ FOUND: " << path << std::endl;
            file.close();
        } else {
            std::cout << "❌ NOT FOUND: " << path << std::endl;
        }
    }
    return 0;
}
