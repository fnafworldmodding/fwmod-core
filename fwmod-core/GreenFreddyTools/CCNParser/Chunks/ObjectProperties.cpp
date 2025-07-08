#include "ObjectProperties.h"
#include "ObjectHeaders.h"
#include "Utils/Decompressor.h"
#include <type_traits> // For std::is_same
#include <zlib.h>
constexpr size_t objectCommonSize = sizeof(ObjectCommon);

// TODO: expose this as an api?
template <typename T>
int loadComObject(const unsigned char* compressedData, size_t compressedSize,
	T* objectCommon, std::vector<char>& properties) {
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
		ObjectCom.unknown = reader.ReadInt32();
		ObjectCom.Size = reader.ReadInt32();
		ObjectCom.Type = header;
		ObjectCom.Flags = 2;
		if (!DECOMPRESS_COMOBJECTS) { // do not pre-decompress objects
			ObjectCom.Flags = 1;
			ObjectCom.data.resize(ObjectCom.Size); // save objects raw data in properties
			reader.ReadToMemory(ObjectCom.data.data(), ObjectCom.Size);
			continue;
		}

		unsigned char* cdata = new unsigned char[ObjectCom.Size]; // compressed data
		reader.ReadToMemory(cdata, ObjectCom.Size);
		// fixme: there is an extra int (4 bytes)? that I have no idea where they are coming from. 
		// check ObjectStructures specifically ObjectCommon. 
		// NOTE I did not add this extra value to the other object structs so expect them to be corrupted
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
		ObjectCom.Flags = 0;
	}
	this->FreeData();
	return true;
}


void ObjectProperties::Write(BinaryWriter& buffer, bool compress) {
	this->size = 0;
	this->WriteHeader(buffer);
	buffer.WriteDataWithDynamicSize([&](BinaryWriter& buffer, size_t _) {
		for (auto& object : this->Objects) {
			buffer.WriteInt32(object.unknown); // unknown value
			buffer.WriteInt32(object.Size); // Write the size of the object
			if (object.Flags == 1) { // the object is already compressed
				buffer.WriteFromMemory(object.data.data(), object.Size);
				continue;
			}
			ObjectHeader* header = object.Type;
			// TODO: compress again
			// write the object based on its type
			switch (header->Type) {
				case 0: // QuickBackdrop
					buffer.WriteFromMemory(&object.OCIObjectQuickBackdrop, sizeof(ObjectQuickBackdrop) - sizeof(ObjectShape)); // Write ObjectQuickBackdrop
					object.OCIObjectQuickBackdrop.Shape.Write(buffer); // Write the shape data
					break;
				case 1: // Backdrop
					buffer.WriteFromMemory(&object.OCIObjectBackdrop, sizeof(ObjectBackdrop)); // Write ObjectBackdrop
					break;
				default: // Common
					buffer.WriteFromMemory(&object.OCIObjectCommon, sizeof(ObjectCommon)); // Write ObjectCommon
					if (object.data.size() > 0) {
						buffer.WriteFromMemory(object.data.data(), object.data.size()); // Write Object properties data
					}
					break;
			}
			//object.Size = sizeof(ObjectCommon) + object.data.size();
			//buffer.WriteFromMemory(&object, sizeof(ObjectCommon)); // Write ObjectCommon
			//buffer.WriteFromMemory(object.data.data(), object.data.size()); // Write Object properties data
		}
	});
}
