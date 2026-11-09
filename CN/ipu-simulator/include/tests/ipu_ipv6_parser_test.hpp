#pragma once

#include "core/test.hpp"

class IpuIpv6ParserTest final : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
