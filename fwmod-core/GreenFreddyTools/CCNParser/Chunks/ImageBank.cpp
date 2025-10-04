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
        int handle = buffer.ReadUint32();
        // accessing a non-existent key would construct one by default, unordered_map creates it
        this->images[handle].ReadEx(buffer, handle);
    }
    this->FreeData();
    return true;
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
    // Write each image in any order
    for (const auto& [_, img] : this->images) {
        img.Write(buffer);
    }
}


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
            img.Write(buffer);
        }

    });
}

void Image::DecompressImage() {
    int compressedSize = this->dataSize - 4; // exclude the decompSizePlus field
	int decompressSize = this->decompSizePlus; // THE size of the uncompressed data
	// TODO: use a Decompressor class instead of using LZ4 directly
    std::vector<uint8_t> uncompressedData(decompressSize);

    // Decompress the image data using LZ4
    int decompressedSize = LZ4_decompress_safe(
        reinterpret_cast<const char*>(this->data.data()),
        reinterpret_cast<char*>(uncompressedData.data()),
        compressedSize, decompressSize
    );

    if (decompressedSize < 0) {
        throw std::runtime_error("Decompression failed");
    }
    uncompressedData.resize(decompressedSize);
    this->data = std::move(uncompressedData);
    this->Flags.SetFlag(ImageFlags::LZX, false); // Clear the LZX flag after decompression
}


uint32_t ImageBank::AddImage(Image& image, OffsetsVector* offsets) {
    auto it = images.find(image.Handle);
    // fixme: comparing uint32_t with -1... bad
    if (image.Handle == -1) { // image add request
        // do nullptr offsets check
        // find a free offset by checking if it's 0
        auto it = std::find_if(offsets->begin(), offsets->end(), [](int offset) {
            return offset == 0;
        });

        if (it != offsets->end()) {
            image.Handle = static_cast<uint32_t>(std::distance(offsets->begin(), it));
        }
        else {
            // no free offset found, append to the end
            image.Handle = static_cast<uint32_t>(offsets->size());
            offsets->push_back(0);
        }
    }
	else if (it == images.end()) { // image not found, add it
        if (!offsets) return -1;
        // check if id is in offsets, if not append till offset is provided
        if (image.Handle >= offsets->size()) {
            offsets->resize(image.Handle + 1, 0); // resize offsets to accommodate the new ID
        }
        // TODO: should I set the offset to 0?
    }
    images[image.Handle] = image;
	return image.Handle;
}

// TODO: create an ReadImageEx that takes a handle (aka image id) and read the rest of the fields, to save memory and avoid copying/moving data
// and unstatic them they can be a method instead of a static method

void Image::ReadEx(BinaryReader& buffer, int handle, bool decompress) 
{
    this->Checksum = buffer.ReadInt32();
    this->References = buffer.ReadInt32();
    this->unknown = buffer.ReadInt32();
    this->dataSize = buffer.ReadInt32();
    this->Width = buffer.ReadInt16();
    this->Height = buffer.ReadInt16();
    this->GraphicMode = buffer.ReadUint8();
    this->Flags.SetValue(buffer.ReadUint8());
    this->padding = buffer.ReadUint16();
    this->HotspotX = buffer.ReadInt16();
    this->HotspotY = buffer.ReadInt16();
    this->ActionPointX = buffer.ReadInt16();
    this->ActionPointY = buffer.ReadInt16();
    this->TransparentColor = buffer.ReadUint32();
	bool islzCompressed = this->Flags.GetFlag(ImageFlags::LZX);

    int ldataSize = this->dataSize - (islzCompressed ? 4 : 0);
    this->data.resize(ldataSize);
    this->decompSizePlus = islzCompressed ? buffer.ReadInt32() : 0;
    buffer.ReadToMemory(this->data.data(), ldataSize);
    // Decompress the image data if necessary
    if (decompress && islzCompressed) {
		this->DecompressImage();
        this->Flags.SetFlag(ImageFlags::LZX, false);
    }
}

void Image::Write(BinaryWriter& buffer, bool compress) const {
    buffer.WriteUint32(this->Handle);
    buffer.WriteInt32(this->Checksum);
    buffer.WriteInt32(this->References);
    buffer.WriteInt32(this->unknown);
    buffer.WriteInt32(this->dataSize);
    buffer.WriteInt16(this->Width);
    buffer.WriteInt16(this->Height);
    buffer.WriteUint8(this->GraphicMode);
    buffer.WriteUint8(this->Flags.Value());
    buffer.WriteUint16(this->padding);
    buffer.WriteInt16(this->HotspotX);
    buffer.WriteInt16(this->HotspotY);
    buffer.WriteInt16(this->ActionPointX);
    buffer.WriteInt16(this->ActionPointY);
    buffer.WriteUint32(this->TransparentColor);

    bool islzCompressed = this->Flags.GetFlag(ImageFlags::LZX);
    int ldataSize = this->dataSize - (islzCompressed ? 4 : 0);
	// TODO: compress the image data if compress is true

    // For writing decompSizePlus
    if (islzCompressed) {
        buffer.WriteInt32(this->decompSizePlus);
    }

    // Write image data
    buffer.WriteFromMemory(this->data.data(), ldataSize);
}
