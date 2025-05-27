#include "preload.h"

#include "common.h"
#include <format>

#include "GreenFreddyTools\CCNParser\CCNPackage.h"
#include "GreenFreddyTools\CCNParser\Chunks\Chunks.h"
#include "EventManager.h"
#include "Logger.h"

#define DAT_PREFIX std::string("og-")
std::atomic<bool> PreloadStateReady = false;

static Logger CoreLogger;
static EventManager<Chunk, BinaryReader&, __int64&> PluginsEventManager;

inline static std::string addPrefix(const std::string& path, const std::string& prefix) {
    size_t lastSlash = path.find_last_of("\\/");
    std::string filename = path.substr(lastSlash + 1);
    return path.substr(0, lastSlash + 1) + prefix + filename;
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

void StartPreloadProcess() {
    SuspendAllThreadsExceptThis();
    PreloadStateReady = true;
    CoreLogger.AddHandler(Logger::CreateCoreFileHandle("FWMCoreLogs.log"));
    PluginsEventManager.AddListener("Chunk", [](Chunk chunk, BinaryReader& reader, __int64& flags) -> void {
        if (chunk.id == static_short(ChunksIDs::AppHeader)) {
            CoreLogger.Debug("[MYAWESOME] Chunk AppHeader Found!");
        }
        else if (chunk.id == static_short(ChunksIDs::ObjectProperties)) {
            CoreLogger.Debug("[MYAWESOME] Chunk ObjectProperties Found!");
            // TODO: read the ObjectProperties chunk
        }
        else if (chunk.id == 0x2253) {
            return; // Implement
        }
    });

    std::string datPath = addPrefix(getDatFilePath(), DAT_PREFIX);
    const char* filePath = datPath.c_str();
    if (GetFileAttributesA(filePath) == INVALID_FILE_ATTRIBUTES) {
        UnsuspendAllThreads();
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
    std::vector<Chunk> chunks;
    Chunk chunk;
    __int64 flags = 0;
    while (true) {
        chunk = Chunk::InitChunk(BR);
        CoreLogger.Info("[Chunk] Read Chunk! - Size: " + std::to_string(chunk.size)
            + ", ID: 0x" + std::format("{:x}", chunk.id)
            + ", Flag: " + std::to_string(chunk.flag));
        PluginsEventManager.Dispatch("Chunk", chunk, BR, flags);
        chunks.push_back(chunk);
        if (chunk.id == static_cast<short>(ChunksIDs::Last)) {
            CoreLogger.Info("[Chunk] Reached LAST Chunk");
            break;
        }
    }
    // TODO: write chunks
    CoreLogger.Info("[Core] Finished reading .dat file: " + datPath);
    std::string datWritePath = getDatFilePath();
	CoreLogger.Info("[Core] Writing .dat file: " + datWritePath);

    BinaryWriter BW(datWritePath);
	ccnPackage.WriteCCN(BW);
	for (Chunk& c : chunks) {
		c.Write(BW);
	}
    CoreLogger.Info("[Core] Finished writing to: " + datWritePath);
    UnsuspendAllThreads();
}