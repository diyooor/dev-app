#include "../../include/services/clock.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

// Constructor implementation
Clock::Clock(boost::asio::io_context& io_context)
    : ioc_(io_context), timer_(io_context) {
    // Constructor code, if any additional setup is needed
}

// Waits asynchronously for the specified duration (in seconds)
void Clock::wait(int seconds, std::function<void()> onWaitComplete) {
    timer_.expires_after(std::chrono::seconds(seconds));
    timer_.async_wait([onWaitComplete](const boost::system::error_code& ec) {
        if (!ec) {
            onWaitComplete();
        }
    });
}

// Destructor implementation
Clock::~Clock() {
    // Destructor code, if any cleanup is needed
}

