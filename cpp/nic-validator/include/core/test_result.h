#pragma once

#include <string>

enum class TestStatus {
    PASS,
    FAIL,
    SKIP,
    ERROR
};

struct TestResult {
    TestStatus status{TestStatus::ERROR};
    std::string message;
    double duration_ms{0.0};

    static TestResult pass(std::string msg = {}) {
        return {TestStatus::PASS, std::move(msg), 0.0};
    }

    static TestResult fail(std::string msg) {
        return {TestStatus::FAIL, std::move(msg), 0.0};
    }

    static TestResult skip(std::string msg) {
        return {TestStatus::SKIP, std::move(msg), 0.0};
    }

    static TestResult error(std::string msg) {
        return {TestStatus::ERROR, std::move(msg), 0.0};
    }
};

inline const char* status_to_string(TestStatus status) {
    switch (status) {
        case TestStatus::PASS: return "PASS";
        case TestStatus::FAIL: return "FAIL";
        case TestStatus::SKIP: return "SKIP";
        case TestStatus::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}
