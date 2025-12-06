#include "Network/Service.h"
#include "Thread/ThreadPool.h"
#include "Timer/AsioTimerManager.h"
#include "Security/BasicXORCipher.h"

namespace GameServer::Network {

Service::Service(int threadCount)
    : _workGuard(asio::make_work_guard(_ioContext)), _threadCount(threadCount) {
    auto threadPool = std::make_shared<Framework::ThreadPool>();
    _threadPool = threadPool;
    _timerManager = std::make_shared<Framework::AsioTimerManager>(threadPool->GetIOContext());
    _packetCipher = std::make_shared<GameServer::Security::BasicXORCipher>();
}

Service::~Service() {
    Stop();
}

void Service::Start() {
    // Start IO threads
    // For now, let's use 1 IO thread and use the rest for Logic (ThreadPool)
    // Or maybe use _threadCount for both?
    // Let's use 1 thread for IO and _threadCount for Logic.
    
    _ioThreads.emplace_back([this]() {
        _ioContext.run();
    });

    _threadPool->Start(_threadCount);
}

void Service::Stop() {
    _ioContext.stop();
    for (auto& t : _ioThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    _ioThreads.clear();
    
    _threadPool->Stop();
}

std::shared_ptr<Framework::IThreadPool> Service::GetThreadPool() {
    return _threadPool;
}

std::shared_ptr<Framework::ITimerManager> Service::GetTimerManager() {
    return _timerManager;
}

std::shared_ptr<GameServer::Packet::IPacketCipher> Service::GetPacketCipher() {
    return _packetCipher;
}

asio::io_context& Service::GetIOContext() {
    return _ioContext;
}

}
