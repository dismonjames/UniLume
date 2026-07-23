// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string_view>

namespace unilume::core {

bool isValidUtf8(std::string_view text);

} // namespace unilume::core
