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
	// is there a reason to have a result parameter when I throw an error? lol?
    static uint8_t*             CompressZlibRaw(uint8_t* data, size_t size, size_t& compressed, int& result);
	static uint8_t*             CompressZlibRaw2(uint8_t* data, size_t size, size_t& compressed, int& result);
    static uint8_t*             CompressRaw(uint8_t* data, size_t size, size_t& compressed, int& result);
    static uint8_t*             CompressRetRaw(const std::vector<uint8_t>& data, int& compressed, int& result);
    static std::vector<uint8_t> Compress(const std::vector<uint8_t>& data, int& compressed, int& result);
    //
    static uint8_t*             DecompressRaw(const uint8_t* buffer, size_t bufferSize, int& decompressed);
    static std::vector<uint8_t> Decompress(BinaryReader& buffer, int& decompressed);
    static int                  DecompressChunk(Chunk& chunk, int& decompressed);
    static int                  DecompressChunk(Chunk& chunk, BinaryReader& buffer, int& decompressed);
    static std::vector<uint8_t> DecompressBlock(const std::vector<uint8_t>& data, int size);
    static uint8_t*             DecompressBlockRaw(uint8_t* buffer, size_t bufferSize, size_t size);
    static int                  DecompressBlock(Chunk& chunk, std::vector<uint8_t>& compressedData, int size);
    //
    static std::string          getErrorMessage(int errorCode);
    static bool                 IsZlib(const std::vector<uint8_t>& check);
    static bool                 IsZlib(uint8_t*& check, size_t size);
};

#endif // DECOMPRESSOR_H


