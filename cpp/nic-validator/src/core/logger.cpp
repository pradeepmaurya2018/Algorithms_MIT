#include "core/logger.h"

#include <iostream>
#include <mutex>

namespace {
std::mutex log_mutex;

const char* prefix(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "[INFO]";
        case LogLevel::PASS:  return "[PASS]";
        case LogLevel::WARN:  return "[WARN]";
        case LogLevel::ERROR: return "[ERROR]";
    }
    return "[????]";
}
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ostream& out = (level == LogLevel::ERROR) ? std::cerr : std::cout;
    out << prefix(level) << " " << message << '\n';
}

void Logger::info(const std::string& message)  { log(LogLevel::INFO, message); }
void Logger::pass(const std::string& message)  { log(LogLevel::PASS, message); }
void Logger::warn(const std::string& message)  { log(LogLevel::WARN, message); }
void Logger::error(const std::string& message) { log(LogLevel::ERROR, message); }
