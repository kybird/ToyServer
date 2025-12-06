#pragma once
#include <vector>
#include <cstdint>

namespace GameServer::Packet {

class IPacketCipher {
public:
    virtual ~IPacketCipher() = default;

    virtual void Encrypt(std::vector<uint8_t>& buffer) = 0;
    virtual void Decrypt(std::vector<uint8_t>& buffer) = 0;
};

}
