#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "../beast.hpp"
#include <boost/asio/steady_timer.hpp>
#include <functional>
#include <chrono>
#include <mutex>
#include <queue>
// The Queue class provides a service for managing request ordering, rate limiting,
// and throttling asychronously using Boost.Asio
class Queue {
public:
    using RequestHandler = std::function<void()>;
    
    // Constructor: Initializes the Queue with an io_context and optional configurations
    Queue(boost::asio::io_context& ioc,
          std::chrono::milliseconds rate_limit = std::chrono::milliseconds(0),
          std::chrono::milliseconds throttle_time = std::chrono::milliseconds(0));
    
    // Destructor: Cleans up any resources held by the Queue service.
    ~Queue();
    
    // Add a request to the queue.
    void enqueue(RequestHandler handler);

    // Start processing the queue.
    void start();

private: 
    // Internal method to process the next request
    void process_next();
    
    // A reference to the io_context for managing asynchronous operations.
    boost::asio::io_context& ioc_;
    
    // The rate limit between processing requests.
    std::chrono::milliseconds rate_limit_;
    
    // The throttle time between processing batches of requests.
    std::chrono::milliseconds throttle_time_;
    
    // Queue to store the incoming requests
    std::queue<RequestHandler> request_queue_;
    
    // Mutex to protect access to the queue
    std::mutex queue_mutex_;

    // Timer for handling delays between requests.
    boost::asio::steady_timer timer_;
};

#endif // QUEUE_HPP


