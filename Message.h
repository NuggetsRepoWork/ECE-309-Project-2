#pragma once

#include <string>

enum class Role { System, User, Assistant };

class Message {
public:
    // Default constructor (required for array allocations)
    Message();

    // Parameterized constructor
    Message(Role role, std::string content);

    // Accessors (marked const noexcept for safety and efficiency)
    Role role() const noexcept;
    const std::string& content() const noexcept;

private:
    Role role_;
    std::string content_;
};#pragma once
