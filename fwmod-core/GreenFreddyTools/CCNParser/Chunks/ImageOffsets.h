#ifndef IMAGEOFFSET_H
#define IMAGEOFFSET_H
#pragma once
#include "Chunks.h"

#define OFFSET_ADDTION 260

typedef std::vector<uint32_t> OffsetsVector;

class ImageOffsets : public Chunk {
public:
	OffsetsVector offsets;

	ImageOffsets() : Chunk() {
		id = static_cast<short>(ChunksIDs::ImageOffsets);
		offsets = OffsetsVector();
	}

	ImageOffsets(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::ImageOffsets);
		offsets = OffsetsVector();
	}

	ImageOffsets(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::ImageOffsets);
		offsets = OffsetsVector();
	}

	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};


#endif // !IMAGEOFFSET_H