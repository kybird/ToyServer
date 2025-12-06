#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include "Network/Service.h"
#include "Network/Session.h"

using namespace GameServer::Network;

class EchoClientSession : public Session {
public:
    using Session::Session;

protected:
    void OnConnected() override {
        std::cout << "Connected to EchoServer!" << std::endl;
        std::string msg = "Hello Echo Server!";
        Send(msg);
    }

    void OnDisconnected() override {
        std::cout << "Disconnected from EchoServer" << std::endl;
    }

    size_t OnRecv(RecvBuffer& buffer) override {
        size_t len = buffer.DataSize();
        std::string msg(reinterpret_cast<const char*>(buffer.ReadPos()), len);
        std::cout << "Received: " << msg << std::endl;
        return len;
    }
};

int main() {
    try {
        Service service(1);
        auto session = std::make_shared<EchoClientSession>(service.GetIOContext());

        asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 8080);
        session->GetSocket().async_connect(endpoint,
            [session](std::error_code ec) {
                if (!ec) {
                    session->Start();
                } else {
                    std::cout << "Connect Failed: " << ec.message() << std::endl;
                }
            });

        service.Start();

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
