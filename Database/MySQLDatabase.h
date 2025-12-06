#pragma once
#include "Database/ISqlDatabase.h"
#include <string>

#ifdef HAS_MYSQL
#include <mysql/mysql.h>
#endif

namespace GameServer::Database {

class MySQLDatabase : public ISqlDatabase {
public:
    MySQLDatabase();
    ~MySQLDatabase() override;

    bool Connect(const std::string& connectionString) override;
    void Disconnect() override;
    bool Execute(const std::string& query) override;

private:
#ifdef HAS_MYSQL
    MYSQL* _conn;
#endif
};

}
