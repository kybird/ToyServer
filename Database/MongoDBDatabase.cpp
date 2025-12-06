#include "MongoDBDatabase.h"
#include <iostream>

namespace GameServer::Database {

MongoDBDatabase::MongoDBDatabase() {
#ifdef HAS_MONGO
    // mongocxx::instance must exist for the lifetime of the application.
    // Making it static ensures it is created once and destroyed at program exit.
    static mongocxx::instance instance{};
#endif
}

bool MongoDBDatabase::Connect(const std::string& connectionString) {
#ifdef HAS_MONGO
    try {
        _client = std::make_unique<mongocxx::client>(mongocxx::uri{connectionString});
        // 연결 검증 - ping 명령 실행
        auto admin = (*_client)["admin"];
        admin.run_command(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("ping", 1)));
        _db = (*_client)["game_db"];
        std::cout << "Connected to MongoDB!" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "MongoDB Connection Failed: " << e.what() << std::endl;
        _client.reset();
        return false;
    }
#else
    std::cout << "[Mock] Connected to MongoDB (Library not linked)" << std::endl;
    return true;
#endif
}

void MongoDBDatabase::Disconnect() {
    // MongoDB 클라이언트가 연결 풀링을 자동 관리
#ifdef HAS_MONGO
    _client.reset();
#endif
}

void MongoDBDatabase::InsertOne(const std::string& collection, const std::string& json) {
#ifdef HAS_MONGO
    if (!_client) return;
    // bsoncxx 사용 구현
    (void)collection;
    (void)json;
#else
    (void)collection;
    (void)json;
    std::cout << "[Mock] MongoDB InsertOne into " << collection << ": " << json << std::endl;
#endif
}

}
