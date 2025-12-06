#pragma once
#include <vector>
#include <cstdint>
#include <cstddef>

namespace GameServer::Utils {

class XORUtils {
public:
    static void ApplyXOR(uint8_t* buffer, size_t length, const std::vector<uint8_t>& key) {
        if (key.empty() || length == 0) return;

        size_t keyLen = key.size();
        for (size_t i = 0; i < length; ++i) {
            buffer[i] ^= key[i % keyLen];
        }
    }

    static void ApplyXOR(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& key) {
        ApplyXOR(buffer.data(), buffer.size(), key);
    }
};

}
