#pragma once
#include "ITimerManager.h"
#include <asio.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>

namespace GameServer::Framework {

class AsioTimerManager : public ITimerManager {
public:
    AsioTimerManager(asio::io_context& ioContext);
    virtual ~AsioTimerManager();

    virtual void ScheduleOnce(std::chrono::milliseconds delay, std::function<void()> task) override;
    virtual TimerID ScheduleRepeat(std::chrono::milliseconds interval, std::function<void()> task) override;
    virtual void Cancel(TimerID id) override;

private:
    void OnTimer(const std::error_code& ec, TimerID id, std::chrono::milliseconds interval, std::function<void()> task, std::shared_ptr<asio::steady_timer> timer);

    asio::io_context& _ioContext;
    std::map<TimerID, std::shared_ptr<asio::steady_timer>> _timers;
    std::mutex _mutex;
    std::atomic<TimerID> _nextId{1};
};

}
