#pragma once

#include "nic/nic_info.h"

class NicDevice {
public:
    virtual ~NicDevice() = default;

    virtual bool discover() = 0;
    virtual const NicInfo& info() const = 0;
};
