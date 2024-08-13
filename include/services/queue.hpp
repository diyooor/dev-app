#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "../beast.hpp"
#include <boost/asio/steady_timer.hpp>
#include <functional>

// The Queue class provides a service for waiting asynchronously
// for a specified duration.

class Queue {
public:
    // Constructor: Initializes the Queue with an io_context.
    Queue(boost::asio::io_context& ioc);


    // Destructor: Cleans up any resources held by the Queue service.
    ~Queue();

private:
    // A reference to the io_context, which is used for managing asynchronous operations.
    boost::asio::io_context& ioc_;

};

#endif // QUEUE_HPP


