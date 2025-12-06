#pragma once
#include <functional>

namespace GameServer::Framework {

class IThreadPool {
public:
    virtual ~IThreadPool() = default;

    virtual void Start(int threadCount) = 0;
    virtual void Stop() = 0;
    virtual void Post(std::function<void()> task) = 0;
};

}
