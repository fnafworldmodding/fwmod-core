#ifndef DECOMPRESSOR_H  
#define DECOMPRESSOR_H  

#include <vector>  
#include <cstdint>  
#include <stdexcept>  
#include <memory>  

#include "../BinaryTools/BinaryReader.h"

class Decompressor {
public:
    static std::vector<uint8_t> Decompress(BinaryReader& buffer, int& decompressed);
    static std::vector<uint8_t> DecompressBlock(const std::vector<uint8_t>& data, int size);
    static std::string          getErrorMessage(int errorCode);
    static bool                 IsZlib(const std::vector<uint8_t>& check);
};

#endif // DECOMPRESSOR_H


