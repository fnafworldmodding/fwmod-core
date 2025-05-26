#ifndef DECOMPRESSOR_H  
#define DECOMPRESSOR_H  

#include <vector>  
#include <cstdint>  
#include <stdexcept>  
#include <memory>  

#include "../CCNParser/Chunks/Chunks.h"
#include "../BinaryTools/BinaryReader.h"

class Decompressor {
public:
    static std::vector<uint8_t> Decompress(BinaryReader& buffer, int& decompressed);
	static int                  DecompressChunk(Chunk& chunk, BinaryReader& buffer, int& decompressed);
    static std::vector<uint8_t> DecompressBlock(const std::vector<uint8_t>& data, int size);
    static int                  DecompressBlock(Chunk& chunk, std::vector<uint8_t>& compressedData, int size);
    static std::string          getErrorMessage(int errorCode);
    static bool                 IsZlib(const std::vector<uint8_t>& check);
};

#endif // DECOMPRESSOR_H


