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

        // 4. Start Service (blocking if single thread, but usually Service::Start is blocking unless we run it in a thread?
        // Service::Start calls io_context.run(). If we want to keep main thread free or controll loop, we should check Service impl.
        // Existing Main.cpp calls service.Start() then loops.
        // Service::Start() in Framework likely just runs the threads?
        // Let's assume standard behavior: Service::Start starts the IO threads.
        
        _service.Start();
    }
}
