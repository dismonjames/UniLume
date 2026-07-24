// SPDX-License-Identifier: GPL-2.0-or-later

// Browser input session lifecycle tests.
//
// These tests connect the full policy lifecycle used by
// InputContextState::keyEvent:
//
//   capability observations
//   → InputModePolicy
//   → DirectCommitController + DeterministicBackend (direct path)
//   → PreeditFallbackController (preedit path)
//   → composition lifecycle / controller state
//   → final document output
//
// Each case models a capability schedule at specific composition
// boundaries rather than relying on application labels.

#include "deterministic_backend.h"
#include "direct_commit_controller.h"
#include "input_mode_policy.h"
#include "preedit_fallback_controller.h"
#include "test_assertions.h"
#include "test_suites.h"

#include <stdexcept>
#include <string>
#include <string_view>

namespace unilume::integration::test {
namespace {

// Session helper mirroring InputContextState key-event flow:
//
//   synchronize (observe capability; demote resets direct controller)
//   → submitChar routes to DirectCommitController or PreeditFallbackController
//   → composition boundary → policy resetForCompositionEnd
//   → control/navigation → commitPendingPreedit then policy reset
class BrowserInputSession {
public:
    BrowserInputSession()
        // Backend always supports replacement. InputModePolicy alone
        // decides whether keys reach DirectCommitController; capability
        // schedules are modeled via synchronize(bool), not by mutating
        // the backend profile after construction.
        : profile_{.surrounding_text_available = true,
                   .record_event_log = true},
          backend_(profile_),
          direct_(backend_)
    {
    }

    platform::InputPath synchronize(bool direct_available);

    std::string submitChar(char key);
    std::string submitString(std::string_view input);

    // Returns the pending preedit text that was committed (may be empty).
    std::string controlShortcut();
    std::string navigationBoundary();
    void focusReset();

    [[nodiscard]] platform::InputPath path() const;
    [[nodiscard]] std::string_view preedit() const;
    // Full document: backend commits (both paths) + active preedit.
    [[nodiscard]] std::string output() const;
    [[nodiscard]] std::size_t backendEventCount() const;
    [[nodiscard]] const core::TransactionMetrics &directMetrics() const;

private:
    void applyPathTransition(platform::InputPath previous,
                             platform::InputPath current);
    void commitToDocument(std::string_view text);
    std::string commitPendingPreedit();
    void drainDirect();
    void pump();

