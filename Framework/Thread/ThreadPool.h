#pragma once
#include "IThreadPool.h"
#include <asio.hpp>
#include <vector>
#include <thread>
#include <memory>

namespace GameServer::Framework {

class ThreadPool : public IThreadPool {
public:
    ThreadPool();
    virtual ~ThreadPool();

    virtual void Start(int threadCount) override;
    virtual void Stop() override;
    virtual void Post(std::function<void()> task) override;

    asio::io_context& GetIOContext() { return _ioContext; }

private:
    asio::io_context _ioContext;
    asio::executor_work_guard<asio::io_context::executor_type> _workGuard;
    std::vector<std::thread> _threads;
};

}
