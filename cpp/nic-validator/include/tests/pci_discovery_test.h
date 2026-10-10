#pragma once

#include "core/test.h"
#include "nic/nic_device.h"

class PciDiscoveryTest final : public Test {
public:
    explicit PciDiscoveryTest(NicDevice& nic);

    std::string name() const override;
    TestResult run() override;

private:
    NicDevice& nic_;
};
