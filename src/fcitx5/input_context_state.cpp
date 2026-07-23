// SPDX-License-Identifier: GPL-2.0-or-later

#include "input_context_state.h"

#include "fcitx_key_mapper.h"

namespace unilume::fcitx5 {

InputContextState::InputContextState(fcitx::InputContext &input_context)
    : backend_(input_context), controller_(backend_)
{
}

void InputContextState::keyEvent(fcitx::KeyEvent &event)
{
    const MappedKey mapped = mapKeyEvent(event);
    if (mapped.status == MappingStatus::ignored) {
        return;
    }
    if (mapped.status == MappingStatus::reset) {
        reset();
        return;
    }

    const core::SubmissionStatus status = controller_.submit(mapped.input());
    if (status != core::SubmissionStatus::unhandled) {
        event.filterAndAccept();
    } else if (mapped.kind == core::KeyKind::backspace) {
        backend_.reset();
    }
}

void InputContextState::reset()
{
    controller_.resetForFocus();
    backend_.reset();
}

} // namespace unilume::fcitx5
