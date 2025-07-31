#include "ObjectProperties.h"
#include "ObjectHeaders.h"
#include "Utils/Decompressor.h"
#include <type_traits> // For std::is_same
#include <zlib.h>

// TODO: remove this function, it is not being used
template <typename T>
int loadComObject(const unsigned char* compressedData, size_t compressedSize,
	T* objectCommon, std::vector<uint8_t>& properties) {
	z_stream strm{};
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = compressedSize; // Input size
	strm.next_in = const_cast<unsigned char*>(compressedData); // Input data

	int ret = inflateInit(&strm);
	size_t remainingCompressedSize{};
	size_t have{};

	if (ret != Z_OK) {
		return ret;
	}


	strm.avail_out = sizeof(T); // Set output size to T size
	strm.next_out = reinterpret_cast<unsigned char*>(objectCommon); // Output buffer

	ret = inflate(&strm, Z_NO_FLUSH); // decompress

	if (ret < 0) { // Check for errors
		goto cleanup;
	}

	// calculate how much data is left in the input buffer
	remainingCompressedSize = strm.avail_in; // remaining compressed data
	have = sizeof(T) - strm.avail_out; // amount of data decompressed

	// check if we have any remaining compressed data
	if (remainingCompressedSize > 0) {
		properties.resize(remainingCompressedSize);
		strm.avail_out = properties.size();
		strm.next_out = reinterpret_cast<unsigned char*>(properties.data()); // Output buffer for properties

		ret = inflate(&strm, Z_NO_FLUSH); // Decompress remaining data

		if (ret < 0) {
			goto cleanup;
		}
	}
	if constexpr (std::is_same<T, ObjectQuickBackdrop>::value) {
		ObjectQuickBackdrop* quickBackdrop = reinterpret_cast<ObjectQuickBackdrop*>(objectCommon);
		quickBackdrop->Shape.Read(properties.data(), properties.size());
		properties.resize(0);
	}
cleanup:
	// Clean up
	inflateEnd(&strm);
	return ret;
}


bool ObjectProperties::Init() {
	BinaryReader reader(this->data.data(), this->data.size());
	if (globalObjectHeaders == nullptr) {
		return false;
	}
	int curHandle = 0;
	this->Objects.resize(globalObjectHeaders->size()); // init objects up front
	while (this->size > reader.Position()) {
		ObjectHeader* header = &globalObjectHeaders->at(curHandle);
		ObjectCommonItem& ObjectCom = this->Objects[curHandle++];

		//reader.Skip(4); // Skip unknown value (4 bytes)
		ObjectCom.DecompSize = reader.ReadInt32();
		ObjectCom.Size = reader.ReadInt32();
		ObjectCom.Type = header;
		ObjectCom.Flags = 2;
		if (!DECOMPRESS_COMOBJECTS) { // do not pre-decompress objects
			ObjectCom.Flags = 1;
			ObjectCom.raw = new uint8_t[ObjectCom.Size]; // allocate memory for raw data
			reader.ReadToMemory(ObjectCom.raw, ObjectCom.Size);
			continue;
		}

		unsigned char* cdata = new unsigned char[ObjectCom.Size]; // compressed data
		reader.ReadToMemory(cdata, ObjectCom.Size);
		if (header->Type == 0) {
			uint8_t* rawData = Decompressor::DecompressBlockRaw(cdata, ObjectCom.Size, ObjectCom.DecompSize);
			ObjectCom.OCIObjectQuickBackdrop = new ObjectQuickBackdrop();
			// copy the raw data to OCIObjectQuickBackdrop using memcpy
			memcpy(ObjectCom.OCIObjectQuickBackdrop, rawData, QUICKBACKDROPSIZE);
			// Read the shape data from the raw data. skip the first QUICKBACKDROPSIZE bytes
			// than read the shape data from the rawData
			ObjectCom.OCIObjectQuickBackdrop->Shape.Read(rawData + QUICKBACKDROPSIZE, ObjectCom.DecompSize - (QUICKBACKDROPSIZE));
		}
		else {
			ObjectCom.raw = Decompressor::DecompressBlockRaw(cdata, ObjectCom.Size, ObjectCom.DecompSize);
		}
		/*
		* TODO: remove this code, it is not needed anymore
		switch (header->Type) {
			case 0: // QuickBackdrop
				loadComObject<ObjectQuickBackdrop>(cdata, ObjectCom.Size, &ObjectCom.OCIObjectQuickBackdrop, ObjectCom.data);
				break;
			case 1: // Backdrop
				loadComObject<ObjectBackdrop>(cdata, ObjectCom.Size, &ObjectCom.OCIObjectBackdrop, ObjectCom.data);
				break;
			default: // Common
				loadComObject<ObjectCommon>(cdata, ObjectCom.Size, &ObjectCom.OCIObjectCommon, ObjectCom.data);
				break;
		}
		*/
		ObjectCom.Flags = 0;
		delete[] cdata;
	}
	this->FreeData();
	return true;
}


