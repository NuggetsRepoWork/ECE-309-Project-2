#include "core/sentinel_scanner.h"

#include <stdexcept>

SentinelScanner::SentinelScanner(std::string sentinel)
    : sentinel_(std::move(sentinel)), pending_("") {

    if (sentinel_.empty()) {
        throw std::invalid_argument("Sentinel cannot be empty");
    }
}

SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {

    std::string combined;

    combined.reserve(pending_.size() + chunk.size());

    combined = pending_;
    combined.append(chunk.data(), chunk.size());

    std::size_t found = combined.find(sentinel_);

    if (found != std::string::npos) {

        std::string safe_text = combined.substr(0, found);

        pending_.clear();

        return { safe_text, true };
    }

    std::size_t keep_count = sentinel_.size() - 1;

    if (combined.size() <= keep_count) {

        pending_ = combined;

        return { "", false };
    }

    std::size_t safe_count = combined.size() - keep_count;

    std::string safe_text = combined.substr(0, safe_count);

    pending_ = combined.substr(safe_count);

    return { safe_text, false };
}

SentinelScanner::Out SentinelScanner::flush() {

    std::string safe_text = pending_;

    pending_.clear();

    return { safe_text, false };
}