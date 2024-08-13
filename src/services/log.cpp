#include "../../include/services/log.hpp"

// Singleton instance
Log& Log::get() {
    static Log instance;
    return instance;
}

// Constructor 
Log::Log() : level_(Level::INFO), console_(true) {}

// Destructor 
Log::~Log() {
    if (file_.is_open()) {
        file_.close();
    }
}

// Set the log level
void Log::set(Level lvl) {
    std::lock_guard<std::mutex> lock(log_);
    level_ = lvl;
}

// Enable or disable console output
void Log::console(bool f) {
    std::lock_guard<std::mutex> lock(log_);
    console_ = f;
}

void Log::file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_);
        if (file_.is_open()) {
            file_.close();
        }
        file_.open(filename, std::ios::app);
}

// Log a message with a specified level
void Log::log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_);

    if (level < level_)
        return;

    std::stringstream log_stream;
    log_stream << "[" << timestamp() << "] "
               << lvltostring(level) << ": "
               << message << std::endl;

    if (console_)
        std::cout << log_stream.str();

    if (file_.is_open())
        file_ << log_stream.str();
}

std::string Log::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

// Convert log level to string
std::string Log::lvltostring(Level lvl) {
    switch (lvl) {
        case Level::DEBUG: return "DEBUG";
        case Level::INFO: return "INFO";
        case Level::WARN: return "WARN";
        case Level::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
