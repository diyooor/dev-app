#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../beast.hpp"
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/connect.hpp>
#include <string>

class Client {
public:
    Client(boost::asio::io_context& io_context, ssl::context& ssl_ctx);

    std::string get(const std::string& host, const std::string& port, const std::string& target, int version = 11);
    std::string post(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int version = 11);

private:
    tcp::resolver resolver_;
    ssl::context& ssl_ctx_; // Keep the SSL context for creating new streams
    boost::asio::io_context& io_context_; // Store io_context reference
};

#endif // CLIENT_HPP

