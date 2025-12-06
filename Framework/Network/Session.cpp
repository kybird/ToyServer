#include "Network/Session.h"
#include "Thread/IThreadPool.h"
#include <iostream>
#include "Packet/PacketHeader.h"
#include "Network/SessionManager.h"
#include <chrono>

using namespace std::chrono;

namespace GameServer::Network {

Session::Session(asio::io_context& ioContext, std::shared_ptr<Framework::IThreadPool> threadPool)
    : _socket(ioContext), _threadPool(threadPool), _recvBuffer(65536) {
}

Session::~Session() {
}

void Session::Start() {
    _sessionId = SessionManager::Instance().Add(shared_from_this());
    UpdateLastRecvTime();
    OnConnected();
    DoRead();
}

void Session::UpdateLastRecvTime() {
    _lastRecvTime = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

void Session::OnConnected() {
    // Default implementation
}

void Session::OnDisconnected() {
    // Default implementation
}



void Session::DoRead() {
    auto self(shared_from_this());
    _socket.async_read_some(asio::buffer(_recvBuffer.WritePos(), _recvBuffer.FreeSize()),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                if (_recvBuffer.OnWrite(length) == false) {
                    SessionManager::Instance().Remove(_sessionId);
                    OnDisconnected();
                    return;
                }

                size_t processed = OnRecv(_recvBuffer);
                if (_recvBuffer.OnRead(processed) == false) {
                   SessionManager::Instance().Remove(_sessionId);
                   OnDisconnected();
                   return;
                }
                
                _recvBuffer.Clean();
                DoRead();
            } else {
                SessionManager::Instance().Remove(_sessionId);
                OnDisconnected();
            }
        });
}

size_t Session::OnRecv(RecvBuffer& buffer) {
    UpdateLastRecvTime();
    size_t processed = 0;
    size_t dataSize = buffer.DataSize();
    uint8_t* readPos = buffer.ReadPos();

    while (dataSize > processed) {
        if (dataSize - processed < sizeof(Packet::PacketHeader))
            break; // Not enough for header

        const Packet::PacketHeader* header = reinterpret_cast<const Packet::PacketHeader*>(&readPos[processed]);
        
        // 헤더 사이즈 유효성 검사 (너무 크거나 작으면 끊어야 함? 일단 길이 체크만)
        if (header->size > dataSize - processed)
            break; // Not enough for full packet

        if (header->size < sizeof(Packet::PacketHeader)) {
             // Malformed packet
             break;
        }

        OnPacket(&readPos[processed], header->size);
        processed += header->size;
    }
    return processed;
}

void Session::Send(const std::string& msg) {
    std::vector<uint8_t> buffer(msg.begin(), msg.end());
    Send(buffer);
}

void Session::Send(const std::vector<uint8_t>& msg) {
    bool writeInProgress = !_writeQueue.empty();
    _writeQueue.push_back(msg);
    if (!writeInProgress) {
        DoWrite();
    }
}

void Session::DoWrite() {
    auto self(shared_from_this());
    asio::async_write(_socket,
        asio::buffer(_writeQueue.front().data(), _writeQueue.front().size()),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                _writeQueue.pop_front();
                if (!_writeQueue.empty()) {
                    DoWrite();
                }
            } else {
                OnDisconnected();
            }
        });
}

}
