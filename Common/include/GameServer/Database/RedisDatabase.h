#pragma once
#include <string>
#include <iostream>
#include <memory>

#ifdef HAS_REDIS
#include <sw/redis++/redis++.h>
#endif

namespace GameServer::Database {

class RedisDatabase {
public:
    bool Connect(const std::string& connectionString) {
#ifdef HAS_REDIS
        try {
            _redis = std::make_unique<sw::redis::Redis>(connectionString);
            std::cout << "Connected to Redis!" << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Redis Connection Failed: " << e.what() << std::endl;
            return false;
        }
#else
        std::cout << "[Mock] Connected to Redis (Library not linked)" << std::endl;
        return true;
#endif
    }

    void Disconnect() {
        // Redis++ handles disconnection
    }



    // Specific Redis methods
    void Set(const std::string& key, const std::string& value) {
#ifdef HAS_REDIS
        _redis->set(key, value);
#else
        std::cout << "[Mock] Redis SET " << key << " = " << value << std::endl;
#endif
    }

    std::string Get(const std::string& key) {
#ifdef HAS_REDIS
        auto val = _redis->get(key);
        return val ? *val : "";
#else
        std::cout << "[Mock] Redis GET " << key << std::endl;
        return "MockValue";
#endif
    }

private:
#ifdef HAS_REDIS
    std::unique_ptr<sw::redis::Redis> _redis;
#endif
};

}
