#pragma once
#include <memory>
#include <vector>
#include "Network/Session.h"
#include "Packet/PacketHeader.h"
#include "Packet/PacketStream.h"
#include "Packet/ClientPacketHandler.h" // Access to Packet Enums and Structs from Shared

namespace GameServer::Framework {
    class DefaultDispatcher;
}

namespace GameClient {
    using namespace GameServer::Network;
    using namespace GameServer::Packet;

    class ClientPacketHandler {
    public:
        static void Init(std::shared_ptr<GameServer::Framework::DefaultDispatcher> dispatcher);

        static void Handle_S_LOGIN_RES(std::shared_ptr<Session> session, const char* buffer, uint16_t size);
        static void Handle_S_LOGIN_RES_PROTO(std::shared_ptr<Session> session, const char* buffer, uint16_t size);
        static void Handle_S_PING(std::shared_ptr<Session> session, const char* buffer, uint16_t size);
        static void Handle_S_DISCONNECT(std::shared_ptr<Session> session, const char* buffer, uint16_t size);
    };
}
