#pragma once

#include "hardware/datapath_pipeline/datapath_pipeline.hpp"
#include "hardware/packet_io/packet_io.hpp"
#include "ipu/ipu_info.hpp"
#include "ipu/port_manager.hpp"

#include <cstddef>

class IpuDevice {
public:
    virtual ~IpuDevice() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() noexcept = 0;

    virtual bool is_initialized() const noexcept = 0;

    virtual const IpuInfo& info() const noexcept = 0;

    virtual PortManager& ports() noexcept = 0;
    virtual const PortManager& ports() const noexcept = 0;

    virtual PacketIoEngine& packet_io() noexcept = 0;

    virtual DatapathPipeline& datapath() noexcept = 0;
    virtual const DatapathPipeline& datapath() const noexcept = 0;

    virtual std::size_t process_packets(std::size_t budget) = 0;
};
