#include "Chunks.h"

Chunk::Chunk() {
	id = 0;
	flag = 0;
	size = 0;
	data = std::vector<char>();
}

Chunk Chunk::InitChunk(BinaryReader& buffer) {
    Chunk chunk;

    chunk.id = buffer.ReadInt16();
    chunk.flag = buffer.ReadInt16();
    chunk.size = buffer.ReadUint32();
    chunk.data.resize(chunk.size);
    buffer.ReadToMemory(chunk.data.data(), chunk.size);

    return chunk;
}
