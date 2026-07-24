// SPDX-License-Identifier: GPL-2.0-or-later

// Browser input session lifecycle tests.
//
// Unlike browser_capability_tests.cpp (which tests PreeditFallbackController
// in isolation), these tests connect the full policy lifecycle:
//
//   capability observations
//   → InputModePolicy
//   → composition lifecycle
//   → selected InputPath
//   → preedit controller output
//
// Each case models a capability schedule at specific composition
// boundaries rather than relying on application labels.

#include "input_mode_policy.h"
#include "preedit_fallback_controller.h"
#include "test_assertions.h"
#include "test_suites.h"

#include <string>
#include <string_view>

namespace unilume::integration::test {
namespace {

// Lightweight session mirroring the key-event flow in InputContextState:
//
//   synchronize (observe capability)
//   → submitChar (route to PreeditFallbackController)
//   → composition boundary → policy resetForCompositionEnd
class BrowserInputSession {
public:
    platform::InputPath synchronize(bool direct_available);

    std::string submitChar(char key);
    std::string submitString(std::string_view input);

    void controlShortcut();
    void navigationBoundary();
    void focusReset();

    [[nodiscard]] platform::InputPath path() const;
    [[nodiscard]] std::string_view preedit() const;

private:
    platform::InputModePolicy policy_;
    core::PreeditFallbackController preedit_;
};

platform::InputPath BrowserInputSession::synchronize(bool direct_available)
{
    return policy_.observe(direct_available);
}

std::string BrowserInputSession::submitChar(char key)
{
    const core::PreeditAction action = preedit_.submit({
        core::KeyKind::text,
        std::string_view{&key, 1},
        false,
        false,
        false,
    });
    if (preedit_.preedit().empty()) {
        policy_.resetForCompositionEnd();
    }
    return std::string(action.commit_text);
}

std::string BrowserInputSession::submitString(std::string_view input)
{
    std::string committed;
    for (const char key : input) {
        committed.append(submitChar(key));
    }
    return committed;
}

void BrowserInputSession::controlShortcut()
{
    policy_.resetForCompositionEnd();
    preedit_.reset();
}

void BrowserInputSession::navigationBoundary()
{
    policy_.resetForCompositionEnd();
    preedit_.reset();
}

void BrowserInputSession::focusReset()
{
    policy_.reset();
    preedit_.reset();
}

platform::InputPath BrowserInputSession::path() const
{
    return policy_.path();
}

std::string_view BrowserInputSession::preedit() const
{
    return preedit_.preedit();
}

} // namespace

void runBrowserInputSessionTests(Assertions &assertions)
{
    // -- Case 1: never has SurroundingText --
    // Capability sequence: false, false, false...
    //
    // Composition 1: observe(false) → preedit
    // Mid-composition: observe(false) → stays preedit
    // Composition end → unknown
    // Composition 2: observe(false) → preedit again
    // Output correct, no lost text across compositions.
    {
        BrowserInputSession session;

        assertions.truth("case1: policy starts unknown",
            session.path() == platform::InputPath::unknown);

        session.synchronize(false);
        assertions.truth("case1: no capability → preedit",
            session.path() == platform::InputPath::preedit);

        session.synchronize(false);
        assertions.truth("case1: mid-composition stays preedit",
            session.path() == platform::InputPath::preedit);

        std::string committed = session.submitString("tooi ");
        assertions.equal("case1: first composition output",
            committed, "tôi ");
        assertions.truth("case1: composition end → unknown",
            session.path() == platform::InputPath::unknown);

        session.synchronize(false);
        assertions.truth("case1: second composition → preedit",
            session.path() == platform::InputPath::preedit);

        committed = session.submitString("ddang ");
        assertions.equal("case1: second composition output",
            committed, "đang ");
        assertions.truth("case1: second composition end → unknown",
            session.path() == platform::InputPath::unknown);
    }

    // -- Case 2: capability appears between compositions --
    // Sequence: false → true
    //
    // Composition 1: observe(false) → preedit
    // Mid-composition: observe(true) → NO promotion
    // Same composition completes via preedit safely
    // Composition end → unknown
    // Composition 2: observe(true) → direct
    {
        BrowserInputSession session;

        session.synchronize(false);
        assertions.truth("case2: first composition → preedit",
            session.path() == platform::InputPath::preedit);

        // Capability becomes true during composition — no promotion
        session.synchronize(true);
        assertions.truth("case2: no promotion during composition",
            session.path() == platform::InputPath::preedit);

        std::string committed = session.submitString("tooi ");
        assertions.equal("case2: output via preedit",
            committed, "tôi ");
        assertions.truth("case2: composition end → unknown",
            session.path() == platform::InputPath::unknown);

        // Between compositions: observe(true) → direct
        session.synchronize(true);
        assertions.truth("case2: next composition → direct",
            session.path() == platform::InputPath::direct);
    }

    // -- Case 3: direct loses capability mid-composition --
    // Sequence: true → false
    //
    // Composition 1: observe(true) → direct
    // Capability lost mid-composition → demote to preedit
    // Capability restored same composition → no re-promote
    // Composition end → unknown
    // Next composition: observe(false) → preedit
    {
        platform::InputModePolicy policy;

        policy.observe(true);
        assertions.truth("case3: starts direct",
            policy.path() == platform::InputPath::direct);

        policy.observe(false);
        assertions.truth("case3: capability loss → preedit",
            policy.path() == platform::InputPath::preedit);

        policy.observe(true);
        assertions.truth("case3: no re-promote same composition",
            policy.path() == platform::InputPath::preedit);

        policy.resetForCompositionEnd();
        assertions.truth("case3: composition end → unknown",
            policy.path() == platform::InputPath::unknown);

        policy.observe(false);
        assertions.truth("case3: next composition → preedit",
            policy.path() == platform::InputPath::preedit);
    }

    // -- Case 4: focus reset --
    //
    // Start composition, call full reset → unknown
    // Next composition re-evaluates from fresh capability.
    {
        BrowserInputSession session;

        session.synchronize(false);
        session.submitString("tooi");
        assertions.equal("case4: preedit before reset",
            session.preedit(), "tôi");

        session.focusReset();
        assertions.truth("case4: focus reset → unknown",
            session.path() == platform::InputPath::unknown);
        assertions.equal("case4: focus reset clears preedit",
            session.preedit(), "");

        session.synchronize(false);
        assertions.truth("case4: after reset → preedit",
            session.path() == platform::InputPath::preedit);
    }

    // -- Case 5: control / navigation boundary --
    //
    // Active preedit → control/navigation → preedit committed
    // Policy → unknown → next key re-evaluates.
    {
        BrowserInputSession session;

        session.synchronize(false);
        session.submitString("tooi");
        assertions.equal("case5: preedit before control",
            session.preedit(), "tôi");

        session.controlShortcut();
        assertions.equal("case5: control clears preedit",
            session.preedit(), "");
        assertions.truth("case5: control → unknown",
            session.path() == platform::InputPath::unknown);

        session.synchronize(false);
        assertions.truth("case5: after control → preedit",
            session.path() == platform::InputPath::preedit);
    }

    // -- Case 6: browser corpus through compositions --
    //
    // 6a: capability always false across multiple compositions.
    // 6b: capability false for composition 1, true for composition 2.
    // Output must be deterministic: no lost, duplicate, or reordered.
    {
        // 6a: always false
        {
            BrowserInputSession session;
            session.synchronize(false);

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

            const std::string committed = session.submitString(input);
            assertions.equal(
                "case6a: corpus output matches expected",
                committed + std::string(session.preedit()),
                expected);
        }

        // 6b: false for first composition, true for second
        {
            BrowserInputSession session;

            session.synchronize(false);
            std::string committed = session.submitString("tooi ");
            assertions.equal("case6b: first composition output",
                committed, "tôi ");
            // Policy resets to unknown at composition end

            session.synchronize(true);
            assertions.truth("case6b: next composition → direct",
                session.path() == platform::InputPath::direct);
        }
    }

    // -- Case 7: policy starts unknown --
    // Verifies the invariant that a fresh InputModePolicy is always
    // in the unknown state before the first observation.
    {
        platform::InputModePolicy fresh;
        assertions.truth("case7: fresh policy unknown",
            fresh.path() == platform::InputPath::unknown);
    }
}

} // namespace unilume::integration::test
