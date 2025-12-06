#include "AsioTimerManager.h"
#include <iostream>

namespace GameServer::Framework {

AsioTimerManager::AsioTimerManager(asio::io_context& ioContext)
    : _ioContext(ioContext) {
}

AsioTimerManager::~AsioTimerManager() {
    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& [id, timer] : _timers) {
        timer->cancel();
    }
    _timers.clear();
}

void AsioTimerManager::ScheduleOnce(std::chrono::milliseconds delay, std::function<void()> task) {
    auto timer = std::make_shared<asio::steady_timer>(_ioContext, delay);
    timer->async_wait([task, timer](const std::error_code& ec) {
        if (!ec) {
            task();
        }
    });
}

TimerID AsioTimerManager::ScheduleRepeat(std::chrono::milliseconds interval, std::function<void()> task) {
    TimerID id = _nextId++;
    auto timer = std::make_shared<asio::steady_timer>(_ioContext, interval);
    
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _timers[id] = timer;
    }

    timer->async_wait([this, id, interval, task, timer](const std::error_code& ec) {
        OnTimer(ec, id, interval, task, timer);
    });

    return id;
}

void AsioTimerManager::Cancel(TimerID id) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _timers.find(id);
    if (it != _timers.end()) {
        it->second->cancel();
        _timers.erase(it);
    }
}

void AsioTimerManager::OnTimer(const std::error_code& ec, TimerID id, std::chrono::milliseconds interval, std::function<void()> task, std::shared_ptr<asio::steady_timer> timer) {
    if (ec) {
        // Timer cancelled or error
        return;
    }

    // Execute task
    task();

    // Reschedule
    timer->expires_at(timer->expiry() + interval);
    timer->async_wait([this, id, interval, task, timer](const std::error_code& ec) {
        OnTimer(ec, id, interval, task, timer);
    });
}

}
