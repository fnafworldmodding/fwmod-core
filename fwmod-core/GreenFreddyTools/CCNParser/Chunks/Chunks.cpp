#include "Chunks.h"


static Chunk InitChunk(BinaryReader& buffer) {
    // Read the chunk ID  
    short id = buffer.ReadInt16();

    // If ID is 0, return a default-constructed Chunk (or handle as needed)  
    if (id == 0) {
        return Chunk();
    }

    // Read the size of the chunk  
    unsigned int size = buffer.ReadUint32();

    std::vector<char> data(size);
    buffer.ReadToMemory(data.data(), size);

    Chunk chunk;
    chunk.id = id;
    chunk.size = size;
    chunk.data = std::make_unique<std::vector<char>>(std::move(data));

    return chunk;
}

//
// Read the chunk from the file
