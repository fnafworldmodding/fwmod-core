#ifndef ObjectProperties_CHUNK_H
#define ObjectProperties_CHUNK_H
#include "Chunks.h"
#include "ObjectStructures.h"
#include "../../Utils/Decompressor.h"
#include "../../Utils/IntEnum.h"

void AdjustOffsets(ObjectCommon* objcom, ushort* member, int delta); // TODO: export

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
	ObjectHeader* Header = nullptr;
	union {
		uint8_t* raw = nullptr;
		ObjectCommon* OCIObjectCommon;
		ObjectQuickBackdrop* OCIObjectQuickBackdrop;
		ObjectBackdrop* OCIObjectBackdrop;
	};
	ObjectCommonItem() = default;
	~ObjectCommonItem() {
		delete[] raw;
	}

	int Write(BinaryWriter& buffer) const {
		// givev the option to write in the uncompress form
		ObjectHeader* header = this->Header;
		uint8_t* rawData = nullptr;
		size_t compressSize = 0;
		int compressionResult = Z_OK; // Result of compression, currently ignored


		if (this->Flags == 1) { // The object is already compressed
			// Write the size of the object
			buffer.WriteInt32(this->DecompSize);
			buffer.WriteInt32(this->Size);
			buffer.WriteFromMemory(this->raw, this->Size);
			return compressionResult;
		}

		// The object is decompressed; we need to compress it (flag 0)
		if (header->Type == 0) { // QuickBackdrop
			ObjectQuickBackdrop* quickBackdrop = this->OCIObjectQuickBackdrop;
			size_t sizeToCompress = QUICKBACKDROPSIZE + quickBackdrop->Shape.CalcDynamicSize() + 10;
			uint8_t* dataToCompress = new uint8_t[sizeToCompress];

			// Copy quickBackdrop data and write shape data
			memcpy(dataToCompress, quickBackdrop, QUICKBACKDROPSIZE);
			quickBackdrop->Shape.Write(dataToCompress + QUICKBACKDROPSIZE, quickBackdrop->Shape.CalcDynamicSize());
			// Fill the rest of the data with zeros because for some reason the game expects 10 more bytes ?
			memset(dataToCompress + QUICKBACKDROPSIZE + quickBackdrop->Shape.CalcDynamicSize(), 0, 10);

			// Compress the data
			rawData = Decompressor::CompressZlibRaw(dataToCompress, sizeToCompress, compressSize, compressionResult);
			buffer.WriteInt32(sizeToCompress); // Write the decompressed size
			delete[] dataToCompress;
		}
		else {
			// Compress the raw object data
			rawData = Decompressor::CompressZlibRaw(this->raw, this->OCIObjectCommon->size, compressSize, compressionResult);
			buffer.WriteInt32(this->OCIObjectCommon->size); // Write the decompressed size
		}

		// Write the compressed size and data
		buffer.WriteInt32(compressSize);
		buffer.WriteFromMemory(rawData, compressSize);
		delete[] rawData;
		return compressionResult;
	}
};

typedef std::vector<ObjectCommonItem> ObjectCommons;

class ObjectProperties : public Chunk {
public:
	ObjectCommons Objects{};
    virtual ~ObjectProperties() override {  
		Objects.clear();
    }

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
#endif // !ObjectProperties_CHUNK_H