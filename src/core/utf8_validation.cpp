// SPDX-License-Identifier: GPL-2.0-or-later

#include "utf8_validation.h"

namespace unilume::core {

bool isValidUtf8(std::string_view text)
{
    std::size_t index = 0;
    while (index < text.size()) {
        const auto lead = static_cast<unsigned char>(text[index]);
        std::size_t continuation = 0;
        if (lead <= 0x7f) {
            continuation = 0;
        } else if (lead >= 0xc2 && lead <= 0xdf) {
            continuation = 1;
        } else if (lead >= 0xe0 && lead <= 0xef) {
            continuation = 2;
        } else if (lead >= 0xf0 && lead <= 0xf4) {
            continuation = 3;
        } else {
            return false;
        }
        if (index + continuation >= text.size()) {
            return false;
        }
        for (std::size_t offset = 1; offset <= continuation; ++offset) {
            if ((static_cast<unsigned char>(text[index + offset]) & 0xc0) !=
                0x80) {
                return false;
            }
        }
        const auto second = continuation == 0
                                ? 0
                                : static_cast<unsigned char>(text[index + 1]);
        if ((lead == 0xe0 && second < 0xa0) ||
            (lead == 0xed && second > 0x9f) ||
            (lead == 0xf0 && second < 0x90) ||
            (lead == 0xf4 && second > 0x8f)) {
            return false;
        }
        index += continuation + 1;
    }
    return true;
}

} // namespace unilume::core
