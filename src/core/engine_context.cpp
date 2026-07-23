// SPDX-License-Identifier: GPL-2.0-or-later

#include "engine_context.h"

#include <cctype>
#include <stdexcept>

namespace unilume::core {
namespace {

bool isCompositionBoundary(unsigned char key)
{
    return key <= 0x7f &&
           (std::isspace(key) != 0 || std::ispunct(key) != 0);
}

} // namespace

EngineContext::EngineContext(UlInputMethod method)
{
    if (ul_engine_create(method, &context_) != UL_STATUS_OK) {
        throw std::runtime_error("cannot create UniLume engine context");
    }
}

EngineContext::~EngineContext()
{
    ul_engine_destroy(context_);
}

KeyResult EngineContext::process(const KeyInput &input)
{
    const std::uint64_t sequence = next_sequence_++;
    switch (input.kind) {
    case KeyKind::text:
        return processText(input, sequence);
    case KeyKind::backspace:
        return processBackspace(sequence);
    case KeyKind::reset:
    case KeyKind::navigation:
        reset();
        return {false, sequence, 0, {}, {}, true, false};
    }
    return makeFailure(sequence, false);
}

void EngineContext::reset()
{
    ul_engine_reset(context_);
}

void EngineContext::setInputMethod(UlInputMethod method)
{
    if (ul_engine_set_input_method(context_, method) != UL_STATUS_OK) {
        throw std::invalid_argument("unsupported UniLume input method");
    }
}

KeyResult EngineContext::processText(const KeyInput &input,
                                     std::uint64_t sequence)
{
    if (input.has_control_modifier || input.text.empty()) {
        reset();
        return {false, sequence, 0, {}, {}, true, false};
    }

    const auto first = static_cast<unsigned char>(input.text.front());
    if (input.text.size() != 1 || first > 0x7f) {
        reset();
        return {true, sequence, 0, input.text, {}, true, false};
    }

    UlEngineEdit edit{};
    const UlStatus status = ul_engine_process_ascii(
        context_,
        first,
        input.shift_pressed,
        input.caps_lock_on,
        output_.data(),
        output_.size(),
        &edit);
    if (status != UL_STATUS_OK) {
        return makeFailure(sequence, true);
    }
    if (!edit.handled) {
        output_[0] = static_cast<char>(first);
        edit.output_size = 1;
    }
    const bool reset_context = isCompositionBoundary(first);
    if (reset_context) {
        reset();
    }
    return {
        true,
        sequence,
        edit.delete_before_cursor,
        std::string_view{output_.data(), edit.output_size},
        {},
        reset_context,
        false,
    };
}

KeyResult EngineContext::processBackspace(std::uint64_t sequence)
{
    UlEngineEdit edit{};
    const UlStatus status = ul_engine_backspace(
        context_, output_.data(), output_.size(), &edit);
    if (status != UL_STATUS_OK) {
        return makeFailure(sequence, false);
    }
    if (!edit.handled) {
        return {false, sequence, 0, {}, {}, false, false};
    }
    return {
        true,
        sequence,
        edit.delete_before_cursor,
        std::string_view{output_.data(), edit.output_size},
        {},
        false,
        false,
    };
}

KeyResult EngineContext::makeFailure(std::uint64_t sequence, bool handled)
{
    reset();
    return {handled, sequence, 0, {}, {}, true, true};
}

} // namespace unilume::core
