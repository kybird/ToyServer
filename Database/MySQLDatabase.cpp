#include "MySQLDatabase.h"
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
        std::cout << "Connected to MySQL!" << std::endl;
        return true;
    } else {
        std::cerr << "MySQL Connection Failed: " << mysql_error(_conn) << std::endl;
        return false;
    }
#else
    (void)connectionString;
    std::cout << "[Mock] Connected to MySQL (Library not linked)" << std::endl;
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
        std::cerr << "MySQL Query Failed: " << mysql_error(_conn) << std::endl;
        return false;
    }
    return true;
#else
    std::cout << "[Mock] Executing Query: " << query << std::endl;
    return true;
#endif
}

}
