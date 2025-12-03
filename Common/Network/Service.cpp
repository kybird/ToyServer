#include "GameServer/Network/Service.h"
#include <iostream>

namespace GameServer::Network {

Service::Service(int threadCount)
    : _workGuard(asio::make_work_guard(_ioContext)), _threadCount(threadCount) {
}

Service::~Service() {
    Stop();
}

void Service::Start() {
    for (int i = 0; i < _threadCount; ++i) {
        _threads.emplace_back([this]() {
            _ioContext.run();
        });
    }
}

void Service::Stop() {
    _ioContext.stop();
    for (auto& t : _threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    _threads.clear();
}

asio::io_context& Service::GetIOContext() {
    return _ioContext;
}

}
