#pragma once

#include "core/test.h"
#include <memory>
#include <vector>

class TestRunner {
public:
    void add(std::unique_ptr<Test> test);
    int run_all();

private:
    std::vector<std::unique_ptr<Test>> tests;
};
