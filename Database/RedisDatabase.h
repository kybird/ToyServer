#pragma once
#include <string>
#include <memory>

#ifdef HAS_REDIS
#include <sw/redis++/redis++.h>
#endif

namespace GameServer::Database {

class RedisDatabase {
public:
    bool Connect(const std::string& connectionString);
    void Disconnect();
    void Set(const std::string& key, const std::string& value);
    std::string Get(const std::string& key);

private:
#ifdef HAS_REDIS
    std::unique_ptr<sw::redis::Redis> _redis;
#endif
};

}
