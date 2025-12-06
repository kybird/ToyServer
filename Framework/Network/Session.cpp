#include "Network/Session.h"
#include "Thread/IThreadPool.h"
#include <iostream>
#include "Packet/PacketHeader.h"

namespace GameServer::Network {

Session::Session(asio::io_context& ioContext, std::shared_ptr<Framework::IThreadPool> threadPool)
    : _socket(ioContext), _threadPool(threadPool) {
}

Session::~Session() {
}

void Session::Start() {
    OnConnected();
    DoRead();
}

void Session::OnConnected() {
    // Default implementation
}

void Session::OnDisconnected() {
    // Default implementation
}



void Session::DoRead() {
    auto self(shared_from_this());
    _socket.async_read_some(asio::buffer(_recvBuffer, max_length),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                size_t processed = OnRecv(_recvBuffer, length);
                (void)processed; // Suppress unused variable warning
                // TODO: Handle partial packets properly with a ring buffer or similar if needed.
                // For now, we assume simple read. In a real scenario, we need a SessionBuffer.
                // But to keep it simple as per plan, let's implement a basic OnRecv that calls OnPacket.
                
                DoRead();
            } else {
                OnDisconnected();
            }
        });
}

size_t Session::OnRecv(const uint8_t* buffer, size_t len) {
    size_t processed = 0;
    while (len > processed) {
        if (len - processed < sizeof(Packet::PacketHeader))
            break; // Not enough for header

        const Packet::PacketHeader* header = reinterpret_cast<const Packet::PacketHeader*>(&buffer[processed]);
        if (header->size > len - processed)
            break; // Not enough for full packet

        OnPacket(&buffer[processed], header->size);
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
