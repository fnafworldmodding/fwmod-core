#include <zlib.h>

#include "Decompressor.h"

#include "../BinaryTools/BinaryReader.h"

class Chunk;

std::vector<uint8_t> Decompressor::Decompress(BinaryReader& buffer, int& decompressed) {
    int decomp_size = buffer.ReadInt32();
    int comp_size = buffer.ReadInt32();
    std::vector<uint8_t> compressedData(comp_size);
    buffer.ReadToMemory(compressedData.data(), comp_size);
    decompressed = decomp_size;
    return DecompressBlock(compressedData, decomp_size);
}


int Decompressor::DecompressChunk(Chunk& chunk, BinaryReader& buffer, int& decompressed) {
    int decomp_size = buffer.ReadInt32();
    int comp_size = buffer.ReadInt32();
    std::vector<uint8_t> compressedData(comp_size);
    buffer.ReadToMemory(compressedData.data(), comp_size);
    decompressed = decomp_size;
    return DecompressBlock(chunk, compressedData, decomp_size);
}

int Decompressor::DecompressChunk(Chunk& chunk, int& decompressed) {
    BinaryReader buffer(chunk.data.data(), chunk.data.size());
    int decomp_size = buffer.ReadInt32();
    int comp_size = buffer.ReadInt32();
    std::vector<uint8_t> compressedData(comp_size);
    buffer.ReadToMemory(compressedData.data(), comp_size);
    decompressed = decomp_size;
    return DecompressBlock(chunk, compressedData, decomp_size);
}


int Decompressor::DecompressBlock(Chunk& chunk, std::vector<uint8_t>& compressedData, int size) {
    if (chunk.data.empty() || size == 0) {
        return Z_STREAM_ERROR;
    }

    // Resize the vector to hold the decompressed data
    chunk.data.resize(size);
    uLongf destLen = static_cast<uLong>(size);

    int result;
    if (IsZlib(compressedData)) {
		result = uncompress(
            reinterpret_cast<Bytef*>(chunk.data.data()),
			&destLen,
			compressedData.data(),
			static_cast<uLong>(compressedData.size())
		);
    }
    else {
        z_stream strm = {};
        strm.next_in = const_cast<Bytef*>(compressedData.data());
        strm.avail_in = static_cast<uInt>(compressedData.size());
        strm.next_out = reinterpret_cast<Bytef*>(chunk.data.data());
        strm.avail_out = static_cast<uInt>(chunk.data.size());



        result = inflateInit2(&strm, -MAX_WBITS); // raw deflate
        if (result == Z_OK) {
            result = inflate(&strm, Z_FINISH);
            inflateEnd(&strm);
            destLen = strm.total_out;
        }
    }

    return result;
}

std::vector<uint8_t> Decompressor::DecompressBlock(const std::vector<uint8_t>& data, int size) {
    std::vector<uint8_t> decompressedData(size);
    uLongf destLen = static_cast<uLong>(size);
    if (data.empty()) return decompressedData;

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

    if (result != Z_OK) {
        throw std::runtime_error(getErrorMessage(result));
    }

    decompressedData.resize(size);
    return decompressedData;
}

uint8_t* Decompressor::DecompressBlockRaw(uint8_t* buffer, size_t bufferSize, size_t size) {
    uLongf destLen = static_cast<uLong>(size);
	uint8_t* data = new uint8_t[size]; // decompressed data pointer
    int result;

    if (IsZlib(buffer, bufferSize)) {
        // ZEXTERN int ZEXPORT uncompress(Bytef * dest, uLongf * destLen,
        //    const Bytef * source, uLong sourceLen);
        result = uncompress(data, &destLen, buffer, static_cast<uLong>(bufferSize));
    }
    else {
        // Raw deflate stream (no zlib header)
        z_stream strm = {};
        strm.next_in = const_cast<Bytef*>(buffer);
        strm.avail_in = static_cast<uInt>(bufferSize);
        strm.next_out = data;
        strm.avail_out = static_cast<uInt>(size);

        result = inflateInit2(&strm, -MAX_WBITS); // raw deflate
        if (result == Z_OK) {
            result = inflate(&strm, Z_FINISH);
            inflateEnd(&strm);
            destLen = strm.total_out;
        }
    }

    if (result != Z_OK) {
        throw std::runtime_error(getErrorMessage(result));
    }

    return data;
}




