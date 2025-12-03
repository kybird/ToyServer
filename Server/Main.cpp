#include <iostream>
#include "GameServer/Network/Service.h"
#include "GameServer/Network/Listener.h"
#include "GameServer/Network/PacketSession.h"
#include "GameServer/Packet/PacketDispatcher.h"
#include "GameServer/Packet/ClientPacketHandler.h"
#include "GameServer/Database/MySQLDatabase.h"
#include "GameServer/Database/MongoDBDatabase.h"
#include "GameServer/Database/RedisDatabase.h"
#include "GameServer/Repository/PlayerRepository.h"
#include "GameServer/Config/ConfigLoader.h"

using namespace GameServer::Network;
using namespace GameServer::Packet;
using namespace GameServer::Database;
using namespace GameServer::Repository;
using namespace GameServer::Config;

int main() {
    std::cout << "Starting Game Server..." << std::endl;

    // Load Configuration
    ServerConfig config = ConfigLoader::Load("ServerConfig.json");
    std::cout << "Loaded Config - Port: " << config.serverPort << std::endl;

    // Database & Repository Setup
    auto db = std::make_shared<MySQLDatabase>();
    if (db->Connect(config.mysqlConnectionString)) {
        auto playerRepo = std::make_shared<PlayerRepository>(db);
        
        // Test Save
        Player newPlayer{ 1, "NewUser", 10 };
        playerRepo->Save(newPlayer);

        // Test Find
        auto foundPlayer = playerRepo->Find(1);
        if (foundPlayer) {
            std::cout << "Found Player: " << foundPlayer->name << std::endl;
        }
    }

    // MongoDB Test
    auto mongoDb = std::make_shared<MongoDBDatabase>();
    if (mongoDb->Connect(config.mongoConnectionString)) {
        mongoDb->InsertOne("players", "{ \"id\": 1, \"name\": \"MongoUser\" }");
    }

    // Redis Test
    auto redisDb = std::make_shared<RedisDatabase>();
    if (redisDb->Connect(config.redisConnectionString)) {
        redisDb->Set("player:1", "RedisUser");
        std::cout << "Redis Get: " << redisDb->Get("player:1") << std::endl;
    }

    PacketDispatcher::Instance().RegisterHandler(PacketID::C_LOGIN, ClientPacketHandler::HandlePacket);

    Service service(1);
    
    auto sessionFactory = [](asio::io_context& ioContext) {
        return std::make_shared<PacketSession>(ioContext);
    };

    Listener listener(service.GetIOContext(), config.serverPort, sessionFactory);
    listener.Start();

    service.Start(); // This runs io_context.run() in threads

    // Keep main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