    BackendProfile profile_;
    DeterministicBackend backend_;
    platform::InputModePolicy policy_;
    core::DirectCommitController direct_;
    core::PreeditFallbackController preedit_;
    bool last_capability_{false};
    std::uint64_t preedit_commit_sequence_{1};
};

platform::InputPath BrowserInputSession::synchronize(bool direct_available)
{
    last_capability_ = direct_available;
    const platform::InputPath previous = policy_.path();
    const platform::InputPath current = policy_.observe(direct_available);
    applyPathTransition(previous, current);
    return current;
}

void BrowserInputSession::applyPathTransition(
    platform::InputPath previous,
    platform::InputPath current)
{
    if (previous == current) {
        return;
    }
    // Mirror InputContextState::synchronizeMode on demotion: drop any
    // in-flight direct transaction state before continuing on preedit.
    if (current == platform::InputPath::preedit) {
        direct_.resetForFocus();
    }
    preedit_.reset();
}

std::string BrowserInputSession::submitChar(char key)
{
    // Production re-observes capability on every key that can reach the
    // engine, so demotion can happen mid-composition during typing.
    synchronize(last_capability_);

    const core::KeyInput input{
        core::KeyKind::text,
        std::string_view{&key, 1},
        false,
        false,
        false,
    };

    if (policy_.path() == platform::InputPath::direct) {
        direct_.submit(input);
        drainDirect();
        return {};
    }

    const core::PreeditAction action = preedit_.submit(input);
    if (!action.commit_text.empty()) {
        commitToDocument(action.commit_text);
    }
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

std::string BrowserInputSession::commitPendingPreedit()
{
    // Mirror InputContextState::commitPendingPreedit: flush pending
    // preedit into the document instead of discarding it.
    const std::string pending(preedit_.preedit());
    if (!pending.empty()) {
        commitToDocument(pending);
    }
    preedit_.reset();
    return pending;
}

std::string BrowserInputSession::controlShortcut()
{
    std::string committed;
    if (policy_.path() == platform::InputPath::preedit) {
        committed = commitPendingPreedit();
    }
    policy_.resetForCompositionEnd();
    return committed;
}

std::string BrowserInputSession::navigationBoundary()
{
    std::string committed;
    if (policy_.path() == platform::InputPath::preedit) {
        committed = commitPendingPreedit();
    }
    direct_.resetForFocus();
    policy_.resetForCompositionEnd();
    return committed;
}

void BrowserInputSession::focusReset()
{
    // Focus reset discards pending preedit without committing
    // (mirrors InputContextState::reset).
    direct_.resetForFocus();
    preedit_.reset();
    policy_.reset();
}

platform::InputPath BrowserInputSession::path() const
{
    return policy_.path();
}

std::string_view BrowserInputSession::preedit() const
{
    return preedit_.preedit();
}

std::string BrowserInputSession::output() const
{
    return backend_.text() + std::string(preedit_.preedit());
}

std::size_t BrowserInputSession::backendEventCount() const
{
    return backend_.eventLog().size();
}

const core::TransactionMetrics &BrowserInputSession::directMetrics() const
{
    return direct_.metrics();
}

void BrowserInputSession::commitToDocument(std::string_view text)
{
    // Preedit commits land in the application document the same way
    // input_context.commitString does. Model that as a zero-delete
    // backend replacement so a later direct path sees surrounding text.
    if (text.empty()) {
        return;
    }
    const platform::ReplacementStatus status = backend_.requestReplacement(
        preedit_commit_sequence_++, 0, text);
    if (status != platform::ReplacementStatus::completed) {
        throw std::runtime_error("preedit commit into document failed");
    }
}

void BrowserInputSession::drainDirect()
{
    for (std::size_t step = 0; step < 10000; ++step) {
        if (!backend_.hasPending() &&
            direct_.transactionState() == core::TransactionState::idle &&
            direct_.metrics().queue_depth == 0) {
            return;
        }
        if (backend_.hasPending()) {
            pump();
        } else if (direct_.activeSequence() != 0) {
            direct_.timeout(direct_.activeSequence());
        }
    }
    if (direct_.activeSequence() != 0) {
        direct_.timeout(direct_.activeSequence());
    }
    if (backend_.hasPending() || direct_.metrics().queue_depth != 0) {
        throw std::runtime_error("direct path event queue did not drain");
    }
}

void BrowserInputSession::pump()
{
    for (const BackendCompletion &completion : backend_.advance()) {
        direct_.complete(completion.sequence_id, completion.success);
    }
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
        assertions.equal("case1: document after first composition",
            session.output(), "tôi ");

        session.synchronize(false);
        assertions.truth("case1: second composition → preedit",
            session.path() == platform::InputPath::preedit);

        committed = session.submitString("ddang ");
        assertions.equal("case1: second composition output",
            committed, "đang ");
        assertions.truth("case1: second composition end → unknown",
            session.path() == platform::InputPath::unknown);
        assertions.equal("case1: document after both compositions",
            session.output(), "tôi đang ");
        assertions.truth("case1: direct queue idle after preedit path",
            session.directMetrics().queue_depth == 0);
    }

    // -- Case 2: capability appears between compositions --
    // Sequence: false → true
    //
    // Composition 1: observe(false) → preedit
    // Mid-composition: observe(true) → NO promotion
    // Same composition completes via preedit safely
    // Composition end → unknown
    // Composition 2: observe(true) → direct with real backend output
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
        assertions.equal("case2: document after preedit composition",
            session.output(), "tôi ");

        // Between compositions: observe(true) → direct
        session.synchronize(true);
        assertions.truth("case2: next composition → direct",
            session.path() == platform::InputPath::direct);

        const std::size_t events_before = session.backendEventCount();
        session.submitString("ddang ");
        assertions.truth("case2: direct path still direct after typing",
            session.path() == platform::InputPath::direct);
        assertions.equal("case2: direct path produces composed output",
            session.output(), "tôi đang ");
        assertions.truth("case2: direct path issued backend transactions",
            session.backendEventCount() > events_before);
        assertions.truth("case2: direct queue drained",
            session.directMetrics().queue_depth == 0);
        // Preedit state must not leak into the direct composition.
        assertions.equal("case2: no residual preedit after path switch",
            session.preedit(), "");
    }

    // -- Case 3: direct loses capability mid-composition --
    // Sequence: true → false
    //
    // Composition 1: observe(true) → direct with real output
    // Capability lost mid-composition → demote to preedit
    // Direct controller state is reset safely
    // Capability restored same composition → no re-promote
    // Continue on preedit without lost/duplicated text
    {
        BrowserInputSession session;

        session.synchronize(true);
        assertions.truth("case3: starts direct",
            session.path() == platform::InputPath::direct);

        session.submitString("tooi ");
        assertions.equal("case3: direct composition output",
            session.output(), "tôi ");
        assertions.truth("case3: direct issued backend events",
            session.backendEventCount() > 0);

        // Demote mid-composition: controller must reset safely.
        session.synchronize(false);
        assertions.truth("case3: capability loss → preedit",
            session.path() == platform::InputPath::preedit);
        assertions.truth("case3: demotion leaves direct queue empty",
            session.directMetrics().queue_depth == 0);
        assertions.equal("case3: demotion keeps committed document",
            session.output(), "tôi ");

        session.synchronize(true);
        assertions.truth("case3: no re-promote same composition",
            session.path() == platform::InputPath::preedit);

        session.submitString("ddang ");
        assertions.equal("case3: preedit after demotion appends correctly",
            session.output(), "tôi đang ");
        assertions.truth("case3: composition end → unknown",
            session.path() == platform::InputPath::unknown);

        session.synchronize(false);
        assertions.truth("case3: next composition → preedit",
            session.path() == platform::InputPath::preedit);
    }

