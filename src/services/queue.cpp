#include "../../include/services/queue.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

// Constructor implementation
Queue::Queue(boost::asio::io_context& ioc,
             std::chrono::milliseconds rate_limit,
             std::chrono::milliseconds throttle_time)
    : ioc_(ioc),
      rate_limit_(rate_limit),
      throttle_time_(throttle_time),
      timer_(ioc) {
    // Constructor code, if any additional setup is needed
}


// Destructor implementation
Queue::~Queue() {
    // Destructor code, if any cleanup is needed
}

void Queue::enqueue(RequestHandler handler) {
    std::lock_guard<std::mutex> lock{queue_mutex_};
    request_queue_.push(std::move(handler));
    if (request_queue_.size() == 1) {
        start();
    }
}

void Queue::start() { process_next(); }

void Queue::process_next() {
    std::lock_guard<std::mutex> lock{queue_mutex_};
    if (request_queue_.empty()) {
        return;
    }

    auto handler = std::move(request_queue_.front());
    request_queue_.pop();

    handler();

    auto delay = rate_limit_ + throttle_time_;
    if (delay.count() > 0) {
        timer_.expires_after(delay);
        timer_.async_wait([this](const boost::system::error_code& ec) {
                if (!ec) {
                    process_next();
                }
            });
    } else {
        process_next();
    }
}
