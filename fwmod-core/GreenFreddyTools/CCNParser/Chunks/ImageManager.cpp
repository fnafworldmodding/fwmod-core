#include "ImageManager.h"
#include "BinaryTools/BinaryReader.h"
#include "ImageOffsets.h"
#include "../../../Globals.h"


void ImageManager::Write(BinaryWriter& buffer, bool compress) {
	if (!(this->imageBank && this->imageOffsets)) {
		throw std::runtime_error("ImageBank or imageOffsets are null. this should never happen");
	}
	// NOTE: there is probably a better way of doing this (no idea what I was saying here)
	CoreLogger.Debug("Writing ImageBank and ImageOffsets to buffer!");
    OffsetsVector& offsets = this->imageOffsets->offsets;
	this->imageBank->Write(buffer, compress, offsets);
	this->imageOffsets->Write(buffer, compress);
}