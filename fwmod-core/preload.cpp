#include "preload.h"
#include "loader.h"

#include "common.h"
#include "Globals.h"

#include "CCNParser\CCNPackage.h"
#include "CCNParser\Chunks\Chunks.h"
#include "Utils\Decompressor.h"
#include "CCNParser\Chunks\ImageManager.h" // for PluginsEventManager.AddListener("Chunks", ...)


#define DAT_SUFFIX std::string("og-")
std::atomic<bool> PreloadStateReady = false;

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


void StartPreloadProcess() {
    PreloadStateReady = true;
    CoreLogger.AddHandler(Logger::CreateCoreFileHandle("FWMCoreLogs.log"));
    PluginsEventManager.AddListener("Chunks", [](std::vector<Chunk*>& chunks, BinaryReader& reader, __int64& flags) -> void {
        auto imagebankpos = std::distance(chunks.begin(), std::find_if(
            chunks.begin(), chunks.end(),
            [](Chunk* ch) { return ch->id == static_short(ChunksIDs::ImageBank); }
        ));
        ImageBank* imagebank = PopChunkByID<ImageBank*>(chunks, static_short(ChunksIDs::ImageBank));
        ImageOffsets* imageoffsets = PopChunkByID<ImageOffsets*>(chunks, static_short(ChunksIDs::ImageOffsets));
        // raise an runtime error if one of them is nullptr
        if (!imagebank || !imageoffsets) {
            CoreLogger.Error("[Core] ImageBank or ImageOffsets chunk not found in the .dat file.");
            ExitProcess(1);
		}
        CoreLogger.Info("[Core] ImageBank and ImageOffsets chunks found in the .dat file.");
		loadImagesFromFolderToMap(imagebank->images); // Load images from the preload folder
        ImageManager* imageManager = new ImageManager();
        imageManager->imageBank = imagebank;
        imageManager->imageOffsets = imageoffsets;
        // Insert the ImageManager chunk at the position where ImageBank was removed
        chunks.insert(chunks.begin() + imagebankpos, imageManager);
    });
     
    std::string datPath = addSuffix(getDatFilePath(), DAT_SUFFIX);
    const char* filePath = datPath.c_str();
    if (GetFileAttributesA(filePath) == INVALID_FILE_ATTRIBUTES) {
        char currentDir[MAX_PATH];
        CoreLogger.Info("[Core] finding .dat file " + datPath + " failed");
        if (_getcwd(currentDir, MAX_PATH)) { // Get the current working directory
            std::ostringstream errorMsg;
            errorMsg << "File does not exist: " << filePath << "\n";
            errorMsg << "Searched in: " << currentDir;
            MessageBoxA(nullptr, errorMsg.str().c_str(), "Error", MB_OK | MB_ICONERROR);
        }
        else {
            MessageBoxA(nullptr, "Failed to retrieve the current working directory.", "Error", MB_OK | MB_ICONERROR);
        }
        ExitProcess(1);
    }

    CCNPackage ccnPackage;
    BinaryReader BR(filePath);
    ccnPackage.ReadCCN(BR);
    std::vector<Chunk*> chunks;
    Chunk* chunk;
    __int64 flags = 0;
    while (true) {
        chunk = Chunk::InitChunk(BR);
        CoreLogger.Info("[Chunk] Read Chunk! - Size: " + std::to_string(chunk->size)
            + ", ID: 0x" + std::format("{:x}", chunk->id)
            + ", Flag: " + std::to_string(chunk->flag));
        if (!chunk->Init()) {
            CoreLogger.Error("[Chunk] Failed to initialize chunk with ID: 0x" + std::format("{:x}", chunk->id));
            ExitProcess(1);
        }
        chunks.push_back(chunk);
        if (chunk->id == static_cast<short>(ChunksIDs::Last)) {
            CoreLogger.Info("[Chunk] Reached LAST Chunk");
            break;
        }
    }
    CoreLogger.Info("[Core] Finished reading .dat file: " + datPath);
    PluginsEventManager.Dispatch("Chunks", chunks, BR, flags);
    std::string datWritePath = getDatFilePath();
	CoreLogger.Info("[Core] Writing .dat file: " + datWritePath);

    BinaryWriter BW(datWritePath);
	ccnPackage.WriteCCN(BW);

    for (Chunk* c : chunks) {
		c->Write(BW);
        if (BW.bad()) {
			CoreLogger.Error("[Core] Failed to write chunk with ID: 0x" + std::format("{:x}", c->id) + " to .dat file. stream seems to gone bad");
			ExitProcess(1);
        }
        if (BW.fail()) {
            CoreLogger.Error("[Core] Failed to write chunk with ID: 0x" + std::format("{:x}", c->id) + " to .dat file. stream failed");
            ExitProcess(1);
        }
	}
    CoreLogger.Info("[Core] Finished writing to: " + datWritePath);
}