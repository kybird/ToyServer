#pragma once
#include "Packet/PacketHeader.h"
#include "Packet/PacketStream.h"
#include "Network/Session.h"
#include <string>
#include <iostream>

namespace GameServer::Packet {

enum PacketID : uint16_t {
    C_LOGIN = 1001,
    S_LOGIN_RES = 1002,
    PKT_C_LOGIN_PROTO = 2001,
    PKT_S_LOGIN_RES_PROTO = 2002,
};

struct PKT_C_LOGIN {
    std::string username;
    std::string password;

    void Write(PacketStream& stream) {
        stream << username << password;
    }

    void Read(PacketStream& stream) {
        stream >> username >> password;
    }
};

struct PKT_S_LOGIN_RES {
    bool success;
    uint32_t playerId;

    void Write(PacketStream& stream) {
        stream << success << playerId;
    }

    void Read(PacketStream& stream) {
        stream >> success >> playerId;
    }
};

class ClientPacketHandler {
public:
    static void HandlePacket(std::shared_ptr<Network::Session> session, const char* buffer, uint16_t size) {
        PacketHeader* header = (PacketHeader*)buffer;
        std::vector<uint8_t> vecBuffer(buffer, buffer + size);
        PacketStream stream(vecBuffer, sizeof(PacketHeader));

        switch (header->id) {
        case C_LOGIN:
            Handle_C_LOGIN(session, stream);
            break;
        default:
            std::cout << "Unknown Packet ID: " << header->id << std::endl;
            break;
        }
    }

    static void Handle_C_LOGIN(std::shared_ptr<Network::Session> session, PacketStream& stream) {
        PKT_C_LOGIN pkt;
        pkt.Read(stream);
        std::cout << "Login Request: " << pkt.username << "/" << pkt.password << std::endl;

        // Send Response
        PKT_S_LOGIN_RES resPkt;
        resPkt.success = true;
        resPkt.playerId = 12345;
        
        auto sendBuffer = MakeSendBuffer(S_LOGIN_RES, resPkt);
        session->Send(sendBuffer);
    }

    template<typename T>
    static std::vector<uint8_t> MakeSendBuffer(uint16_t id, T& pkt) {
        std::vector<uint8_t> buffer;
        buffer.resize(sizeof(PacketHeader));
        
        PacketStream stream(buffer);
        // Reset size to header size before writing body, but PacketStream appends.
        // Actually PacketStream constructor takes buffer ref.
        // We need to handle the offset.
        // Let's just use PacketStream to write payload, then fill header.
        
        // Re-create stream pointing to end? No, PacketStream appends.
        // So buffer already has space for header.
        // We need to skip header for stream writing?
        // My PacketStream implementation appends to the end.
        // So if we resize(sizeof(PacketHeader)), the stream will append AFTER it.
        // That's correct.
        
        pkt.Write(stream);

        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer.data());
        header->size = static_cast<uint16_t>(buffer.size());
        header->id = id;

        return buffer;
    }
};

}
