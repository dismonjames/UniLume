// SPDX-License-Identifier: GPL-2.0-or-later

#include "test_support/engine_fixture.h"

namespace unilume::test {

namespace {

std::size_t previousUtf8Character(const std::string &text)
{
    if (text.empty()) {
        return 0;
    }
    std::size_t position = text.size() - 1;
    while (position > 0 &&
           (static_cast<unsigned char>(text[position]) & 0xc0) == 0x80) {
        --position;
    }
    return position;
}

} // namespace

EngineFixture::EngineFixture()
{
    UnikeySetup();
}

EngineFixture::~EngineFixture()
{
    UnikeyCleanup();
}

std::string EngineFixture::type(std::string_view keys, UkInputMethod method)
{
    begin(method);
    feed(keys);
    return output_;
}

void EngineFixture::begin(UkInputMethod method)
{
    output_.clear();
    UnikeyResetBuf();
    UnikeySetInputMethod(method);
    UnikeySetCapsState(0, 0);
}

void EngineFixture::feed(std::string_view keys)
{
    for (const unsigned char key : keys) {
        UnikeyFilter(key);
        if (UnikeyBackspaces == 0 && UnikeyBufChars == 0) {
            output_.push_back(static_cast<char>(key));
        } else {
            applyEngineOutput();
        }
    }
}

void EngineFixture::pressBackspace()
{
    UnikeyBackspacePress();
    applyEngineOutput();
}

void EngineFixture::restoreKeyStrokes()
{
    UnikeyRestoreKeyStrokes();
    applyEngineOutput();
}

void EngineFixture::resetContext()
{
    UnikeyResetBuf();
}

void EngineFixture::setInputMethod(UkInputMethod method)
{
    UnikeySetInputMethod(method);
}

const std::string &EngineFixture::output() const
{
    return output_;
}

void EngineFixture::applyEngineOutput()
{
    eraseUtf8Characters(UnikeyBackspaces);
    output_.append(reinterpret_cast<const char *>(UnikeyBuf),
                   static_cast<std::size_t>(UnikeyBufChars));
}

void EngineFixture::eraseUtf8Characters(int count)
{
    while (count-- > 0 && !output_.empty()) {
        output_.erase(previousUtf8Character(output_));
    }
}

} // namespace unilume::test
