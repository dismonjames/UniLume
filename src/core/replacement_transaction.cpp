// SPDX-License-Identifier: GPL-2.0-or-later

#include "replacement_transaction.h"

#include <algorithm>

namespace unilume::core {

bool ReplacementTransaction::prepare(
    std::uint64_t sequence_id,
    std::int32_t delete_before_cursor,
    std::string_view commit_text,
    std::string_view fallback_text)
{
    if (active() || commit_text.size() > commit_buffer_.size() ||
        fallback_text.size() > fallback_buffer_.size() ||
        delete_before_cursor < 0) {
        return false;
    }
    std::copy(commit_text.begin(), commit_text.end(), commit_buffer_.begin());
    std::copy(
        fallback_text.begin(), fallback_text.end(), fallback_buffer_.begin());
    commit_size_ = commit_text.size();
    fallback_size_ = fallback_text.size();
    sequence_id_ = sequence_id;
    delete_before_cursor_ = delete_before_cursor;
    state_ = TransactionState::prepared;
    return true;
}

void ReplacementTransaction::markRequested()
{
    state_ = TransactionState::commit_requested;
}

void ReplacementTransaction::complete()
{
    state_ = TransactionState::completed;
}

void ReplacementTransaction::abort()
{
    state_ = TransactionState::aborted;
}

void ReplacementTransaction::clear()
{
    commit_size_ = 0;
    fallback_size_ = 0;
    sequence_id_ = 0;
    delete_before_cursor_ = 0;
    state_ = TransactionState::idle;
}

TransactionState ReplacementTransaction::state() const
{
    return state_;
}

bool ReplacementTransaction::active() const
{
    return state_ == TransactionState::prepared ||
           state_ == TransactionState::commit_requested;
}

std::uint64_t ReplacementTransaction::sequenceId() const
{
    return sequence_id_;
}

std::int32_t ReplacementTransaction::deleteBeforeCursor() const
{
    return delete_before_cursor_;
}

std::string_view ReplacementTransaction::commitText() const
{
    return {commit_buffer_.data(), commit_size_};
}

std::string_view ReplacementTransaction::fallbackText() const
{
    return {fallback_buffer_.data(), fallback_size_};
}

} // namespace unilume::core
