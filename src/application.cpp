#include "../include/application.hpp"
#include "../include/services/clock.hpp"
#include "../include/services/client.hpp"
#include "../include/services/queue.hpp"
// Constructor implementation
Application::Application(boost::asio::io_context& ioc, boost::asio::ssl::context& ssl_ctx) {
    log_ = std::make_shared<Log>();
    clock_ = std::make_shared<Clock>(ioc);
    client_ = std::make_shared<Client>(ioc, ssl_ctx); // Pass the SSL context to the Client
    queue_ = std::make_shared<Queue>(ioc, std::chrono::milliseconds(100), std::chrono::milliseconds(0));
}
std::shared_ptr<Log> Application::get_log() const { return log_; }

// Accessor for Clock
std::shared_ptr<Clock> Application::get_clock() const { return clock_; }

// Accessor for Client
std::shared_ptr<Client> Application::get_client() const { return client_; }

// Accessor for Queue
std::shared_ptr<Queue> Application::get_queue() const { return queue_; }
