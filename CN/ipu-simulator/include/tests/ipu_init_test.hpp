#pragma once

#include "core/test.hpp"
#include "ipu/ipu_device.hpp"

class IpuInitTest final : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
