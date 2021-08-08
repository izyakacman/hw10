#pragma once

#include <stdint.h>

#include <boost/asio.hpp>

#include "sql_processor.hpp"

class Session : public std::enable_shared_from_this<Session>
{
public:

    Session(boost::asio::ip::tcp::socket socket, SqlProcessor& sql_processor);

    void Start();

private:

    void Read();

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    SqlProcessor& sql_processor_;
};

class TcpServer
{
public:

    TcpServer(boost::asio::io_context& io_context, short port, SqlProcessor& sql_processor);

private:

    void Accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    SqlProcessor& sql_processor_;
};