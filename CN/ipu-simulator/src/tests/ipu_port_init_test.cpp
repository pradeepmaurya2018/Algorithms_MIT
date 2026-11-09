#include "tests/ipu_port_init_test.hpp"
#include "ipu/ipu_simulator.hpp"

#include <string>
#include <unordered_set>

std::string IpuPortInitTest::name() const
{
    return "IPU_PORT_INIT";
}

TestResult IpuPortInitTest::run()
{
    IpuSimulator ipu;

    const auto fail = [&](const std::string& reason) {
        ipu.shutdown();
        return TestResult{
            TestStatus::FAIL,
            name(),
            reason
        };
    };

    if (!ipu.initialize()) {
        return fail("IPU initialization failed");
    }

    const IpuInfo& info = ipu.info();
    const PortManager& ports = ipu.ports();

    // Validate the number of initialized ports.
    if (ports.port_count() != info.port_count) {
        return fail("Port count does not match IPU capabilities");
    }

    if (ports.port_count() == 0) {
        return fail("No virtual Ethernet ports initialized");
    }

    std::unordered_set<std::string> mac_addresses;

    for (std::uint32_t id = 0; id < ports.port_count(); ++id) {
        const EthernetPort* port = ports.get_port(id);

        if (port == nullptr) {
            return fail("Unable to retrieve port " + std::to_string(id));
        }

        if (port->port_id != id) {
            return fail("Port identifier mismatch");
        }

        if (port->mac_address.empty()) {
            return fail("Port MAC address is empty");
        }

        if (!mac_addresses.insert(port->mac_address).second) {
            return fail("Duplicate MAC address detected");
        }

        if (port->mtu != 1500) {
            return fail("Unexpected default MTU");
        }

        if (port->admin_state != PortAdminState::DOWN) {
            return fail("Port is not administratively DOWN initially");
        }

        if (port->oper_state != PortOperState::DOWN) {
            return fail("Port is not operationally DOWN initially");
        }

        if (port->link_present) {
            return fail("Link unexpectedly present initially");
        }
    }

    // Invalid port identifiers must be rejected.
    if (ports.get_port(ports.port_count()) != nullptr) {
        return fail("Invalid port identifier returned a valid port");
    }

    // Test administrative state and simulated link behavior.
    PortManager& mutable_ports = ipu.ports();

    if (!mutable_ports.set_admin_state(0, PortAdminState::UP)) {
        return fail("Failed to bring port 0 administratively UP");
    }

    const EthernetPort* port0 = mutable_ports.get_port(0);

    if (port0 == nullptr || port0->oper_state != PortOperState::DOWN) {
        return fail("Port became operational without a link");
    }

    if (!mutable_ports.set_link_present(0, true)) {
        return fail("Failed to enable simulated link on port 0");
    }

    port0 = mutable_ports.get_port(0);

    if (port0 == nullptr || port0->oper_state != PortOperState::UP) {
        return fail("Port did not become operational after link activation");
    }

    if (!mutable_ports.set_admin_state(0, PortAdminState::DOWN)) {
        return fail("Failed to bring port 0 administratively DOWN");
    }

    port0 = mutable_ports.get_port(0);

    if (port0 == nullptr || port0->oper_state != PortOperState::DOWN) {
        return fail("Port remained operational after administrative shutdown");
    }

    // Test MTU configuration and validation.
    if (!mutable_ports.set_mtu(0, 9000)) {
        return fail("Failed to configure MTU 9000");
    }

    port0 = mutable_ports.get_port(0);

    if (port0 == nullptr || port0->mtu != 9000) {
        return fail("MTU configuration was not retained");
    }

    if (mutable_ports.set_mtu(0, 67)) {
        return fail("Invalid MTU 67 was accepted");
    }

    ipu.shutdown();

    if (ipu.is_initialized()) {
        return {
            TestStatus::FAIL,
            name(),
            "IPU remained initialized after shutdown"
        };
    }

    if (ipu.ports().port_count() != 0) {
        return {
            TestStatus::FAIL,
            name(),
            "Ports were not released during shutdown"
        };
    }

    return {
        TestStatus::PASS,
        name(),
        "Port creation, identity, defaults, link state, MTU and shutdown validated"
    };
}
