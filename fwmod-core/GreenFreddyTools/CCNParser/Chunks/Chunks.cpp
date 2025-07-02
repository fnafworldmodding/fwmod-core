#include "Chunks.h"
#include "AppHeader.h"
#include "ImageBank.h"
#include "ImageOffsets.h"
#include "ObjectHeaders.h"
#include "ObjectProperties.h"
#include "../../Utils/Decompressor.h"
#include "../../Utils/BitDict.h"


Chunk::Chunk() {
	id = 0;
	flag = 0;
	size = 0;
	data = std::vector<char>();
}

Chunk::Chunk(short id, short flag, int size) {
	this->id = id;
	this->flag = flag;
	this->size = size;
	this->data = std::vector<char>(size);
}


Chunk::Chunk(short flag, int size) {
	this->id = 0;
	this->flag = flag;
	this->size = size;
	this->data = std::vector<char>(size);
}

void Chunk::Read(BinaryReader& buffer, bool decompress) {
	this->data.resize(this->size);
	if (decompress == true && this->flag == 1) { // compressed flag
		int size = static_cast<int>(this->size);
		int status = Decompressor::DecompressChunk(*this, buffer, size); // TODO: varify if there is any errors that been raised
		this->flag = 0;
		this->size = size;
	}
	else {
		buffer.ReadToMemory(this->data.data(), this->size);
	}
}
void Chunk::WriteHeader(BinaryWriter& buffer) {
	buffer.WriteInt16(this->id);
	buffer.WriteInt16(this->flag);
	buffer.WriteUint32(this->size);
}

void Chunk::Write(BinaryWriter& buffer, bool _) {
	// TODO: implement compression
	//Decompressor::CompressChunk(*this, buffer);
	//this->flag = 1; // set compressed flag
	this->WriteHeader(buffer);
	buffer.WriteFromMemory(this->data.data(), this->size);
}

Chunk* Chunk::InitChunk(BinaryReader& buffer, int flags) {
	BitDict chflag(std::vector<std::string>{"Decompress", "DontDecompressUnknownChunks"}, flags);
    short id = buffer.ReadInt16();
	short flag = buffer.ReadInt16();
	int size = buffer.ReadUint32();

	Chunk* chunk = nullptr;
	switch (id) {
	case static_cast<short>(ChunksIDs::AppHeader):
		chunk = new AppHeader(id, flag, size);
		break;
	case static_cast<short>(ChunksIDs::ImageBank):
		chunk = new ImageBank(flag, size);
		break;
	case static_cast<short>(ChunksIDs::ImageOffsets):
		chunk = new ImageOffsets(flag, size);
		break;
	// object chunks
	case static_cast<short>(ChunksIDs::ObjectHeaders):
		chunk = new ObjectHeaders(flag, size);
		break;
	case static_cast<short>(ChunksIDs::ObjectProperties):
		chunk = new ObjectProperties(flag, size);
		break;
	default:
		chunk = new Chunk(id, flag, size);
		if (chflag.GetFlag("DontDecompressUnknownChunks")) {
			chunk->Read(buffer, false); // no point in decompressing unknown chunks
			return chunk;
		}
		break;
	}

	chunk->Read(buffer, chflag.GetFlag("Decompress"));
	return chunk;
}
