#pragma once
#include <asio.hpp>
#include <vector>
#include <thread>
#include <memory>
#include "Thread/IThreadPool.h"
#include "Thread/IThreadPool.h"
#include "Timer/ITimerManager.h"
#include "Packet/IPacketCipher.h"
#include "Network/SessionManager.h"

namespace GameServer::Network {

class Service {
public:
    Service(int threadCount = 1);
    ~Service();

    void Start();
    void Stop();

    asio::io_context& GetIOContext();
    std::shared_ptr<Framework::IThreadPool> GetThreadPool();
    std::shared_ptr<Framework::ITimerManager> GetTimerManager();
    std::shared_ptr<GameServer::Packet::IPacketCipher> GetPacketCipher();
    SessionManager& GetSessionManager() { return SessionManager::Instance(); }

private:
    asio::io_context _ioContext;
    asio::executor_work_guard<asio::io_context::executor_type> _workGuard;
    std::vector<std::thread> _ioThreads; // Separate threads for IO
    std::shared_ptr<Framework::IThreadPool> _threadPool;
    std::shared_ptr<Framework::ITimerManager> _timerManager;
    std::shared_ptr<GameServer::Packet::IPacketCipher> _packetCipher;
    int _threadCount;
};

}
