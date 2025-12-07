#include "ServerSession.h"
#include "Packet/ClientPacketHandler.h"
#include "Game.pb.h"
#include "Protobuf/ProtobufPacket.h"

namespace GameClient {
    using namespace GameServer::Packet;

    void ServerSession::OnConnected() {
        std::cout << "Connected to Server!" << std::endl;
        SendLoginRequest();
    }

    void ServerSession::OnDisconnected() {
        std::cout << "Disconnected from Server!" << std::endl;
    }



    void ServerSession::SendLoginRequest() {
        // 2. Send Protobuf Packet
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

        Send(sendBuffer);
    }
}
