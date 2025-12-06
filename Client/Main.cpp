#include <iostream>
#include "Network/Service.h"
#include "Network/Session.h"
#include "Network/PacketSession.h"
#include "Packet/PacketHeader.h"
#include "Dispatcher/DefaultDispatcher.h"
#include "Packet/ClientPacketHandler.h"
#include "Protobuf/ProtobufPacket.h"
#include "Game.pb.h"

using namespace GameServer::Network;
using namespace GameServer::Packet;

void OnLoginResponse(std::shared_ptr<Session> session, const char* buffer, uint16_t size) {
    std::vector<uint8_t> vecBuffer(buffer, buffer + size);
    PacketStream stream(vecBuffer, sizeof(PacketHeader));
    
    PKT_S_LOGIN_RES pkt;
    pkt.Read(stream);
    
    std::cout << "Login Result: " << (pkt.success ? "Success" : "Failed") << ", PlayerID: " << pkt.playerId << std::endl;
}

void OnLoginResponseProto(std::shared_ptr<Session> session, const char* buffer, uint16_t size) {
    const char* payload = buffer + sizeof(PacketHeader);
    uint16_t payloadSize = size - sizeof(PacketHeader);

    GameServer::Packet::S_LOGIN_RES_PROTO pkt;
    if (pkt.ParseFromArray(payload, payloadSize)) {
        std::cout << "[Protobuf] Login Result: " << (pkt.success() ? "Success" : "Failed") << ", PlayerID: " << pkt.player_id() << std::endl;
    }
}

int main() {
    std::cout << "Starting Game Client..." << std::endl;

    auto dispatcher = std::make_shared<GameServer::Framework::DefaultDispatcher>();
    dispatcher->RegisterHandler(PacketID::S_LOGIN_RES, OnLoginResponse);
    dispatcher->RegisterHandler(PacketID::PKT_S_LOGIN_RES_PROTO, OnLoginResponseProto);

    Service service(1);
    auto session = std::make_shared<PacketSession>(service.GetIOContext(), dispatcher, service.GetThreadPool(), service.GetPacketCipher());

    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 4242);
    session->GetSocket().async_connect(endpoint,
        [session](std::error_code ec) {
            if (!ec) {
                std::cout << "Connected to Server!" << std::endl;
                session->Start();

                // Send legacy packet
                PKT_C_LOGIN pkt;
                pkt.username = "test_user";
                pkt.password = "password123";
                auto buffer = ClientPacketHandler::MakeSendBuffer(PacketID::C_LOGIN, pkt);
                session->Send(buffer);

                // Send Protobuf packet
                GameServer::Packet::C_LOGIN_PROTO protoPkt;
                protoPkt.set_username("proto_user");
                protoPkt.set_password("proto_pass");

                GameServer::Module::Protobuf::ProtobufPacket<GameServer::Packet::C_LOGIN_PROTO> packet(PacketID::PKT_C_LOGIN_PROTO, std::make_shared<GameServer::Packet::C_LOGIN_PROTO>(protoPkt));
                
                std::vector<uint8_t> sendBuffer;
                sendBuffer.resize(sizeof(PacketHeader));
                packet.Serialize(sendBuffer);

                PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer.data());
                header->size = static_cast<uint16_t>(sendBuffer.size());
                header->id = PacketID::PKT_C_LOGIN_PROTO;

                session->Send(sendBuffer);

            } else {
                std::cout << "Connect Failed: " << ec.message() << std::endl;
            }
        });

    service.Start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
