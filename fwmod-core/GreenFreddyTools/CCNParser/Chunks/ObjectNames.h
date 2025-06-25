#include "Chunks.h"

// huh names are unicode because of the CCN format, so we need to use std::wstring, create a unicode macro for the linker

#ifdef _UNICODE
#define ObjectNameStringType std::wstring
#else
#define ObjectNameStringType std::string
#endif // !_UNICODE

typedef std::vector<ObjectNameStringType> ObjectNamesVector;


class ObjectNames : public Chunk {
public:
	ObjectNamesVector names;
	ObjectNames() : Chunk() {
		id = static_cast<short>(ChunksIDs::ObjectNames);
		names = ObjectNamesVector();
	}
	ObjectNames(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::ObjectNames);
		names = ObjectNamesVector();
	}
	ObjectNames(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::ObjectNames);
		names = ObjectNamesVector();
	}
	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};