void ObjectProperties::Write(BinaryWriter& buffer, bool compress) {
	this->size = 0;
	this->WriteHeader(buffer);

	buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t _) {
		for (auto& object : this->Objects) {
			if (object.Flags == 2) {
				// Skip the uninitialized object
				continue;
			}

			uint8_t* rawData = nullptr;
			size_t compressSize = 0;
			int compressionResult = 0; // Result of compression, currently ignored

			ObjectHeader* header = object.Type;

			if (object.Flags == 1) { // The object is already compressed
				// Write the size of the object
				buffer.WriteInt32(object.DecompSize);
				buffer.WriteInt32(object.Size);
				buffer.WriteFromMemory(object.raw, object.Size);
				continue;
			}

			// The object is decompressed; we need to compress it (flag 0)
			// TODO: correctly serialize the QuickBackdrop
			// TODO: serialize Backdrop
			if (header->Type == 0) { // QuickBackdrop
				ObjectQuickBackdrop* quickBackdrop = object.OCIObjectQuickBackdrop;
				size_t sizeToCompress = QUICKBACKDROPSIZE + quickBackdrop->Shape.CalcDynamicSize() + 10;
				uint8_t* dataToCompress = new uint8_t[sizeToCompress];

				// Copy quickBackdrop data and write shape data
				memcpy(dataToCompress, quickBackdrop, QUICKBACKDROPSIZE);
				quickBackdrop->Shape.Write(dataToCompress + QUICKBACKDROPSIZE, quickBackdrop->Shape.CalcDynamicSize());
				// Fill the rest of the data with zeros because for some reason the game expects 10 more bytes ?
				memset(dataToCompress + QUICKBACKDROPSIZE + quickBackdrop->Shape.CalcDynamicSize(), 0, 10);

				// Compress the data
				rawData = Decompressor::CompressZlibRaw(dataToCompress, sizeToCompress, compressSize, compressionResult);
				buffer.WriteInt32(sizeToCompress); // Write the decompressed size
				delete[] dataToCompress;
			}
			else {
				// Compress the raw object data
				rawData = Decompressor::CompressZlibRaw(object.raw, object.OCIObjectCommon->size, compressSize, compressionResult);
				buffer.WriteInt32(object.OCIObjectCommon->size); // Write the decompressed size
			}

			// Write the compressed size and data
			buffer.WriteInt32(compressSize);
			buffer.WriteFromMemory(rawData, compressSize);
			delete[] rawData;
		}
		});
}

void ObjectProperties::Write(BinaryWriter& buffer, bool compress, OffsetsVector& offset) {
	this->size = 0;
	this->WriteHeader(buffer);

	buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t ChunkPosition) {
		for (auto& object : this->Objects) {
			ObjectHeader* header = object.Type;
			if (object.Flags == 2) { // we should never reach this point, may as well throw an error
				// Skip the uninitialized object
				offset[header->Handle] = 0; // is adding the additional OFFSET_ADDTION needed?
				continue;
			}

			offset[header->Handle] = buffer.Position() - ChunkPosition; // is adding the additional OFFSET_ADDTION needed?

			uint8_t* rawData = nullptr;
			size_t compressSize = 0;
			int compressionResult = 0; // Result of compression, currently ignored


			if (object.Flags == 1) { // The object is already compressed
				// Write the size of the object
				buffer.WriteInt32(object.DecompSize);
				buffer.WriteInt32(object.Size);
				buffer.WriteFromMemory(object.raw, object.Size);
				continue;
			}

			// The object is decompressed; we need to compress it (flag 0)
			// TODO: correctly serialize the QuickBackdrop
			// TODO: serialize Backdrop
			if (header->Type == 0) { // QuickBackdrop
				ObjectQuickBackdrop* quickBackdrop = object.OCIObjectQuickBackdrop;
				size_t sizeToCompress = QUICKBACKDROPSIZE + quickBackdrop->Shape.CalcDynamicSize() + 10;
				uint8_t* dataToCompress = new uint8_t[sizeToCompress];

				// Copy quickBackdrop data and write shape data
				memcpy(dataToCompress, quickBackdrop, QUICKBACKDROPSIZE);
				quickBackdrop->Shape.Write(dataToCompress + QUICKBACKDROPSIZE, quickBackdrop->Shape.CalcDynamicSize());
				// Fill the rest of the data with zeros because for some reason the game expects 10 more bytes ?
				memset(dataToCompress + QUICKBACKDROPSIZE + quickBackdrop->Shape.CalcDynamicSize(), 0, 10);

				// Compress the data
				rawData = Decompressor::CompressZlibRaw(dataToCompress, sizeToCompress, compressSize, compressionResult);
				buffer.WriteInt32(sizeToCompress); // Write the decompressed size
				delete[] dataToCompress;
			}
			else {
				// Compress the raw object data
				rawData = Decompressor::CompressZlibRaw(object.raw, object.OCIObjectCommon->size, compressSize, compressionResult);
				buffer.WriteInt32(object.OCIObjectCommon->size); // Write the decompressed size
			}

			// Write the compressed size and data
			buffer.WriteInt32(compressSize);
			buffer.WriteFromMemory(rawData, compressSize);
			delete[] rawData;
		}
	});
}