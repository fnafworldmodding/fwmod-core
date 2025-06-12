#include "loader.h"
#include "CCNParser\Chunks\ImageBank.h"
#include "CCNParser\Chunks\Image.h"
#include <filesystem>
#include <string>
#include <unordered_map>

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
                if (img2.dataSize > 0) {
                    // Allocate space for decompressed data
                    std::vector<uint8_t> decompressedData(img2.decompSizePlus);

                    // Perform decompression
                    int decompressedSize = LZ4_decompress_safe(
                        reinterpret_cast<const char*>(img2.data.data()),    // Source buffer
                        reinterpret_cast<char*>(decompressedData.data()),    // Destination buffer
                        img2.data.size(),                                   // Source size
                        img2.decompSizePlus                                 // Destination size
                    );

                    // Check for decompression errors
                    if (decompressedSize < 0) {
						continue; // Skip this image if decompression failed
                    }

                    // Update the image structure with decompressed data
                    // TODO: recompress them
                    img2.data = std::vector<char>(decompressedData.begin(), decompressedData.end());
                    img2.dataSize = decompressedSize + 4;
                    img2.decompSizePlus = 0;
                    img2.Flags = 128;
                    //img2.TransparentColor = 16711927;
					BinaryWriter writer("img og-" + std::to_string(img2.Handle) + ".bitmap");
					writer.WriteFromMemory(img2.data.data(), img2.dataSize - 4);
					imageBank[id] = img2; // Update the image in the map
                    continue;
                }


                // Create Image object using provided createImage function
                Image img = createImage(id, entry.path().string(), false);
                img.HotspotX = imageBank[id].HotspotX;
                img.HotspotY = imageBank[id].HotspotY;
                img.ActionPointX = imageBank[id].ActionPointX;
                img.ActionPointY = imageBank[id].ActionPointY;
                img.References = imageBank[id].References;
				img.TransparentColor = imageBank[id].TransparentColor;
                // Turn off the ALPHA flag
                img.Flags &= ~0x05;
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