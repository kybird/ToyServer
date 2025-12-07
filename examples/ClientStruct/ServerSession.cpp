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
        // 1. Send Legacy Packet (Struct)
        PKT_C_LOGIN pkt;
        pkt.username = "test_user";
        pkt.password = "password123";
        // Using Shared ClientPacketHandler helper for creating buffer
        auto buffer = GameServer::Packet::ClientPacketHandler::MakeSendBuffer(PacketID::C_LOGIN, pkt);
        Send(buffer);
    }
}
