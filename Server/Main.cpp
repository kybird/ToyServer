#include <iostream>
#include "Network/Service.h"
#include "Network/Listener.h"
#include "Network/PacketSession.h"
#include "Dispatcher/DefaultDispatcher.h"
#include "Packet/ClientPacketHandler.h"
#include "MySQLDatabase.h"
#include "MongoDBDatabase.h"
#include "RedisDatabase.h"
#include "Repository/PlayerRepository.h"
#include "Config/JsonConfigLoader.h"
#include "Logger/Logger.h"
#include "Protobuf/ProtobufPacket.h"
#include "Game.pb.h"

using namespace GameServer::Network;
using namespace GameServer::Packet;
using namespace GameServer::Database;
using namespace GameServer::Repository;
using namespace GameServer::Config;

void Handle_C_LOGIN_PROTO(std::shared_ptr<Session> session, const char* buffer, uint16_t size) {
    // Skip header
    const char* payload = buffer + sizeof(PacketHeader);
    uint16_t payloadSize = size - sizeof(PacketHeader);

    GameServer::Packet::C_LOGIN_PROTO pkt;
    if (pkt.ParseFromArray(payload, payloadSize)) {
        LogInfo("[Protobuf] Login Request: {}/{}", pkt.username(), pkt.password());

        // Send Response
        GameServer::Packet::S_LOGIN_RES_PROTO resPkt;
        resPkt.set_success(true);
        resPkt.set_player_id(9999);

        GameServer::Module::Protobuf::ProtobufPacket<GameServer::Packet::S_LOGIN_RES_PROTO> protoPacket(PacketID::PKT_S_LOGIN_RES_PROTO, std::make_shared<GameServer::Packet::S_LOGIN_RES_PROTO>(resPkt));
        
        std::vector<uint8_t> sendBuffer;
        // Reserve space for header
        sendBuffer.resize(sizeof(PacketHeader));
        
        // Serialize body
        protoPacket.Serialize(sendBuffer);

        // Fill header
        PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer.data());
        header->size = static_cast<uint16_t>(sendBuffer.size());
        header->id = PacketID::PKT_S_LOGIN_RES_PROTO;

        session->Send(sendBuffer);
    } else {
        LogError("[Protobuf] Failed to parse C_LOGIN_PROTO");
    }
}

#include "Packet/IPacketFactory.h"

// Test Packet
struct TestPacket {
    int x;
    TestPacket(int val) : x(val) { LogInfo("TestPacket Constructed: {}", x); }
    ~TestPacket() { LogInfo("TestPacket Destructed: {}", x); }
};

void TestPacketFactory() {
    LogInfo("--- Packet Factory Test Start ---");
    
    // 1. Create packet A
    void* addrA = nullptr;
    {

        auto packetA = GameServer::Framework::Packet::IPacketFactory::CreatePacket<TestPacket>(10);
        addrA = packetA.get();
        LogInfo("Packet A Address: {}, Value: {}", addrA, packetA->x);
        // Scope ends, packetA destroyed and returned to pool
    }

    // 2. Create packet B (Should reuse A's memory)
    void* addrB = nullptr;
    {
        auto packetB = GameServer::Framework::Packet::IPacketFactory::CreatePacket<TestPacket>(20);
        addrB = packetB.get();
        LogInfo("Packet B Address: {}, Value: {}", addrB, packetB->x);

        if (addrA == addrB) {
            LogInfo("SUCCESS: Memory Reused!");
        } else {
            LogError("FAILURE: Memory NOT Reused!");
        }
    }

    LogInfo("--- Packet Factory Test End ---");
}

int main() {
    try {
        GameServer::Utils::Logger::Init();

        // Run verification
        TestPacketFactory();

        // ASCII Art Banner
        LogInfo(R"(
___ ____ _   _ ____ ____ ____ _  _ ____ ____ 
 |  |  |  \_/  [__  |___ |__/ |  | |___ |__/ 
 |  |__|   |   ___] |___ |  \  \/  |___ |  \ 
)
=============================================
Starting Game Server...
)");
 
        LogInfo("Loading configuration...");
        JsonConfigLoader loader;
        ServerConfig config = loader.Load("ServerConfig.json");
        LogInfo("Loaded Config - Port: {}, Threads: {}", config.server.port, config.server.threadCount);

    // Database & Repository Setup
    auto db = std::make_shared<MySQLDatabase>();
    if (db->Connect(config.database.mysql.ToConvertionalString())) {
        auto playerRepo = std::make_shared<PlayerRepository>(db);
        
        // Test Save
        Player newPlayer{ 1, "NewUser", 10 };
        playerRepo->Save(newPlayer);

        // Test Find
        auto foundPlayer = playerRepo->Find(1);
                if (foundPlayer) {
            LogInfo("Found Player: {}", foundPlayer->name);
        }
    }


    // MongoDB Test
    auto mongoDb = std::make_shared<MongoDBDatabase>();
    if (mongoDb->Connect(config.database.mongo.ToConvertionalString())) {
        mongoDb->InsertOne("players", "{ \"id\": 1, \"name\": \"MongoUser\" }");
    }

    // Redis Test
    auto redisDb = std::make_shared<RedisDatabase>();
    if (redisDb->Connect(config.database.redis.ToConvertionalString())) {
        redisDb->Set("player:1", "RedisUser");
        LogInfo("Redis Get: {}", redisDb->Get("player:1"));
    }


    auto dispatcher = std::make_shared<GameServer::Framework::DefaultDispatcher>();
    dispatcher->RegisterHandler(PacketID::C_LOGIN, ClientPacketHandler::HandlePacket);
    dispatcher->RegisterHandler(PacketID::PKT_C_LOGIN_PROTO, Handle_C_LOGIN_PROTO);

    Service service(config.server.threadCount); // Use Config ThreadCount
    
    auto sessionFactory = [&service, dispatcher](asio::io_context& ioContext) {
        return std::make_shared<PacketSession>(ioContext, dispatcher, service.GetThreadPool(), service.GetPacketCipher());
    };

    Listener listener(service.GetIOContext(), config.server.port, sessionFactory);
    listener.Start();

    auto timerManager = service.GetTimerManager();
    timerManager->ScheduleRepeat(std::chrono::seconds(5), []() {
        LogInfo("[Timer] 5 seconds tick!");
    });

    service.Start(); // This runs io_context.run() in threads

    // Keep main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    } catch (const std::exception& e) {
        LogError("Exception: {}", e.what());
        return 1;
    } catch (...) {
        LogError("Unknown exception occurred!");
        return 1;
    }

    return 0;
}
