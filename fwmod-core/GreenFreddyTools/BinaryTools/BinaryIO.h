#pragma once
#include <vector>
#include <string>
#include <span>
#include <memory>
#include <functional>
#include "../common.h"

// class that provides both read and write functionality on a memory buffer
// similar to python's BytesIO - can read, write, and access raw content
class DLLCALL BinaryIO
{
public:
    // Create empty BinaryIO
    BinaryIO();
    // Create BinaryIO with initial data
    BinaryIO(const std::vector<uint8_t>& initialData);
    // Create BinaryIO with initial data from span
    BinaryIO(std::span<const uint8_t> initialData);
    // Create BinaryIO with initial data from raw pointer
    BinaryIO(const uint8_t* data, size_t size);
    // either reserve or resize buffer
    BinaryIO(size_t size, bool reserve);
    // Destructor
    ~BinaryIO() = default;

    // === READ METHODS ===
    [[nodiscard]] uint8_t ReadUint8();
    [[nodiscard]] uint16_t ReadUint16();
    [[nodiscard]] uint32_t ReadUint32();
    [[nodiscard]] uint64_t ReadUint64();

    [[nodiscard]] int8_t ReadInt8();
    [[nodiscard]] int16_t ReadInt16();
    [[nodiscard]] int32_t ReadInt32();
    [[nodiscard]] int64_t ReadInt64();

    [[nodiscard]] char ReadChar();
    [[nodiscard]] wchar_t ReadCharWide();
    [[nodiscard]] std::string ReadNullTerminatedString();
    [[nodiscard]] std::string ReadFixedLengthString(size_t length);
    [[nodiscard]] std::wstring ReadNullTerminatedStringWide();
    [[nodiscard]] std::wstring ReadFixedLengthStringWide(size_t length);
    [[nodiscard]] std::vector<std::string> ReadSizedStringList(size_t listSize);
    [[nodiscard]] char PeekChar();
    [[nodiscard]] uint32_t PeekUint32();
    [[nodiscard]] wchar_t PeekCharWide();
    [[nodiscard]] bool HasMemory(int size);

    [[nodiscard]] float ReadFloat();
    [[nodiscard]] double ReadDouble();

    void ReadToMemory(void* destination, size_t size);

    // === WRITE METHODS ===
    void WriteUint8(uint8_t value);
    void WriteUint16(uint16_t value);
    void WriteUint32(uint32_t value);
    void WriteUint64(uint64_t value);

    void WriteInt8(int8_t value);
    void WriteInt16(int16_t value);
    void WriteInt32(int32_t value);
    void WriteInt64(int64_t value);

    void WriteChar(char value);
    void WriteWChar(wchar_t value);
    void WriteNullTerminatedString(const std::string& value);
    void WriteNullTerminatedStringWide(const std::wstring& value);
    void WriteFixedLengthString(const std::string& value);

    void WriteFloat(float value);
    void WriteDouble(double value);

    void WriteFromMemory(const void* data, size_t size);

    template<typename T>
    inline void Write(const T& data)
    {
        static_assert(!std::is_pointer<T>(), "BinaryIO::Write<T> requires T to be a non pointer type.");
        WriteFromMemory(&data, sizeof(T));
    }

    template<typename T>
    void WriteSpan(std::span<T> data)
    {
        WriteFromMemory(data.data(), data.size_bytes());
    }

    void WriteDataWithDynamicSize(const std::function<void(BinaryIO&, size_t)>& writeFunction);
    void WriteNullBytes(size_t bytesToWrite);

    // === STREAM CONTROL ===
    void SeekBeg(size_t absoluteOffset);
    void SeekCur(size_t relativeOffset);
    void SeekReverse(size_t relativeOffset);
    void Skip(size_t bytesToSkip);
    size_t Align(size_t alignmentValue = 2048);
    void Flush();

    // === POSITION AND SIZE ===
    size_t Position() const;
    size_t Length() const;

    // === RAW DATA ACCESS ===
    uint8_t* Data();
    std::span<const uint8_t> GetSpan() const;
    std::span<uint8_t> GetSpan();
    std::vector<uint8_t>& GetVector();

    // === UTILITY ===
    void Resize(size_t newSize);
    void Reserve(size_t capacity);
    void Clear();
    bool Empty() const;

private:
    std::vector<uint8_t> data_;
    size_t position_;
    bool insertMode = false;

    void EnsureCapacity(size_t requiredSize);
    
    void ReadBytes(void* destination, size_t size);
    void WriteBytes(const void* source, size_t size);
};
