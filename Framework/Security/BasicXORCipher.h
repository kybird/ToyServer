#pragma once
#include "Packet/IPacketCipher.h"
#include <vector>
#include <string>

namespace GameServer::Security {

class BasicXORCipher : public GameServer::Packet::IPacketCipher {
public:
    BasicXORCipher(const std::string& key = "DefaultKey");
    virtual ~BasicXORCipher() = default;

    virtual void Encrypt(std::vector<uint8_t>& buffer) override;
    virtual void Decrypt(std::vector<uint8_t>& buffer) override;

private:
    std::vector<uint8_t> _key;
};

}
