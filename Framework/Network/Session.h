#pragma once
#include <asio.hpp>
#include <cstdint>
#include <memory>
#include <iostream>
#include <deque>
#include <vector>
#include <string>

#include "Thread/IThreadPool.h"
#include "Network/RecvBuffer.h"

namespace GameServer::Network {

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(asio::io_context& ioContext, std::shared_ptr<Framework::IThreadPool> threadPool = nullptr);
    virtual ~Session();

    asio::ip::tcp::socket& GetSocket() { return _socket; }
    uint32_t GetSessionId() const { return _sessionId; }
    void SetSessionId(uint32_t id) { _sessionId = id; }

    void Start();
    void Send(const std::string& msg);
    virtual void Send(const std::vector<uint8_t>& msg); // Overload for binary data
    virtual void SendHeartbeat() { } // Override to implement custom heartbeat
    
    void UpdateLastRecvTime();
    uint64_t GetLastRecvTime() const { return _lastRecvTime; }

protected:
    virtual void OnConnected();
    virtual void OnDisconnected();
    virtual size_t OnRecv(RecvBuffer& buffer); // Changed to use RecvBuffer
    virtual void OnPacket(const uint8_t* /*buffer*/, size_t /*len*/) { } // Default empty

private:
    void DoRead();
    void DoWrite();

    asio::ip::tcp::socket _socket;
    std::deque<std::vector<uint8_t>> _writeQueue; // Changed to vector<uint8_t> for binary

protected:
    enum { max_length = 4096 };
    // uint8_t _recvBuffer[max_length]; // Removed
    RecvBuffer _recvBuffer;
    uint32_t _sessionId = 0;
    std::atomic<uint64_t> _lastRecvTime = 0;
    std::shared_ptr<Framework::IThreadPool> _threadPool;
};

}
