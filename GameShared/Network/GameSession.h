#pragma once
#include "Network/PacketSession.h"
#include "Packet/ClientPacketHandler.h"
#include <iostream>

namespace GameServer::GameShared {

class GameSession : public GameServer::Network::PacketSession {
public:
    using GameServer::Network::PacketSession::PacketSession; // Inherit constructors

    virtual void SendHeartbeat() override {
        // Create Ping Packet
        GameServer::Packet::PKT_S_PING pingPkt;
        auto sendBuffer = GameServer::Packet::ClientPacketHandler::MakeSendBuffer(GameServer::Packet::PacketID::S_PING, pingPkt);
        Send(sendBuffer);
    }

    virtual void OnRequestGracefulDisconnect() override {
        GameServer::Packet::PKT_S_DISCONNECT pkt;
        auto sendBuffer = GameServer::Packet::ClientPacketHandler::MakeSendBuffer(GameServer::Packet::PacketID::S_DISCONNECT, pkt);
        Send(sendBuffer);
        // Do NOT call ForceDisconnect here. Wait for client to close or Timer to fire.
    }
};

}
