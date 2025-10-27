#include <iostream>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;

context_ptr on_tls_init() {
    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                        boost::asio::ssl::context::no_sslv2 |
                        boost::asio::ssl::context::no_sslv3);
        ctx->set_verify_mode(boost::asio::ssl::verify_none);
    } catch (std::exception& e) {
        std::cout << "Error in TLS init: " << e.what() << std::endl;
    }
    return ctx;
}

int main() {
    std::string infura_ws = "wss://mainnet.infura.io/ws/v3/c4ac605f5f484c73b8135c6f2ad20ec0";
    
    client c;
    c.init_asio();
    c.set_tls_init_handler(&on_tls_init);
    
    c.set_open_handler([&infura_ws](websocketpp::connection_hdl hdl) {
        std::cout << "✅ Connected to Infura WebSocket: " << infura_ws << std::endl;
    });
    
    c.set_fail_handler([&infura_ws](websocketpp::connection_hdl hdl) {
        std::cout << "❌ Failed to connect to Infura WebSocket: " << infura_ws << std::endl;
    });
    
    c.set_message_handler([](websocketpp::connection_hdl hdl, 
                           client::message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;
    });
    
    websocketpp::lib::error_code ec;
    client::connection_ptr con = c.get_connection(infura_ws, ec);
    
    if (ec) {
        std::cout << "Connection error: " << ec.message() << std::endl;
        return 1;
    }
    
    c.connect(con);
    
    // Run for 10 seconds to test connection
    std::thread t([&c]() {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        c.stop();
    });
    
    c.run();
    t.join();
    
    return 0;
}
