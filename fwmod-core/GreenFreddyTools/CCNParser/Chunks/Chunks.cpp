#include "Chunks.h"

Chunk::Chunk() {
	id = 0;
	flag = 0;
	size = 0;
	data = nullptr;
}

Chunk Chunk::InitChunk(BinaryReader& buffer) {
    short id = buffer.ReadInt16();
    unsigned int size = buffer.ReadUint32();

    Chunk chunk;

    chunk.id = id;
    chunk.size = size;
    chunk.data = new std::vector<char>(size);
    buffer.ReadToMemory(chunk.data->data(), size);

    return chunk;
}

