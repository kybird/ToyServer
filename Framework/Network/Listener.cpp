#include "Network/Listener.h"
#include "Network/Service.h"
#include <iostream>

namespace GameServer::Network {

Listener::Listener(asio::io_context& ioContext, unsigned short port, SessionFactory sessionFactory)
    : _ioContext(ioContext),
      _acceptor(ioContext),
      _sessionFactory(sessionFactory) {
    
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
    std::cout << "[Listener] Binding to port: " << port << std::endl;
    
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    try {
        _acceptor.bind(endpoint);
        _acceptor.listen();
        std::cout << "[Listener] Listening on " << _acceptor.local_endpoint().port() << std::endl;
    } catch (std::exception& e) {
        std::cout << "[Listener] Failed to bind/listen: " << e.what() << std::endl;
        throw;
    }
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
