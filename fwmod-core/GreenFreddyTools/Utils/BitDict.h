
#ifndef BITDICT_H
#define BITDICT_H

#include <type_traits>
#include "../../common.h"

template<typename Enum, typename T = int, bool premasked = false>
class BitDict {
    static_assert(std::is_enum<Enum>::value, "Enum must be an enumeration type.");

public:
    BitDict() : value_(0) {}
    BitDict(T value) : value_(value) {}


    // Get flag by enum value (aka index)
    bool GetFlag(Enum flag) const {
        return (value_ & CreateMask(flag)) != 0;
    }

    void SetFlag(Enum flag, bool state) {
        if (state) {
            this->value_ |= CreateMask(flag);
        }
        else {
            this->value_ &= ~CreateMask(flag);
        }
    }

    // Toggle flag by enum value (aka index)
    void ToggleFlag(Enum flag) {
        this->value_ ^= CreateMask(flag);
    }

    // Get the raw value
    T Value() const { return value_; }

    // Set the raw value
    void SetValue(T v) { value_ = v; }

private:
    inline T CreateMask(Enum flag) const {
        if constexpr (premasked) {
			return static_cast<T>(flag);
        }
        return (static_cast<T>(1) << static_cast<T>(flag));
    }
    T value_; // Only member variable to hold the bit value
};

#endif // !BITDICT_H