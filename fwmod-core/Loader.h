#pragma once
#include "CCNParser/Chunks/ImageBank.h"
#include "CCNParser/Chunks/FontBank.h"

void loadImagesFromFolderToMap(ImagesMap& imageBank, OffsetsVector* offsets = nullptr);
void LoadFontsFromFolderToMap(FontsMap& fontBank);
