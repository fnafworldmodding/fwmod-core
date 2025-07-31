#include "FontOffsets.h"
#include <algorithm>
#include <string>

bool FontOffsets::Init() {
	BinaryReader reader(this->data.data(), this->data.size());
	int count = this->size / sizeof(int);
	OffsetsVector& offsets = this->offsets;
	offsets.resize(count);
	memcpy(this->offsets.data(), this->data.data(), this->size);
	this->FreeData();
	return true;
}


void FontOffsets::Write(BinaryWriter& buffer, bool compress) {
	this->size = this->offsets.size() * sizeof(int);
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(this->offsets.data(), this->size);
}
