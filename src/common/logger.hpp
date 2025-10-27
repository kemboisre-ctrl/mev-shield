#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace mev_shield {

class Logger {
public:
    static Logger& get_instance() {
        static Logger instance;
        return instance;
    }
    
    bool initialize(const std::string& name = "mev_shield", 
                   const std::string& log_dir = "logs",
                   spdlog::level::level_enum level = spdlog::level::info) {
        try {
            // Create log directory if it doesn't exist
            std::filesystem::create_directories(log_dir);
            
            // Create rotating file sink (100MB max, 5 files)
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_dir + "/" + name + ".log", 1024 * 1024 * 100, 5);
            
            // Create console sink
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            
            // Create logger with both sinks
            std::vector<spdlog::sink_ptr> sinks{file_sink, console_sink};
            logger_ = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
            
            // Set pattern and level
            logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
            logger_->set_level(level);
            logger_->flush_on(spdlog::level::warn);
            
            spdlog::register_logger(logger_);
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Logger initialization failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    std::shared_ptr<spdlog::logger> get_logger() { return logger_; }

private:
    Logger() = default;
    std::shared_ptr<spdlog::logger> logger_;
};

// Helper macros
#define LOG_TRACE(...)    SPDLOG_LOGGER_TRACE(mev_shield::Logger::get_instance().get_logger(), __VA_ARGS__)
#define LOG_DEBUG(...)    SPDLOG_LOGGER_DEBUG(mev_shield::Logger::get_instance().get_logger(), __VA_ARGS__)
#define LOG_INFO(...)     SPDLOG_LOGGER_INFO(mev_shield::Logger::get_instance().get_logger(), __VA_ARGS__)
#define LOG_WARN(...)     SPDLOG_LOGGER_WARN(mev_shield::Logger::get_instance().get_logger(), __VA_ARGS__)
#define LOG_ERROR(...)    SPDLOG_LOGGER_ERROR(mev_shield::Logger::get_instance().get_logger(), __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(mev_shield::Logger::get_instance().get_logger(), __VA_ARGS__)

} // namespace mev_shield
