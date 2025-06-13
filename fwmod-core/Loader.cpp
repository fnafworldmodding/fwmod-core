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

                // Create Image object using provided createImage function
                Image img = createImage(id, entry.path().string(), true);
                img.HotspotX = imageBank[id].HotspotX;
                img.HotspotY = imageBank[id].HotspotY;
                img.ActionPointX = imageBank[id].ActionPointX;
                img.ActionPointY = imageBank[id].ActionPointY;
                img.References = imageBank[id].References;
				img.TransparentColor = imageBank[id].TransparentColor;
                // Turn off the ALPHA flag
                //img.Flags.SetFlag("ALPHA", false);
                // Store in map with filename as key
                BinaryWriter writer("img loaded-" + std::to_string(img.Handle) + ".bitmap");
                writer.WriteFromMemory(img.data.data(), img.dataSize);
                imageBank[id] = img;
            }

        }
    }
    catch (const std::exception& e) {
        // Log error and continue
        // TODO: Replace with proper logging mechanism
        //std::cerr << "Error loading images: " << e.what() << '\n';
        //std::cout << "Filesystem error: " << e.what() << std::endl;
        throw;
    }

    // Shutdown GDI+
    //Gdiplus::GdiplusShutdown(gdiplusToken); // FIXME: commenting this out will get me assassinated just remember to NOT push to github till you solve it
}