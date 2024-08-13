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
    std::cout << "[Queue] Initialized with rate limit: " << rate_limit.count()
              << " ms, throttle time: " << throttle_time.count() << " ms\n";
}


// Destructor implementation
Queue::~Queue() {
    std::cout << "[Queue] Destructor called, cleaning up resources.\n";
}

void Queue::enqueue(RequestHandler handler) {
    {
        std::lock_guard<std::mutex> lock{queue_mutex_};
        request_queue_.push(std::move(handler));
        std::cout << "[Queue] Request enqueued. Queue size: " << request_queue_.size() << "\n";
    }
    if (request_queue_.size() == 1) {
        start();
    }
}

void Queue::start() {
    std::cout << "[Queue] Starting to process the queue.\n";
    process_next();
}

void Queue::process_next() {
    std::lock_guard<std::mutex> lock{queue_mutex_};
    if (request_queue_.empty()) {
        std::cout << "[Queue] No more requests to process. Queue is empty.\n";
        return;
    }

    auto handler = std::move(request_queue_.front());
    request_queue_.pop();
    std::cout << "[Queue] Processing request. Remaining queue size: " << request_queue_.size() << "\n";

    handler();

    auto delay = rate_limit_ + throttle_time_;
    if (delay.count() > 0) {
        std::cout << "[Queue] Waiting for " << delay.count() << " ms before processing the next request.\n";
        timer_.expires_after(delay);
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                process_next();
            }
        });
    } else {
        std::cout << "[Queue] No delay configured. Processing the next request immediately.\n";
        process_next();
    }
}

