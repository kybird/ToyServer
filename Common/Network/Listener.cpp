#include "GameServer/Network/Listener.h"
#include <iostream>

namespace GameServer::Network {

Listener::Listener(asio::io_context& ioContext, unsigned short port, SessionFactory sessionFactory)
    : _ioContext(ioContext),
      _acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      _sessionFactory(sessionFactory) {
}

Listener::~Listener() {
}

void Listener::Start() {
    DoAccept();
}

void Listener::DoAccept() {
    auto session = _sessionFactory(_ioContext);
    _acceptor.async_accept(session->GetSocket(),
        [this, session](std::error_code ec) {
            if (!ec) {
                session->Start();
            }
            DoAccept();
        });
}

}
