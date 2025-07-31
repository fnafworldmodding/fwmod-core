#ifndef IMAGEMANAGERCHUNK_H
#define IMAGEMANAGERCHUNK_H
#include "Chunks.h"
#include "ImageBank.h"
#include "ImageOffsets.h"

class ImageBank;
class ImageOffsets;

#define IMAGEMANAGER_CHUNK_ID static_cast<short>(0x9999)

// rename to ImagesManager?
class ImageManager : public Chunk {
public:
    ImageBank* imageBank = nullptr;
    ImageOffsets* imageOffsets = nullptr;

    ImageManager() : Chunk() {
        id = IMAGEMANAGER_CHUNK_ID;
    }

    ImageManager(short id, short flag, int size) : Chunk(id, flag, size) {
        this->id = IMAGEMANAGER_CHUNK_ID;
    }

    ImageManager(short flag, int size) : Chunk(flag, size) {
        id = IMAGEMANAGER_CHUNK_ID;
    }

    virtual ~ImageManager() override {
        delete imageBank;
        delete imageOffsets;
	}

    virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};

#endif // !IMAGEMANAGERCHUNK_H