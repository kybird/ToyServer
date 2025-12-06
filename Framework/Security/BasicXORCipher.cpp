#include "BasicXORCipher.h"
#include "Utils/XORUtils.h"

namespace GameServer::Security {

BasicXORCipher::BasicXORCipher(const std::string& key) {
    _key.assign(key.begin(), key.end());
}

void BasicXORCipher::Encrypt(std::vector<uint8_t>& buffer) {
    GameServer::Utils::XORUtils::ApplyXOR(buffer, _key);
}

void BasicXORCipher::Decrypt(std::vector<uint8_t>& buffer) {
    GameServer::Utils::XORUtils::ApplyXOR(buffer, _key);
}

}
