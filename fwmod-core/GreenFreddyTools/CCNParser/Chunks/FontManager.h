#pragma once
#ifndef FONTMANAGERCHUNK_H_
#define FONTMANAGERCHUNK_H_
#include "Chunks.h"
#include "FontBank.h"
#include "FontOffsets.h"

#define FONTMANAGER_CHUNK_ID static_cast<short>(0x9997)

class FontManager : public Chunk {
public:
	FontBank* fontBank = nullptr;
	FontOffsets* fontOffsets = nullptr;
	FontManager() : Chunk() {
		id = FONTMANAGER_CHUNK_ID;
	}
	FontManager(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = FONTMANAGER_CHUNK_ID;
	}
	FontManager(short flag, int size) : Chunk(flag, size) {
		id = FONTMANAGER_CHUNK_ID;
	}
	virtual ~FontManager() override {
		delete fontBank;
		delete fontOffsets;
	}
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};

#endif // !FONTMANAGERCHUNK_H_
