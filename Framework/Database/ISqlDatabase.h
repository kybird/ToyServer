#pragma once
#include <string>
#include <vector>
#include <memory>

namespace GameServer::Database {

class ISqlDatabase {
public:
    virtual ~ISqlDatabase() = default;

    virtual bool Connect(const std::string& connectionString) = 0;
    virtual void Disconnect() = 0;
    virtual bool Execute(const std::string& query) = 0;
    // Future: virtual std::shared_ptr<IResultSet> Query(const std::string& query) = 0;
};

}
