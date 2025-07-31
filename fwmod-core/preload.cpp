#include "preload.h"
#include "loader.h"
#include "Shared.h"

#include "common.h"
#include "Globals.h"

#include "CCNParser\Chunks\ImageManager.h"
#include "CCNParser\Chunks\ObjectsManager.h"


void StartPreloadProcess() {
    CoreLogger.AddHandler(Logger::CreateCoreFileHandle("FWMCoreLogs.log"));
    loadPlugins();
    // move this function
    PluginsEventManager.AddListener("Chunks", [](std::vector<Chunk*>& chunks, BinaryReader& reader, __int64& flags) -> void {
        auto imagebankpos = std::distance(chunks.begin(), std::find_if(
            chunks.begin(), chunks.end(),
            [](Chunk* ch) { return ch->id == static_short(ChunksIDs::ImageBank); }
		)); // Find the position of the ImageBank chunk

        ImageBank* imagebank = PopChunkByID<ImageBank*>(chunks, static_short(ChunksIDs::ImageBank));
        ImageOffsets* imageoffsets = PopChunkByID<ImageOffsets*>(chunks, static_short(ChunksIDs::ImageOffsets));
        // raise an runtime error if one of them is nullptr
        if (!imagebank || !imageoffsets) {
            CoreLogger.Error("[Core] ImageBank or ImageOffsets chunk not found in the .dat file.");
            ExitProcess(1);
		}
        CoreLogger.Info("[Core] ImageBank and ImageOffsets chunks found in the .dat file!");
		loadImagesFromFolderToMap(imagebank->images); // Load images from the preload folder
        ImageManager* imageManager = new ImageManager();
        imageManager->imageBank = imagebank;
        imageManager->imageOffsets = imageoffsets;
        // Insert the ImageManager chunk at the position where ImageBank was removed
        chunks.insert(chunks.begin() + imagebankpos, imageManager);
		// Create Objects Manager and pop chunks
        auto objectsPropertiesPos = std::distance(chunks.begin(), std::find_if(
            chunks.begin(), chunks.end(),
            [](Chunk* ch) { return ch->id == static_short(ChunksIDs::ObjectProperties); }
        ));
        ObjectProperties* objectProperties = PopChunkByID<ObjectProperties*>(chunks, static_short(ChunksIDs::ObjectProperties));
        ObjectsPropOffsets* objectsOffsets = PopChunkByID<ObjectsPropOffsets*>(chunks, static_short(ChunksIDs::ObjectPropertiesOffsets));
        if (!objectProperties) {
            CoreLogger.Error("[Core] ObjectsManager chunk not found in the .dat file.");
            ExitProcess(1);
        }
		CoreLogger.Info("[Core] ObjectProperties and ObjectsPropOffsets chunk found in .dat file!");
		ObjectsManager* objectsManager = new ObjectsManager();
		objectsManager->objectsProperties = objectProperties;
        objectsManager->objectsOffsets = objectsOffsets;
		// Insert the ObjectsManager chunk at the position where ObjectsProperties was removed
		chunks.insert(chunks.begin() + objectsPropertiesPos, objectsManager);
    });
     //
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
	// Free all chunks
    for (Chunk* c : chunks) {
        delete c;
    }
    unloadPlugins();
}
