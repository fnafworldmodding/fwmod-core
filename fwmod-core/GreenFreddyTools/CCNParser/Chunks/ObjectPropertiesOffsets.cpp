#include "ObjectPropertiesOffsets.h"
#include "../../Utils/Decompressor.h"
#include <algorithm>
#include <string>

bool ObjectsPropOffsets::Init() {
	BinaryReader reader(this->data.data(), this->data.size());
	int count = this->size / sizeof(int); // size is alawys even
	OffsetsVector& offsets = this->offsets;
	offsets.resize(count);
	memcpy(this->offsets.data(), this->data.data(), this->size);

	return true;
}


void ObjectsPropOffsets::Write(BinaryWriter& buffer, bool compress) {
	if (compress) {
		size_t outcompSize = 0;
		int result = 0;
		// TODO: check result instead of throwing an error in CompressRaw
		void* compdata = Decompressor::CompressRaw((uint8_t *)this->offsets.data(), this->offsets.size() * sizeof(int), outcompSize, result);
		buffer.WriteFromMemory(compdata, outcompSize);
		return;
	}
	this->size = this->offsets.size() * sizeof(int);
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(this->offsets.data(), this->size);
}
