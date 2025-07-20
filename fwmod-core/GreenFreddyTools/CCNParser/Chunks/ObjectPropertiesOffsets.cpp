#include "ObjectPropertiesOffsets.h"

#include <algorithm>
#include <string>

bool ObjectsPropOffsets::Init() {
	BinaryReader reader(this->data.data(), this->data.size());
	int count = this->size / 4; // size is alawys even
	OffsetsVector& offsets = this->offsets;
	offsets.resize(count);
	memcpy(this->offsets.data(), this->data.data(), this->size);

	return true;
}


void ObjectsPropOffsets::Write(BinaryWriter& buffer, bool compress) {
	this->size = this->offsets.size() * sizeof(int);
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(this->offsets.data(), this->size);
}
