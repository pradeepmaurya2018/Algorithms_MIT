#pragma once

#include "ipu/ipu_device.hpp"

class IpuSimulator final : public IpuDevice {
public:
    IpuSimulator();

    bool initialize() override;
    void shutdown() noexcept override;

    bool is_initialized() const noexcept override;

    const IpuInfo& info() const noexcept override;

    PortManager& ports() noexcept override;
    const PortManager& ports() const noexcept override;

    PacketIoEngine& packet_io() noexcept override;

    DatapathPipeline& datapath() noexcept override;
    const DatapathPipeline& datapath() const noexcept override;

    std::size_t process_packets(std::size_t budget) override;

private:
    IpuInfo info_;
    PortManager port_manager_;
    DatapathPipeline datapath_;
    PacketIoEngine packet_io_;
    bool initialized_{false};
};
