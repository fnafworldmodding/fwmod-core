#ifndef ObjectsManagerCHUNK_H
#define ObjectsManagerCHUNK_H
#include "Chunks.h"
#include "ObjectProperties.h"
#include "ObjectHeaders.h"
#include "ObjectPropertiesOffsets.h"

class ObjectProperties;
class ObjectsPropOffsets;

#define OBJECTSMANAGER_CHUNK_ID static_cast<short>(0x9998)

class ObjectsManager : public Chunk {
public:
    ObjectProperties* objectsProperties = nullptr;
    ObjectsPropOffsets* objectsOffsets = nullptr;

    ObjectsManager() : Chunk() {
        id = OBJECTSMANAGER_CHUNK_ID;
    }

    ObjectsManager(short id, short flag, int size) : Chunk(id, flag, size) {
        this->id = OBJECTSMANAGER_CHUNK_ID;
    }

    ObjectsManager(short flag, int size) : Chunk(flag, size) {
        id = OBJECTSMANAGER_CHUNK_ID;
    }

    virtual void Write(BinaryWriter& buffer, bool compress = false) override;
};

#endif // !ObjectsManagerCHUNK_H