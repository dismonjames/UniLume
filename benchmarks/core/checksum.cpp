// SPDX-License-Identifier: GPL-2.0-or-later

#include "checksum.h"

namespace unilume::benchmark {

void Checksum::add(std::string_view bytes)
{
    for (const unsigned char byte : bytes) {
        value_ ^= byte;
        value_ *= 1099511628211ULL;
    }
}

void Checksum::add(std::uint64_t value)
{
    for (unsigned int shift = 0; shift < 64; shift += 8) {
        value_ ^= static_cast<unsigned char>(value >> shift);
        value_ *= 1099511628211ULL;
    }
}

std::uint64_t Checksum::value() const
{
    return value_;
}

} // namespace unilume::benchmark
