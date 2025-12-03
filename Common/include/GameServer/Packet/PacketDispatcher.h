#pragma once
#include "GameServer/Network/Session.h"
#include "GameServer/Packet/PacketHeader.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace GameServer::Packet {

using PacketHandler = std::function<void(std::shared_ptr<Network::Session>, const char* buffer, uint16_t size)>;

class PacketDispatcher {
public:
    static PacketDispatcher& Instance() {
        static PacketDispatcher instance;
        return instance;
    }

    void RegisterHandler(uint16_t id, PacketHandler handler) {
        _handlers[id] = handler;
    }

    void Dispatch(std::shared_ptr<Network::Session> session, const char* buffer, uint16_t size) {
        if (size < sizeof(PacketHeader)) return;

        const PacketHeader* header = reinterpret_cast<const PacketHeader*>(buffer);
        auto it = _handlers.find(header->id);
        if (it != _handlers.end()) {
            it->second(session, buffer, size);
        } else {
            std::cout << "Unknown Packet ID: " << header->id << std::endl;
        }
    }

private:
    std::unordered_map<uint16_t, PacketHandler> _handlers;
};

}
