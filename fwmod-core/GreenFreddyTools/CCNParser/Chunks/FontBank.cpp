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
            else if (font.Flags == 1) {
                // The font is already compressed
                buffer.WriteInt32(font.Handle);
                buffer.WriteInt32(font.DecompSize);
                buffer.WriteInt32(font.Size);
                buffer.WriteFromMemory(font.raw, font.Size);
                continue;
            }
            buffer.WriteInt32(font.Handle);
            // fixme: the decompress size and compressed size aka Size is so confusing, maybe depend on flags rework the size determination
            int result = 0; // Result of compression, currently ignored because we throw the error in the function
            size_t outCompSize = 0;
            uint8_t* rawData = Decompressor::CompressZlibRaw((uint8_t *)&font.raw, font.DecompSize, outCompSize, result);
            buffer.WriteInt32(font.DecompSize); // write the decompressed size
            buffer.WriteInt32(outCompSize); // write the compressed size
            buffer.WriteFromMemory(rawData, outCompSize);
            delete[] rawData;
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
            if (font.Flags == 1) {
                // The font is already compressed
                buffer.WriteInt32(font.Handle);
                buffer.WriteInt32(font.DecompSize);
                buffer.WriteInt32(font.Size);
                buffer.WriteFromMemory(font.raw, font.Size);
                continue;
            }
            buffer.WriteInt32(font.Handle);
            // fixme: the decompress size and compressed size aka Size is so confusing, maybe depend on flags rework the size determination
            int result = 0; // Result of compression, currently ignored because we throw the error in the function
            size_t outCompSize = 0;
            uint8_t* rawData = Decompressor::CompressZlibRaw((uint8_t*)&font.raw, font.DecompSize, outCompSize, result);
            buffer.WriteInt32(font.DecompSize); // write the decompressed size
            buffer.WriteInt32(outCompSize); // write the compressed size
            buffer.WriteFromMemory(rawData, outCompSize);
            delete[] rawData;
        }
    });
}