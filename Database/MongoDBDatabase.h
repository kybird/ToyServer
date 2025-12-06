#pragma once
#include <string>
#include <memory>

#ifdef HAS_MONGO
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#endif

namespace GameServer::Database {

class MongoDBDatabase {
public:
    MongoDBDatabase();
    
    bool Connect(const std::string& connectionString);
    void Disconnect();
    void InsertOne(const std::string& collection, const std::string& json);

private:
#ifdef HAS_MONGO
    std::unique_ptr<mongocxx::client> _client;
    mongocxx::database _db;
#endif
};

}
