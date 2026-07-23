// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_suites.h"
#include "test_support/test_context.h"

namespace unilume::test {

void runEditingTests(EngineFixture &engine, TestContext &context)
{
    engine.type("aw", UkTelex);
    engine.pressBackspace();
    context.expectEqual("backspace composed character", engine.output(), "");
    context.expectValidUtf8("backspace composed character", engine.output());

    engine.type("tieengs", UkTelex);
    engine.pressBackspace();
    context.expectEqual("backspace after composed word", engine.output(), "tiến");
    engine.pressBackspace();
    context.expectEqual("repeated backspace", engine.output(), "tiế");

    engine.type("aw", UkTelex);
    engine.restoreKeyStrokes();
    context.expectEqual("restore original keystrokes", engine.output(), "aw");
    context.expectValidUtf8("restore original keystrokes", engine.output());

    engine.begin(UkTelex);
    engine.feed("aw");
    engine.resetContext();
    engine.feed("s");
    context.expectEqual("reset context", engine.output(), "ăs");

    engine.begin(UkTelex);
    engine.feed("aw");
    engine.setInputMethod(UkVni);
    engine.feed("a6");
    context.expectEqual("switch method resets context", engine.output(), "ăâ");
    context.expectValidUtf8("switch method resets context", engine.output());
}

} // namespace unilume::test
