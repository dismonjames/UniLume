// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

namespace unilume::platform {

enum class InputPath {
    unknown,
    direct,
    preedit,
};

class InputModePolicy {
public:
    InputPath observe(bool direct_available);
    void resetForCompositionEnd();
    void reset();
    [[nodiscard]] InputPath path() const;

private:
    InputPath path_{InputPath::unknown};
};

} // namespace unilume::platform
