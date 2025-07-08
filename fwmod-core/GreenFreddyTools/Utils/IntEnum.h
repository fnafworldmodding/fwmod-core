#pragma once
#ifndef _FWMC_INTENUM_
#define _FWMC_INTENUM_


#define IntEnum(name, type) \
    enum class name : type; \
    inline name operator+(name a, type b) { return static_cast<name>(static_cast<type>(a) + b); } \
    inline name operator-(name a, type b) { return static_cast<name>(static_cast<type>(a) - b); } \
    inline name operator*(name a, type b) { return static_cast<name>(static_cast<type>(a) * b); } \
    inline name operator/(name a, type b) { return static_cast<name>(static_cast<type>(a) / b); } \
    inline name operator%(name a, type b) { return static_cast<name>(static_cast<type>(a) % b); } \
    inline name operator&(name a, type b) { return static_cast<name>(static_cast<type>(a) & b); } \
    inline name operator|(name a, name b) { return static_cast<name>(static_cast<type>(a) | static_cast<type>(b)); } \
    inline name operator^(name a, type b) { return static_cast<name>(static_cast<type>(a) ^ b); } \
    inline name operator<<(name a, type b) { return static_cast<name>(static_cast<type>(a) << b); } \
    inline name operator>>(name a, type b) { return static_cast<name>(static_cast<type>(a) >> b); } \
    inline bool operator==(name a, name b) { return static_cast<type>(a) == static_cast<type>(b); } \
    inline bool operator!=(name a, name b) { return static_cast<type>(a) != static_cast<type>(b); } \
    inline bool operator<(name a, name b) { return static_cast<type>(a) < static_cast<type>(b); } \
    inline bool operator<=(name a, name b) { return static_cast<type>(a) <= static_cast<type>(b); } \
    inline bool operator>(name a, name b) { return static_cast<type>(a) > static_cast<type>(b); } \
    inline bool operator>=(name a, name b) { return static_cast<type>(a) >= static_cast<type>(b); } \
    enum class name : type


#endif // !_FWMC_INTENUM_
