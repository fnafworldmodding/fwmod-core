#ifndef OBJECTSTUCTURES_H_
#define OBJECTSTUCTURES_H_
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
    union { // TODO: find a better name for both the member and the struct member
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
    union {
        short LineFlags; // Only used if ShapeType == 1
        struct {
            int Color1;  // RGBA Color // Only used if FillType == 1 or FillType == 2
            int Color2;  // RGBA Color // Only used if FillType == 2
            short VerticalGradient; // Only used if FillType == 2
        };
    };
    ushort Image = 0;

    ObjectShape() {
		memset(this, 0, sizeof(ObjectShape));
    }

    size_t CalcDynamicSize() const {
        size_t size = sizeof(BorderSize) + sizeof(borderColor) + sizeof(ShapeType) + sizeof(FillType);
        if (ShapeType == 1) {
            size += sizeof(LineFlags);
        } else if (FillType == 1) {
            size += sizeof(Color1);
        } else if (FillType == 2) {
            size += sizeof(Color1) + sizeof(Color2) + sizeof(VerticalGradient);
        }
        return size + sizeof(Image);
	};

    void Read(BinaryReader& reader) {
		BorderSize = reader.ReadInt16();
        borderColor = reader.ReadInt32();
        ShapeType = reader.ReadInt16();
        FillType = reader.ReadInt16();
        if (ShapeType == 1) {
            LineFlags = reader.ReadInt16();
        } else if (FillType == 1) {
            Color1 = reader.ReadInt32();
        } else if (FillType == 2) {
            Color1 = reader.ReadInt32();
            Color2 = reader.ReadInt32();
            VerticalGradient = reader.ReadInt16() != 0;
        }
		Image = reader.ReadInt16();
    };

    void Write(BinaryWriter& writer) const {
        writer.WriteInt16(BorderSize);
        writer.WriteInt32(borderColor);
        writer.WriteInt16(ShapeType);
        writer.WriteInt16(FillType);
        if (ShapeType == 1) {
            writer.WriteInt16(LineFlags);
        } else if (FillType == 1) {
            writer.WriteInt32(Color1);
        } else if (FillType == 2) {
            writer.WriteInt32(Color1);
            writer.WriteInt32(Color2);
            writer.WriteInt16(VerticalGradient);
		}
        writer.WriteInt16(Image);
    };
    void Write(uint8_t* data, size_t size) const {
        BinaryWriter writer(data, size);
		this->Write(writer);
    }

    void Read(uint8_t* data, size_t size) {
		BinaryReader reader(data, size);
		Read(reader);
    }
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
    int size = 0; // a guess
    ushort AnimationOffset = 0;
    ushort MovementsOffset = 0;
    ushort Version; // a guess
    ushort CounterOffset; // a guess 
    ushort ExtensionOffset = 0;
    ushort ValueOffset = 0;
    int ObjectFlags = 0;
    short Qualifiers[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    ushort DataOffset = 0;
    ushort AlterableValuesOffset = 0;
    ushort AlterableStringsOffset = 0;
    ushort NewObjectFlags = 0;
    ushort PreferenceFlags = 0;
    char Identifier[4] = { 0,0,0,0 }; // maybe use int instead?
    int BackColor = 0; // RGBA Color
};


constexpr size_t OBJECT_COMMON_SIZE = sizeof(ObjectCommon);
constexpr size_t QUICKBACKDROPSIZE = sizeof(ObjectQuickBackdrop) - sizeof(ObjectShape); // size of ObjectQuickBackdrop without Shape

// Unused
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


#endif // !OBJECTSTUCTURES_H_
