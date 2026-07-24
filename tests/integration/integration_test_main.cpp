// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"

#include <iostream>
#include <string_view>

int main(int argc, char **argv)
{
    using namespace unilume::integration::test;

    if (argc != 2) {
        std::cerr << "usage: unilume_integration_tests <suite>\n";
        return 2;
    }

    Assertions assertions;
    const std::string_view suite{argv[1]};
    if (suite == "immediate") {
        runImmediateTests(assertions);
    } else if (suite == "delayed") {
        runDelayedTests(assertions);
    } else if (suite == "duplicate") {
        runDuplicateTests(assertions);
    } else if (suite == "transaction") {
        runTransactionTests(assertions);
    } else if (suite == "preedit-fallback") {
        runPreeditFallbackTests(assertions);
    } else if (suite == "browser-capability") {
        runBrowserCapabilityTests(assertions);
    } else if (suite == "mode-policy") {
        runInputModePolicyTests(assertions);
    } else if (suite == "burst") {
        runBurstTests(assertions);
    } else if (suite == "soak-smoke") {
        runSoakSmokeTests(assertions);
    } else {
        std::cerr << "unknown integration suite: " << suite << '\n';
        return 2;
    }

    if (assertions.failures() != 0) {
        std::cerr << assertions.failures() << " integration assertion(s) failed\n";
        return 1;
    }
    return 0;
}
