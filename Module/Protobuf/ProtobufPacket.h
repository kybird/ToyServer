#pragma once
#include "Network/IPacket.h"
#include <google/protobuf/message.h>
#include <stdexcept>

namespace GameServer::Module::Protobuf {

template <typename T>
class ProtobufPacket : public GameServer::Network::IPacket {
    static_assert(std::is_base_of<google::protobuf::Message, T>::value, "T must be a protobuf message");

public:
    ProtobufPacket() : _pkt(std::make_shared<T>()), _id(0) {}
    ProtobufPacket(uint16_t id) : _pkt(std::make_shared<T>()), _id(id) {}
    ProtobufPacket(uint16_t id, std::shared_ptr<T> pkt) : _pkt(pkt), _id(id) {}

    virtual uint16_t GetId() const override { return _id; }
    
    virtual uint16_t GetSize() const override { 
        return static_cast<uint16_t>(_pkt->ByteSizeLong()); 
    }

    virtual void Serialize(std::vector<uint8_t>& buffer) const override {
        size_t size = _pkt->ByteSizeLong();
        size_t oldSize = buffer.size();
        buffer.resize(oldSize + size);
        if (!_pkt->SerializeToArray(buffer.data() + oldSize, static_cast<int>(size))) {
            throw std::runtime_error("Failed to serialize protobuf message");
        }
    }

    virtual void Deserialize(const uint8_t* buffer, uint16_t size) override {
        if (!_pkt->ParseFromArray(buffer, size)) {
            throw std::runtime_error("Failed to deserialize protobuf message");
        }
    }

    std::shared_ptr<T> GetMessage() const { return _pkt; }

private:
    std::shared_ptr<T> _pkt;
    uint16_t _id;
};

}
