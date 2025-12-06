#pragma once
#include "Dispatcher/IDispatcher.h"
#include "Network/Session.h"
#include "Packet/PacketHeader.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace GameServer::Framework {

using PacketHandler = std::function<void(std::shared_ptr<Network::Session>, const char* buffer, uint16_t size)>;

class DefaultDispatcher : public IDispatcher {
public:
    void RegisterHandler(uint16_t id, PacketHandler handler) {
        _handlers[id] = handler;
    }

    void Dispatch(std::shared_ptr<Network::Session> session, const char* buffer, uint16_t size) override {
        using namespace GameServer::Packet; // For PacketHeader if needed, though we use reinterpret_cast directly
        
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
