#include "ThreadPool.h"

namespace GameServer::Framework {

ThreadPool::ThreadPool() 
    : _workGuard(asio::make_work_guard(_ioContext)) {
}

ThreadPool::~ThreadPool() {
    Stop();
}

void ThreadPool::Start(int threadCount) {
    for (int i = 0; i < threadCount; ++i) {
        _threads.emplace_back([this]() {
            _ioContext.run();
        });
    }
}

void ThreadPool::Stop() {
    _ioContext.stop();
    for (auto& t : _threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    _threads.clear();
}

void ThreadPool::Post(std::function<void()> task) {
    asio::post(_ioContext, task);
}

}
