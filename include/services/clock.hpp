#ifndef CLOCK_HPP
#define CLOCK_HPP

#include "../beast.hpp"
#include <boost/asio/steady_timer.hpp>
#include <functional>

// The Clock class provides a service for waiting asynchronously
// for a specified duration.

class Clock {
public:
    // Constructor: Initializes the Clock with an io_context.
    Clock(boost::asio::io_context& io_context);

    // Waits asynchronously for the specified duration in seconds.
    void wait(int seconds, std::function<void()> onWaitComplete);

    // Destructor: Cleans up any resources held by the Clock service.
    ~Clock();

private:
    // A reference to the io_context, which is used for managing asynchronous operations.
    boost::asio::io_context& ioc_;

    // Asynchronous timer used to wait for a specified duration.
    boost::asio::steady_timer timer_;
};

#endif // CLOCK_HPP

