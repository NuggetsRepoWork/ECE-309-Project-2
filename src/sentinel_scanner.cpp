#include "core/sentinel_scanner.h"

SentinelScanner::SentinelScanner(std::string sentinel)
    : sentinel_(sentinel), pending_("") {
}

SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    Out result;
    result.safe_text = "";
    result.sentinel_found = false;

    for (char c : chunk) {
        pending_ = pending_ + c;

        if (pending_.size() <= sentinel_.size()) {
            if (pending_ == sentinel_) {
                result.sentinel_found = true;
                pending_.clear();
                break;
            }
        }

        if (pending_.size() >= sentinel_.size()) {
            bool matches = true;

            for (std::size_t i = 0; i < sentinel_.size(); i++) {
                if (pending_[i] != sentinel_[i]) {
                    matches = false;
                    break;
                }
            }

            if (matches) {
                result.sentinel_found = true;
                pending_.clear();
                break;
            }

            result.safe_text = result.safe_text + pending_[0];
            pending_.erase(0, 1);
        }
    }

    return result;
}

SentinelScanner::Out SentinelScanner::flush() {
    Out result;

    result.safe_text = pending_;
    result.sentinel_found = false;

    pending_.clear();

    return result;
}