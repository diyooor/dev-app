#include "../include/application.hpp"
#include "../include/services/clock.hpp"
#include "../include/services/client.hpp"

// Constructor implementation
Application::Application(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_ctx) {
    clock_ = std::make_shared<Clock>(io_context);
    client_ = std::make_shared<Client>(io_context, ssl_ctx); // Pass the SSL context to the Client
}

// Accessor for Clock
std::shared_ptr<Clock> Application::get_clock() const {
    return clock_;
}

// Accessor for Client
std::shared_ptr<Client> Application::get_client() const {
    return client_;
}

