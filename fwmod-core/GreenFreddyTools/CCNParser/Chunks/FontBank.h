#pragma once
#ifndef FONTBANK_H // !FONTBANK_H
#define FONTBANK_H
#include "Chunks.h"
#include <unordered_map>
#include "Utils/Decompressor.h"

// TODO: create ReadFontEx function same as the ReadImageEx usag

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
	void Write(BinaryWriter& buffer) const {
			if (this->Flags == 1) {
				// The font is already compressed
				buffer.WriteInt32(this->Handle);
				buffer.WriteInt32(this->DecompSize);
				buffer.WriteInt32(this->Size);
				buffer.WriteFromMemory(this->raw, this->Size);
				return;
			}
			buffer.WriteInt32(this->Handle);
			// fixme: the decompress size and compressed size aka Size is so confusing, maybe depend on flags rework the size determination
			int result = 0; // Result of compression, currently ignored because we throw the error in the function (ik bad)
			size_t outCompSize = 0;
			uint8_t* rawData = Decompressor::CompressZlibRaw((uint8_t*)&this->raw, this->DecompSize, outCompSize, result);
			buffer.WriteInt32(this->DecompSize); // write the decompressed size
			buffer.WriteInt32(outCompSize); // write the compressed size
			buffer.WriteFromMemory(rawData, outCompSize);
			delete[] rawData;
	}
	void ReadEx(BinaryReader& buffer, int handle, bool decompress) {
		this->Handle = handle;

		this->Flags = 1; // 1 == compressed
		this->DecompSize = buffer.ReadInt32();
		this->Size = buffer.ReadInt32();
		buffer.ReadToMemory(&this->raw, this->Size);
		if (decompress) {
			this->Decompress();
		}
	}

	bool Decompress() {
		// TODO: could improve by not copying data but directly letting zlib write to this->raw
		if (this->Flags != 1) {
			return false;
		}
		int result = 0;
		uint8_t* rawData = Decompressor::DecompressBlockRaw(this->raw, this->Size, this->DecompSize);
		if (result != 0) {
			return false;
		}
		memcpy(&this->raw, rawData, this->DecompSize);
		this->Flags = 0;
		delete[] rawData;
		return true;
	}
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
	virtual void Write(BinaryWriter& buffer, bool compress, OffsetsVector& offsets);
};

#endif // !FONTBANK_H
