#include "ObjectNames.h"

bool ObjectNames::Init() {
	size_t size = this->data.size();
	BinaryReader buffer(this->data.data(), size);
	while (size < buffer.Position()) {
#ifdef _UNICODE
		this->names.push_back(buffer.ReadNullTerminatedStringWide());
#else
		this->names.push_back(buffer.ReadNullTerminatedString());
#endif
	}
	return false;
}

void ObjectNames::Write(BinaryWriter& buffer, bool compress) {
	this->size = 0;
	this->WriteHeader(buffer); // id short, flag short, size int
	buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t ChunkPosition) {
		int namesCount = static_cast<int>(this->names.size());
		buffer.WriteInt32(namesCount);
		for (const auto& name : this->names) {
#ifdef _UNICODE
			buffer.WriteNullTerminatedStringWide(name);
#else
			buffer.WriteNullTerminatedString(name);
#endif
		}
	});
}