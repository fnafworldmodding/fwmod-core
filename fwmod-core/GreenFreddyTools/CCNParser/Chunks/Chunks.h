#pragma once  
#ifndef CHUNKS_H  
#define CHUNKS_H  
#include <vector>

#include "../../Utils/IntEnum.h"
#include "../../../common.h"
//
#include "../../BinaryTools/BinaryReader.h"
#include "../../BinaryTools/BinaryWriter.h"

// for offset chunks

// for some reason clickteam adds 260 to every offset
#define OFFSET_ADDTION 260

typedef std::vector<uint32_t> OffsetsVector;
//

#define CHUNK_HEADER_SIZE 8 // 2 shorts + 4 bytes for size

enum class ChunksIDs {  
   Preview = 0x1122,  
   MiniHeader = 0x2222,  
   AppHeader = 0x2223,  
   AppName = 0x2224,  
   Author = 0x2225,  
   MenuBar = 0x2226,  
   ExtensionsPath = 0x2227,
   Frame = 0x3333,  
   FrameHeader = 0x3334,  
   FrameName = 0x3335,  
   FramePassword = 0x3336,  
   FramePalette = 0x3337,  
   FrameInstances = 0x3338,  
   FrameTransitionIn = 0x333B,  
   FrameTransitionOut = 0x333C,  
   FrameEvents = 0x333D,  
   FrameLayers = 0x3341,  
   FrameRect = 0x3342,  
   FrameSeed = 0x3344,  
   FrameLayerEffects = 0x3345,  
   FrameMoveTimer = 0x3347,  
   FrameMosaicTable = 0x3348,  
   FrameEffects = 0x3349,  
   FrameHandle = 0x334C,  
   ObjectHeaders = 0x2253,
   ObjectNames = 0x2254,
   ObjectShaders = 0x2255,
   ObjectProperties = 0x2256,
   ObjectPropertiesOffsets = 0x2257,
   ObjectInfoHeader = 0x4444,  
   ObjectInfoName = 0x4445,  
   ObjectCommon = 0x4446,  
   ObjectInfoShader = 0x4448,  
   ObjectAnimations = 0x4449,  
   AnimationOffsets = 0x444A,  
   ImageOffsets = 0x5555,  
   FontOffsets = 0x5556,  
   SoundOffsets = 0x5557,  
   MusicOffsets = 0x5558,  
   ImageBank = 0x6666,  
   FontBank = 0x6667,  
   SoundBank = 0x6668,  
   Last = 0x7F7F  
};

IntEnum(InitFlags, int) {
	DECOMPRESS = 1 << 0, // Decompress chunks if compressed
    IGNOREUNKNOWN = 1 << 1, // Ignore unknown chunks
};

//#pragma warning(push)
//#pragma warning(disable : 4251)

// Export all STL types in the warning chain for std::vector<uint8_t>
template class DLLCALL std::allocator<uint8_t>;
template struct DLLCALL std::_Simple_types<uint8_t>;
struct DLLCALL std::_Container_base12; // <-- Add this line
template class DLLCALL std::_Vector_val<std::_Simple_types<uint8_t>>;
template class DLLCALL std::_Compressed_pair<
    std::allocator<uint8_t>,
    std::_Vector_val<std::_Simple_types<uint8_t>>,
    true
>;
template class DLLCALL std::vector<uint8_t>;

class DLLCALL Chunk {
public:
    short id;
    short flag;
    unsigned int size;

    std::vector<uint8_t> data; // TODO: change type from char to unsigned char for better compatibility with decompression libraries

    Chunk();
    Chunk(short id, short flag, int size);
    Chunk(short flag, int size);
    virtual ~Chunk() = default;
    bool IsCompressed() const { return flag == 1; }; // compressed flag
    virtual bool Init() { return true; };

    void FreeData() {
        this->data.resize(0);
        this->data.shrink_to_fit();
    };
    static Chunk* InitChunk(BinaryReader& buffer, int flags = static_cast<int>(InitFlags::DECOMPRESS | InitFlags::IGNOREUNKNOWN)); // InitFlags (DECOMPRESS | IGNOREUNKNOWN)
    virtual void Read(BinaryReader& buffer, bool decompress = true);
	virtual void WriteHeader(BinaryWriter& buffer);
    virtual void Write(BinaryWriter& buffer, bool compress = false); // INFO: don't care about compressing at the moment
};

//#pragma warning(pop)

#endif CHUNKS_H