#include "../../include/services/queue.hpp"
#include "../../include/services/log.hpp"  // Include the Log service

// Constructor implementation
Queue::Queue(boost::asio::io_context& ioc,
             std::chrono::milliseconds rate_limit,
             std::chrono::milliseconds throttle_time)
    : ioc_(ioc),
      rate_limit_(rate_limit),
      throttle_time_(throttle_time),
      timer_(ioc) {
    Log::get().log(Level::INFO, "[Queue] Initialized with rate limit: " + std::to_string(rate_limit.count()) +
                                  " ms, throttle time: " + std::to_string(throttle_time.count()) + " ms");
}

// Destructor implementation
Queue::~Queue() {
    Log::get().log(Level::INFO, "[Queue] Destructor called, cleaning up resources.");
}

void Queue::enqueue(RequestHandler handler) {
    {
        std::lock_guard<std::mutex> lock{queue_mutex_};
        request_queue_.push(std::move(handler));
        Log::get().log(Level::INFO, "[Queue] Request enqueued. Queue size: " + std::to_string(request_queue_.size()));
    }
    if (request_queue_.size() == 1) {
        start();
    }
}

void Queue::start() {
    Log::get().log(Level::INFO, "[Queue] Starting to process the queue.");
    process_next();
}

void Queue::process_next() {
    std::lock_guard<std::mutex> lock{queue_mutex_};
    if (request_queue_.empty()) {
        Log::get().log(Level::INFO, "[Queue] No more requests to process. Queue is empty.");
        return;
    }

    auto handler = std::move(request_queue_.front());
    request_queue_.pop();
    Log::get().log(Level::INFO, "[Queue] Processing request. Remaining queue size: " + std::to_string(request_queue_.size()));

    handler();

    auto delay = rate_limit_ + throttle_time_;
    if (delay.count() > 0) {
        Log::get().log(Level::INFO, "[Queue] Waiting for " + std::to_string(delay.count()) + " ms before processing the next request.");
        timer_.expires_after(delay);
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                process_next();
            }
        });
    } else {
        Log::get().log(Level::INFO, "[Queue] No delay configured. Processing the next request immediately.");
        process_next();
    }
}

