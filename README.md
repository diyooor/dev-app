## Dependencies

To compile and run this application, ensure the following libraries and tools are installed:

- **g++** (C++17 or later)
- **Boost Libraries**:
- **OpenSSL** (for SSL/TLS support)

## Features

### Log
The `Log` class provides a centralized logging mechanism for the application. It supports:

- **Log Levels**: Configurable log levels (DEBUG, INFO, WARN, ERROR) to control the verbosity of the log output.
- **Output Configuration**: Logs can be directed to the console, a file, or both, depending on the configuration.
- **Thread Safety**: Ensures that log entries are consistently handled in multi-threaded environments.

The `Log` class integrates seamlessly with other services, allowing for comprehensive logging across the entire application.

### Queue
The ```Queue``` class provides a flexible and configurable mechanism for managing the flow of incoming HTTP requests within the application. It supports:

- Request Ordering: Ensures requests are processed in the order they are received.
- Rate Limiting: Controls the rate at which requests are processed, preventing the server from being overwhelmed.
- Throttling: Introduces configurable delays between processing batches of requests, allowing for controlled resource usage.

### Client
The `Client` class manages HTTP requests to external services, allowing the application to act as an HTTP client. It can send GET requests to specified hosts and ports, retrieve responses, and manage connections.

### Clock
The `Clock` class demonstrates basic asynchronous timing using Boost.Asio, providing a simple mechanism for scheduling tasks without blocking other operations
.
### Compiling

To compile the application, use the following command:

```bash
g++ -std=c++17 main.cpp src/http_tools.cpp src/listener.cpp src/session.cpp src/application.cpp src/services/clock.cpp src/services/queue.cpp src/services/test.cpp src/services/client.cpp src/services/log.cpp -o main -lboost_system -lboost_filesystem -lboost_thread -lssl -lcrypto -lpthread
```

## Running the Application

To run the application, use the following command:

```bash
./main 0.0.0.0 8080 www 2
```

- 0.0.0.0: The address on which the server listens (binds to all interfaces).
- 8080: The port on which the server listens for incoming connections.
- www: The root directory for serving static files.
- 2: The number of threads to use for handling requests.

