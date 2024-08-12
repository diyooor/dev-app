#include "../../include/services/client.hpp"
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <iostream>


// Constructor implementation
Client::Client(boost::asio::io_context& io_context, ssl::context& ssl_ctx)
    : resolver_(net::make_strand(io_context)), ssl_ctx_(ssl_ctx), io_context_(io_context) {}

// Performs an HTTPS GET request
std::string Client::get(const std::string& host, const std::string& port, const std::string& target, int version) {
    try {
        beast::ssl_stream<beast::tcp_stream> stream{net::make_strand(io_context_), ssl_ctx_};
        auto const results = resolver_.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        beast::error_code ec;
        stream.shutdown(ec);
        if(ec == net::error::eof) {
            ec.assign(0, ec.category());
        }
        if(ec && ec != ssl::error::stream_truncated) {
            throw beast::system_error{ec};
        }

        return res.body();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

// Performs an HTTPS POST request
std::string Client::post(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int version) {
    try {
        beast::ssl_stream<beast::tcp_stream> stream{net::make_strand(io_context_), ssl_ctx_};
        auto const results = resolver_.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req{http::verb::post, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/x-www-form-urlencoded");
        req.content_length(body.size());
        req.body() = body;

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        beast::error_code ec;
        stream.shutdown(ec);
        if(ec == net::error::eof) {
            ec.assign(0, ec.category());
        }
        if(ec && ec != ssl::error::stream_truncated) {
            throw beast::system_error{ec};
        }

        return res.body();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}
