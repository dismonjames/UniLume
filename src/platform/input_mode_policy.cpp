// SPDX-License-Identifier: GPL-2.0-or-later

#include "input_mode_policy.h"

namespace unilume::platform {

InputPath InputModePolicy::observe(bool direct_available)
{
    // If already decided, only allow demotion from direct to preedit.
    if (path_ != InputPath::unknown) {
        if (path_ == InputPath::direct && !direct_available) {
            path_ = InputPath::preedit;
        }
        return path_;
    }
    // First observation: select path based on current capability.
    // Never promote a context with active composition from preedit
    // to direct — a frontend may still be applying an asynchronous
    // preedit update.
    path_ = direct_available ? InputPath::direct : InputPath::preedit;
    return path_;
}

void InputModePolicy::resetForCompositionEnd()
{
    // Between compositions, reset to unknown so the next composition
    // re-evaluates the path based on current frontend capability.
    path_ = InputPath::unknown;
}

void InputModePolicy::reset()
{
    path_ = InputPath::unknown;
}

InputPath InputModePolicy::path() const
{
    return path_;
}

} // namespace unilume::platform
