#ifndef PRELOAD_H
#define PRELOAD_H
#pragma once
#include <atomic>
#include <windows.h>


#include "CCNParser\CCNPackage.h"
#include "CCNParser\Chunks\Chunks.h"
//#include "CCNParser\Chunks\ImageManager.h"

#define static_short(value) static_cast<short>(value)

void StartPreloadProcess();

#define DAT_SUFFIX std::string("og")

inline static std::string addSuffix(const std::string& path, const std::string& suffix) {
    size_t lastSlash = path.find_last_of("\\/");
    size_t lastDot = path.find_last_of('.', path.length());

    std::string name = (lastDot == std::string::npos) ? path.substr(lastSlash + 1) : path.substr(lastSlash + 1, lastDot - lastSlash - 1);
    std::string ext = (lastDot == std::string::npos) ? "" : path.substr(lastDot);

    return path.substr(0, lastSlash + 1) + name + "-" + suffix + ext;
}


inline static  std::string getCurrentProcessName() {
    char buffer[MAX_PATH];
    DWORD bufferSize = MAX_PATH;

    // Pass NULL to get the current process
    DWORD result = GetModuleFileNameA(nullptr, buffer, bufferSize);

    if (result == 0) {
        // Handle error
        return std::string("Error");
    }

    return std::string(buffer);
}

inline static std::string getDatFilePath() {
    // Find the last dot in the path
    std::string exePath = getCurrentProcessName();
    size_t dotPos = exePath.find_last_of('.');

    if (dotPos == std::string::npos) {
        // Handle case where there's no extension
        return exePath + ".dat";
    }

    // Replace .exe with .dat
    return exePath.substr(0, dotPos) + ".dat";
}
template<typename T>
static inline T FindChunkByID(std::vector<Chunk*>& chunks, short id) {
    static_assert(std::is_pointer<T>::value, "T must be a pointer type");
    static_assert(std::is_base_of<Chunk, std::remove_pointer_t<T>>::value,
        "T must be a pointer to a type derived from Chunk");
    auto it = std::find_if(chunks.begin(), chunks.end(), [id](Chunk* ch) { return ch->id == id; });
    if (it != chunks.end()) {
        return static_cast<T>(it);
    }
    return nullptr;
}

template<typename T>
static inline T PopChunkByID(std::vector<Chunk*>& chunks, short id) {
    static_assert(std::is_pointer<T>::value, "T must be a pointer type");
    static_assert(std::is_base_of<Chunk, std::remove_pointer_t<T>>::value,
        "T must be a pointer to a type derived from Chunk");
    auto it = std::find_if(chunks.begin(), chunks.end(), [id](Chunk* ch) { return ch->id == id; });
    if (it != chunks.end()) {
        T result = static_cast<T>(*it);
        chunks.erase(it);
        return result;
    }
    return nullptr;
}



#endif // PRELOAD_H