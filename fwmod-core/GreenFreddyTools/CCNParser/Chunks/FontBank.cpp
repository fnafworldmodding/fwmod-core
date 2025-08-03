#include "FontBank.h"
#include "../../Utils/Decompressor.h"

bool FontBank::Init() {
    BinaryReader buffer(this->data.data(), this->data.size());
    int count = buffer.ReadInt32();
	this->fonts.reserve(count);
    for (int i = 0; i < count; i++) {
        FontItem& font = this->fonts[i];
        font.Handle = buffer.ReadInt32();
		font.Flags = 1; // 1 == compressed
        font.DecompSize = buffer.ReadInt32();
        font.Size = buffer.ReadInt32();
        font.raw = new uint8_t[font.Size];
        buffer.ReadToMemory(font.raw, font.Size);
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
            // TODO: include the name in the compressing process. 
            // it currently expects the name to be beside the fontinfo struct directly at the memory location

            // fixme: the decompress size and compressed size aka Size is so confusing, maybe depend on flags rework the size determination
            int result = 0; // Result of compression, currently ignored because we throw the error in the function
            size_t outCompSize = 0;
            uint8_t* rawData = Decompressor::CompressZlibRaw(font.raw, font.DecompSize, outCompSize, result);
            buffer.WriteInt32(font.DecompSize); // write the decompressed size
            buffer.WriteInt32(outCompSize); // write the compressed size
            buffer.WriteFromMemory(rawData, outCompSize);
            delete[] rawData;
        }
    });
}

void FontBank::Write(BinaryWriter& buffer, bool compress, OffsetsVector offsets) {
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
            // Add the offset for the font location in the fontbank chunk
            // int to skip the count of fonts
            offsets.push_back((buffer.Position() - ChunkPosition) + sizeof(int));
            buffer.WriteInt32(font.Handle);
            // TODO: include the name in the compressing process. 
            // it currently expects the name to be beside the fontinfo struct directly at the memory location
            
            // fixme: the decompress size and compressed size aka Size is so confusing, maybe depend on flags rework the size determination
            int result = 0; // Result of compression, currently ignored because we throw the error in the function
            size_t outCompSize = 0;
            uint8_t* rawData = Decompressor::CompressZlibRaw(font.raw, font.DecompSize, outCompSize, result);
            buffer.WriteInt32(font.DecompSize); // write the decompressed size
            buffer.WriteInt32(outCompSize); // write the compressed size
            buffer.WriteFromMemory(rawData, outCompSize);
            delete[] rawData;
        }
    });
}