// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "deterministic_backend.h"
#include "direct_commit_controller.h"

#include <string_view>

namespace unilume::integration::test {

class IntegrationFixture {
public:
    explicit IntegrationFixture(BackendProfile profile = {});

    void type(std::string_view text);
    void backspace();
    void reset();
    void focusChange();
    void pump(std::size_t events = 1);
    void drain();

    [[nodiscard]] const std::string &output() const;
    [[nodiscard]] const core::TransactionMetrics &metrics() const;
    [[nodiscard]] DeterministicBackend &backend();
    [[nodiscard]] core::DirectCommitController &controller();

private:
    static std::size_t codePointLength(std::string_view text);

    DeterministicBackend backend_;
    core::DirectCommitController controller_;
};

} // namespace unilume::integration::test
