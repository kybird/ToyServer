#include "Config/JsonConfigLoader.h"
#include "Logger/Logger.h"
#include <iostream>

namespace GameServer::Config {

using json = nlohmann::json;

ServerConfig JsonConfigLoader::Load(const std::string& path) {
    ServerConfig config;
    
    // Default Values
    config.server.ip = "0.0.0.0";
    config.server.port = 4242;
    config.server.threadCount = 4;
    config.server.maxSession = 1000;

    std::ifstream file(path);
    if (!file.is_open()) {
        LogWarn("[JsonConfigLoader] Failed to open config file: {}. Using defaults.", path);
        return config;
    }

    try {
        json j;
        file >> j;

        if (j.contains("Server")) {
            auto& s = j["Server"];
            if (s.contains("IP")) config.server.ip = s["IP"];
            if (s.contains("Port")) config.server.port = s["Port"];
            if (s.contains("ThreadCount")) config.server.threadCount = s["ThreadCount"];
            if (s.contains("MaxSession")) config.server.maxSession = s["MaxSession"];
        }

        if (j.contains("Database")) {
            auto& db = j["Database"];
            auto parseDB = [](const json& node, DBConnectionInfo& info, int defaultPort) {
                if (node.contains("Host")) info.host = node["Host"];
                if (node.contains("User")) info.user = node["User"];
                if (node.contains("PW")) info.password = node["PW"];
                if (node.contains("DBName")) info.dbName = node["DBName"];
                info.port = node.value("Port", defaultPort);
            };

            if (db.contains("MySQL")) parseDB(db["MySQL"], config.database.mysql, 3306);
            if (db.contains("Mongo")) parseDB(db["Mongo"], config.database.mongo, 27017);
            if (db.contains("Redis")) parseDB(db["Redis"], config.database.redis, 6379);
        }

        LogInfo("[JsonConfigLoader] Config loaded successfully.");

    } catch (const std::exception& e) {
        LogError("[JsonConfigLoader] JSON Parsing Error: {}", e.what());
    }

    return config;
}

}
