#pragma once
#ifndef BITDICT_H
#define BITDICT_H
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <array>

template<typename T = uint32_t>
class BitDict {
private:
    std::vector<std::string> keys_;
    std::unordered_map<std::string, size_t> keyToIndex_;
    T value_;

public:
    // Constructor taking vector of strings
    explicit BitDict(const std::vector<std::string>& keys)
        : keys_(keys), value_(0) {
        for (size_t i = 0; i < keys.size(); ++i) {
            if (!keys[i].empty()) {
                keyToIndex_[keys[i]] = i;
            }
        }
    }

    // Constructor taking array and size
    template<size_t N>
    explicit BitDict(const std::array<std::string, N>& array)
        : keys_(array.begin(), array.end()), value_(0) {
        for (size_t i = 0; i < N; ++i) {
            if (!array[i].empty()) {
                keyToIndex_[array[i]] = i;
            }
        }
    }

    // Constructor taking C-style array and size
    BitDict(const std::string* array, size_t size)
        : keys_(array, array + size), value_(0) {
        for (size_t i = 0; i < size; ++i) {
            if (!array[i].empty()) {
                keyToIndex_[array[i]] = i;
            }
        }
    }

    // Get flag by name
    bool GetFlag(const std::string& key) const {
        auto it = keyToIndex_.find(key);
        if (it == keyToIndex_.end()) {
            throw std::invalid_argument("Key not found");
        }
        return (value_ & (static_cast<T>(1) << it->second)) != 0;
    }

    // Get flag by index
    bool GetFlag(size_t index) const {
        if (index >= keys_.size()) {
            throw std::out_of_range("Index out of range");
        }
        return (value_ & (static_cast<T>(1) << index)) != 0;
    }

    // Set flag by name
    void SetFlag(const std::string& key, bool flag) {
        auto it = keyToIndex_.find(key);
        if (it == keyToIndex_.end()) {
            throw std::invalid_argument("Key not found");
        }
        SetFlag(it->second, flag);
    }

    // Set flag by index
    void SetFlag(size_t index, bool flag) {
        if (index >= keys_.size())
            throw std::out_of_range("Index out of range");
        if (flag)
            value_ |= (1u << index);
        else
            value_ &= ~(1u << index);
    }

    // Get the raw value
    T Value() const { return value_; }

    // Set the raw value
    void SetValue(T v) { value_ = v; }

    // Get the key for a given index
    std::string Key(size_t index) const {
        if (index >= keys_.size()) {
            throw std::out_of_range("Index out of range");
        }
        return keys_[index];
    }

    // Get number of flags
    size_t Size() const { return keys_.size(); }

    // Check if key exists
    bool HasKey(const std::string& key) const {
        return keyToIndex_.find(key) != keyToIndex_.end();
    }
};
#endif // !BITDICT_H
