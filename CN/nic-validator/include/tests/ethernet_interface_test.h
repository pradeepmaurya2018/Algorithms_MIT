#pragma once

#include "core/test.h"
#include "nic/nic_device.h"

class EthernetInterfaceTest final : public Test {
public:
    explicit EthernetInterfaceTest(NicDevice& nic);

    std::string name() const override;
    TestResult run() override;

private:
    NicDevice& nic_;
};
