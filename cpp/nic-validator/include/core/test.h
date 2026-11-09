#pragma once

#include "core/test_result.h"
#include <string>

class Test {
public:
    virtual ~Test() = default;

    virtual std::string name() const = 0;

    virtual TestResult setup() {
        return TestResult::pass();
    }

    virtual TestResult run() = 0;

    virtual void cleanup() {}
};
