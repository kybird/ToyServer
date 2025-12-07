#pragma once
#include "Network/PacketSession.h"
#include <iostream>

namespace GameClient {
    using namespace GameServer::Network;

    class ServerSession : public PacketSession {
    public:
        using PacketSession::PacketSession; // Use base constructors

        ~ServerSession() {
            std::cout << "~ServerSession" << std::endl;
        }

        virtual void OnConnected() override;
        virtual void OnDisconnected() override;

        void SendLoginRequest();
    };
}
