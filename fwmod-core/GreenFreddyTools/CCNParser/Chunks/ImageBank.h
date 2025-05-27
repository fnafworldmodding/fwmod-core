#ifndef IMAGEBANKCHUNK_H
#define IMAGEBANKCHUNK_H
#include "Chunks.h"
#include <unordered_map>

struct Image {
    uint32_t Handle;
    int32_t Checksum; // can be -1 ig is some ignore value
    int32_t References;
	int32_t unknown; // Unknown field. always 1 for some reason
	int32_t dataSize; // Size of the image data
    int16_t Width;
    int16_t Height;
    uint8_t GraphicMode;
    uint8_t Flags;
    uint16_t padding;  // Skip 2 bytes
    int16_t HotspotX;
    int16_t HotspotY;
    int16_t ActionPointX;
    int16_t ActionPointY;
    uint32_t TransparentColor; // TODO: create a structure, RGBA color
    int32_t decompSizePlus;
	std::vector<char> data;
};


class ImageBank : Chunk {
public:
	std::unordered_map<uint32_t, Image> images;

	ImageBank() : Chunk() {
		id = static_cast<short>(ChunksIDs::ImageBank);
		images = std::unordered_map<uint32_t, Image>();
	}

	virtual bool Init() override;
};
#endif // !IMAGEBANKCHUNK_H