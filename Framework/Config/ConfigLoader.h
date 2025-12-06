#pragma once
#include <string>
#include <iostream>

namespace GameServer::Config {

struct ServerConfig {
    int serverPort;
    std::string mysqlConnectionString;
    std::string mongoConnectionString;
    std::string redisConnectionString;
};

class ConfigLoader {
public:
    static ServerConfig Load(const std::string& path) {
        // TODO: Implement actual JSON loading.
        // For now, return default values to ensure build passes.
        // The original file was lost during refactoring.
        ServerConfig config;
        config.serverPort = 4242;
        config.mysqlConnectionString = "host=localhost;user=root;password=password;db=game";
        config.mongoConnectionString = "mongodb://localhost:27017";
        config.redisConnectionString = "tcp://127.0.0.1:6379";
        
        std::cout << "[ConfigLoader] Loaded mock configuration." << std::endl;
        return config;
    }
};

}
