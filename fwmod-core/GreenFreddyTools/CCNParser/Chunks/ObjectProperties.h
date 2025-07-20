#include "Chunks.h"
#include "ObjectStructures.h"
#include "../../Utils/IntEnum.h"

IntEnum(PropertiesFlags, int) { // currently unused
		Compressed = 0, // Compressed object
		Decompressed = 2, // Decompressed object
};

constexpr bool DECOMPRESS_COMOBJECTS = false;

struct ObjectCommonItem {
	int DecompSize = 0;
	int Size = 0;
	int Flags = 2;
	// TODO: change the name, it's misleading 
	ObjectHeader* Type = nullptr;
	union {
		uint8_t* raw = nullptr;
		ObjectCommon* OCIObjectCommon;
		ObjectQuickBackdrop* OCIObjectQuickBackdrop;
		ObjectBackdrop* OCIObjectBackdrop;
	};
};

typedef std::vector<ObjectCommonItem> ObjectCommons;

class ObjectProperties : public Chunk {
public:
	ObjectCommons Objects{};
	// TODO: create a deconstructor to free memory of Objects

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
	virtual void Write(BinaryWriter& buffer, bool compress, OffsetsVector& offsets);
};
