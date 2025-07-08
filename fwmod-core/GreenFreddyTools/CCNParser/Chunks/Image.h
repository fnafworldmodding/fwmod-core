#pragma once
#ifndef FWIMAGE_H
#define FWIMAGE_H
#include "ImageBank.h"
#include "../../Globals.h"

#include <windows.h>
#include <vector>
#include <mutex>
#include <cstdint>
#include <filesystem>
#include <objidl.h>
#include <gdiplus.h>
#define LZ4_STATIC_LINKING_ONLY
#include <lz4.h>

 // Converts Image struct to GDI+ Bitmap
DLLCALL Gdiplus::Bitmap* ToBitmap(const Image& image) {
    int Width = image.Width;
    int Height = image.Height;
    const std::vector<char>& data = image.data;

    if (Width <= 0 || Height <= 0 || data.empty()) {
        return new Gdiplus::Bitmap(1, 1); // fallback if invalid
    }

    Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(Width, Height, PixelFormat32bppARGB);

    Gdiplus::BitmapData bmpData;
    Gdiplus::Rect rect(0, 0, Width, Height);

    // Lock bitmap memory for writing
    if (bitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok) {
        const BYTE* source = reinterpret_cast<const BYTE*>(data.data());

        // Calculate how much we can safely copy
        // and handle stride (row padding in memory)
        for (int y = 0; y < Height; ++y) {
            BYTE* destRow = reinterpret_cast<BYTE*>(
                static_cast<BYTE*>(bmpData.Scan0) + y * bmpData.Stride);
            const BYTE* srcRow = source + y * Width * 4;
            memcpy(destRow, srcRow, Width * 4); // 4 bytes per pixel (ARGB)
        }

        bitmap->UnlockBits(&bmpData);
    }

    return bitmap;
}

DLLCALL Image createImage(uint32_t handle, const std::string& path, bool compress) {
    // Implement a cache system for speed, maybe a folder called "__plcache__" or similar
    // instead of using std::string, we can use std::filesystem::path especially when implementing the cache system\
    // get the hotspots and action points from a .data file or something of that kind
    Image img;
    img.Handle = static_cast<uint32_t>(handle);

    // Convert std::string to wide string
    std::wstring wPath(path.begin(), path.end());

    // Load the image using GDI+
    Gdiplus::Bitmap bitmap(wPath.c_str(), false);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) {
        return img;
    }

    img.Width = static_cast<int16_t>(bitmap.GetWidth());
    img.Height = static_cast<int16_t>(bitmap.GetHeight());
    img.data.resize(img.Width * img.Height * 4); // 4 bytes per pixel (ARGB)

    Gdiplus::BitmapData bmpData;
    Gdiplus::Rect rect(0, 0, img.Width, img.Height);

    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpData) == Gdiplus::Ok) {
        BYTE* dest = reinterpret_cast<BYTE*>(img.data.data());
        BYTE* src = reinterpret_cast<BYTE*>(bmpData.Scan0);

        for (int y = 0; y < img.Height; ++y) {
            memcpy(dest + y * img.Width * 4, src + y * bmpData.Stride, img.Width * 4);
        }

        bitmap.UnlockBits(&bmpData);
        img.Flags.SetValue(144); // it's both RGBA and Alpha

        if (compress) {
            // Calculate compressed size
            int maxCompressedSize = LZ4_compressBound(img.data.size());
            std::vector<char> compressedData(maxCompressedSize);

            // Perform compression
            int compressedSize = LZ4_compress_default(
                reinterpret_cast<const char*>(img.data.data()),
                reinterpret_cast<char*>(compressedData.data()),
                img.data.size(),
                maxCompressedSize
            );

            if (compressedSize > 0) {
                img.data = std::move(compressedData);
                img.data.resize(compressedSize);
                img.decompSizePlus = img.Width * img.Height * 4; // because it's a bitmap
                img.Flags.SetFlag(ImageFlags::LZX, true);
            }
            else {
                // Compression failed, raise runtime error
				throw std::runtime_error("Image compression failed for image: " + std::to_string(handle));
            }
        }
    }

    img.dataSize = static_cast<int32_t>(img.data.size()) + (compress ? 4 : 0);
    img.GraphicMode = 8; // ctf 2.5+ format
    return img;
}


#endif // !FWIMAGE_H