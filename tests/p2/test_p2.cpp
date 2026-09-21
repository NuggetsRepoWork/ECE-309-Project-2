#include <cassert>
#include <iostream>
#include <string>
#include <stdexcept>
#include "core/message.h"
#include "core/conversation.h"
#include "core/sentinel_scanner.h"

void test_empty_conversation() {

    Conversation conv;

    assert(conv.size() == 0);
    assert(conv.begin() == conv.end());
}

void test_default_message() {

    Message message;

    assert(message.role() == Role::System);
    assert(message.content() == "");
}

void test_message_constructor() {

    Message message(Role::User, "hello");

    assert(message.role() == Role::User);
    assert(message.content() == "hello");
}

void test_conversation_append() {

    Conversation conv;

    conv.append(Message(Role::User, "hello"));
    conv.append(Message(Role::Assistant, "hi"));

    assert(conv.size() == 2);
    assert(conv.at(0).content() == "hello");
    assert(conv.at(1).content() == "hi");
}

void test_system_message_ordering() {

    Conversation conv;

    conv.append(Message(Role::User, "hello"));
    conv.append(Message(Role::Assistant, "hi"));
    conv.append(Message(Role::System, "system"));

    assert(conv.size() == 3);
    assert(conv.at(0).role() == Role::System);
    assert(conv.at(0).content() == "system");
    assert(conv.at(1).content() == "hello");
    assert(conv.at(2).content() == "hi");
}

void test_at_bounds() {

    Conversation conv;

    conv.append(Message(Role::User, "hello"));

    bool threw = false;

    try {
        conv.at(1);
    }
    catch (const std::out_of_range&) {
        threw = true;
    }

    assert(threw);
}

void test_growth() {

    Conversation conv;

    for (int i = 0; i < 100; i = i + 1) {

        conv.append(
            Message(Role::User, std::to_string(i))
        );
    }

    assert(conv.size() == 100);

    for (int i = 0; i < 100; i = i + 1) {
        assert(conv.at(i).content() == std::to_string(i));
    }
}

void test_copy_constructor() {

    Conversation original;

    original.append(Message(Role::User, "one"));
    original.append(Message(Role::Assistant, "two"));

    Conversation copy(original);

    assert(copy.size() == original.size());
    assert(copy.at(0).content() == "one");
    assert(copy.at(1).content() == "two");

    assert(copy.begin() != original.begin());
}

void test_copy_assignment() {

    Conversation original;

    original.append(Message(Role::User, "one"));
    original.append(Message(Role::Assistant, "two"));

    Conversation copy;

    copy = original;

    assert(copy.size() == 2);
    assert(copy.at(0).content() == "one");
    assert(copy.at(1).content() == "two");
    assert(copy.begin() != original.begin());
}

void test_move_constructor() {

    Conversation original;

    original.append(Message(Role::User, "one"));
    original.append(Message(Role::Assistant, "two"));

    const Message* original_data = original.begin();

    Conversation moved(std::move(original));

    assert(moved.begin() == original_data);
    assert(moved.size() == 2);

    assert(original.size() == 0);
    assert(original.begin() == nullptr);
    assert(original.end() == nullptr);
}

void test_move_assignment() {

    Conversation original;

    original.append(Message(Role::User, "one"));
    original.append(Message(Role::Assistant, "two"));

    const Message* original_data = original.begin();

    Conversation moved;

    moved = std::move(original);

    assert(moved.begin() == original_data);
    assert(moved.size() == 2);

    assert(original.size() == 0);
    assert(original.begin() == nullptr);
    assert(original.end() == nullptr);
}

void test_scanner_clean_text() {

    SentinelScanner scanner("<|end_conversation|>");

    auto out = scanner.feed("Hello world.");

    assert(out.sentinel_found == false);
    assert(out.safe_text == "");

    auto final = scanner.flush();

    assert(final.sentinel_found == false);
    assert(final.safe_text == "Hello world.");
}

void test_scanner_whole_sentinel() {

    SentinelScanner scanner("<|end_conversation|>");

    auto out = scanner.feed(
        "Goodbye.<|end_conversation|>"
    );

    assert(out.sentinel_found == true);
    assert(out.safe_text == "Goodbye.");
}

void test_scanner_split_sentinel() {

    const std::string sentinel = "<|end_conversation|>";

    const std::string text =
        "Goodbye." + sentinel;

    for (std::size_t split = 0;
        split <= text.size();
        split = split + 1) {

        SentinelScanner scanner(sentinel);

        auto out1 = scanner.feed(
            text.substr(0, split)
        );

        auto out2 = scanner.feed(
            text.substr(split)
        );

        assert(
            out1.sentinel_found ||
            out2.sentinel_found
        );

        assert(
            out1.safe_text +
            out2.safe_text
            == "Goodbye."
        );
    }
}

void test_scanner_one_character_at_a_time() {

    const std::string sentinel =
        "<|end_conversation|>";

    const std::string text =
        "Hello there." + sentinel;

    SentinelScanner scanner(sentinel);

    std::string safe;

    for (char c : text) {

        auto out = scanner.feed(
            std::string(1, c)
        );

        safe = safe + out.safe_text;

        if (out.sentinel_found) {
            assert(safe == "Hello there.");
            return;
        }
    }

    assert(false);
}

void test_scanner_false_alarm() {

    SentinelScanner scanner(
        "<|end_conversation|>"
    );

    auto out = scanner.feed(
        "Hello <|end_world|> goodbye"
    );

    assert(out.sentinel_found == false);

    auto final = scanner.flush();

    assert(
        out.safe_text + final.safe_text
        == "Hello <|end_world|> goodbye"
    );
}

void test_scanner_flush() {

    SentinelScanner scanner(
        "<|end_conversation|>"
    );

    auto out = scanner.feed("abc");

    assert(out.safe_text == "");
    assert(out.sentinel_found == false);

    auto final = scanner.flush();

    assert(final.safe_text == "abc");
    assert(final.sentinel_found == false);
}

int main() {

    test_empty_conversation();
    test_default_message();
    test_message_constructor();
    test_conversation_append();
    test_system_message_ordering();
    test_at_bounds();
    test_growth();
    test_copy_constructor();
    test_copy_assignment();
    test_move_constructor();
    test_move_assignment();
    test_scanner_clean_text();
    test_scanner_whole_sentinel();
    test_scanner_split_sentinel();
    test_scanner_one_character_at_a_time();
    test_scanner_false_alarm();
    test_scanner_flush();

    std::cout << "All P2 core tests passed.\n";

    return 0;
}