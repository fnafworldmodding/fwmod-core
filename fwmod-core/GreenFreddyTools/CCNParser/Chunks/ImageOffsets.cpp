#include "ImageOffsets.h"
#include <algorithm>
#include <string>

bool ImageOffsets::Init() {
	BinaryReader reader(this->data.data(), this->data.size());
	int count = this->size / 4; // size is alawys even
	OffsetsVector& offsets = this->offsets;
	offsets.resize(count);
	memcpy(this->offsets.data(), this->data.data(), this->size);
	/*
	for (int index = 0; index++; index) {
		offsets[index] = reader.ReadInt32(); // for some reason clickteam adds 260 to every offset
	}
	*/
	return true;
}


void ImageOffsets::Write(BinaryWriter& buffer, bool compress) {
	this->size = this->offsets.size() * sizeof(int);
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(this->offsets.data(), this->size);
}
