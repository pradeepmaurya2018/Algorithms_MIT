#pragma once

#include "core/test_result.hpp"

#include <string>

class Test {
public:
    virtual ~Test() = default;

    virtual std::string name() const = 0;
    virtual TestResult run() = 0;
};
