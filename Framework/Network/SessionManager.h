#pragma once
#include <map>
#include <memory>
#include <shared_mutex>
#include <atomic>
#include <mutex>
#include <vector>

namespace GameServer::Network {

class Session;

class SessionManager {
public:
    uint32_t Add(std::shared_ptr<Session> session);
    void Remove(uint32_t sessionId);
    void Broadcast(const std::vector<uint8_t>& buffer);
    std::shared_ptr<Session> Find(uint32_t sessionId);
    void CheckHeartbeat(uint64_t now, uint64_t timeoutInterval);
    size_t GetSessionCount();

    static SessionManager& Instance() {
        static SessionManager instance;
        return instance;
    }

private:
    std::shared_mutex _lock;
    std::map<uint32_t, std::shared_ptr<Session>> _sessions;
    std::atomic<uint32_t> _sessionIdGen = 1;
};

}
