#include "server.hpp"

#include <iostream>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>

namespace ba = boost::asio;

Session::Session(boost::asio::ip::tcp::socket socket, SqlProcessor& sql_processor) :
    socket_(std::move(socket)), 
    data_{},
    sql_processor_{ sql_processor }
{
}

void Session::Start()
{
    Read();
}

void Session::Read()
{
    auto self(shared_from_this());

    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::string s{ data_, length };

                sql_processor_.ProcessRequest(s, socket_);

                Read();
            }
        });
}

TcpServer::TcpServer(boost::asio::io_context& io_context, short port, SqlProcessor& sql_processor) :
    acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), 
    sql_processor_{ sql_processor }
{
    Accept();
}

void TcpServer::Accept()
{
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            if (!ec)
            {
                std::make_shared<Session>(std::move(socket), sql_processor_)->Start();
            }

            Accept();
        });
}
