// SPDX-License-Identifier: GPL-2.0-or-later

#include "input_mode_policy.h"

namespace unilume::platform {

InputPath InputModePolicy::observe(bool direct_available)
{
    // Never promote a live context from asynchronous client preedit to
    // direct replacement. A frontend may still be applying the final
    // preedit update when the first delete/commit request arrives.
    if (!initialized_) {
        initialized_ = true;
        path_ = direct_available ? InputPath::direct
                                 : InputPath::preedit;
        return path_;
    }
    if (path_ == InputPath::direct) {
        if (!direct_available) {
            path_ = InputPath::preedit;
        }
        return path_;
    }

    return path_;
}

InputPath InputModePolicy::path() const
{
    return path_;
}

} // namespace unilume::platform
