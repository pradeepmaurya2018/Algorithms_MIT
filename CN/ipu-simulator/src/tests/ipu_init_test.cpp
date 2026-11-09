#include "tests/ipu_init_test.hpp"
#include "ipu/ipu_simulator.hpp"

#include <string>

std::string IpuInitTest::name() const
{
    return "IPU_INIT";
}

TestResult IpuInitTest::run()
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

    // Check initial lifecycle state.
    if (ipu.is_initialized()) {
        return fail("Simulator unexpectedly initialized before initialize()");
    }

    // Initialize the simulator.
    if (!ipu.initialize()) {
        return fail("Simulator initialization returned false");
    }

    if (!ipu.is_initialized()) {
        return fail("Simulator state is not INITIALIZED after initialize()");
    }

    // Validate device identity.
    const IpuInfo& info = ipu.info();

    if (info.name.empty()) {
        return fail("Device name is empty");
    }

    if (info.model.empty()) {
        return fail("Device model is empty");
    }

    if (info.version.empty()) {
        return fail("Device version is empty");
    }

    // Validate basic capabilities.
    if (info.port_count == 0) {
        return fail("Device reports zero virtual ports");
    }

    if (info.max_packet_size < 64) {
        return fail("Maximum packet size is below the minimum Ethernet frame size");
    }

    // Initialization should be idempotent.
    if (!ipu.initialize()) {
        return fail("Repeated initialization failed");
    }

    if (!ipu.is_initialized()) {
        return fail("Simulator became uninitialized after repeated initialization");
    }

    // Verify shutdown.
    ipu.shutdown();

    if (ipu.is_initialized()) {
        return fail("Simulator remains initialized after shutdown()");
    }

    return {
        TestStatus::PASS,
        name(),
        "Initialization, capabilities, idempotency and shutdown validated"
    };
}
