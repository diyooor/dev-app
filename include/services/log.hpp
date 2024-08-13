#ifndef LOG
#define LOG

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <mutex>
#include <memory>
#include <chrono>
#include <iomanip>

enum class Level {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Log {
    public:
        // Singleton pattern to get the single instance of Log
        static Log& get();

        // Set Log level
        void set(Level lvl);

        // Log to conosle and/or file
        void log(Level level, const std::string& message);

        // Configure output streams (console, file)
        void console(bool f);
        void file(const std::string& filename);
        ~Log();
        Log();
        std::string timestamp();
        std::string lvltostring(Level lvl);
    private:

        Log(const Log&) = delete; // Delete copy constructor
        Log& operator=(const Log&) = delete; // Delete assignment operator
        

        Level level_;
        bool console_;
        std::ofstream file_;
        std::mutex log_;
};

#endif
