#pragma once

#include "core/test.hpp"

class IpuTxDescriptorRingTest : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
