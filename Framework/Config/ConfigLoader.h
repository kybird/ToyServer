#pragma once
#include <string>
#include <iostream>

namespace GameServer::Config {

struct DBConnectionInfo {
    std::string host;
    std::string user;
    std::string password;
    std::string dbName;
    int port;

    // Helper for MySQL/PostgreSQL format
    std::string ToConvertionalString() const {
        // "host=localhost;user=root;password=password;db=game;port=3306"
        return "tcp://" + host + ":" + std::to_string(port); // ToyServer uses this format currently for some DBs
    }
};

struct ServerConfig {
    struct {
        std::string ip;
        int port;
        int threadCount;
        int maxSession;
    } server;
    
    struct {
        DBConnectionInfo mysql;
        DBConnectionInfo mongo;
        DBConnectionInfo redis;
    } database;
};

class IConfigLoader {
public:
    virtual ~IConfigLoader() = default;
    virtual ServerConfig Load(const std::string& path) = 0;
};

}
