// SPDX-License-Identifier: GPL-2.0-or-later

#include "addon.h"

#include <fcitx/addoninstance.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontextmanager.h>

namespace unilume::fcitx5 {

UniLumeAddon::UniLumeAddon(fcitx::Instance &instance)
    : instance_(instance),
      state_factory_([](fcitx::InputContext &input_context) {
          return new InputContextState(input_context);
      })
{
    instance_.inputContextManager().registerProperty(
        "unilume-input-context", &state_factory_);
}

void UniLumeAddon::keyEvent(const fcitx::InputMethodEntry &,
                            fcitx::KeyEvent &event)
{
    event.inputContext()->propertyFor(&state_factory_)->keyEvent(event);
}

void UniLumeAddon::reset(const fcitx::InputMethodEntry &,
                         fcitx::InputContextEvent &event)
{
    event.inputContext()->propertyFor(&state_factory_)->reset();
}

fcitx::AddonInstance *UniLumeFactory::create(fcitx::AddonManager *manager)
{
    return new UniLumeAddon(*manager->instance());
}

} // namespace unilume::fcitx5

#ifdef FCITX_ADDON_FACTORY_V2
FCITX_ADDON_FACTORY_V2(unilume, unilume::fcitx5::UniLumeFactory)
#else
FCITX_ADDON_FACTORY(unilume::fcitx5::UniLumeFactory)
#endif
