#include "windows.h"
#include <cstdint>
#include <string>
#include <vector>

typedef unsigned short ushort;

struct BGRColor {
    uint8_t b = 0;
    uint8_t g = 0;
    uint8_t r = 0;
};

struct InkEffectParam {
    BGRColor color;
    uint8_t BlendCoeff = 0;
};

struct ObjectHeader {
    ushort Handle = 0;
    ushort Type = 0;
    ushort flags = 0;
    ushort padding = 0;
    short InkEffect = 0;
    ushort InkEffectFlags = 0;
    union { // TODO: find a better name for both the member and the struct
        InkEffectParam Effect;
        int RawEffect = 0;
    };
};

// dynamic structure
struct ObjectShape {
    short BorderSize = 0;
    int borderColor = 0; // RGBA Color
    short ShapeType = 0;
    short FillType = 0;
    short LineFlags = 0;
    short Color1 = 0; // RGBA Color
    short Color2 = 0; // RGBA Color
    short VerticalGradient = 0;
    short Image = 0;
};

struct ObjectQuickBackdrop {
    int size = 0;
    ushort ObstacleType = 0;
    ushort CollisionType = 0;
    int width = 0;
    int height = 0;
    ObjectShape Shape;
};

struct ObjectBackdrop {
    int size = 0;
    ushort ObstacleType = 0;
    ushort CollisionType = 0;
    int width = 0;
    int height = 0;
    ushort Image = 0;
};

// CTF 2.5+ object common structure
struct ObjectCommon {
    ushort AnimationOffset = 0;
    ushort ExtensionOffset = 0;
    ushort ValueOffset = 0;
    int ObjectFlags = 0;
    short Qualifiers[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    ushort DataOffset = 0;
    ushort AlterableValuesOffset = 0;
    ushort AlterableStringsOffset = 0;
    ushort NewObjectFlags = 0;
    ushort PreferenceFlags = 0;
    char Identifier[4]; // maybe use int instead?
    int BackColor = 0; // RGBA Color
};

template <typename StringType>
std::vector<StringType> ReadObjectNames(BinaryReader& reader, size_t size) {
    // TODO: use size to prevent infinite loop or buffer overflow?
    std::vector<StringType> names{};
    // sizeof(typename StringType::value_type) is the size of a single character, 
    // for example for std::wstring would be sizeof(wchar_t)
    while (reader.HasMemory(sizeof(typename StringType::value_type))) {
        if constexpr (std::is_same_v<StringType, std::wstring>) {
            names.push_back(reader.ReadNullTerminatedStringWide());
        }
        else if constexpr (std::is_same_v<StringType, std::string>) {
            names.push_back(reader.ReadNullTerminatedString());
        }
    }
    return names;
}
