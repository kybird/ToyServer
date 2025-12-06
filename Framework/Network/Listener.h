#pragma once
#include <asio.hpp>
#include <functional>
#include <memory>
#include "Network/Session.h"
#include "Network/Service.h"

namespace GameServer::Network {

class Listener {
public:
    using SessionFactory = std::function<std::shared_ptr<Session>(asio::io_context&)>;

    Listener(asio::io_context& ioContext, unsigned short port, SessionFactory sessionFactory);
    ~Listener();

    void Start();

private:
    void DoAccept();

    asio::io_context& _ioContext;
    asio::ip::tcp::acceptor _acceptor;
    SessionFactory _sessionFactory;
};

}
