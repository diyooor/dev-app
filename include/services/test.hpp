#ifndef TEST_HPP
#define TEST_HPP

#include "clock.hpp"
#include "client.hpp"
#include <string>
#include <memory>
#include <functional>

class Test {
public:
    // Constructor: Initializes the Test service with Clock and Client objects
    Test(std::shared_ptr<Clock> clock, std::shared_ptr<Client> client);

    // Method to run a test that waits for a specific duration and then makes an HTTP GET request
    void TestGet(const std::string& host, const std::string& port, const std::string& target, int waitSeconds, std::function<void(const std::string&)> callback);

    // Method to run a test that waits for a specific duration and then makes an HTTP POST request
    void TestPost(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int waitSeconds, std::function<void(const std::string&)> callback);

private:
    // Clock and Client objects
    std::shared_ptr<Clock> clock_;
    std::shared_ptr<Client> client_;
};

#endif // TEST_HPP

