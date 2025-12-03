#pragma once
#include <asio.hpp>
#include <vector>
#include <thread>
#include <memory>

namespace GameServer::Network {

class Service {
public:
    Service(int threadCount = 1);
    ~Service();

    void Start();
    void Stop();

    asio::io_context& GetIOContext();

private:
    asio::io_context _ioContext;
    asio::executor_work_guard<asio::io_context::executor_type> _workGuard;
    std::vector<std::thread> _threads;
    int _threadCount;
};

}
