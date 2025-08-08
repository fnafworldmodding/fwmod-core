#include "loader.h"
#include "CCNParser\Chunks\ImageBank.h"
#include "CCNParser\Chunks\Image.h"
#include "Globals.h"
#include "Utils/Decompressor.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <json.hpp>

void loadImagesFromFolderToMap(ImageBank& imageBank, OffsetsVector* offsets) {
    // Get absolute path to preload folder
    namespace fs = std::filesystem;


    auto preloadPath = fs::current_path() / "preload";
    if (!fs::exists(preloadPath) || !fs::is_directory(preloadPath)) {
        throw std::runtime_error("preload directory not found or it's not a directory");
    }

    for (const auto& entry : fs::directory_iterator(preloadPath)) {
        if (entry.is_regular_file() &&
            entry.path().extension() == ".png") {

            // Get the filename without extension
            std::string filename = entry.path().stem().string();

            // Convert filename to integer ID
            int id = static_cast<uint32_t>(std::stoi(filename));

            // TODO: providing the image path in the json file aka mate file is better than whateve this is
            Image img = createImage(id, entry.path().string(), true);
            // TODO: read mate file for hotspot and action point values/other
            img.HotspotX = imageBank[id].HotspotX;
            img.HotspotY = imageBank[id].HotspotY;
            img.ActionPointX = imageBank[id].ActionPointX;
            img.ActionPointY = imageBank[id].ActionPointY;
            img.References = imageBank[id].References;
            img.TransparentColor = imageBank[id].TransparentColor;
            // Store in map with filename as key
            //imageBank[id] = img;
            imageBank.AddImage(img, offsets);
        }

    }
}

static void DecompressFontItem(FontItem& fontItem) {
    if (fontItem.Flags == 1) { // Compressed
        int result = 0;
        uint8_t* rawData = Decompressor::DecompressBlockRaw(fontItem.raw, fontItem.Size, fontItem.DecompSize);
        if (result != 0) {
            throw std::runtime_error("Failed to decompress font item with ID: " + std::to_string(fontItem.Handle));
        }
        //uint32_t newcheck = ClickteamChecksumGenerator(rawData, fontItem.DecompSize);
        memcpy(&fontItem.raw, rawData, fontItem.DecompSize);
        fontItem.Flags = 0;
    }
}



void LoadFontsFromFolderToMap(FontsMap& fontBank) {
    // TODO: create a flag to indicate if the user wants to append or replace a font
    namespace fs = std::filesystem;
    using json = nlohmann::json;
    try {
        auto preloadPath = fs::current_path() / "preload";
        if (!fs::exists(preloadPath) || !fs::is_directory(preloadPath)) {
            throw std::runtime_error("preload directory not found or it's not a directory");
        }

        for (const auto& entry : fs::directory_iterator(preloadPath)) {
            std::string pathStr = entry.path().string();
            if (entry.is_regular_file() && pathStr.size() >= 10 && pathStr.substr(pathStr.size() - 10) == ".font.json") {
                // Get the filename without extension
                std::string filename = entry.path().stem().stem().string();
                // Convert filename to integer ID
                int id = static_cast<uint32_t>(std::stoi(filename));
                FontItem& fontItem = fontBank[id];
                // Load json
                // Okay so we have a font.json file, we need to read it and modify the FontItem with it
                DecompressFontItem(fontItem);
                std::ifstream f(entry.path().string());
                json data = json::parse(f);

                // fontItem.font->Checksum and fontItem.font->Size should be updated dynamically
                // Set fontite.font attributes based on data in json. like what's in json and what's it's counterpart in fontItem.font
                size_t name_size = 0;
                FontInfo* fontPtr = &fontItem.font;

                if (data.contains("References")) fontPtr->References = data["References"].get<int>();
                if (data.contains("Height")) fontPtr->Height = data["Height"].get<int>();
                if (data.contains("Width")) fontPtr->Width = data["Width"].get<int>();
                if (data.contains("Escapement")) fontPtr->Escapement = data["Escapement"].get<int>();
                if (data.contains("Orientation")) fontPtr->Orientation = data["Orientation"].get<int>();
                if (data.contains("Weight")) fontPtr->Weight = data["Weight"].get<int>();
                if (data.contains("Italic")) fontPtr->Italic = data["Italic"].get<uint8_t>();
                if (data.contains("Underline")) fontPtr->Underline = data["Underline"].get<uint8_t>();
                if (data.contains("StrikeOut")) fontPtr->StrikeOut = data["StrikeOut"].get<uint8_t>();
                if (data.contains("CharSet")) fontPtr->CharSet = data["CharSet"].get<uint8_t>();
                if (data.contains("OutPrecision")) fontPtr->OutPrecision = data["OutPrecision"].get<uint8_t>();
                if (data.contains("ClipPrecision")) fontPtr->ClipPrecision = data["ClipPrecision"].get<uint8_t>();
                if (data.contains("Quality")) fontPtr->Quality = data["Quality"].get<uint8_t>();
                if (data.contains("PitchAndFamily")) fontPtr->PitchAndFamily = data["PitchAndFamily"].get<uint8_t>();

                if (data.contains("Name")) {
#ifndef _UNICODE
                    std::string name = data["Name"].get<std::string>();
                    name_size = name.size() + 1;
#else
                    std::string _name = data["Name"].get<std::string>();
                    int wlen = MultiByteToWideChar(CP_UTF8, 0, _name.c_str(), -1, nullptr, 0);
                    std::wstring name(wlen, 0);
                    MultiByteToWideChar(CP_UTF8, 0, _name.c_str(), -1, &name[0], wlen);
                    if (!name.empty() && name.back() == L'\0') name.pop_back();
                    name_size = (name.size() + 1) * sizeof(wchar_t);
#endif
                    // TODO: size check
                    memset(&fontPtr->Name, 0, sizeof(fontPtr->Name)); // fill the rest with 0's
                    memcpy(&fontPtr->Name, name.data(), name_size); // Copy the name to the end of the FontInfo structure
                }
                fontPtr->Checksum = -1; // Update Checksum
            }
        }
    }
    catch (const std::exception&) {
        throw;
    }
}