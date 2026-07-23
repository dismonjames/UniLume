// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "unikey.h"

#include <string>
#include <string_view>

namespace unilume::test {

class EngineFixture {
public:
    EngineFixture();
    ~EngineFixture();

    EngineFixture(const EngineFixture &) = delete;
    EngineFixture &operator=(const EngineFixture &) = delete;

    std::string type(std::string_view keys, UkInputMethod method);
    void begin(UkInputMethod method);
    void feed(std::string_view keys);
    void pressBackspace();
    void restoreKeyStrokes();
    void resetContext();
    void setInputMethod(UkInputMethod method);

    [[nodiscard]] const std::string &output() const;

private:
    void applyEngineOutput();
    void eraseUtf8Characters(int count);

    std::string output_;
};

} // namespace unilume::test
