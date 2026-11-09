#include "core/test_runner.h"
#include "core/logger.h"

#include <chrono>
#include <iomanip>
#include <iostream>

void TestRunner::add(std::unique_ptr<Test> test) {
    tests.push_back(std::move(test));
}

int TestRunner::run_all() {
    int failed = 0;
    int passed = 0;
    int skipped = 0;

    std::cout << "\n============================================================\n";
    std::cout << "                 NIC VALIDATION FRAMEWORK\n";
    std::cout << "============================================================\n";

    for (auto& test : tests) {
        std::cout << "\n[TEST] " << test->name() << '\n';

        const auto start = std::chrono::steady_clock::now();

        TestResult result = test->setup();
        if (result.status == TestStatus::PASS) {
            result = test->run();
        }

        test->cleanup();

        const auto end = std::chrono::steady_clock::now();
        result.duration_ms =
            std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "       " << status_to_string(result.status)
                  << "  " << std::fixed << std::setprecision(2)
                  << result.duration_ms << " ms";

        if (!result.message.empty()) {
            std::cout << "  " << result.message;
        }
        std::cout << '\n';

        switch (result.status) {
            case TestStatus::PASS:  ++passed; break;
            case TestStatus::FAIL:
            case TestStatus::ERROR: ++failed; break;
            case TestStatus::SKIP:  ++skipped; break;
        }
    }

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "PASS: " << passed
              << "  FAIL/ERROR: " << failed
              << "  SKIP: " << skipped << '\n';
    std::cout << "============================================================\n";

    return failed == 0 ? 0 : 1;
}
