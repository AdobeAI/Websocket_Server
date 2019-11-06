#include "websocket.h"

void fail(boost::system::error_code ec,
          char const* what) {
    std::cerr<< what << " : " << ec.message() << "\n";
}

WebSocket::WebSocket(tcp::socket socket)
    : ws(std::move(socket))
    , strand(ws.get_executor()) {
}

void WebSocket::run() {
    // Accept the websocket handshake
    ws.async_accept(
                boost::asio::bind_executor(
                    strand,
                    std::bind(
                        &WebSocket::onAccept,
                        shared_from_this(),
                        std::placeholders::_1)));
}

void WebSocket::onAccept(boost::system::error_code ec) {
    if (ec)
        return fail(ec, "accept");

    // Read a message
    doRead();
}

void WebSocket::doRead() {
    // Read a message into our buffer
    ws.async_read(
                buffer,
                boost::asio::bind_executor(
                    strand,
                    std::bind(
                        &WebSocket::onRead,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2)));
}

void WebSocket::onRead(boost::system::error_code ec,
                       std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This indicates that the session was closed
    if (ec == websocket::error::closed)
        return;

    if (ec)
        fail(ec, "read");

    // Echo the message
    ws.text(ws.got_text());
    std::cout << boost::beast::buffers(buffer.data()) << std::endl;
    ws.async_write(
                buffer.data(),
                boost::asio::bind_executor(
                    strand,
                    std::bind(
                        &WebSocket::onWrite,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2)));

}

void WebSocket::onWrite(boost::system::error_code ec,
                        std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    // Clear the buffer
    buffer.consume(buffer.size());

    // Do another read
    doRead();
}

Listener::Listener(boost::asio::io_context& ioc,
                   tcp::endpoint endpoint)
    : acceptor(ioc)
    , socket(ioc) {
    boost::system::error_code ec;

    //open the acceptor
    acceptor.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    //allow address reuse
    acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    //bind to the server address
    acceptor.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    //start listening for connections
    acceptor.listen (boost::asio::socket_base::max_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }
}

void Listener::run() {
    if (!acceptor.is_open())
        return;

    doAccept();
}

void Listener::doAccept() {
    acceptor.async_accept(
                socket,
                std::bind(
                    &Listener::onAccept,
                    shared_from_this(),
                    std::placeholders::_1));
}

void Listener::onAccept(boost::system::error_code ec) {
    if (ec) {
        fail(ec, "accept");
    } else {
        //create the session and run it
        std::make_shared<WebSocket>(std::move(socket))->run();
    }
    //accept anothor connection
    doAccept();
}
