#pragma once
#include <chrono>
#include <functional>
#include <cstdint>

namespace GameServer::Framework {

using TimerID = uint64_t;

class ITimerManager {
public:
    virtual ~ITimerManager() = default;

    virtual void ScheduleOnce(std::chrono::milliseconds delay, std::function<void()> task) = 0;
    virtual TimerID ScheduleRepeat(std::chrono::milliseconds interval, std::function<void()> task) = 0;
    virtual void Cancel(TimerID id) = 0;
};

}
