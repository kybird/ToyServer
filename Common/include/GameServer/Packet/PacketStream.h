#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace GameServer::Packet {

class PacketStream {
public:
    PacketStream(std::vector<uint8_t>& buffer) : _buffer(buffer), _readPos(0) {}
    PacketStream(std::vector<uint8_t>& buffer, size_t offset) : _buffer(buffer), _readPos(offset) {}

    // Writer
    template<typename T>
    PacketStream& operator<<(const T& data) {
        size_t size = sizeof(T);
        size_t oldSize = _buffer.size();
        _buffer.resize(oldSize + size);
        std::memcpy(&_buffer[oldSize], &data, size);
        return *this;
    }

    PacketStream& operator<<(const std::string& data) {
        uint16_t size = static_cast<uint16_t>(data.size());
        *this << size;
        size_t oldSize = _buffer.size();
        _buffer.resize(oldSize + size);
        std::memcpy(&_buffer[oldSize], data.data(), size);
        return *this;
    }

    // Reader
    template<typename T>
    PacketStream& operator>>(T& data) {
        size_t size = sizeof(T);
        if (_readPos + size > _buffer.size()) return *this; // Error handling needed
        std::memcpy(&data, &_buffer[_readPos], size);
        _readPos += size;
        return *this;
    }

    PacketStream& operator>>(std::string& data) {
        uint16_t size = 0;
        *this >> size;
        if (_readPos + size > _buffer.size()) return *this;
        data.assign(reinterpret_cast<const char*>(&_buffer[_readPos]), size);
        _readPos += size;
        return *this;
    }

private:
    std::vector<uint8_t>& _buffer;
    size_t _readPos;
};

}
