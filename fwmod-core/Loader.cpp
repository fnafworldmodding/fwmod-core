#include "loader.h"
#include "CCNParser\Chunks\ImageBank.h"
#include "CCNParser\Chunks\Image.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include "Globals.h"

void loadImagesFromFolderToMap(ImageMap& imageBank) {
    // instead of making a another map, why not directly use the ImageBank.images map?
    // just overwrite entries with the same ID directly to ImageBank.images

    // Get absolute path to preload folder
    namespace fs = std::filesystem;


    try {
        auto preloadPath = fs::current_path() / "preload";
        if (!fs::exists(preloadPath) || !fs::is_directory(preloadPath)) {
            throw std::runtime_error("preload directory not found or it's not a directory");
        }

        // Scan directory for PNG files
        for (const auto& entry : fs::directory_iterator(preloadPath)) {
            if (entry.is_regular_file() &&
                entry.path().extension() == ".png") {

                // Get the filename without extension
                std::string filename = entry.path().stem().string();

                // Convert filename to integer ID
                int id = static_cast<uint32_t>(std::stoi(filename));
                Image img2 = imageBank[id];

                Image img = createImage(id, entry.path().string(), true);
				// TODO: read mate file for hotspot and action point values/other
                img.HotspotX = imageBank[id].HotspotX;
                img.HotspotY = imageBank[id].HotspotY;
                img.ActionPointX = imageBank[id].ActionPointX;
                img.ActionPointY = imageBank[id].ActionPointY;
                img.References = imageBank[id].References;
				img.TransparentColor = imageBank[id].TransparentColor;
                // Store in map with filename as key
                imageBank[id] = img;
            }

        }
    }
    catch (const std::exception&) {
        throw;
    }
}