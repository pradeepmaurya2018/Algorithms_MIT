#include "core/test_runner.hpp"

#include <exception>
#include <iostream>
#include <utility>

void TestRunner::add(std::unique_ptr<Test> test)
{
    tests_.push_back(std::move(test));
}

int TestRunner::run_all()
{
    unsigned int passed = 0;
    unsigned int failed = 0;
    unsigned int errors = 0;

    std::cout << "\n========== IPU VALIDATION ==========\n";

    for (const auto& test : tests_) {
        std::cout << "\n[ RUN      ] " << test->name() << '\n';

        TestResult result;

        try {
            result = test->run();
        } catch (const std::exception& ex) {
            result = {
                TestStatus::ERROR,
                test->name(),
                ex.what()
            };
        } catch (...) {
            result = {
                TestStatus::ERROR,
                test->name(),
                "Unknown exception"
            };
        }

        switch (result.status) {
        case TestStatus::PASS:
            ++passed;
            std::cout << "[       OK ] " << result.message << '\n';
            break;

        case TestStatus::FAIL:
            ++failed;
            std::cout << "[  FAILED  ] " << result.message << '\n';
            break;

        case TestStatus::ERROR:
            ++errors;
            std::cout << "[  ERROR   ] " << result.message << '\n';
            break;
        }
    }

    std::cout << "\n========== SUMMARY ================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "Errors: " << errors << '\n';
    std::cout << "===================================\n";

    return (failed == 0 && errors == 0) ? 0 : 1;
}
