#include "include/server_certificate.hpp"
#include "include/http_tools.hpp"
#include "include/listener.hpp"
#include "include/application.hpp"
#include "include/services/test.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <thread>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        std::cerr <<
            "Usage: http-server-async-ssl <address> <port> <doc_root> <threads>\n" <<
            "Example:\n" <<
            "    http-server-async-ssl 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const doc_root = std::make_shared<std::string>(argv[3]);
    auto const threads = std::max<int>(1, std::atoi(argv[4]));
    
    // Initialize the io_context
    net::io_context ioc{threads};

    // Initialize SSL context
    ssl::context ctx{ssl::context::tlsv12};
    load_server_certificate(ctx);

    // Initialize the Application with the shared io_context and SSL context
    auto app = std::make_shared<Application>(ioc, ctx);

    // Start the listener to accept incoming connections
    std::make_shared<listener>(
        ioc,
        ctx,
        tcp::endpoint{address, port},
        doc_root,
        app)->run();

    // Initialize the Clock and Client services with the SSL context
    auto clock = std::make_shared<Clock>(ioc);
    auto client = std::make_shared<Client>(ioc, ctx);

    // Initialize Test service
    auto testService = std::make_shared<Test>(clock, client);

    // Run the first GET test after 5 seconds
    testService->TestGet("sattar.xyz", "8080", "/", 5, [testService](const std::string& response) {
        std::cout << "GET Test Response: " << response << std::endl;

        // After the first test completes, start the second POST test with a slight delay
        testService->TestPost("sattar.xyz", "8080", "/", "param1=value1&param2=value2", 2, [](const std::string& response) {
            std::cout << "POST Test Response: " << response << std::endl;
        });
    });

    // Run the I/O context in multiple threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc] { ioc.run(); });
    
    // Run the I/O context in the main thread
    ioc.run();

    return EXIT_SUCCESS;
}

