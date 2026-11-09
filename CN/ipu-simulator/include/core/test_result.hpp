#pragma once

#include <string>

enum class TestStatus {
    PASS,
    FAIL,
    ERROR
};

struct TestResult {
    TestStatus status{TestStatus::ERROR};
    std::string test_name;
    std::string message;
};
