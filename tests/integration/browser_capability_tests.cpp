// SPDX-License-Identifier: GPL-2.0-or-later

// Browser capability lifecycle tests.
//
// Each browser profile models the observed surrounding-text capability
// timing from real-app validation (docs/real-application-validation.md).
//
// Firefox/Chrome/Electron: surrounding text NOT available on first key,
// not available during composition, not available after composition end.
// These contexts stay on client-preedit fallback permanently.
//
// Qt (KWrite) / GTK (Zenity): surrounding text available from first key.
// These contexts use direct zero-preedit.

#include "preedit_fallback_controller.h"
#include "test_assertions.h"
#include "test_suites.h"

#include <string>

namespace unilume::integration::test {
namespace {

void typeText(core::PreeditFallbackController &controller,
              std::string_view input,
              std::string &committed)
{
    for (const char key : input) {
        const core::PreeditAction action = controller.submit({
            core::KeyKind::text,
            std::string_view{&key, 1},
            false,
            false,
            false,
        });
        committed.append(action.commit_text);
    }
}

} // namespace

void runBrowserCapabilityTests(Assertions &assertions)
{
    // -- Firefox-like profile: no surrounding text ever --
    // The context starts without SurroundingText capability.
    // Every key goes through client-preedit fallback.
    // Each composition boundary allows re-evaluation, but the
    // browser never provides surrounding text, so it stays on preedit.
    {
        core::PreeditFallbackController firefox;
        std::string committed;

        // First composition
        typeText(firefox, "tooi ", committed);
        assertions.equal(
            "Firefox first word commits correctly",
            committed, "tôi ");
        assertions.equal(
            "Firefox first word preedit drained",
            firefox.preedit(), "");

        // Second composition
        typeText(firefox, "ddang ", committed);
        assertions.equal(
            "Firefox second word commits correctly",
            committed, "tôi đang ");
        assertions.equal(
            "Firefox second word preedit drained",
            firefox.preedit(), "");
    }

    // -- Chrome-like profile (same as Firefox behavior) --
    {
        core::PreeditFallbackController chrome;
        std::string committed;

        typeText(chrome, "tieengs ", committed);
        assertions.equal(
            "Chrome word commits correctly",
            committed, "tiếng ");
        assertions.equal(
            "Chrome preedit drained",
            chrome.preedit(), "");

        // URL literal must pass through unchanged
        typeText(chrome, "http://abc.com/a1 ", committed);
        assertions.equal(
            "Chrome URL preserved",
            committed, "tiếng http://abc.com/a1 ");
    }

    // -- VSCode/Electron-like profile --
    {
        core::PreeditFallbackController electron;
        std::string committed;

        // Code-like input with Telex composition
        typeText(electron, "std::vector<int> ", committed);
        assertions.equal(
            "Electron code literal first pass",
            committed, "std::vector<int> ");

        typeText(electron, "foo_bar->value ", committed);
        assertions.equal(
            "Electron code literal second pass",
            committed, "std::vector<int> foo_bar->value ");
    }

    // -- Multi-composition browser corpus --
    {
        core::PreeditFallbackController browser;
        std::string committed;

        const std::string input =
            "tooi tieengs dday laf booj gox tieengs Vieetj "
            "http://abc.com/a1 user@example.com "
            "std::vector<int> Console.WriteLine(\"hello\"); "
            "foo_bar->value ";
        const std::string expected =
            "tôi tiếng đay là bộ gõ tiếng Việt "
            "http://abc.com/a1 user@example.com "
            "std::vector<int> Console.WriteLine(\"hello\"); "
            "foo_bar->value ";

        typeText(browser, input, committed);
        assertions.equal(
            "browser corpus output matches expected",
            committed + std::string(browser.preedit()),
            expected);
    }

    // -- Backspace within browser preedit --
    {
        core::PreeditFallbackController editing;
        std::string committed;

        typeText(editing, "tieengs", committed);
        assertions.equal(
            "preedit before backspace", editing.preedit(), "tiếng");

        const core::PreeditAction bs = editing.submit({
            core::KeyKind::backspace, {}, false, false, false});
        assertions.truth("backspace handled", bs.handled);
        assertions.equal(
            "preedit after backspace", editing.preedit(), "tiến");
    }

    // -- Reset between compositions works --
    {
        core::PreeditFallbackController reset_test;
        std::string committed;

        typeText(reset_test, "tooi", committed);
        reset_test.reset();
        assertions.equal(
            "reset clears preedit", reset_test.preedit(), "");

        typeText(reset_test, "aw", committed);
        assertions.equal(
            "post-reset composition works",
            reset_test.preedit(), "ă");
    }

    // -- Control modifier clears preedit --
    {
        core::PreeditFallbackController ctrl;
        std::string committed;

        typeText(ctrl, "tooi", committed);
        const core::PreeditAction action = ctrl.submit({
            core::KeyKind::text, {}, false, false, true});
        assertions.equal(
            "control modifier clears preedit",
            ctrl.preedit(), "");
    }
}

} // namespace unilume::integration::test
