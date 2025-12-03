#include "GameServer/Network/Service.h"
#include "GameServer/Network/Listener.h"
#include "GameServer/Network/Session.h"
#include <iostream>
#include <string>

using namespace GameServer::Network;

class EchoSession : public Session {
public:
    using Session::Session;

protected:
    void OnConnected() override {
        std::cout << "Client Connected" << std::endl;
    }

    void OnDisconnected() override {
        std::cout << "Client Disconnected" << std::endl;
    }

    void OnRead(size_t bytesTransferred) override {
        std::string msg(_data, bytesTransferred);
        std::cout << "Received: " << msg;
        Send(msg);
    }
};

int main() {
    Service service(2);
    Listener listener(service.GetIOContext(), 8080, [](asio::io_context& io) {
        return std::make_shared<EchoSession>(io);
    });

    listener.Start();
    service.Start();

    std::cout << "Server started on port 8080..." << std::endl;
    std::cin.get(); 

    service.Stop();
    return 0;
}
