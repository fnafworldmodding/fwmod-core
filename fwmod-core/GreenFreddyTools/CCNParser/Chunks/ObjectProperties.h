#include "Chunks.h"
#include "ObjectStructures.h"

constexpr size_t ObjectCommonSize = sizeof(ObjectCommon);
constexpr bool DECOMPRESS_COMOBJECTS = false;

struct ObjectCommonItem {
	int unknown = 0;
	int Size = 0;
	int Flags = 2;
	ObjectHeader* Type = nullptr;
	union {
		char raw[ObjectCommonSize];
		ObjectCommon OCIObjectCommon;
		ObjectQuickBackdrop OCIObjectQuickBackdrop;
		ObjectBackdrop OCIObjectBackdrop;
	};
	std::vector<char> data;
	ObjectCommonItem() {
		std::memset(raw, 0, sizeof(raw));
	};
};

typedef std::vector<ObjectCommonItem> ObjectCommons;

class ObjectProperties : public Chunk {
public:
	ObjectCommons Objects{};

	ObjectProperties() : Chunk() {
		id = static_cast<short>(ChunksIDs::ObjectProperties);
	}

	ObjectProperties(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::ObjectProperties);
	}

	ObjectProperties(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::ObjectProperties);
	}

	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};
