#pragma once

#include "core/test.hpp"

class IpuL2ForwardingTest final : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
