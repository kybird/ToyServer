#pragma once
#include "GameServer/Network/Session.h"
#include "GameServer/Packet/PacketDispatcher.h"

namespace GameServer::Network {

class PacketSession : public Session {
public:
    using Session::Session; // Inherit constructors

protected:
    virtual void OnPacket(const uint8_t* buffer, size_t len) override {
        Packet::PacketDispatcher::Instance().Dispatch(shared_from_this(), reinterpret_cast<const char*>(buffer), static_cast<uint16_t>(len));
    }
};

}
