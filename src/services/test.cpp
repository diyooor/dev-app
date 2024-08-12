#include "../../include/services/test.hpp"
#include <iostream>

// Constructor implementation
Test::Test(std::shared_ptr<Clock> clock, std::shared_ptr<Client> client)
    : clock_(std::move(clock)), client_(std::move(client)) {
}

void Test::TestGet(const std::string& host, const std::string& port, const std::string& target, int wait, std::function<void(const std::string&)> callback) {
    std::cout << "TestGet invoked. Waiting for " << wait << " seconds." << std::endl;
    clock_->wait(wait, [this, host, port, target, callback]() {
        std::cout << "Wait over, initiating GET request to " << target << std::endl;
        try {
            std::string response = client_->get(host, port, target);
            callback(response);
        } catch (const std::exception& e) {
            std::cerr << "GET request failed: " << e.what() << std::endl;
            callback("");
        }
    });
}


// Method to run a test that waits for a specific duration and then makes an HTTP POST request
void Test::TestPost(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int wait, std::function<void(const std::string&)> callback) {
    clock_->wait(wait, [this, host, port, target, body, callback]() {
        try {
            std::string response = client_->post(host, port, target, body);
            callback(response);
        } catch (const std::exception& e) {
            std::cerr << "POST request failed: " << e.what() << std::endl;
            callback("");
        }
    });
}

