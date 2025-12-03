#include <iostream>
#include "GameServer/Network/Service.h"
#include "GameServer/Network/PacketSession.h"
#include "GameServer/Packet/PacketHeader.h"
#include "GameServer/Packet/PacketDispatcher.h"
#include "GameServer/Packet/ClientPacketHandler.h"

using namespace GameServer::Network;
using namespace GameServer::Packet;

void OnLoginResponse(std::shared_ptr<Session> session, const char* buffer, uint16_t size) {
    std::vector<uint8_t> vecBuffer(buffer, buffer + size);
    PacketStream stream(vecBuffer, sizeof(PacketHeader));
    
    PKT_S_LOGIN_RES pkt;
    pkt.Read(stream);
    
    std::cout << "Login Result: " << (pkt.success ? "Success" : "Failed") << ", PlayerID: " << pkt.playerId << std::endl;
}

int main() {
    std::cout << "Starting Game Client..." << std::endl;

    PacketDispatcher::Instance().RegisterHandler(PacketID::S_LOGIN_RES, OnLoginResponse);

    Service service(1);
    auto session = std::make_shared<PacketSession>(service.GetIOContext());

    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 4242);
    session->GetSocket().async_connect(endpoint,
        [session](std::error_code ec) {
            if (!ec) {
                std::cout << "Connected to Server!" << std::endl;
                session->Start();

                PKT_C_LOGIN pkt;
                pkt.username = "test_user";
                pkt.password = "password123";

                auto buffer = ClientPacketHandler::MakeSendBuffer(PacketID::C_LOGIN, pkt);
                session->Send(buffer);
            } else {
                std::cout << "Connect Failed: " << ec.message() << std::endl;
            }
        });

    service.Start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
