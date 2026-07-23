// SPDX-License-Identifier: GPL-2.0-or-later

#include "corpus_loader.h"

#include <array>
#include <fstream>
#include <stdexcept>
#include <string>

namespace unilume::benchmark {
namespace {

UkInputMethod parseMethod(std::string_view method)
{
    if (method == "telex") {
        return UkTelex;
    }
    if (method == "vni") {
        return UkVni;
    }
    if (method == "viqr") {
        return UkViqr;
    }
    throw std::runtime_error("unknown input method: " + std::string(method));
}

std::vector<KeyEvent> parseEvents(std::string_view encoded)
{
    std::vector<KeyEvent> events;
    for (std::size_t index = 0; index < encoded.size();) {
        if (encoded.substr(index).starts_with("<BS>")) {
            events.push_back({EventType::backspace, 0});
            index += 4;
        } else if (encoded.substr(index).starts_with("<RESET>")) {
            events.push_back({EventType::reset, 0});
            index += 7;
        } else {
            events.push_back(
                {EventType::key, static_cast<unsigned char>(encoded[index])});
            ++index;
        }
    }
    return events;
}

std::array<std::string_view, 4> splitLine(const std::string &line)
{
    std::array<std::string_view, 4> fields;
    std::size_t start = 0;
    for (std::size_t field = 0; field < fields.size(); ++field) {
        const std::size_t end =
            field + 1 == fields.size() ? line.size() : line.find('\t', start);
        if (end == std::string::npos) {
            throw std::runtime_error("corpus row must contain four fields");
        }
        fields[field] = std::string_view{line}.substr(start, end - start);
        start = end + 1;
    }
    return fields;
}

Corpus loadCorpus(const std::filesystem::path &path)
{
    std::ifstream input{path};
    if (!input) {
        throw std::runtime_error("cannot open corpus: " + path.string());
    }

    Corpus corpus{path.stem().string(), {}};
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty() || line.starts_with('#')) {
            continue;
        }
        try {
            const auto fields = splitLine(line);
            corpus.scenarios.push_back({
                std::string(fields[0]),
                parseMethod(fields[1]),
                parseEvents(fields[2]),
                std::string(fields[3]),
            });
        } catch (const std::exception &error) {
            throw std::runtime_error(path.string() + ":" +
                                     std::to_string(line_number) + ": " +
                                     error.what());
        }
    }
    if (corpus.scenarios.empty()) {
        throw std::runtime_error("corpus has no scenarios: " + path.string());
    }
    return corpus;
}

} // namespace

std::vector<Corpus> loadCorpora(const std::filesystem::path &directory,
                                std::string_view selection)
{
    static constexpr std::array names{
        "telex",
        "vni",
        "viqr",
        "urls_and_emails",
        "code_like",
        "unicode",
        "mixed",
    };

    std::vector<Corpus> corpora;
    for (const std::string_view name : names) {
        if (selection == "all" || selection == name) {
            corpora.push_back(loadCorpus(directory / (std::string(name) +
                                                       ".tsv")));
        }
    }
    if (corpora.empty()) {
        throw std::runtime_error("unknown corpus selection: " +
                                 std::string(selection));
    }
    return corpora;
}

} // namespace unilume::benchmark
