// UniLume engine regression tests
// SPDX-License-Identifier: GPL-2.0-or-later

#include "unikey.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

std::size_t previousUtf8Character(const std::string &text, std::size_t end)
{
    if (end == 0) {
        return 0;
    }

    std::size_t pos = end - 1;
    while (pos > 0 &&
           (static_cast<unsigned char>(text[pos]) & 0xc0) == 0x80) {
        --pos;
    }
    return pos;
}

void eraseUtf8Characters(std::string &text, int count)
{
    while (count-- > 0 && !text.empty()) {
        text.erase(previousUtf8Character(text, text.size()));
    }
}

std::string type(const std::string &keys, UkInputMethod method)
{
    UnikeyResetBuf();
    UnikeySetInputMethod(method);
    UnikeySetCapsState(0, 0);

    std::string output;
    for (std::string::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        UnikeyFilter(static_cast<unsigned char>(*it));
        if (UnikeyBackspaces == 0 && UnikeyBufChars == 0) {
            output.push_back(*it);
        } else {
            eraseUtf8Characters(output, UnikeyBackspaces);
            output.append(reinterpret_cast<const char *>(UnikeyBuf),
                          static_cast<std::size_t>(UnikeyBufChars));
        }
    }
    return output;
}

void pressBackspace(std::string &output)
{
    UnikeyBackspacePress();
    eraseUtf8Characters(output, UnikeyBackspaces);
    output.append(reinterpret_cast<const char *>(UnikeyBuf),
                  static_cast<std::size_t>(UnikeyBufChars));
}

bool isValidUtf8(const std::string &text)
{
    std::size_t i = 0;
    while (i < text.size()) {
        const unsigned char lead = static_cast<unsigned char>(text[i]);
        std::size_t continuation = 0;
        if (lead <= 0x7f) {
            continuation = 0;
        } else if (lead >= 0xc2 && lead <= 0xdf) {
            continuation = 1;
        } else if (lead >= 0xe0 && lead <= 0xef) {
            continuation = 2;
        } else if (lead >= 0xf0 && lead <= 0xf4) {
            continuation = 3;
        } else {
            return false;
        }

        if (i + continuation >= text.size()) {
            return false;
        }
        for (std::size_t j = 1; j <= continuation; ++j) {
            if ((static_cast<unsigned char>(text[i + j]) & 0xc0) != 0x80) {
                return false;
            }
        }
        i += continuation + 1;
    }
    return true;
}

int failures = 0;

void expectEqual(const char *name,
                 const std::string &actual,
                 const std::string &expected)
{
    if (actual == expected) {
        return;
    }
    std::cerr << "FAIL " << name << ": expected [" << expected
              << "], got [" << actual << "]\n";
    ++failures;
}

void expectTrue(const char *name, bool value)
{
    if (!value) {
        std::cerr << "FAIL " << name << "\n";
        ++failures;
    }
}

} // namespace

int main()
{
    UnikeySetup();

    expectEqual("empty input", type("", UkTelex), "");
    expectEqual("plain ASCII", type("hello 123", UkTelex), "hello 123");
    expectEqual("basic Telex", type("tieengs Vieetj", UkTelex),
                "tiếng Việt");
    expectEqual("basic VNI", type("tie61ng Vie65t", UkVni),
                "tiếng Việt");
    expectEqual("Telex letters", type("aw aa ee oo ow uw dd", UkTelex),
                "ă â ê ô ơ ư đ");
    expectEqual("uppercase", type("DDUOWNGF", UkTelex), "ĐƯỜNG");
    expectEqual("repeated modifier", type("aaa", UkTelex), "aa");
    expectEqual("remove tone", type("asz", UkTelex), "a");

    std::string corrected = type("aw", UkTelex);
    pressBackspace(corrected);
    expectEqual("backspace removes a composed character", corrected, "");
    expectTrue("backspace output is UTF-8", isValidUtf8(corrected));

    expectTrue("Unicode output is valid UTF-8",
               isValidUtf8(type("tieengs Vieetj", UkTelex)));

    UnikeyCleanup();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "All engine tests passed\n";
    return EXIT_SUCCESS;
}
