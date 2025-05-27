#ifndef APPHEADERCHUNK_H
#define APPHEADERCHUNK_H
#include "Chunks.h"


class AppHeader : public Chunk {
public:
	AppHeader() : Chunk() {
		id = static_cast<short>(ChunksIDs::AppHeader);
	}
	AppHeader(short id, short flag, int size) : Chunk(id, flag, size) {}
};

#endif // !APPHEADERCHUNK_H