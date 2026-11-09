#pragma once

#include "core/test.hpp"

class IpuVlanTest final : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
