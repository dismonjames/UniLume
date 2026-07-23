// SPDX-License-Identifier: GPL-2.0-or-later

#include "integration_fixture.h"
#include "test_assertions.h"
#include "test_suites.h"

namespace unilume::integration::test {

void runImmediateTests(Assertions &assertions)
{
    IntegrationFixture fixture;
    fixture.type("tooi ddang gox tieengs Vieetj");
    fixture.drain();
    assertions.equal(
        "direct commit Telex", fixture.output(), "tôi đang gõ tiếng Việt");
    assertions.truth("direct commit UTF-8", isValidUtf8(fixture.output()));
    assertions.equal(
        "direct commit final queue", fixture.metrics().queue_depth, 0);
    assertions.truth(
        "direct commit no active transaction",
        !fixture.metrics().active_transaction);

    IntegrationFixture url;
    url.type("http://abc.com/a1 user@example.com value_2");
    url.drain();
    assertions.equal(
        "URL email and code passthrough",
        url.output(),
        "http://abc.com/a1 ủe@ẽample.com value_2");

    IntegrationFixture unicode;
    unicode.type("ID 日本語 한국어 中文 🚀");
    unicode.drain();
    assertions.equal(
        "Unicode passthrough",
        unicode.output(),
        "ID 日本語 한국어 中文 🚀");

    IntegrationFixture editing;
    editing.type("tieengs");
    editing.backspace();
    editing.drain();
    assertions.equal("backspace direct commit", editing.output(), "tiến");
    editing.reset();
    editing.type(" aw");
    editing.drain();
    assertions.equal("reset then compose", editing.output(), "tiến ă");

    IntegrationFixture first_context;
    IntegrationFixture second_context;
    first_context.type("w");
    second_context.type("w");
    first_context.drain();
    second_context.drain();
    assertions.equal("first independent context", first_context.output(), "ư");
    assertions.equal(
        "second independent context", second_context.output(), "ư");
}

} // namespace unilume::integration::test
