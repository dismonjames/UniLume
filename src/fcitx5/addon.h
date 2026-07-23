// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "input_context_state.h"

#include <fcitx/addonfactory.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

namespace unilume::fcitx5 {

class UniLumeAddon final : public fcitx::InputMethodEngine {
public:
    explicit UniLumeAddon(fcitx::Instance &instance);

    void keyEvent(const fcitx::InputMethodEntry &entry,
                  fcitx::KeyEvent &event) override;
    void reset(const fcitx::InputMethodEntry &entry,
               fcitx::InputContextEvent &event) override;

private:
    fcitx::Instance &instance_;
    fcitx::FactoryFor<InputContextState> state_factory_;
};

class UniLumeFactory final : public fcitx::AddonFactory {
public:
    fcitx::AddonInstance *create(fcitx::AddonManager *manager) override;
};

} // namespace unilume::fcitx5
