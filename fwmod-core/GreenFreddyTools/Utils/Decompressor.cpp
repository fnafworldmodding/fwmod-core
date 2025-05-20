#include <zlib.h>

#include "Decompressor.h"

#include "../BinaryTools/BinaryReader.h"


std::vector<uint8_t> Decompressor::Decompress(BinaryReader& buffer, int& decompressed) {
    int decomp_size = buffer.ReadInt32();
    int comp_size = buffer.ReadInt32();
    std::vector<uint8_t> compressedData(comp_size);
    buffer.ReadToMemory(compressedData.data(), comp_size);
    decompressed = decomp_size;
    return DecompressBlock(compressedData, decomp_size);
}

bool Decompressor::IsZlib(const std::vector<uint8_t>& check) {
    if (check.size() < 2) return false;
    if (check[0] != 0x78) return false;
    return check[1] == 0x01 || check[1] == 0x5E || check[1] == 0x9C || check[1] == 0xDA;
}

std::vector<uint8_t> Decompressor::DecompressBlock(const std::vector<uint8_t>& data, int size) {
    if (data.empty()) return {};

    // Estimate output size, or use a fixed buffer and grow if needed
    std::vector<uint8_t> decompressedData(size);
    uLongf destLen = static_cast<uLong>(size);

    int result;
    if (IsZlib(data)) {
        // ZEXTERN int ZEXPORT uncompress(Bytef * dest, uLongf * destLen,
        //    const Bytef * source, uLong sourceLen);
        result = uncompress(decompressedData.data(), &destLen, data.data(), static_cast<uLong>(data.size()));
    }
    else {
        // Raw deflate stream (no zlib header)
        z_stream strm = {};
        strm.next_in = const_cast<Bytef*>(data.data());
        strm.avail_in = static_cast<uInt>(data.size());
        strm.next_out = decompressedData.data();
        strm.avail_out = static_cast<uInt>(decompressedData.size());

        result = inflateInit2(&strm, -MAX_WBITS); // raw deflate
        if (result == Z_OK) {
            result = inflate(&strm, Z_FINISH);
            inflateEnd(&strm);
            destLen = strm.total_out;
        }
    }

    if (result != Z_STREAM_END) {
        throw std::runtime_error(getErrorMessage(result));
    }

    decompressedData.resize(destLen);
    return decompressedData;
}

std::string Decompressor::getErrorMessage(int errorCode) {
    switch (errorCode) {
    case Z_OK:
        return "No error";
    case Z_STREAM_END:
        return "Stream end reached";
    case Z_NEED_DICT:
        return "Dictionary needed";
    case Z_ERRNO:
        return "File I/O error";
    case Z_STREAM_ERROR:
        return "Stream error";
    case Z_DATA_ERROR:
        return "Data error";
    case Z_MEM_ERROR:
        return "Memory allocation error";
    case Z_VERSION_ERROR:
        return "Version mismatch";
    case Z_BUF_ERROR:
        return "Buffer error";
    default:
        return "Unknown error";
    }
}