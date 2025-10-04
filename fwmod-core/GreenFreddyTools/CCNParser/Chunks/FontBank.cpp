#include "FontBank.h"
#include "../../Utils/Decompressor.h"
// TODO: make font read/write methods such as images struct

bool FontBank::Init() {
    BinaryReader buffer(this->data.data(), this->data.size());
    int count = buffer.ReadInt32();
	this->fonts.reserve(count);
    for (int i = 0; i < count; i++) {
        FontItem font{};
        font.Handle = buffer.ReadInt32();
		font.Flags = 1; // 1 == compressed
        font.DecompSize = buffer.ReadInt32();
        font.Size = buffer.ReadInt32();
        buffer.ReadToMemory(&font.raw, font.Size);
        this->fonts[font.Handle] = font;
    }
    return true;
}


void FontBank::Write(BinaryWriter& buffer, bool compress) {
    this->size = 0;
    this->WriteHeader(buffer); // id short, flag short, size int
    buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t ChunkPosition) {
        int fontsCount = static_cast<int>(this->fonts.size());
        buffer.WriteInt32(fontsCount);
        for (const auto& [_, font] : this->fonts) {
            if (font.Flags == 2) {
                // Skip the uninitialized font
                continue;
            }
            font.Write(buffer);
        }
    });
}

void FontBank::Write(BinaryWriter& buffer, bool compress, OffsetsVector& offsets) {
    this->size = 0;
    this->WriteHeader(buffer); // id short, flag short, size int
    buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t ChunkPosition) {
        int fontsCount = static_cast<int>(this->fonts.size());
        buffer.WriteInt32(fontsCount);
        for (const auto& [_, font] : this->fonts) {
            if (font.Flags == 2) {
                // Skip the uninitialized font
                continue;
            }
            // Add the offset for the font location in the fontbank chunk
            offsets[font.Handle - 1] = (buffer.Position() - ChunkPosition) + OFFSET_ADDTION;
            font.Write(buffer);
        }
    });
}