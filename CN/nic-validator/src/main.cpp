#include "core/test_runner.h"
#include "nic/linux_nic.h"
#include "tests/pci_discovery_test.h"
#include "tests/ethernet_interface_test.h"

#include <iostream>
#include <memory>
#include <string>

static void usage(const char* argv0) {
    std::cout
        << "Usage:\n"
        << "  sudo " << argv0 << " --test PCI_DISCOVERY [--pci 0000:02:00.0]\n"
        << "  sudo " << argv0 << " --test ETHERNET_INTERFACE [--pci 0000:02:00.0]\n"
        << "  sudo " << argv0 << " --all [--pci 0000:02:00.0]\n"
        << "\n"
        << "Phase 1 tests:\n"
        << "  PCI_DISCOVERY\n"
        << "  ETHERNET_INTERFACE\n";
}

int main(int argc, char* argv[]) {
    std::string pci = "0000:02:00.0";
    std::string test;
    bool all = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--pci" && i + 1 < argc) {
            pci = argv[++i];
        } else if (arg == "--test" && i + 1 < argc) {
            test = argv[++i];
        } else if (arg == "--all") {
            all = true;
        } else if (arg == "--help" || arg == "-h") {
            usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            usage(argv[0]);
            return 2;
        }
    }

    if (!all && test.empty()) {
        usage(argv[0]);
        return 2;
    }

    LinuxNic nic(pci);
    TestRunner runner;

    if (all || test == "PCI_DISCOVERY") {
        runner.add(std::make_unique<PciDiscoveryTest>(nic));
    }

    if (all || test == "ETHERNET_INTERFACE") {
        runner.add(std::make_unique<EthernetInterfaceTest>(nic));
    }

    if (!all &&
        test != "PCI_DISCOVERY" &&
        test != "ETHERNET_INTERFACE") {
        std::cerr << "Unknown test: " << test << '\n';
        return 2;
    }

    return runner.run_all();
}
