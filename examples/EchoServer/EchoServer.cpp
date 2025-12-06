#include <iostream>
#include <string>
#include <memory>
#include "Network/Service.h"
#include "Network/Listener.h"
#include "Network/Session.h"
#include "Network/SessionManager.h"

using namespace GameServer::Network;

class EchoSession : public Session {
public:
    using Session::Session;

protected:
    void OnConnected() override {
        std::cout << "Client Connected: " << GetSocket().remote_endpoint() << std::endl;
    }

    void OnDisconnected() override {
        std::cout << "Client Disconnected" << std::endl;
    }

    size_t OnRecv(RecvBuffer& buffer) override {
        size_t len = buffer.DataSize();
        std::string msg(reinterpret_cast<const char*>(buffer.ReadPos()), len);
        std::cout << "Received: " << msg << std::endl;
        
        // Echo back
        Send(msg);
        return len; // Processed all
    }
};

int main() {
    try {
        Service service(2);
        Listener listener(service.GetIOContext(), 8080, [](asio::io_context& io) {
            return std::make_shared<EchoSession>(io);
        });

        listener.Start();
        service.Start();

        std::cout << "EchoServer started on port 8080..." << std::endl;
        
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "Current Sessions: " << SessionManager::Instance().GetSessionCount() << std::endl;
        }

        service.Stop();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
