#include "GameClient.h"
#include "ServerSession.h"
#include "ClientPacketHandler.h" // Local Handler
#include "Dispatcher/DefaultDispatcher.h"
#include "Network/Service.h"

namespace GameClient {
    using namespace GameServer::Network;
    using namespace GameServer::Framework;

    GameClient::GameClient(std::string ip, uint16_t port, int threadCount)
        : _ip(ip), _port(port), _service(threadCount) {
    }

    GameClient::~GameClient() {
    }

    void GameClient::Start() {
        // 1. Create Dispatcher and Register Handlers
        auto dispatcher = std::make_shared<DefaultDispatcher>();
        ClientPacketHandler::Init(dispatcher);

        // 2. Create Session
        _session = std::make_shared<ServerSession>(_service.GetIOContext(), dispatcher, _service.GetTimerManager(), _service.GetThreadPool(), _service.GetPacketCipher());

        // 3. Connect
        asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(_ip), _port);
        _session->GetSocket().async_connect(endpoint,
            [this](std::error_code ec) {
                if (!ec) {
                    _session->Start(); // Will trigger OnConnected -> SendLoginRequest
                } else {
                    std::cout << "Connect Failed: " << ec.message() << std::endl;
                }
            });

        // 4. Start Service (Non-blocking, runs IO threads)
        
        _service.Start();
    }
}
