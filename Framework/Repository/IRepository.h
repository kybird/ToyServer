#pragma once
#include <vector>
#include <memory>
#include <optional>

namespace GameServer::Repository {

template<typename T, typename Key>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool Save(const T& entity) = 0;
    virtual std::optional<T> Find(Key id) = 0;
    virtual bool Delete(Key id) = 0;
};

}
