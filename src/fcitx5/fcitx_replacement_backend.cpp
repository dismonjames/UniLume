// SPDX-License-Identifier: GPL-2.0-or-later

#include "fcitx_replacement_backend.h"

#include "utf8_validation.h"

#include <fcitx-utils/capabilityflags.h>
#include <fcitx/surroundingtext.h>
#include <string>

namespace unilume::fcitx5 {

FcitxReplacementBackend::FcitxReplacementBackend(
    fcitx::InputContext &input_context)
    : input_context_(input_context)
{
}

bool FcitxReplacementBackend::canReplace(
    std::int32_t delete_before_cursor) const
{
    if (delete_before_cursor <= 0) {
        return true;
    }
    if (!input_context_.capabilityFlags().test(
            fcitx::CapabilityFlag::SurroundingText)) {
        return false;
    }
    const auto delete_count =
        static_cast<std::size_t>(delete_before_cursor);
    if (committed_characters_ >= delete_count) {
        return true;
    }

    const fcitx::SurroundingText &surrounding =
        input_context_.surroundingText();
    return surrounding.isValid() &&
           surrounding.anchor() == surrounding.cursor() &&
           surrounding.cursor() >= delete_count &&
           core::isValidUtf8(surrounding.text());
}

platform::ReplacementStatus
FcitxReplacementBackend::requestReplacement(
    std::uint64_t sequence_id,
    std::int32_t delete_before_cursor,
    std::string_view commit_text)
{
    if (sequence_id <= last_sequence_id_ ||
        delete_before_cursor < 0 ||
        !core::isValidUtf8(commit_text) ||
        !canReplace(delete_before_cursor)) {
        return platform::ReplacementStatus::failed;
    }

    const auto delete_count =
        static_cast<std::size_t>(delete_before_cursor);
    if (delete_count != 0) {
        input_context_.deleteSurroundingText(
            -delete_before_cursor,
            static_cast<unsigned int>(delete_count));
        committed_characters_ =
            committed_characters_ > delete_count
                ? committed_characters_ - delete_count
                : 0;
    }
    if (!commit_text.empty()) {
        input_context_.commitString(std::string(commit_text));
        committed_characters_ += utf8Characters(commit_text);
    }
    last_sequence_id_ = sequence_id;
    return platform::ReplacementStatus::completed;
}

bool FcitxReplacementBackend::cancel(std::uint64_t)
{
    // Fcitx commit/delete calls are synchronous requests on the event thread.
    return false;
}

void FcitxReplacementBackend::reset()
{
    committed_characters_ = 0;
}

std::size_t FcitxReplacementBackend::utf8Characters(std::string_view text)
{
    std::size_t count = 0;
    for (const unsigned char byte : text) {
        count += (byte & 0xc0) != 0x80;
    }
    return count;
}

} // namespace unilume::fcitx5
