// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace unilume::platform {

enum class InputPath {
    preedit,
    direct,
};

class InputModePolicy {
public:
    InputPath observe(bool direct_available);
    [[nodiscard]] InputPath path() const;

private:
    InputPath path_{InputPath::preedit};
    bool initialized_{};
};

} // namespace unilume::platform
