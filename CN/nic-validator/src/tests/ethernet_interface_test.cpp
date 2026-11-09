#include "tests/ethernet_interface_test.h"
#include "core/logger.h"

#include <array>
#include <sstream>
#include <string>

namespace {

bool is_valid_mac(const std::string& mac)
{
    std::istringstream input(mac);
    std::string octet;
    std::array<unsigned int, 6> bytes{};

    for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (!std::getline(input, octet, ':') || octet.size() != 2)
            return false;

        try {
            std::size_t parsed = 0;
            bytes[i] = std::stoul(octet, &parsed, 16);

            if (parsed != 2 || bytes[i] > 0xff)
                return false;
        } catch (...) {
            return false;
        }
    }

    if (std::getline(input, octet, ':'))
        return false;

    bool all_zero = true;

    for (const auto byte : bytes) {
        if (byte != 0) {
            all_zero = false;
            break;
        }
    }

    if (all_zero || (bytes[0] & 0x01U))
        return false;

    return true;
}

}

EthernetInterfaceTest::EthernetInterfaceTest(NicDevice& nic)
    : nic_(nic)
{
}

std::string EthernetInterfaceTest::name() const
{
    return "ETHERNET_INTERFACE";
}

TestResult EthernetInterfaceTest::run()
{
    if (!nic_.discover())
        return TestResult::error("Unable to discover PCI NIC");

    const auto& nic = nic_.info();

    if (nic.interface_name.empty())
        return TestResult::fail("No Ethernet interface associated with PCI device");

    if (!is_valid_mac(nic.mac_address))
        return TestResult::fail("Invalid Ethernet MAC address: " + nic.mac_address);

    if (nic.mtu < 68 || nic.mtu > 65535)
        return TestResult::fail("Invalid interface MTU: " + std::to_string(nic.mtu));

    if (nic.carrier != -1 && nic.carrier != 0 && nic.carrier != 1)
        return TestResult::fail("Invalid carrier state");

    std::ostringstream output;

    output << "\nEthernet interface validation"
           << "\n  Interface : " << nic.interface_name
           << "\n  Driver    : " << nic.driver
           << "\n  MAC       : " << nic.mac_address
           << "\n  MTU       : " << nic.mtu
           << "\n  Operstate : "
           << (nic.operstate.empty() ? "unavailable" : nic.operstate)
           << "\n  Carrier   : ";

    if (nic.carrier == 1)
        output << "present";
    else if (nic.carrier == 0)
        output << "absent";
    else
        output << "unavailable";

    Logger::info(output.str());

    return TestResult::pass("Ethernet interface properties validated");
}
