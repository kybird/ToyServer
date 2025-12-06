#pragma once
#include <memory>
#include <cstdint>

namespace GameServer::Network {
    class Session;
}

namespace GameServer::Framework {

class IDispatcher {
public:
    virtual ~IDispatcher() = default;
    virtual void Dispatch(std::shared_ptr<Network::Session> session, const char* buffer, uint16_t size) = 0;
};

}
