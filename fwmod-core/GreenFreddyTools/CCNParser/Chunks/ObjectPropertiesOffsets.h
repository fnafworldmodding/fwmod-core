#ifndef OBJECTPROPSOFFSET_H
#define OBJECTPROPSOFFSET_H
#pragma once
#include "Chunks.h"

class ObjectsPropOffsets : public Chunk {
public:
	OffsetsVector offsets;

	ObjectsPropOffsets() : Chunk() {
		id = static_cast<short>(ChunksIDs::ObjectPropertiesOffsets);
		offsets = OffsetsVector();
	}

	ObjectsPropOffsets(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::ObjectPropertiesOffsets);
		offsets = OffsetsVector();
	}

	ObjectsPropOffsets(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::ObjectPropertiesOffsets);
		offsets = OffsetsVector();
	}

	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};


#endif // !OBJECTPROPSOFFSET_H