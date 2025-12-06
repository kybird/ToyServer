#include "MySQLDatabase.h"
#include "Logger/Logger.h"
#include <iostream>

namespace GameServer::Database {

MySQLDatabase::MySQLDatabase() {
#ifdef HAS_MYSQL
    _conn = mysql_init(nullptr);
#endif
}

MySQLDatabase::~MySQLDatabase() {
    Disconnect();
}

bool MySQLDatabase::Connect(const std::string& connectionString) {
#ifdef HAS_MYSQL
    // Simple parsing of connection string for demo purposes
    // Format: host=localhost;user=root;password=pass;db=game
    // In reality, we'd parse this properly.
    (void)connectionString; // Suppress unused warning for now
    if (mysql_real_connect(_conn, "localhost", "root", "password", "game", 0, nullptr, 0)) {
        LogInfo("Connected to MySQL!");
        return true;
    } else {
        LogWarn("MySQL Connection Failed: {}", mysql_error(_conn));
        return false;
    }
#else
    (void)connectionString;
    LogInfo("[Mock] Connected to MySQL (Library not linked)");
    return true;
#endif
}

void MySQLDatabase::Disconnect() {
#ifdef HAS_MYSQL
    if (_conn) {
        mysql_close(_conn);
        _conn = nullptr;
    }
#endif
}

bool MySQLDatabase::Execute(const std::string& query) {
#ifdef HAS_MYSQL
    if (mysql_query(_conn, query.c_str())) {
        LogError("MySQL Query Failed: {}", mysql_error(_conn));
        return false;
    }
    return true;
#else
    LogInfo("[Mock] Executing Query: {}", query);
    return true;
#endif
}

}
