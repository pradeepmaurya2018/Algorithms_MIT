#include "tests/pci_discovery_test.h"
#include "core/logger.h"

#include <iomanip>
#include <sstream>

PciDiscoveryTest::PciDiscoveryTest(NicDevice& nic)
    : nic_(nic) {}

std::string PciDiscoveryTest::name() const {
    return "PCI_DISCOVERY";
}

TestResult PciDiscoveryTest::run() {
    if (!nic_.discover()) {
        return TestResult::fail("PCI device not found or sysfs discovery failed");
    }

    const auto& n = nic_.info();

    std::ostringstream out;
    out << std::hex << std::setfill('0');

    out << "\n"
        << "       PCI address : " << n.pci_address << "\n"
        << "       Vendor      : 0x" << std::setw(4) << n.vendor_id << "\n"
        << "       Device      : 0x" << std::setw(4) << n.device_id << "\n"
        << "       Revision    : 0x" << std::setw(2)
        << static_cast<unsigned>(n.revision) << "\n"
        << "       Interface   : " << (n.interface_name.empty() ? "<none>" : n.interface_name) << "\n"
        << "       Driver      : " << (n.driver.empty() ? "<none>" : n.driver) << "\n"
        << "       MAC         : " << (n.mac_address.empty() ? "<none>" : n.mac_address);

    for (const auto& bar : n.bars) {
        out << "\n       BAR" << bar.index
            << "        : 0x" << bar.start
            << " / 0x" << bar.size
            << " bytes / " << (bar.is_io ? "IO" : "MMIO");
    }

    Logger::info(out.str());

    // This first test is intentionally generic. Do not hard-code the Realtek ID
    // into the framework. Device-specific validation belongs in a backend/test.
    if (n.vendor_id == 0 || n.device_id == 0) {
        return TestResult::fail("invalid PCI vendor/device ID");
    }

    if (n.bars.empty()) {
        return TestResult::fail("no active PCI BARs discovered");
    }

    return TestResult::pass("PCI discovery successful");
}
