#pragma once
#include <string>

namespace mev_shield {
    struct Config {
        std::string rpc_url = "http://localhost:8545";
        std::string websocket_url = "ws://localhost:8546";
        std::string log_file = "mev_shield.log";
        // Add other config options as needed
    };
}
