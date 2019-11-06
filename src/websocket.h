#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using tcp = boost::asio::ip::tcp;
namespace websocket = boost::beast::websocket;

class WebSocket : public std::enable_shared_from_this<WebSocket> {
    websocket::stream<tcp::socket> ws;
    boost::asio::strand<boost::asio::io_context::executor_type> strand;
    boost::beast::multi_buffer buffer;

public:
    explicit WebSocket(tcp::socket socket);

    void run();

    void onAccept(boost::system::error_code ec);

    void doRead();

    void onRead(boost::system::error_code ec,
                std::size_t bytes_transferred);

    void onWrite(boost::system::error_code ec,
                 std::size_t bytes_transferred);
};

class Listener : public std::enable_shared_from_this<Listener> {
    tcp::acceptor acceptor;
    tcp::socket socket;

public:
    Listener(boost::asio::io_context& ioc,
             tcp::endpoint endpoint);

    void run();

    void doAccept();

    void onAccept(boost::system::error_code ec);
};

