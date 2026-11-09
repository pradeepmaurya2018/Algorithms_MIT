#pragma once

#include "core/test.hpp"

class IpuPacketBufferIoTest : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
