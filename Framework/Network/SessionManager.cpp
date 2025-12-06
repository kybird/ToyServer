#include "Network/SessionManager.h"
#include "Network/Session.h"

namespace GameServer::Network {

uint32_t SessionManager::Add(std::shared_ptr<Session> session) {
    std::unique_lock<std::shared_mutex> lock(_lock);
    
    uint32_t sessionId = _sessionIdGen.fetch_add(1);
    _sessions.insert({sessionId, session});
    
    return sessionId;
}

void SessionManager::Remove(uint32_t sessionId) {
    std::unique_lock<std::shared_mutex> lock(_lock);
    _sessions.erase(sessionId);
}

std::shared_ptr<Session> SessionManager::Find(uint32_t sessionId) {
    std::shared_lock<std::shared_mutex> lock(_lock);
    
    auto it = _sessions.find(sessionId);
    if (it == _sessions.end())
        return nullptr;
    
    return it->second;
}

void SessionManager::Broadcast(const std::vector<uint8_t>& buffer) {
    std::shared_lock<std::shared_mutex> lock(_lock);
    
    // 복사해서 사용하지 않고 직접 순회 (Send가 비동기라 락 잡고 있어도 크게 문제 없으나, 
    // Send 내부에서 SessionManager를 다시 호출하면 데드락 가능성 있음.
    // Session::Send는 writeQueue에 넣고 끝내므로 안전함.)
    for (auto& session : _sessions) {
        session.second->Send(buffer);
    }
}

void SessionManager::CheckHeartbeat(uint64_t now, uint64_t timeoutInterval) {
    std::vector<uint32_t> deadSessions;
    
    {
        std::shared_lock<std::shared_mutex> lock(_lock);
        for (auto& pair : _sessions) {
            auto session = pair.second;
            uint64_t lastRecv = session->GetLastRecvTime();
            if (now - lastRecv > timeoutInterval) {
                deadSessions.push_back(pair.first);
            } else {
                // 아직 살아있다면, 애플리케이션 레벨의 핑을 보낼 기회를 줌
                session->SendHeartbeat();
            }
        }
    } // Lock release

    // 죽은 세션들 정리
    for (uint32_t sessionId : deadSessions) {
        // Find로 확인 후 Disconnect (혹은 그냥 Find해서 Disconnect)
        // Disconnect는 virtual이고 내부적으로 close socket -> error handler -> remove session 호출됨
        // 하지만 여기서는 이미 Remove가 필요한 상황일 수 있음.
        // 그냥 Session->GetSocket().close()를 호출하여 OnDisconnected를 유도하는 것이 정석.
        // 하지만 Session 객체에 접근하려면 다시 Find해야 함.
        
        auto session = Find(sessionId);
        if (session) {
            // 강제 종료. OnDisconnected 트리기 됨 -> SessionManager::Remove 호출됨.
            // 하지만 여기서 Remove를 직접 부르면 중복 호출이나 데드락?
            // Remove는 unique_lock 잡으므로, 여기서 Remove 불러도 됨 (위에서 shared_lock 풀었으므로).
            // 그러나 OnDisconnected 로직을 태우는게 깔끔함.
            session->GetSocket().close();
        }
    }
}

size_t SessionManager::GetSessionCount() {
    std::shared_lock<std::shared_mutex> lock(_lock);
    return _sessions.size();
}

void SessionManager::DisconnectAll() {
    std::shared_lock<std::shared_mutex> lock(_lock);
    for (auto& session : _sessions) {
        session.second->ForceDisconnect();
    }
}

}
