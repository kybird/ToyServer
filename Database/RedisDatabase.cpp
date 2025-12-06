#include "RedisDatabase.h"
#include "Logger/Logger.h"
#include <iostream>

namespace GameServer::Database {

bool RedisDatabase::Connect(const std::string& connectionString) {
#ifdef HAS_REDIS
    try {
        _redis = std::make_unique<sw::redis::Redis>(connectionString);
        _redis->ping();  // 실제 연결 검증
        LogInfo("Connected to Redis!");
        return true;
    } catch (const std::exception& e) {
        LogWarn("Redis Connection Failed: {}", e.what());
        _redis.reset(); // Ensure pointer is null on failure
        return false;
    }
#else
    (void)connectionString;
    LogInfo("[Mock] Connected to Redis (Library not linked)");
    return true;
#endif
}

void RedisDatabase::Disconnect() {
#ifdef HAS_REDIS
    _redis.reset();
#endif
}

void RedisDatabase::Set(const std::string& key, const std::string& value) {
#ifdef HAS_REDIS
    if (!_redis) return;
    try {
        _redis->set(key, value);
    } catch (const std::exception& e) {
        LogError("Redis Set Failed: {}", e.what());
    }
#else
    (void)key;
    (void)value;
    LogInfo("[Mock] Redis SET {} = {}", key, value);
#endif
}

std::string RedisDatabase::Get(const std::string& key) {
#ifdef HAS_REDIS
    if (!_redis) return "";
    try {
        auto val = _redis->get(key);
        return val ? *val : "";
    } catch (const std::exception& e) {
        LogError("Redis Get Failed: {}", e.what());
        return "";
    }
#else
    LogInfo("[Mock] Redis GET {}: MockValue", key);
    return "MockValue";
#endif
}

}