bool Decompressor::IsZlib(const std::vector<uint8_t>& check) {
    if (check.size() < 2) return false;
    if (check[0] != 0x78) return false;
    return check[1] == 0x01 || check[1] == 0x5E || check[1] == 0x9C || check[1] == 0xDA;
}


bool Decompressor::IsZlib(uint8_t*& check, size_t size) {
    if (size < 2) return false;
    if (check[0] != 0x78) return false;
    return check[1] == 0x01 || check[1] == 0x5E || check[1] == 0x9C || check[1] == 0xDA;
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

std::vector<uint8_t> Decompressor::Compress(const std::vector<uint8_t>& data, int& compressed, int& result) {
    if (data.empty()) {
        compressed = 0;
        return {};
    }

    uLongf compBound = compressBound(static_cast<uLong>(data.size()));
    std::vector<uint8_t> compressedData(compBound);

    result = compress2(
        compressedData.data(),
        &compBound,
        data.data(),
        static_cast<uLong>(data.size()),
        Z_BEST_SPEED
    );

    if (result != Z_OK) {
        throw std::runtime_error(Decompressor::getErrorMessage(result));
    }

    compressedData.resize(compBound);
    compressed = static_cast<int>(compBound);
    return compressedData;
}


uint8_t* Decompressor::CompressRetRaw(const std::vector<uint8_t>& data, int& compressed, int& result) {
    if (data.empty()) {
        compressed = 0;
        return {};
    }

    uLongf compBound = compressBound(static_cast<uLong>(data.size()));
	uint8_t* compressedDataPtr = new uint8_t[compBound];

    result = compress2(
        compressedDataPtr,
        &compBound,
        data.data(),
        static_cast<uLong>(data.size()),
        Z_BEST_SPEED
    );

    if (result != Z_OK) {
        throw std::runtime_error(Decompressor::getErrorMessage(result));
    }

    compressed = static_cast<int>(compBound);
    return compressedDataPtr;
}

uint8_t* Decompressor::CompressRaw(uint8_t* data, size_t size, size_t& compressed, int& result) {
    uLongf compBound = compressBound(static_cast<uLong>(size));
    uint8_t* compressedDataPtr = new uint8_t[compBound];

    result = compress2(
        compressedDataPtr,
        &compBound,
        data,
        static_cast<uLong>(size),
        Z_BEST_SPEED
    );

    if (result != Z_OK) {
		delete[] compressedDataPtr;
        throw std::runtime_error(Decompressor::getErrorMessage(result));
    }

    compressed = static_cast<int>(compBound);
    return compressedDataPtr;
}

uint8_t* Decompressor::CompressZlibRaw(uint8_t* data, size_t size, size_t& compressed, int& result) {
    // Initialize the zlib stream
    z_stream strm{};
    strm.avail_in = static_cast<uInt>(size);
    strm.next_in = data;

    uLongf compBound = compressBound(static_cast<uLong>(size));
    uint8_t* compressedDataPtr = new uint8_t[compBound];


    result = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    if (result != Z_OK) {
        delete[] compressedDataPtr;
        throw std::runtime_error(Decompressor::getErrorMessage(result));
    }

    strm.avail_out = compBound;
    strm.next_out = compressedDataPtr;

    result = deflate(&strm, Z_FINISH);
    if (result < Z_OK) {
        deflateEnd(&strm);
        delete[] compressedDataPtr;
        throw std::runtime_error(Decompressor::getErrorMessage(result));
    }

    // Get the size of the compressed data
    compressed = compBound - strm.avail_out;

    // Clean up
    deflateEnd(&strm);
    return compressedDataPtr;
}


uint8_t* Decompressor::CompressZlibRaw2(uint8_t* data, size_t size, size_t& compressed, int& result) {
    // Calculate the maximum compressed size
    uLongf compBound = compressBound(static_cast<uLong>(size));
    uint8_t* compressedDataPtr = new uint8_t[compBound];

    // Perform the compression
    result = compress(compressedDataPtr, &compBound, data, static_cast<uLong>(size));

    if (result != Z_OK) {
        delete[] compressedDataPtr;
        throw std::runtime_error(Decompressor::getErrorMessage(result));
    }

    // Set the actual size of the compressed data
    compressed = static_cast<size_t>(compBound);
    return compressedDataPtr;
}
