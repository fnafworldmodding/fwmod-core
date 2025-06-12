#include "ImageBank.h"
#include "ImageOffsets.h"
#include "../../Utils/Decompressor.h"
#include "../../Utils/BitDict.h"
#include <lz4.h>
#include <algorithm>

#ifdef _DEBUG
#include <vector>
static std::vector<uint32_t> originalImageHandlesOrder;
#endif

bool ImageBank::Init() {
    // TODO: check if this->data is compressed and handle accordingly, do we even need to decompress data?
    BinaryReader buffer(this->data.data(), this->data.size());
    //BinaryWriter writer("og-ImageBank.dat");
    //this->WriteHeader(writer);
    //writer.WriteFromMemory(this->data.data(), this->data.size());
    int count = buffer.ReadInt32();

    this->images.reserve(count);
    for (int i = 0; i < count; ++i) {
        Image img;
        buffer.ReadToMemory(&img, IMAGESIZE);

        // Read the image data
        const int ldataSize = std::max(0, img.dataSize - 4);
        img.data.resize(ldataSize);
        buffer.ReadToMemory(img.data.data(), ldataSize);

        this->images[img.Handle] = img;
#ifdef _DEBUG
        originalImageHandlesOrder.push_back(img.Handle);
#endif
    }
    return true;
}

static void WriteImage(BinaryWriter& writer, const Image& img) {
    /*
    // Handle field
    writer.WriteInt32(img.Handle);
    // Checksum field
    writer.WriteInt32(img.Checksum);
    // References field
    writer.WriteInt32(img.References);
    // unknown field (always 1)
    writer.WriteInt32(img.unknown);
    // dataSize field
    writer.WriteInt32(img.dataSize);
    // Width and Height
    writer.WriteInt16(img.Width);
    writer.WriteInt16(img.Height);
    // GraphicMode and Flags
    writer.WriteUint8(img.GraphicMode);
    writer.WriteUint8(img.Flags);
    // padding field
    writer.WriteUint16(img.padding);
    // Hotspot coordinates
    writer.WriteInt16(img.HotspotX);
    writer.WriteInt16(img.HotspotY);
    // Action point coordinates
    writer.WriteInt16(img.ActionPointX);
    writer.WriteInt16(img.ActionPointY);
    // Transparent color
    writer.WriteUint32(img.TransparentColor);
    // decompSizePlus
    writer.WriteInt32(img.decompSizePlus);
    // Write the actual image data
    BitDict dict(std::vector<std::string>{ // FIXME: this is wrong, LZX is false when it's true, probably bit indexing issues
        "RLE",
        "RLEW",
        "RLET",
        "LZX",
        "Alpha",
        "ACE",
        "Mac",
        "RGBA"
    });
    */
    writer.WriteFromMemory(reinterpret_cast<const void*>(&img), IMAGESIZE);
    writer.WriteFromMemory(img.data.data(), img.dataSize - 4);
}

void ImageBank::Write(BinaryWriter& buffer, bool _) {
    // TODO: implement compression?
    // Write the size of the image bank
#ifdef _DEBUG
    BinaryWriter writer("ImageBank.dat");
#endif
    int imagesCount = static_cast<int>(this->images.size());
    this->size = sizeof(int32_t); // Size of count integer
    for (const auto& [_, img] : this->images) {
        this->size += (IMAGESIZE); // Struct size without vector
        this->size += img.dataSize - 4;
    }
    this->WriteHeader(buffer);
    buffer.WriteInt32(imagesCount);
#ifdef _DEBUG
    this->WriteHeader(writer);
    writer.WriteInt32(imagesCount);
    // Write images in the order of originalImageHandlesOrder
    for (uint32_t handle : originalImageHandlesOrder) {
        auto it = this->images.find(handle);
        if (it != this->images.end()) {
            WriteImage(buffer, it->second);
            WriteImage(writer, it->second);
        }
    }
#else
    // Write each image in any order
    for (const auto& [_, img] : this->images) {
        WriteImage(buffer, img);
    }
#endif
}

void ImageBank::Write(BinaryWriter& buffer, bool _, OffsetsVector& offsets) {
    // TODO: implement compression?
    // Write the size of the image bank
    this->size = 0;
    this->WriteHeader(buffer); // id short, flag short, size int
    size_t ChunkPosition = std::move(buffer.Position());
    /*
    sizeof(int32_t); // Size of count integer
    for (const auto& [_, img] : this->images) {
		offsets.push_back(this->size + OFFSET_ADDTION);
        this->size += IMAGESIZE; // Struct size without vector
        this->size += img.dataSize - 4;
    }
    */
    int imagesCount = static_cast<int>(this->images.size());
    buffer.WriteInt32(imagesCount);
    // Write each image in any order
    for (uint32_t handle : originalImageHandlesOrder) {
        auto it = this->images.find(handle);
        if (it != this->images.end()) {
			int offset = (buffer.Position() - ChunkPosition) + OFFSET_ADDTION;
            offsets[handle-1] = offset; // Store offset for each image
            WriteImage(buffer, it->second);
        }
    }

    size_t finalSize = (buffer.Position() - ChunkPosition) + sizeof(int32_t);
    size_t sizePosition = ChunkPosition - sizeof(int32_t);
    buffer.SeekBeg(sizePosition);
    buffer.WriteInt32(static_cast<int32_t>(finalSize));
    buffer.SeekCur(finalSize);  // Return to end of file
}