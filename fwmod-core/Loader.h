#pragma once
#include "CCNParser/Chunks/ImageBank.h"
#include "CCNParser/Chunks/FontBank.h"
#include "CCNParser/Chunks/ObjectProperties.h"

void LoadImageToImageBank(ImageBank* imageBank, const std::string& path, int id, OffsetsVector* offsets = nullptr);
void loadImagesFromFolderToMap(ImageBank* imageBank, OffsetsVector* offsets = nullptr);
//void loadImagesFromFolderToMap(ImagesMap& imageBank, OffsetsVector* offsets = nullptr);
void LoadFontsFromFolderToMap(FontsMap& fontBank);
// objects
void LoadObjectsFromFolderToMap(ObjectProperties& objectProperties);