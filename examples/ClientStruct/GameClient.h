#pragma once
#include <memory>
#include <string>
#include "Network/Service.h"

namespace GameClient {
    class ServerSession;

    class GameClient {
    public:
        GameClient(std::string ip, uint16_t port, int threadCount = 1);
        ~GameClient();

        void Start();

    private:
        std::string _ip;
        uint16_t _port;
        GameServer::Network::Service _service;
        std::shared_ptr<ServerSession> _session;
    };
}
