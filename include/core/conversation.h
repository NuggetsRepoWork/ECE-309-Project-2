#pragma once

#include "ECE 309 Project 2.h"

class Conversation {
public:
    // Constructor & Destructor
    Conversation();
    ~Conversation();

    // Core Mutators
    void append(Message m);

    // Accessors & Iterators
    std::size_t size() const noexcept;
    const Message& at(std::size_t i) const;
    const Message* begin() const noexcept;
    const Message* end() const noexcept;

private:
    Message* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;

    // Growth helper
    void reallocate(std::size_t new_capacity);
};#pragma once
