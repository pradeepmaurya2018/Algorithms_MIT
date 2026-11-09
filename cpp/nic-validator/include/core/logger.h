#pragma once

#include <string>

enum class LogLevel {
    INFO,
    PASS,
    WARN,
    ERROR
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message);
    static void info(const std::string& message);
    static void pass(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
};
