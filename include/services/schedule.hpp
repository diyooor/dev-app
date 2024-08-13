#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <boost/asio.hpp>
#include <functional>
#include <chrono>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "log.hpp"

class Schedule {
    public:
        using Task = std::function<void()>;

        Schedule(boost::asio::io_context& ioc);
        ~Schedule();

        void add_recurring_task(const std::string& name, std::chrono::seconds interval, Task task);

        void add_one_time_task(const std::string& name, std::chrono::system_clock::time_point time, Task task);

        void cancel_task(const std::string& name);

    private:
        void run_task(const std::string& name, Task task);
        boost::asio::io_context& ioc_;
        std::unordered_map<std::string, std::shared_ptr<boost::asio::steady_timer>> timers_;
        std::mutex tasks_mutex_;
};

#endif
