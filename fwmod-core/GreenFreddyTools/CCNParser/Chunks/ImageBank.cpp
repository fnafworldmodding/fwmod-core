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

// use Image:WriteImage and Image::ReadImage static functions instead of ImageBank::WriteImage

bool ImageBank::Init() {
    // TODO: check if this->data is compressed and handle accordingly, do we even need to decompress data?
    BinaryReader buffer(this->data.data(), this->data.size());
    //BinaryWriter writer("og-ImageBank.dat");
    //this->WriteHeader(writer);
    //writer.WriteFromMemory(this->data.data(), this->data.size());
    int count = buffer.ReadInt32();

    this->images.reserve(count);
    for (int i = 0; i < count; ++i) {
        Image img = Image::ReadImage(buffer);
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
	Image::WriteImage(writer, img, false); // Write without decompression
}

void ImageBank::Write(BinaryWriter& buffer, bool _) {
    // TODO: either update or remove
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

Image& Image::DecompressImage(Image& img) {
    int compressedSize = img.dataSize - 4; // exclude the decompSizePlus field
	int decompressSize = img.decompSizePlus; // THE size of the uncompressed data

    std::vector<char> uncompressedData(decompressSize);

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
    img.Flags.SetFlag("LZX", false); // Clear the LZX flag after decompression
}

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
	bool islzCompressed = img.Flags.GetFlag("LZX");

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


void Image::WriteImage(BinaryWriter& buffer, const Image& img, bool decompress) {
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

    bool islzCompressed = img.Flags.GetFlag("LZX");
    int ldataSize = img.dataSize - (islzCompressed ? 4 : 0);
	// TODO: compress the image data if compress is true

    // For writing decompSizePlus
    if (islzCompressed) {
        buffer.WriteInt32(img.decompSizePlus);
    }

    // Write image data
    buffer.WriteFromMemory(img.data.data(), ldataSize);
}


#include <cstdint>

// Assumed helper functions and types
/*
int GetPadding(const Image & img) {
    // Assume 4-byte alignment for scanlines
    int rowSize = img.Width * 3;
    int pad = (4 - (rowSize % 4)) % 4;
    return pad;
}

struct NebulaCore {
    static constexpr bool Plus = true;
    static constexpr float Fusion = 2.5f;
    static constexpr bool Android = false;
    static constexpr bool iOS = false;
    static constexpr int Build = 300;
};
*/

static int GetPadding(Image img)
{
    int colorModeSize = 3;
    int modSize = 2;

    switch (img.GraphicMode)
    {
    case 3:
        colorModeSize = 1;
        modSize = 4;
        break;
    case 6:
    case 7:
        colorModeSize = 2;
        break;
    case 0:
    case 8:
        colorModeSize = 4;
        break;
    }

    return img.Width * colorModeSize % modSize;
}

int GetAlphaPadding(const Image& img) {
    return (4 - (img.Width % 4)) % 4;
}

struct TransparentColorStruct {
    uint8_t R, G, B, A = 0;
};


// Assumes Image struct has: Width, Height, data (std::vector<char>), Flags (BitDict), TransparentColor (uint32_t or struct), etc.
// Assumes TransparentColorStruct { uint8_t R, G, B, A; } and GetPadding, GetAlphaPadding are available.
// TODO: fix the single off pixel problem
std::vector<uint8_t> RGBAToRGBMasked(const Image& img) {
    // b output buffer: over-allocate, will resize at end
    std::vector<uint8_t> colorArray(img.Width * img.Height * 8);
    int stride = img.Width * 4;
    int pad = GetPadding(img);
    int position = 0;

    // Extract transparent color
    TransparentColorStruct transparentColor;
    transparentColor.R = (img.TransparentColor >> 16) & 0xFF;
    transparentColor.G = (img.TransparentColor >> 8) & 0xFF;
    transparentColor.B = (img.TransparentColor) & 0xFF;
    transparentColor.A = (img.TransparentColor >> 24) & 0xFF;

    bool hasAlpha = img.Flags.GetFlag("Alpha");
    bool hasRGBA = img.Flags.GetFlag("RGBA");

    // Main RGB loop
    for (int y = 0; y < img.Height; ++y) {
        for (int x = 0; x < img.Width; ++x) {
            int pos = (y * stride) + (x * 4);
            if (pos + 3 >= static_cast<int>(img.data.size()) || position + 2 >= static_cast<int>(colorArray.size()))
                break;

            colorArray[position + 0] = static_cast<uint8_t>(img.data[pos + 0]);
            colorArray[position + 1] = static_cast<uint8_t>(img.data[pos + 1]);
            colorArray[position + 2] = static_cast<uint8_t>(img.data[pos + 2]);
            colorArray[position + 3] = static_cast<uint8_t>(img.data[pos + 3]);

            if (!hasAlpha && hasRGBA && static_cast<uint8_t>(img.data[pos + 3]) != 255) {
                colorArray[position + 2] = transparentColor.R;
                colorArray[position + 1] = transparentColor.G;
                colorArray[position + 0] = transparentColor.B;
            }

            position += 3;
        }
        position += pad * 3;
    }

    // Alpha channel mask
    if (hasAlpha) {
        int aPad = GetAlphaPadding(img);
        for (int y = 0; y < img.Height; ++y) {
            for (int x = 0; x < img.Width; ++x) {
                int pos = (y * stride) + (x * 4);
                if (position >= static_cast<int>(colorArray.size()) || pos + 3 >= static_cast<int>(img.data.size()))
                    break;
                colorArray[position] = static_cast<uint8_t>(img.data[pos + 3]);
                position += 1;
            }
            position += aPad;
        }
    }

    colorArray.resize(position);
    return colorArray;
}


// Converts a 2.5+ format image to RGBA byte array
std::vector<uint8_t> TwoFivePlusToRGBA(const Image& img) {
    // Logging (optional, can be removed or replaced with your logger)
    // std::cout << "TwoFivePlusToRGBA, Image Data Size: " << img.data.size()
    //           << ", Size: " << img.Width << "x" << img.Height
    //           << ", Alpha: " << img.Flags.GetFlag("Alpha")
    //           << ", Transparent Color: " << img.TransparentColor
    //           << ", img.Flags[\"RGBA\"]: " << img.Flags.GetFlag("RGBA") << std::endl;

    std::vector<uint8_t> colorArray(img.Width * img.Height * 4, 0);
    int stride = img.Width * 4;
    int pad = GetPadding(img);
    int position = 0;

    // Extract transparent color
    TransparentColorStruct transparentColor;
    transparentColor.R = (img.TransparentColor >> 16) & 0xFF;
    transparentColor.G = (img.TransparentColor >> 8) & 0xFF;
    transparentColor.B = (img.TransparentColor) & 0xFF;
    transparentColor.A = (img.TransparentColor >> 24) & 0xFF;

    bool hasAlpha = img.Flags.GetFlag("Alpha");
    bool hasRGBA = img.Flags.GetFlag("RGBA");

    // NebulaCore::Fusion and NebulaCore::Seeded are not defined in this context.
    // For now, always use the "else" branch. If you have these, replace accordingly.
    constexpr float NebulaCore_Fusion = 2.5f;
    constexpr bool NebulaCore_Seeded = false;
    constexpr bool PremultipliedAlpha = true; // Set to true if needed

    for (int y = 0; y < img.Height; ++y) {
        for (int x = 0; x < img.Width; ++x) {
            int newPos = (y * stride) + (x * 4);
            if (position + 2 >= static_cast<int>(img.data.size()))
                break;

            if (NebulaCore_Fusion == 3.0f && !NebulaCore_Seeded) {
                colorArray[newPos + 0] = static_cast<uint8_t>(img.data[position + 2]);
                colorArray[newPos + 1] = static_cast<uint8_t>(img.data[position + 1]);
                colorArray[newPos + 2] = static_cast<uint8_t>(img.data[position + 0]);
            } else {
                colorArray[newPos + 0] = static_cast<uint8_t>(img.data[position + 0]);
                colorArray[newPos + 1] = static_cast<uint8_t>(img.data[position + 1]);
                colorArray[newPos + 2] = static_cast<uint8_t>(img.data[position + 2]);
            }
            colorArray[newPos + 3] = 255;

            if (hasAlpha || hasRGBA) {
                if (PremultipliedAlpha) {
                    float a = static_cast<uint8_t>(img.data[position + 3]) / 255.0f;
                    if (a > 0.0f) {
                        colorArray[newPos + 0] = static_cast<uint8_t>(colorArray[newPos + 0] / a);
                        colorArray[newPos + 1] = static_cast<uint8_t>(colorArray[newPos + 1] / a);
                        colorArray[newPos + 2] = static_cast<uint8_t>(colorArray[newPos + 2] / a);
                    }
                }
                colorArray[newPos + 3] = static_cast<uint8_t>(img.data[position + 3]);
            } else {
                if (colorArray[newPos + 2] == transparentColor.R &&
                    colorArray[newPos + 1] == transparentColor.G &&
                    colorArray[newPos + 0] == transparentColor.B) {
                    colorArray[newPos + 3] = 0;
                }
            }
            position += 4;
        }
        position += pad * 4;
    }

    if (position == static_cast<int>(img.data.size()))
        return colorArray;

    if (hasAlpha && !hasRGBA) {
        int aPad = GetAlphaPadding(img);
        int aStride = img.Width * 4;
        for (int y = 0; y < img.Height; ++y) {
            for (int x = 0; x < img.Width; ++x) {
                if (position >= static_cast<int>(img.data.size()))
                    break;
                colorArray[(y * aStride) + (x * 4) + 3] = static_cast<uint8_t>(img.data[position]);
                position += 1;
            }
            position += aPad;
        }
    }

    return colorArray;
}
