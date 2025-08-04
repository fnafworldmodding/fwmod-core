#pragma once
#ifndef FONTBANK_H // !FONTBANK_H
#define FONTBANK_H
#include "Chunks.h"
#include <unordered_map>

struct FontInfo {
	int Checksum = 0;
	int References = 0;
	int Size = 0;
	int Height = 0;
	int Width = 0;
	int Escapement = 0;
	int Orientation = 0;
	int Weight = 0;

	uint8_t Italic = 0;
	uint8_t Underline = 0;
	uint8_t StrikeOut = 0;
	uint8_t CharSet = 0;
	uint8_t OutPrecision = 0;
	uint8_t ClipPrecision = 0;
	uint8_t Quality = 0;
	uint8_t PitchAndFamily = 0;
#ifndef _UNICODE
	char Name[32];
#else
	wchar_t Name[32];
#endif // !_UNICODE
};

struct FontItem {
	unsigned int DecompSize = 0;
	int Size = 0;
	int Handle = 0;
	int Flags = 2;
	union {
		uint8_t raw[sizeof(FontInfo)]{};
		FontInfo font;
	};
	FontItem() = default;
};


typedef std::unordered_map<int, FontItem> FontsMap;

class FontBank : public Chunk {
public:
	FontsMap fonts{};

	FontBank() : Chunk() {
		id = static_cast<short>(ChunksIDs::FontBank);
	}
	FontBank(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::FontBank);
	}
	FontBank(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::FontBank);
	}

	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
	virtual void Write(BinaryWriter& buffer, bool compress, OffsetsVector offsets);
};

#endif // !FONTBANK_H
