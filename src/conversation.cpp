#include "core/conversation.h"

Conversation::~Conversation() {
    delete[] data_;
}

Conversation::Conversation(const Conversation& other) {
    size_ = other.size_;
    capacity_ = other.capacity_;

    if (capacity_ > 0) {
        data_ = new Message[capacity_];

        for (std::size_t i = 0; i < size_; i++) {
            data_[i] = other.data_[i];
        }
    }
}

Conversation& Conversation::operator=(const Conversation& other) {
    if (this != &other) {
        Message* newData = nullptr;

        if (other.capacity_ > 0) {
            newData = new Message[other.capacity_];

            for (std::size_t i = 0; i < other.size_; i++) {
                newData[i] = other.data_[i];
            }
        }

        delete[] data_;

        data_ = newData;
        size_ = other.size_;
        capacity_ = other.capacity_;
    }

    return *this;
}

Conversation::Conversation(Conversation&& other) noexcept {
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

Conversation& Conversation::operator=(Conversation&& other) noexcept {
    if (this != &other) {
        delete[] data_;

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    return *this;
}

void Conversation::append(Message m) {
    if (size_ == capacity_) {
        std::size_t newCapacity;

        if (capacity_ == 0) {
            newCapacity = 1;
        }
        else {
            newCapacity = capacity_ * 2;
        }

        Message* newData = new Message[newCapacity];

        for (std::size_t i = 0; i < size_; i++) {
            newData[i] = data_[i];
        }

        delete[] data_;

        data_ = newData;
        capacity_ = newCapacity;
    }

    data_[size_] = m;
    size_ = size_ + 1;
}

std::size_t Conversation::size() const noexcept {
    return size_;
}

const Message& Conversation::at(std::size_t i) const {
    return data_[i];
}

const Message* Conversation::begin() const noexcept {
    return data_;
}

const Message* Conversation::end() const noexcept {
    return data_ + size_;
}