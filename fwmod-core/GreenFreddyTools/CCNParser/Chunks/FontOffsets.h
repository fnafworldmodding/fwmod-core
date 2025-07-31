#pragma once
#ifndef FONTOFFSETS_H_
#define FONTOFFSETS_H_
#include "Chunks.h"

class FontOffsets : public Chunk {
public:
	OffsetsVector offsets;
	FontOffsets() : Chunk() {
		id = static_cast<short>(ChunksIDs::FontOffsets);
	}
	FontOffsets(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::FontOffsets);
	}
	FontOffsets(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::FontOffsets);
	}
	virtual ~FontOffsets() override = default;
	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};

#endif // !FONTOFFSETS_H_
