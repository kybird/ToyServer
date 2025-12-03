#pragma once
#include <cstdint>

namespace GameServer::Packet {

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size;
    uint16_t id; // Packet ID
};
#pragma pack(pop)

}
