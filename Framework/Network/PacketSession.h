#pragma once
#include "Network/Session.h"
#include "Dispatcher/IDispatcher.h"
#include "Thread/IThreadPool.h"
#include "Packet/IPacketCipher.h"

namespace GameServer::Network {

class PacketSession : public Session {
public:
    PacketSession(asio::io_context& ioContext, std::shared_ptr<Framework::IDispatcher> dispatcher, std::shared_ptr<Framework::IThreadPool> threadPool = nullptr, std::shared_ptr<GameServer::Packet::IPacketCipher> cipher = nullptr)
        : Session(ioContext, threadPool), _dispatcher(dispatcher), _cipher(cipher) {}

    virtual void Send(const std::vector<uint8_t>& msg) override {
        if (_cipher) {
            std::vector<uint8_t> encryptedMsg = msg;
            // Skip header? Assuming header is first few bytes.
            // PacketHeader is sizeof(PacketHeader).
            // We should only encrypt the body.
            // But wait, msg includes header.
            // Let's assume standard packet format: Header + Body.
            // Header size is fixed.
            
            // Need to know header size. PacketHeader is in GameShared/Packet/PacketHeader.h
            // But PacketSession is in Framework.
            // Framework should ideally not depend on GameShared specific header size if possible, or define it in Framework.
            // However, PacketHeader is currently in GameShared.
            // Let's assume a fixed header size or pass it?
            // For now, let's just encrypt the whole thing or skip a fixed size.
            // The user said "BasicXORCipher... Encrypt and Decrypt methods".
            // If we encrypt the header, the length might be garbled if it's not encrypted carefully (e.g. length field).
            // Usually we don't encrypt the length field so we can know how much to read.
            // Let's assume we skip the header.
            // How do we know header size here?
            // We can include "Packet/PacketHeader.h" if it's available to Framework.
            // It is in GameShared. Framework depends on GameShared? No, GameShared depends on Framework.
            // So Framework cannot include GameShared headers.
            // This is a dependency issue.
            // PacketHeader should probably be in Framework or common.
            // Or we just encrypt everything and rely on the fact that we read the length before decrypting?
            // Wait, if we encrypt the length, we can't read the packet size to know when the packet ends.
            // So we MUST NOT encrypt the length field.
            // We need to know the offset of the body.
            // Let's define a constant or virtual method for header size?
            // Or just hardcode 4 bytes (id + size) for now as per previous observation.
            // PacketHeader struct had id(uint16) and size(uint16). So 4 bytes.
            
            size_t headerSize = 4; 
            if (encryptedMsg.size() > headerSize) {
                std::vector<uint8_t> body(encryptedMsg.begin() + headerSize, encryptedMsg.end());
                _cipher->Encrypt(body);
                std::copy(body.begin(), body.end(), encryptedMsg.begin() + headerSize);
            }
            Session::Send(encryptedMsg);
        } else {
            Session::Send(msg);
        }
    }

protected:
    virtual void OnPacket(const uint8_t* buffer, size_t len) override {
        if (_dispatcher) {
            // Copy buffer to vector to ensure lifetime in lambda
            std::vector<uint8_t> packetData(buffer, buffer + len);
            auto session = std::static_pointer_cast<PacketSession>(shared_from_this());
            
            if (_threadPool) {
                _threadPool->Post([this, session, packetData = std::move(packetData)]() mutable {
                    if (_cipher) {
                        size_t headerSize = 4;
                        if (packetData.size() > headerSize) {
                            std::vector<uint8_t> body(packetData.begin() + headerSize, packetData.end());
                            _cipher->Decrypt(body);
                            std::copy(body.begin(), body.end(), packetData.begin() + headerSize);
                        }
                    }
                    _dispatcher->Dispatch(session, reinterpret_cast<const char*>(packetData.data()), static_cast<uint16_t>(packetData.size()));
                });
            } else {
                // Decrypt in place if no thread pool (though we should always have one now)
                 if (_cipher) {
                    // ... same logic ...
                    // For brevity, let's assume ThreadPool is always used or duplicate logic.
                    // Actually, let's just duplicate for correctness if threadpool is missing.
                    std::vector<uint8_t> decryptedData(buffer, buffer + len);
                    size_t headerSize = 4;
                    if (decryptedData.size() > headerSize) {
                        std::vector<uint8_t> body(decryptedData.begin() + headerSize, decryptedData.end());
                        _cipher->Decrypt(body);
                        std::copy(body.begin(), body.end(), decryptedData.begin() + headerSize);
                    }
                    _dispatcher->Dispatch(session, reinterpret_cast<const char*>(decryptedData.data()), static_cast<uint16_t>(len));
                } else {
                    _dispatcher->Dispatch(session, reinterpret_cast<const char*>(buffer), static_cast<uint16_t>(len));
                }
            }
        }
    }

private:
    std::shared_ptr<Framework::IDispatcher> _dispatcher;
    std::shared_ptr<GameServer::Packet::IPacketCipher> _cipher;
};

}
