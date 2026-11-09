#pragma once

#include "core/test.hpp"

class IpuIpv4ParserTest final : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
