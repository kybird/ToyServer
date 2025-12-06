#pragma once
#include <asio.hpp>
#include <cstdint>
#include <memory>
#include <iostream>
#include <deque>
#include <vector>
#include <string>

#include "Thread/IThreadPool.h"

namespace GameServer::Network {

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(asio::io_context& ioContext, std::shared_ptr<Framework::IThreadPool> threadPool = nullptr);
    virtual ~Session();

    asio::ip::tcp::socket& GetSocket() { return _socket; }
    void Start();
    void Send(const std::string& msg);
    virtual void Send(const std::vector<uint8_t>& msg); // Overload for binary data

protected:
    virtual void OnConnected();
    virtual void OnDisconnected();
    virtual size_t OnRecv(const uint8_t* buffer, size_t len); // Returns bytes processed
    virtual void OnPacket(const uint8_t* /*buffer*/, size_t /*len*/) { } // Default empty

private:
    void DoRead();
    void DoWrite();

    asio::ip::tcp::socket _socket;
    std::deque<std::vector<uint8_t>> _writeQueue; // Changed to vector<uint8_t> for binary

protected:
    enum { max_length = 4096 };
    uint8_t _recvBuffer[max_length]; // Changed to uint8_t
    std::shared_ptr<Framework::IThreadPool> _threadPool;
};

}
