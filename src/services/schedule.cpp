#include "../../include/services/schedule.hpp"

Schedule::Schedule(boost::asio::io_context& ioc) : ioc_(ioc) {}

Schedule::~Schedule() {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    for (auto& [name, timer] : timers_) {
        timer->cancel();
    }
    timers_.clear();
}

void Schedule::add_recurring_task(const std::string& name, std::chrono::seconds interval, Task task) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);

    auto timer = std::make_shared<boost::asio::steady_timer>(ioc_, interval);
    timers_[name] = timer;

    std::function<void()> handler; // Declare the handler first
    handler = [this, name, interval, task, timer, &handler]() mutable {
        run_task(name, task);
        timer->expires_after(interval);
        timer->async_wait([this, &handler](const boost::system::error_code& ec) {
            if (!ec) {
                handler();
            }
        });
    };

    timer->async_wait([handler](const boost::system::error_code& ec) {
        if (!ec) {
            handler();
        }
    });

    Log::get().log(Level::INFO, "[Schedule] Recurring task '" + name + "' scheduled every " + std::to_string(interval.count()) + " seconds.");
}


// Cancel a task by name
void Schedule::cancel_task(const std::string& name) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    auto it = timers_.find(name);
    if (it != timers_.end()) {
        it->second->cancel();
        timers_.erase(it);
        Log::get().log(Level::INFO, "[Schedule] Task '" + name + "' canceled.");
    }
}

// Run a task and log the execution
void Schedule::run_task(const std::string& name, Task task) {
    Log::get().log(Level::INFO, "[Schedule] Running task '" + name + "'.");
    try {
        task();
        Log::get().log(Level::INFO, "[Schedule] Task '" + name + "' completed successfully.");
    } catch (const std::exception& e) {
        Log::get().log(Level::ERROR, "[Schedule] Task '" + name + "' failed: " + std::string(e.what()));
    }
}
