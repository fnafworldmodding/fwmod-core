#include "ObjectHeaders.h"

// NOTE: the data vector already holds the data for the headers, so we can just use it directly with pointers
// instead of duplicating the data in the headers vector

bool ObjectHeaders::Init() {
	BinaryReader reader(this->data.data(), this->size);
	int count = this->size / sizeof(ObjectHeader);
	headers.reserve(count);
	for (int i = 0; i < count; ++i) {
		ObjectHeader header;
		reader.ReadToMemory(&header, sizeof(ObjectHeader));
		headers.push_back(header);
	}
	return true;
}

void ObjectHeaders::Write(BinaryWriter& buffer, bool _) {
	this->size = headers.size() * sizeof(ObjectHeader);
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(headers.data(), this->size);
}
