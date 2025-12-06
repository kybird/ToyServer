#pragma once
#include <cstdint>
#include "Packet/PacketStream.h"

namespace GameServer::Framework {

class IPacket {
public:
    virtual ~IPacket() = default;
    virtual uint16_t GetID() const = 0;
    virtual void Serialize(Packet::PacketStream& stream) = 0;
    virtual void Deserialize(Packet::PacketStream& stream) = 0;
};

}
