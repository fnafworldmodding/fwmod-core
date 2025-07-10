#ifndef DECOMPRESSOR_H  
#define DECOMPRESSOR_H  

#include <vector>  
#include <cstdint>  
#include <stdexcept>  
#include <memory>  

#include "../CCNParser/Chunks/Chunks.h"
#include "../BinaryTools/BinaryReader.h"
#include "../Globals.h"

class DLLCALL Decompressor {
public:
    static std::vector<char>    Decompress(BinaryReader& buffer, int& decompressed);
    static int                  DecompressChunk(Chunk& chunk, int& decompressed);
    static int                  DecompressChunk(Chunk& chunk, BinaryReader& buffer, int& decompressed);
    static std::vector<char>    DecompressBlock(const std::vector<char>& data, int size);
    static int                  DecompressBlock(Chunk& chunk, std::vector<char>& compressedData, int size);
    static std::string          getErrorMessage(int errorCode);
    static bool                 IsZlib(const std::vector<char>& check);
};

#endif // DECOMPRESSOR_H


