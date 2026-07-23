// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "key_input.h"
#include "key_result.h"
#include "unilume_context.h"

#include <array>
#include <cstdint>

namespace unilume::core {

class EngineContext {
public:
    explicit EngineContext(UlInputMethod method = UL_INPUT_METHOD_TELEX);
    ~EngineContext();

    EngineContext(const EngineContext &) = delete;
    EngineContext &operator=(const EngineContext &) = delete;

    KeyResult process(const KeyInput &input);
    void reset();
    void setInputMethod(UlInputMethod method);

private:
    KeyResult processText(const KeyInput &input, std::uint64_t sequence);
    KeyResult processBackspace(std::uint64_t sequence);
    KeyResult makeFailure(std::uint64_t sequence, bool handled);

    static constexpr std::size_t output_capacity = 1024;
    UlEngineContext *context_{};
    std::array<char, output_capacity> output_{};
    std::uint64_t next_sequence_{1};
};

} // namespace unilume::core
