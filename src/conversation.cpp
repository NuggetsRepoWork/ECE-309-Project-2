#include "core/conversation.h"

#include <stdexcept>
#include <utility>

Conversation::Conversation()
    : data_(nullptr), size_(0), capacity_(0) {
}

Conversation::~Conversation() {
    delete[] data_;
}

Conversation::Conversation(const Conversation& other)
    : data_(nullptr), size_(0), capacity_(0) {

    if (other.capacity_ > 0) {
        data_ = new Message[other.capacity_];

        capacity_ = other.capacity_;

        for (std::size_t i = 0; i < other.size_; i = i + 1) {
            data_[i] = other.data_[i];
        }

        size_ = other.size_;
    }
}

Conversation& Conversation::operator=(const Conversation& other) {

    if (this == &other) {
        return *this;
    }

    Message* new_data = nullptr;

    if (other.capacity_ > 0) {
        new_data = new Message[other.capacity_];

        for (std::size_t i = 0; i < other.size_; i = i + 1) {
            new_data[i] = other.data_[i];
        }
    }

    delete[] data_;

    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

Conversation::Conversation(Conversation&& other) noexcept
    : data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_) {

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

Conversation& Conversation::operator=(Conversation&& other) noexcept {

    if (this == &other) {
        return *this;
    }

    delete[] data_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}

void Conversation::append(Message m) {

    if (m.role() == Role::System) {

        if (size_ == capacity_) {

            std::size_t new_capacity;

            if (capacity_ == 0) {
                new_capacity = 1;
            }
            else {
                new_capacity = capacity_ * 2;
            }

            Message* new_data = new Message[new_capacity];

            for (std::size_t i = 0; i < size_; i = i + 1) {
                new_data[i] = data_[i];
            }

            delete[] data_;

            data_ = new_data;
            capacity_ = new_capacity;
        }

        for (std::size_t i = size_; i > 0; i = i - 1) {
            data_[i] = data_[i - 1];
        }

        data_[0] = m;

        size_ = size_ + 1;

        return;
    }

    if (size_ == capacity_) {

        std::size_t new_capacity;

        if (capacity_ == 0) {
            new_capacity = 1;
        }
        else {
            new_capacity = capacity_ * 2;
        }

        Message* new_data = new Message[new_capacity];

        for (std::size_t i = 0; i < size_; i = i + 1) {
            new_data[i] = data_[i];
        }

        delete[] data_;

        data_ = new_data;
        capacity_ = new_capacity;
    }

    data_[size_] = m;
    size_ = size_ + 1;
}

std::size_t Conversation::size() const noexcept {
    return size_;
}

const Message& Conversation::at(std::size_t i) const {

    if (i >= size_) {
        throw std::out_of_range("Conversation::at: index out of range");
    }

    return data_[i];
}

const Message* Conversation::begin() const noexcept {
    return data_;
}

const Message* Conversation::end() const noexcept {
    return data_ + size_;
}