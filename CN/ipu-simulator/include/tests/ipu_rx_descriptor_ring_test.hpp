#pragma once

#include "core/test.hpp"

class IpuRxDescriptorRingTest : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
