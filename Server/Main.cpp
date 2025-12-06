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
#include "Config/ConfigLoader.h"
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
        std::cout << "[Protobuf] Login Request: " << pkt.username() << "/" << pkt.password() << std::endl;

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
        std::cout << "[Protobuf] Failed to parse C_LOGIN_PROTO" << std::endl;
    }
}

#include "Packet/IPacketFactory.h"

// Test Packet
struct TestPacket {
    int x;
    TestPacket(int val) : x(val) { std::cout << "TestPacket Constructed: " << x << std::endl; }
    ~TestPacket() { std::cout << "TestPacket Destructed: " << x << std::endl; }
};

void TestPacketFactory() {
    std::cout << "--- Packet Factory Test Start ---\n";
    
    // 1. Create packet A
    void* addrA = nullptr;
    {
        auto packetA = GameServer::Framework::Packet::IPacketFactory::CreatePacket<TestPacket>(10);
        addrA = packetA.get();
        std::cout << "Packet A Address: " << addrA << ", Value: " << packetA->x << std::endl;
        // Scope ends, packetA destroyed and returned to pool
    }

    // 2. Create packet B (Should reuse A's memory)
    void* addrB = nullptr;
    {
        auto packetB = GameServer::Framework::Packet::IPacketFactory::CreatePacket<TestPacket>(20);
        addrB = packetB.get();
        std::cout << "Packet B Address: " << addrB << ", Value: " << packetB->x << std::endl;

        if (addrA == addrB) {
            std::cout << "SUCCESS: Memory Reused!" << std::endl;
        } else {
            std::cout << "FAILURE: Memory NOT Reused!" << std::endl;
        }
    }

    std::cout << "--- Packet Factory Test End ---\n";
}

int main() {
    try {
        // Run verification
        TestPacketFactory();

        // ASCII Art Banner
        std::cout << R"(
___ ____ _   _ ____ ____ ____ _  _ ____ ____ 
 |  |  |  \_/  [__  |___ |__/ |  | |___ |__/ 
 |  |__|   |   ___] |___ |  \  \/  |___ |  \ 
)" << std::endl;
        std::cout << "=============================================" << std::endl;
        std::cout << "Starting Game Server..." << std::endl;
        std::cout.flush();

        // Load Configuration
        std::cout << "Loading configuration..." << std::endl;
        std::cout.flush();
        ServerConfig config = ConfigLoader::Load("ServerConfig.json");
        std::cout << "Loaded Config - Port: " << config.serverPort << std::endl;
        std::cout.flush();

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


    auto dispatcher = std::make_shared<GameServer::Framework::DefaultDispatcher>();
    dispatcher->RegisterHandler(PacketID::C_LOGIN, ClientPacketHandler::HandlePacket);
    dispatcher->RegisterHandler(PacketID::PKT_C_LOGIN_PROTO, Handle_C_LOGIN_PROTO);

    Service service(4); // Use 4 threads (1 IO + 4 Logic)
    
    auto sessionFactory = [&service, dispatcher](asio::io_context& ioContext) {
        return std::make_shared<PacketSession>(ioContext, dispatcher, service.GetThreadPool(), service.GetPacketCipher());
    };

    Listener listener(service.GetIOContext(), config.serverPort, sessionFactory);
    listener.Start();

    auto timerManager = service.GetTimerManager();
    timerManager->ScheduleRepeat(std::chrono::seconds(5), []() {
        std::cout << "[Timer] 5 seconds tick!" << std::endl;
    });

    service.Start(); // This runs io_context.run() in threads

    // Keep main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred!" << std::endl;
        return 1;
    }

    return 0;
}
