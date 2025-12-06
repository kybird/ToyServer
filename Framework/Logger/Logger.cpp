#include "Logger/Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <vector>
#include <iostream>
#include <filesystem>

namespace GameServer::Utils {

    void Logger::Init() {
        try {
            // Create logs directory if not exists
            std::filesystem::create_directories("logs");

            // Async Logger Setup
            spdlog::init_thread_pool(8192, 1); // Queue size & Thread count

            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            spdlog::filename_t logPath = SPDLOG_FILENAME_T("logs/server.log");
            auto dailySink = std::shared_ptr<spdlog::sinks::daily_file_sink_mt>(
                new spdlog::sinks::daily_file_sink_mt(logPath, 0, 0)
            );

            std::vector<spdlog::sink_ptr> sinks { consoleSink, dailySink };

            // 4. Async Logger 생성 (핵심 변경 부분)
            // 동기 로거(spdlog::logger) 대신 비동기 로거(spdlog::async_logger)를 사용해야 합니다.
            // 생성자 인자: 이름, 싱크 시작, 싱크 끝, 스레드풀, 오버플로우 정책
            auto logger = std::make_shared<spdlog::async_logger>(
                "server", 
                sinks.begin(), 
                sinks.end(), 
                spdlog::thread_pool(), // 전역 스레드풀 전달
                spdlog::async_overflow_policy::block // 큐가 꽉 차면 대기
            );
            
            // Format: [Time] [Level] [Thread] Message
            logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
            logger->set_level(spdlog::level::info); // Default level
            
            spdlog::register_logger(logger);
            spdlog::set_default_logger(logger);
            spdlog::flush_every(std::chrono::seconds(3)); // Auto flush

            LogInfo("Logger initialized successfully (Async Mode).");

        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
        }
    }

}
