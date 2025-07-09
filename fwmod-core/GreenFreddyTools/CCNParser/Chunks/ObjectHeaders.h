#include "Chunks.h"
#include "ObjectStructures.h"

extern DLLCALL std::vector<ObjectHeader>* globalObjectHeaders;

class ObjectHeaders : public Chunk {
	public:
	std::vector<ObjectHeader> headers;
	ObjectHeaders() : Chunk() {
		id = static_cast<short>(ChunksIDs::ObjectHeaders);
		headers = std::vector<ObjectHeader>();
	}
	ObjectHeaders(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::ObjectHeaders);
		headers = std::vector<ObjectHeader>();
	}
	ObjectHeaders(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::ObjectHeaders);
		headers = std::vector<ObjectHeader>();
	}
	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};