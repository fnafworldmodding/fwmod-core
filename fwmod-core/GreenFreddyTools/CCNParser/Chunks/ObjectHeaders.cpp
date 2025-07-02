#include "ObjectHeaders.h"

extern std::vector<ObjectHeader>* globalObjectHeaders = nullptr;
// NOTE: the data vector already holds the data for the headers, so we can just use it directly with pointers
// instead of duplicating the data in the headers vector

bool ObjectHeaders::Init() {
	BinaryReader reader(this->data.data(), this->size);
	int count = this->size / sizeof(ObjectHeader);
	headers.reserve(count);
	// TODO: you can already calculate the size, just either create a vector of pointers or instantly memcpy to vector data
	for (int i = 0; i < count; ++i) {
		ObjectHeader header;
		reader.ReadToMemory(&header, sizeof(ObjectHeader));
		headers.push_back(header);
	}
	globalObjectHeaders = &headers;
	return true;
}

void ObjectHeaders::Write(BinaryWriter& buffer, bool _) {
	this->size = headers.size() * sizeof(ObjectHeader);
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(headers.data(), this->size);
}
