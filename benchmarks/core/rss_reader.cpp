// SPDX-License-Identifier: GPL-2.0-or-later

#include "rss_reader.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <sys/resource.h>

namespace unilume::benchmark {

std::uint64_t currentRssKiB()
{
    std::ifstream status{"/proc/self/status"};
    std::string key;
    while (status >> key) {
        if (key == "VmRSS:") {
            std::uint64_t value{};
            std::string unit;
            status >> value >> unit;
            if (unit != "kB") {
                throw std::runtime_error("unexpected VmRSS unit");
            }
            return value;
        }
        std::string remainder;
        std::getline(status, remainder);
    }
    throw std::runtime_error("VmRSS is unavailable in /proc/self/status");
}

std::uint64_t maximumRssKiB()
{
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) != 0) {
        throw std::runtime_error("getrusage failed");
    }
    return static_cast<std::uint64_t>(usage.ru_maxrss);
}

} // namespace unilume::benchmark
