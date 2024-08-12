#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "services/clock.hpp"
#include "services/client.hpp"
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class Application {
public:
    // Constructor that initializes the application with io_context and ssl_context
    Application(boost::asio::io_context& io_context, boost::asio::ssl::context& ssl_ctx);

    // Accessors to get the Clock and Client services
    std::shared_ptr<Clock> get_clock() const;
    std::shared_ptr<Client> get_client() const;

private:
    std::shared_ptr<Clock> clock_;
    std::shared_ptr<Client> client_;
};

#endif // APPLICATION_HPP

