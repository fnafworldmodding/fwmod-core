#include "common.h"

class BitDict {
public:
    BitDict(const std::vector<std::string>& keys)
        : keys_(keys), value_(0) {
        for (size_t i = 0; i < keys.size(); ++i) {
            if (!keys[i].empty())
                keyToIndex_[keys[i]] = i;
        }
    }

	BitDict(const array, size_t size)
		: keys_(size), value_(0) {
		for (size_t i = 0; i < size; ++i) {
			if (!array[i].empty())
				keyToIndex_[array[i]] = i;
		}
	}

    // Get flag by name
    bool GetFlag(const std::string& key) const {
        auto it = keyToIndex_.find(key);
        if (it == keyToIndex_.end())
            throw std::invalid_argument("Key not found");
        return (value_ & (1u << it->second)) != 0;
    }

    // Get flag by index
    bool GetFlag(size_t index) const {
        if (index >= keys_.size())
            throw std::out_of_range("Index out of range");
        return (value_ & (1u << index)) != 0;
    }

    // Set flag by name
    void SetFlag(const std::string& key, bool flag) {
        auto it = keyToIndex_.find(key);
        if (it == keyToIndex_.end())
            throw std::invalid_argument("Key not found");
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
    uint32_t Value() const { return value_; }
    // Set the raw value
    void SetValue(uint32_t v) { value_ = v; }

    // Get the key for a given index
    std::string Key(size_t index) const {
        if (index >= keys_.size())
            throw std::out_of_range("Index out of range");
        return keys_[index];
    }

    std::vector<std::string> keys_;
    std::unordered_map<std::string, size_t> keyToIndex_;
    uint32_t value_;
};
