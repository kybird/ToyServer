#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // For custom types support if needed

namespace GameServer::Utils {

    class Logger {
    public:
        static void Init();
    };

}

// Global Logging Macros
// Mapped to spdlog's default logger (which we will configure)
#define LogInfo(...)    spdlog::info(__VA_ARGS__)
#define LogWarn(...)    spdlog::warn(__VA_ARGS__)
#define LogError(...)   spdlog::error(__VA_ARGS__)
#define LogDebug(...)   spdlog::debug(__VA_ARGS__)
#define LogTrace(...)   spdlog::trace(__VA_ARGS__)
