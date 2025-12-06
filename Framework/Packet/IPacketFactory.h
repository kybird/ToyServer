#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <functional>

namespace GameServer::Framework::Packet {

/**
 * @brief 패킷 생성 및 관리를 위한 팩토리 인터페이스
 * 메모리 풀링을 통해 패킷 객체의 할당과 해제를 최적화합니다.
 */
class IPacketFactory {
public:
    virtual ~IPacketFactory() = default;

    /**
     * @brief T 타입의 패킷을 생성하거나 풀에서 가져옵니다.
     * @tparam T 패킷 타입
     * @tparam Args 패킷 생성자 인자 타입
     * @param args 패킷 생성자 인자
     * @return std::shared_ptr<T> 풀링된 패킷의 스마트 포인터
     */
    template<typename T, typename... Args>
    static std::shared_ptr<T> CreatePacket(Args&&... args) {
        // 1. 풀에서 메모리 포인터 획득 (없으면 할당)
        void* rawPtr = PacketPool<T>::Instance().Pop();
        if (rawPtr == nullptr) {
            rawPtr = ::operator new(sizeof(T));
        }

        // 2. 획득한 메모리에 객체 생성 (Placement New)
        T* packet = nullptr;
        try {
            packet = new (rawPtr) T(std::forward<Args>(args)...);
        } catch (...) {
            // 생성 실패 시 메모리를 다시 풀에 반환 (혹은 해제)해야하나,
            // 여기서는 풀에 반환하여 나중에 재사용되도록 함.
            PacketPool<T>::Instance().Push(rawPtr);
            throw;
        }

        // 3. 커스텀 딜리터가 포함된 shared_ptr 반환
        return std::shared_ptr<T>(packet, [](T* p) {
            IPacketFactory::DestroyPacket(p);
        });
    }

    /**
     * @brief 사용이 끝난 패킷을 풀로 반환합니다.
     * CreatePacket에서 생성된 shared_ptr의 커스텀 삭제자에 의해 자동으로 호출됩니다.
     * @tparam T 패킷 타입
     * @param packet 반환할 패킷 포인터
     */
    template<typename T>
    static void DestroyPacket(T* packet) {
        if (packet == nullptr) return;
        
        // 1. 소멸자 명시적 호출
        packet->~T();
        
        // 2. Raw Memory 상태로 풀에 반환
        // (void*로 캐스팅하여 타입 정보 제거)
        PacketPool<T>::Instance().Push(static_cast<void*>(packet));
    }

private:
    /**
     * @brief 각 패킷 타입별 메모리 풀
     * T는 풀링할 객체 타입이지만, 저장은 void* (Raw Memory)로 관리함.
     * (T의 생성자/소멸자와 무관하게 메모리 청크만 관리)
     */
    template<typename T>
    class PacketPool {
    public:
        static PacketPool& Instance() {
            static PacketPool instance;
            return instance;
        }

        void* Pop() {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_pool.empty()) {
                return nullptr;
            }
            void* ptr = _pool.back();
            _pool.pop_back();
            return ptr;
        }

        void Push(void* ptr) {
            std::lock_guard<std::mutex> lock(_mutex);
            _pool.push_back(ptr);
        }

        ~PacketPool() {
            // 풀에 남아있는 메모리 해제
            for (void* ptr : _pool) {
                ::operator delete(ptr);
            }
            _pool.clear();
        }

    private:
        std::mutex _mutex;
        std::vector<void*> _pool;
    };
};

}
