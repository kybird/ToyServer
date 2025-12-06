#include "ClientPacketHandler.h"
#include <iostream>
#include "Game.pb.h"
#include "Dispatcher/DefaultDispatcher.h"

namespace GameClient {

    void ClientPacketHandler::Init(std::shared_ptr<GameServer::Framework::DefaultDispatcher> dispatcher) {
        dispatcher->RegisterHandler(PacketID::S_LOGIN_RES, Handle_S_LOGIN_RES);
        dispatcher->RegisterHandler(PacketID::PKT_S_LOGIN_RES_PROTO, Handle_S_LOGIN_RES_PROTO);
        dispatcher->RegisterHandler(PacketID::S_PING, Handle_S_PING);
        dispatcher->RegisterHandler(PacketID::S_DISCONNECT, Handle_S_DISCONNECT);
    }

    void ClientPacketHandler::Handle_S_LOGIN_RES(std::shared_ptr<Session> session, const char* buffer, uint16_t size) {
        std::vector<uint8_t> vecBuffer(buffer, buffer + size);
        PacketStream stream(vecBuffer, sizeof(PacketHeader));

        PKT_S_LOGIN_RES pkt;
        pkt.Read(stream);

        std::cout << "Login Result: " << (pkt.success ? "Success" : "Failed") << ", PlayerID: " << pkt.playerId << std::endl;
    }

    void ClientPacketHandler::Handle_S_LOGIN_RES_PROTO(std::shared_ptr<Session> session, const char* buffer, uint16_t size) {
        const char* payload = buffer + sizeof(PacketHeader);
        uint16_t payloadSize = size - sizeof(PacketHeader);

        GameServer::Packet::S_LOGIN_RES_PROTO pkt;
        if (pkt.ParseFromArray(payload, payloadSize)) {
            std::cout << "[Protobuf] Login Result: " << (pkt.success() ? "Success" : "Failed") << ", PlayerID: " << pkt.player_id() << std::endl;
        }
    }

    void ClientPacketHandler::Handle_S_PING(std::shared_ptr<Session> session, const char* /*buffer*/, uint16_t /*size*/) {
        // Reply with Pong
        PKT_C_PONG pongPkt;
        // reusing MakeSendBuffer from Shared namespace which is effectively just a template helper
        auto sendBuffer = GameServer::Packet::ClientPacketHandler::MakeSendBuffer(PacketID::C_PONG, pongPkt);
        session->Send(sendBuffer);
        std::cout << "Ping received, Pong sent" << std::endl;
    }

    void ClientPacketHandler::Handle_S_DISCONNECT(std::shared_ptr<Session> session, const char* /*buffer*/, uint16_t /*size*/) {
        std::cout << "Server requested Disconnect. Closing connection..." << std::endl;
        session->ForceDisconnect();
        std::exit(0);
    }
}