    // -- Case 4: focus reset --
    //
    // Start composition, call full reset → unknown
    // Pending preedit is discarded (focus semantics), path re-evaluates.
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
        // Focus does not commit pending preedit into the document.
        assertions.equal("case4: focus does not commit pending preedit",
            session.output(), "");

        session.synchronize(false);
        assertions.truth("case4: after reset → preedit",
            session.path() == platform::InputPath::preedit);
    }

    // -- Case 5: control / navigation boundary --
    //
    // Active preedit → control/navigation → preedit committed (not discarded)
    // Policy → unknown → next key re-evaluates.
    {
        BrowserInputSession session;

        session.synchronize(false);
        session.submitString("tooi");
        assertions.equal("case5: preedit before control",
            session.preedit(), "tôi");

        const std::string flushed = session.controlShortcut();
        assertions.equal("case5: control commits pending preedit",
            flushed, "tôi");
        assertions.equal("case5: control clears preedit buffer",
            session.preedit(), "");
        assertions.equal("case5: control preserves text in document",
            session.output(), "tôi");
        assertions.truth("case5: control → unknown",
            session.path() == platform::InputPath::unknown);

        session.synchronize(false);
        assertions.truth("case5: after control → preedit",
            session.path() == platform::InputPath::preedit);

        session.submitString("ddang");
        assertions.equal("case5: preedit after control before navigation",
            session.preedit(), "đang");

        const std::string nav_flushed = session.navigationBoundary();
        assertions.equal("case5: navigation commits pending preedit",
            nav_flushed, "đang");
        assertions.equal("case5: navigation document keeps all commits",
            session.output(), "tôiđang");
        assertions.truth("case5: navigation → unknown",
            session.path() == platform::InputPath::unknown);
    }

    // -- Case 6: browser corpus through compositions --
    //
    // 6a: capability always false across multiple compositions.
    // 6b: capability false for composition 1, true for composition 2;
    //     direct path produces correct corpus tail via backend.
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

            session.submitString(input);
            assertions.equal(
                "case6a: corpus output matches expected",
                session.output(),
                expected);
        }

        // 6b: false for first composition, true for second
        {
            BrowserInputSession session;

            session.synchronize(false);
            session.submitString("tooi ");
            assertions.equal("case6b: first composition output",
                session.output(), "tôi ");
            assertions.truth("case6b: first composition end → unknown",
                session.path() == platform::InputPath::unknown);

            session.synchronize(true);
            assertions.truth("case6b: next composition → direct",
                session.path() == platform::InputPath::direct);

            const std::string direct_tail =
                "tieengs dday laf booj gox tieengs Vieetj "
                "http://abc.com/a1 user@example.com "
                "std::vector<int> Console.WriteLine(\"hello\"); "
                "foo_bar->value ";
            const std::string expected =
                "tôi tiếng đay là bộ gõ tiếng Việt "
                "http://abc.com/a1 user@example.com "
                "std::vector<int> Console.WriteLine(\"hello\"); "
                "foo_bar->value ";

            const std::size_t events_before = session.backendEventCount();
            session.submitString(direct_tail);
            assertions.equal(
                "case6b: mixed preedit→direct corpus matches expected",
                session.output(),
                expected);
            assertions.equal("case6b: no residual preedit on direct path",
                session.preedit(), "");
            assertions.truth("case6b: direct queue drained after corpus",
                session.directMetrics().queue_depth == 0);
            assertions.truth(
                "case6b: direct path issued additional backend events",
                session.backendEventCount() > events_before);
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

    // -- Case 8: pure direct path from the first composition --
    {
        BrowserInputSession session;
        session.synchronize(true);
        assertions.truth("case8: first observation → direct",
            session.path() == platform::InputPath::direct);

        session.submitString("tooi ddang gox tieengs Vieetj");
        assertions.equal("case8: pure direct Telex output",
            session.output(), "tôi đang gõ tiếng Việt");
        assertions.truth("case8: pure direct used backend transactions",
            session.backendEventCount() > 0);
        assertions.truth("case8: pure direct queue empty",
            session.directMetrics().queue_depth == 0);
    }
}

} // namespace unilume::integration::test
