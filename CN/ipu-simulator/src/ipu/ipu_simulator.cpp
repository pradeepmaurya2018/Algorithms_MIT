#include "ipu/ipu_simulator.hpp"

IpuSimulator::IpuSimulator()
    : datapath_(port_manager_),
      packet_io_(port_manager_)
{
    info_.name = "Software IPU Simulator";
    info_.model = "Generic IPU";
    info_.version = "0.1.0";

    info_.port_count = 4;
    info_.max_packet_size = 9728;
}

bool IpuSimulator::initialize()
{
    if (initialized_) {
        return true;
    }

    if (!port_manager_.initialize(info_.port_count)) {
        return false;
    }

    initialized_ = true;
    return true;
}

void IpuSimulator::shutdown() noexcept
{
    packet_io_.reset();
    datapath_.forwarding().clear();
    datapath_.vlans().reset();
    port_manager_.reset();
    initialized_ = false;
}

bool IpuSimulator::is_initialized() const noexcept
{
    return initialized_;
}

const IpuInfo& IpuSimulator::info() const noexcept
{
    return info_;
}

PortManager& IpuSimulator::ports() noexcept
{
    return port_manager_;
}

const PortManager& IpuSimulator::ports() const noexcept
{
    return port_manager_;
}

PacketIoEngine& IpuSimulator::packet_io() noexcept
{
    return packet_io_;
}

DatapathPipeline& IpuSimulator::datapath() noexcept
{
    return datapath_;
}

const DatapathPipeline& IpuSimulator::datapath() const noexcept
{
    return datapath_;
}

std::size_t IpuSimulator::process_packets(std::size_t budget)
{
    if (!initialized_) {
        return 0;
    }

    return packet_io_.process_packets(budget, datapath_);
}
