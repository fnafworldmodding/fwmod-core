#include "ImageBank.h"
#include "../../Utils/Decompressor.h"
#include <lz4.h>
#include <algorithm>


bool ImageBank::Init() {
	// TODO: check if this->data is compressed and handle accordingly, do we even need to decompress data?
    BinaryReader buffer(this->data.data(), this->data.size());
    int size = buffer.ReadInt32();

    this->images.reserve(size);
    // TODO: use a for loop instead of a while loop
    while (buffer.HasMemory(sizeof(Image) - sizeof(std::vector<char>))) {
        Image img;
        buffer.ReadToMemory(&img, sizeof(Image) - sizeof(std::vector<char>));

        // Read the image data
        img.data.resize(std::max(0, img.dataSize - 4));
        buffer.ReadToMemory(img.data.data(), img.dataSize);

        this->images[img.Handle] = img;
    }
    return true;
}
