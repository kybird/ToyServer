#pragma once
#include <string>
#include <iostream>
#include <memory>

#ifdef HAS_MONGO
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#endif

namespace GameServer::Database {

class MongoDBDatabase {
public:
    MongoDBDatabase() {
#ifdef HAS_MONGO
        mongocxx::instance instance{}; // This should be global ideally
#endif
    }

    bool Connect(const std::string& connectionString) {
#ifdef HAS_MONGO
        try {
            _client = mongocxx::client{mongocxx::uri{connectionString}};
            _db = _client["game_db"];
            std::cout << "Connected to MongoDB!" << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "MongoDB Connection Failed: " << e.what() << std::endl;
            return false;
        }
#else
        std::cout << "[Mock] Connected to MongoDB (Library not linked)" << std::endl;
        return true;
#endif
    }

    void Disconnect() {
        // MongoDB client handles connection pooling automatically
    }



    // Specific MongoDB methods
    void InsertOne(const std::string& collection, const std::string& json) {
#ifdef HAS_MONGO
        // Implementation using bsoncxx
#else
        std::cout << "[Mock] MongoDB InsertOne into " << collection << ": " << json << std::endl;
#endif
    }

private:
#ifdef HAS_MONGO
    mongocxx::client _client;
    mongocxx::database _db;
#endif
};

}
