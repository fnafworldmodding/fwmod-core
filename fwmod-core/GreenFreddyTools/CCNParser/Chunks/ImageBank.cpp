#include "ImageBank.h"
#include "ImageOffsets.h"
#include "../../Utils/Decompressor.h"
#include "../../Utils/BitDict.h"
#include "../Globals.h"
#include <lz4.h>
#include <algorithm>

// TODO: use Image:WriteImage and Image::ReadImage methods instead of ImageBank::WriteImage

bool ImageBank::Init() {
    BinaryReader buffer(this->data.data(), this->data.size());
    int count = buffer.ReadInt32();

    this->images.reserve(count); 
    for (int i = 0; i < count; ++i) {
        // TODO: probably a better thing would be directly reading the data to the struct than having a static method (avoid copying/moving)
        Image img = Image::ReadImage(buffer);
        this->images[img.Handle] = img;
    }
    this->FreeData();
    return true;
}

static void WriteImage(BinaryWriter& writer, const Image& img) {
	Image::WriteImage(writer, img, false); // Write without decompression
}

void ImageBank::Write(BinaryWriter& buffer, bool _) {
    // TODO: either update or remove
    // TODO: implement compression?
    // Write the size of the image bank
	throw std::runtime_error("ImageBank::Write needs to be reimplemented. Use ImageBank::Write with OffsetsVector instead.");
    int imagesCount = static_cast<int>(this->images.size());
    this->size = sizeof(int32_t); // Size of count integer
    for (const auto& [_, img] : this->images) {
        this->size += (IMAGESIZE); // Struct size without vector
        this->size += img.dataSize - 4;
    }
    this->WriteHeader(buffer);
    buffer.WriteInt32(imagesCount);
#ifdef IMAGEOGORDER
    // Write images in the order of originalImageHandlesOrder
    for (uint32_t handle : originalImageHandlesOrder) {
        auto it = this->images.find(handle);
        if (it != this->images.end()) {
            WriteImage(buffer, it->second);
        }
    }
#else
    // Write each image in any order
    for (const auto& [_, img] : this->images) {
        WriteImage(buffer, img);
    }
#endif
}

    /*
    sizeof(int32_t); // Size of count integer
    for (const auto& [_, img] : this->images) {
        offsets.push_back(this->size + OFFSET_ADDTION);
        this->size += IMAGESIZE; // Struct size without vector
        this->size += img.dataSize - 4;
    }
    */


void ImageBank::Write(BinaryWriter& buffer, bool _, OffsetsVector& offsets) {
    // TODO: implement compression?
    this->size = 0;
    this->WriteHeader(buffer); // id short, flag short, size int
    buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t ChunkPosition) {
        int imagesCount = static_cast<int>(this->images.size());
        buffer.WriteInt32(imagesCount);

        for (const auto& [handle, img] : this->images) {
            int offset = (buffer.Position() - ChunkPosition) + OFFSET_ADDTION;
            offsets[handle - 1] = offset;
            WriteImage(buffer, img);
        }

    });
}

void Image::DecompressImage(Image& img) {
    int compressedSize = img.dataSize - 4; // exclude the decompSizePlus field
	int decompressSize = img.decompSizePlus; // THE size of the uncompressed data

    std::vector<uint8_t> uncompressedData(decompressSize);

    // Decompress the image data using LZ4
    int decompressedSize = LZ4_decompress_safe(
        reinterpret_cast<const char*>(img.data.data()),
        reinterpret_cast<char*>(uncompressedData.data()),
        compressedSize, decompressSize
    );

    if (decompressedSize < 0) {
        throw std::runtime_error("Decompression failed");
    }
    uncompressedData.resize(decompressedSize);
    img.data = std::move(uncompressedData);
    img.Flags.SetFlag(ImageFlags::LZX, false); // Clear the LZX flag after decompression
}

// TODO: create an ReadImageEx that takes a handle (aka image id) and read the rest of the fields, to save memory and avoid copying/moving data
// and unstatic them they can be a method instead of a static method
Image Image::ReadImage(BinaryReader& buffer, bool decompress) {
    Image img;
    img.Handle = buffer.ReadUint32();
    img.Checksum = buffer.ReadInt32();
    img.References = buffer.ReadInt32();
    img.unknown = buffer.ReadInt32();
    img.dataSize = buffer.ReadInt32();
    img.Width = buffer.ReadInt16();
    img.Height = buffer.ReadInt16();
    img.GraphicMode = buffer.ReadUint8();
    img.Flags.SetValue(buffer.ReadUint8());
    img.padding = buffer.ReadUint16();
    img.HotspotX = buffer.ReadInt16();
    img.HotspotY = buffer.ReadInt16();
    img.ActionPointX = buffer.ReadInt16();
    img.ActionPointY = buffer.ReadInt16();
    img.TransparentColor = buffer.ReadUint32();
	bool islzCompressed = img.Flags.GetFlag(ImageFlags::LZX);

    int ldataSize = img.dataSize - (islzCompressed ? 4 : 0);
    img.data.resize(ldataSize);
    img.decompSizePlus = islzCompressed ? buffer.ReadInt32() : 0;
    buffer.ReadToMemory(img.data.data(), ldataSize);
    // Decompress the image data if necessary
    if (decompress && islzCompressed) {
		Image::DecompressImage(img);
    }

    return img;
}


void Image::WriteImage(BinaryWriter& buffer, const Image& img, bool compress) {
    buffer.WriteUint32(img.Handle);
    buffer.WriteInt32(img.Checksum);
    buffer.WriteInt32(img.References);
    buffer.WriteInt32(img.unknown);
    buffer.WriteInt32(img.dataSize);
    buffer.WriteInt16(img.Width);
    buffer.WriteInt16(img.Height);
    buffer.WriteUint8(img.GraphicMode);
    buffer.WriteUint8(img.Flags.Value());
    buffer.WriteUint16(img.padding);
    buffer.WriteInt16(img.HotspotX);
    buffer.WriteInt16(img.HotspotY);
    buffer.WriteInt16(img.ActionPointX);
    buffer.WriteInt16(img.ActionPointY);
    buffer.WriteUint32(img.TransparentColor);

    bool islzCompressed = img.Flags.GetFlag(ImageFlags::LZX);
    int ldataSize = img.dataSize - (islzCompressed ? 4 : 0);
	// TODO: compress the image data if compress is true

    // For writing decompSizePlus
    if (islzCompressed) {
        buffer.WriteInt32(img.decompSizePlus);
    }

    // Write image data
    buffer.WriteFromMemory(img.data.data(), ldataSize);
}

