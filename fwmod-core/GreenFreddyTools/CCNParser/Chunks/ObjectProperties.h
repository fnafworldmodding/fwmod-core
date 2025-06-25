#include "Chunks.h"
/*
class ObjectProperties : public Chunk {
	public:
	std::vector<ObjectHeader> headers;
	std::vector<ObjectShape> shapes;
	std::vector<ObjectQuickBackdrop> quickBackdrops;
	std::vector<ObjectBackdrop> backdrops;
	ObjectProperties() : Chunk() {
		id = static_cast<short>(ChunksIDs::ObjectProperties);
		headers = std::vector<ObjectHeader>();
		shapes = std::vector<ObjectShape>();
		quickBackdrops = std::vector<ObjectQuickBackdrop>();
		backdrops = std::vector<ObjectBackdrop>();
	}
	ObjectProperties(short id, short flag, int size) : Chunk(id, flag, size) {
		this->id = static_cast<short>(ChunksIDs::ObjectProperties);
		headers = std::vector<ObjectHeader>();
		shapes = std::vector<ObjectShape>();
		quickBackdrops = std::vector<ObjectQuickBackdrop>();
		backdrops = std::vector<ObjectBackdrop>();
	}
	ObjectProperties(short flag, int size) : Chunk(flag, size) {
		id = static_cast<short>(ChunksIDs::ObjectProperties);
		headers = std::vector<ObjectHeader>();
		shapes = std::vector<ObjectShape>();
		quickBackdrops = std::vector<ObjectQuickBackdrop>();
		backdrops = std::vector<ObjectBackdrop>();
	}
	virtual bool Init() override;
	virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};
*/