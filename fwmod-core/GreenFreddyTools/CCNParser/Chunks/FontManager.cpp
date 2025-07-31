#include "FontManager.h"
#include "../../Utils/Decompressor.h"

void FontManager::Write(BinaryWriter& buffer, bool compress) {
	if (this->fontBank == nullptr || this->fontOffsets) {
		throw std::runtime_error("FontBank is null. this should never happen");
	}
	CoreLogger.Debug("Writing FontBank to buffer!");
	OffsetsVector& offsets = this->fontOffsets->offsets;
	this->fontBank->Write(buffer, compress, offsets);
	this->fontOffsets->Write(buffer, compress);
}