#pragma once
#include <memory>
#include <string>
#include <vector>

namespace GameServer::Database {

class DBConnection {
public:
    virtual ~DBConnection() = default;
    virtual bool Execute(const std::string& query) = 0;
    // Add more methods like Query, Fetch, etc.
};

class DBConnectionPool {
public:
    virtual ~DBConnectionPool() = default;

    virtual bool Initialize(const std::string& connectionString, int poolSize) = 0;
    virtual std::shared_ptr<DBConnection> GetConnection() = 0;
    virtual void ReleaseConnection(std::shared_ptr<DBConnection> conn) = 0;
};

}
