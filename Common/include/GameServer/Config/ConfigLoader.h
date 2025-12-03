#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

namespace GameServer::Config {

struct ServerConfig {
    std::string mysqlConnectionString;
    std::string mongoConnectionString;
    std::string redisConnectionString;
    int serverPort;
};

class ConfigLoader {
public:
    static ServerConfig Load(const std::string& path) {
        ServerConfig config;
        // Default values
        config.mysqlConnectionString = "host=127.0.0.1;user=root;password=password;db=game";
        config.mongoConnectionString = "mongodb://127.0.0.1:27017";
        config.redisConnectionString = "tcp://127.0.0.1:6379";
        config.serverPort = 4242;

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Config file not found: " << path << ". Using defaults." << std::endl;
            return config;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        // Very basic manual JSON parsing for flat key-values
        // Assumes format: "key": "value" or "key": 123
        config.mysqlConnectionString = ParseString(content, "mysql");
        config.mongoConnectionString = ParseString(content, "mongo");
        config.redisConnectionString = ParseString(content, "redis");
        config.serverPort = ParseInt(content, "port");

        return config;
    }

private:
    static std::string ParseString(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";

        size_t colonPos = json.find(":", pos);
        size_t startQuote = json.find("\"", colonPos);
        size_t endQuote = json.find("\"", startQuote + 1);

        if (startQuote != std::string::npos && endQuote != std::string::npos) {
            return json.substr(startQuote + 1, endQuote - startQuote - 1);
        }
        return "";
    }

    static int ParseInt(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return 0;

        size_t colonPos = json.find(":", pos);
        size_t valueStart = json.find_first_of("0123456789", colonPos);
        size_t valueEnd = json.find_first_not_of("0123456789", valueStart);

        if (valueStart != std::string::npos) {
            std::string valStr = json.substr(valueStart, valueEnd - valueStart);
            try {
                return std::stoi(valStr);
            } catch (...) { return 0; }
        }
        return 0;
    }
};

}
