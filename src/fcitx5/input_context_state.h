// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "direct_commit_controller.h"
#include "fcitx_replacement_backend.h"

#include <fcitx/inputcontextproperty.h>

namespace unilume::fcitx5 {

class InputContextState final : public fcitx::InputContextProperty {
public:
    explicit InputContextState(fcitx::InputContext &input_context);

    void keyEvent(fcitx::KeyEvent &event);
    void reset();

private:
    FcitxReplacementBackend backend_;
    core::DirectCommitController controller_;
};

} // namespace unilume::fcitx5
