#ifndef IMAGEBANKCHUNK_H
#define IMAGEBANKCHUNK_H
#pragma once
#include "Chunks.h"
#include "ImageOffsets.h"
#include <unordered_map>
#include "Utils/BitDict.h"

struct Image {
    uint32_t Handle = 0;  
    int32_t Checksum = -1; // can be -1 ig is some ignore value
    int32_t References = 0;  
    int32_t unknown = 1; // Unknown field. always 1 for some reason
    int32_t dataSize = 0; // Size of the image data
    int16_t Width = 0;  
    int16_t Height = 0;  
    uint8_t GraphicMode = 0;  
    BitDict<uint8_t> Flags = BitDict<uint8_t>(std::vector<std::string>{
        "RLE",
        "RLEW",
        "RLET",
        "LZX",
        "Alpha",
        "ACE",
        "Mac",
        "RGBA"
    }, 0);
    uint16_t padding = 0; // Skip 2 bytes
    int16_t HotspotX = 0;  
    int16_t HotspotY = 0;  
    int16_t ActionPointX = 0;  
    int16_t ActionPointY = 0;  
    uint32_t TransparentColor = 0; // TODO: create a structure, RGBA color
    int32_t decompSizePlus = 0;
    std::vector<char> data;
    static void WriteImage(BinaryWriter& writer, const Image& image, bool compress);
    static Image ReadImage(BinaryReader& reader, bool decompress = false);
    static void DecompressImage(Image& img);
};

#define IMAGESIZE sizeof(Image) - sizeof(std::vector<char>)

typedef std::vector<char> ImageData;
typedef std::unordered_map<uint32_t, Image> ImageMap;

class ImageBank : public Chunk {
public:
	ImageMap images;

	ImageBank() : Chunk() {
		id = static_cast<short>(ChunksIDs::ImageBank);
		images = ImageMap();
	}
    ImageBank(short id, short flag, int size) : Chunk(id, flag, size) {
        this->id = static_cast<short>(ChunksIDs::ImageBank);
        images = ImageMap();
	}

    ImageBank(short flag, int size) : Chunk(flag, size) {
        id = static_cast<short>(ChunksIDs::ImageBank);
        images = ImageMap();
	}

	virtual bool Init() override;
    virtual void Write(BinaryWriter& buffer, bool compress = false) override;
    virtual void Write(BinaryWriter& buffer, bool compress, OffsetsVector& offsets);
};

#endif // !IMAGEBANKCHUNK_H