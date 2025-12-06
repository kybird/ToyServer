#pragma once
#include <cstdint>
#include <vector>
#include <memory>

namespace GameServer::Network {

class IPacket {
public:
    virtual ~IPacket() = default;

    virtual uint16_t GetId() const = 0;
    virtual uint16_t GetSize() const = 0;
    virtual void Serialize(std::vector<uint8_t>& buffer) const = 0;
    virtual void Deserialize(const uint8_t* buffer, uint16_t size) = 0;
};

}
