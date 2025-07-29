#include "ObjectsManager.h"
#include "../Globals.h"

void ObjectsManager::Write(BinaryWriter& buffer, bool compress) {
	if (!(this->objectsProperties && this->objectsOffsets)) {
		throw std::runtime_error("ImageBank or imageOffsets are null. this should never happen");
	}
	CoreLogger.Info("[Core] writing ObjectProperties and ObjectsPropOffsets to buffer!");
	OffsetsVector& offsets = this->objectsOffsets->offsets;
	this->objectsProperties->Write(buffer, compress, offsets);
	this->objectsOffsets->Write(buffer, false);
}