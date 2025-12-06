#include "RedisDatabase.h"
#include <iostream>

namespace GameServer::Database {

bool RedisDatabase::Connect(const std::string& connectionString) {
#ifdef HAS_REDIS
    try {
        _redis = std::make_unique<sw::redis::Redis>(connectionString);
        _redis->ping();  // 실제 연결 검증
        std::cout << "Connected to Redis!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Redis Connection Failed: " << e.what() << std::endl;
        _redis.reset(); // Ensure pointer is null on failure
        return false;
    }
#else
    (void)connectionString;
    std::cout << "[Mock] Connected to Redis (Library not linked)" << std::endl;
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
        std::cerr << "Redis Set Failed: " << e.what() << std::endl;
    }
#else
    (void)key;
    (void)value;
    std::cout << "[Mock] Redis SET " << key << " = " << value << std::endl;
#endif
}

std::string RedisDatabase::Get(const std::string& key) {
#ifdef HAS_REDIS
    if (!_redis) return "";
    try {
        auto val = _redis->get(key);
        return val ? *val : "";
    } catch (const std::exception& e) {
        std::cerr << "Redis Get Failed: " << e.what() << std::endl;
        return "";
    }
#else
    std::cout << "[Mock] Redis GET " << key << std::endl;
    return "MockValue";
#endif
}

}
