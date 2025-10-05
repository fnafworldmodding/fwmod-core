#include "BinaryIO.h"
#include <algorithm>
#include <cstring>
#include <stdexcept>

// NOTE: should we eh remove std errors?

BinaryIO::BinaryIO() : position_(0)
{
}

BinaryIO::BinaryIO(const std::vector<uint8_t>& initialData) 
    : data_(initialData), position_(0)
{
}

BinaryIO::BinaryIO(std::span<const uint8_t> initialData)
    : data_(initialData.begin(), initialData.end()), position_(0)
{
}

BinaryIO::BinaryIO(const uint8_t* data, size_t size)
    : data_(data, data + size), position_(0)
{
}

BinaryIO::BinaryIO(uint8_t* data, size_t size)
    : data_(data, data + size), position_(0)
{
}

BinaryIO::BinaryIO(size_t size, bool reserve) : data_(), position_(0)
{
    if (reserve) {
        data_.reserve(size);
        return;
    }
    data_.resize(size);
}

// === READ METHODS ===

uint8_t BinaryIO::ReadUint8()
{
    uint8_t value;
    ReadBytes(&value, 1);
    return value;
}

uint16_t BinaryIO::ReadUint16()
{
    uint16_t value;
    ReadBytes(&value, 2);
    return value;
}

uint32_t BinaryIO::ReadUint32()
{
    uint32_t value;
    ReadBytes(&value, 4);
    return value;
}

uint64_t BinaryIO::ReadUint64()
{
    uint64_t value;
    ReadBytes(&value, 8);
    return value;
}

int8_t BinaryIO::ReadInt8()
{
    return static_cast<int8_t>(ReadUint8());
}

int16_t BinaryIO::ReadInt16()
{
    return static_cast<int16_t>(ReadUint16());
}

int32_t BinaryIO::ReadInt32()
{
    return static_cast<int32_t>(ReadUint32());
}

int64_t BinaryIO::ReadInt64()
{
    return static_cast<int64_t>(ReadUint64());
}

char BinaryIO::ReadChar()
{
    return static_cast<char>(ReadUint8());
}

wchar_t BinaryIO::ReadCharWide()
{
    if (position_ + 1 >= data_.size())
        throw std::runtime_error("BinaryIO: Read beyond end of data");
    
    wchar_t value = *reinterpret_cast<const wchar_t*>(&data_[position_]);
    position_ += 2;
    return value;
}

std::string BinaryIO::ReadNullTerminatedString()
{
    // TODO: optimize using memchr
    std::string result;
    while (position_ < data_.size() && data_[position_] != '\0')
    {
        result += static_cast<char>(data_[position_]);
        position_++;
    }
    if (position_ < data_.size())
        position_++; // Skip null terminator
    return result;
}

std::string BinaryIO::ReadFixedLengthString(size_t length)
{
    if (position_ + length > data_.size())
        throw std::runtime_error("BinaryIO: Read beyond end of data");
    
    std::string result(reinterpret_cast<const char*>(&data_[position_]), length);
    position_ += length;
    return result;
}

std::wstring BinaryIO::ReadNullTerminatedStringWide()
{
    std::wstring result;
    while (position_ + 1 < data_.size())
    {
        wchar_t ch = *reinterpret_cast<const wchar_t*>(&data_[position_]);
        if (ch == L'\0')
        {
            position_ += 2;
            break;
        }
        result += ch;
        position_ += 2;
    }
    return result;
}

std::wstring BinaryIO::ReadFixedLengthStringWide(size_t length)
{
    if (position_ + length * 2 > data_.size())
        throw std::runtime_error("BinaryIO: Read beyond end of data");
    
    std::wstring result;
    for (size_t i = 0; i < length; i++)
    {
        wchar_t ch = *reinterpret_cast<const wchar_t*>(&data_[position_]);
        result += ch;
        position_ += 2;
    }
    return result;
}

std::vector<std::string> BinaryIO::ReadSizedStringList(size_t listSize)
{
    std::vector<std::string> stringList;
    if (listSize == 0)
        return stringList;

    size_t startPos = position_;
    while (position_ - startPos < listSize)
    {
        stringList.push_back(ReadNullTerminatedString());
        while (position_ - startPos < listSize)
        {
            if (position_ < data_.size() && data_[position_] == '\0')
                position_++;
            else
                break;
        }
    }
    return stringList;
}

char BinaryIO::PeekChar()
{
    if (position_ >= data_.size())
        throw std::runtime_error("BinaryIO: Peek beyond end of data");
    return static_cast<char>(data_[position_]);
}

uint32_t BinaryIO::PeekUint32()
{
    if (position_ + 3 >= data_.size())
        throw std::runtime_error("BinaryIO: Peek beyond end of data");
    return *reinterpret_cast<const uint32_t*>(&data_[position_]);
}

wchar_t BinaryIO::PeekCharWide()
{
    if (position_ + 1 >= data_.size())
        throw std::runtime_error("BinaryIO: Peek beyond end of data");
    return *reinterpret_cast<const wchar_t*>(&data_[position_]);
}

bool BinaryIO::HasMemory(int size)
{
    return position_ + size <= data_.size();
}

float BinaryIO::ReadFloat()
{
    if (position_ + 3 >= data_.size())
        throw std::runtime_error("BinaryIO: Read beyond end of data");
    
    float value = *reinterpret_cast<const float*>(&data_[position_]);
    position_ += 4;
    return value;
}

double BinaryIO::ReadDouble()
{
    if (position_ + 7 >= data_.size())
        throw std::runtime_error("BinaryIO: Read beyond end of data");
    
    double value = *reinterpret_cast<const double*>(&data_[position_]);
    position_ += 8;
    return value;
}

void BinaryIO::ReadToMemory(void* destination, size_t size)
{
    ReadBytes(destination, size);
}

// === WRITE METHODS ===

void BinaryIO::WriteUint8(uint8_t value)
{
    Write(value);
}

void BinaryIO::WriteUint16(uint16_t value)
{
    Write(value);
}

void BinaryIO::WriteUint32(uint32_t value)
{
    Write(value);
}

void BinaryIO::WriteUint64(uint64_t value)
{
    Write(value);
}

void BinaryIO::WriteInt8(int8_t value)
{
    Write(value);
}

void BinaryIO::WriteInt16(int16_t value)
{
    Write(value);
}

void BinaryIO::WriteInt32(int32_t value)
{
    Write(value);
}

void BinaryIO::WriteInt64(int64_t value)
{
    Write(value);
}

void BinaryIO::WriteChar(char value)
{
    Write(value);
}

void BinaryIO::WriteWChar(wchar_t value)
{
    Write(value);
}

void BinaryIO::WriteNullTerminatedString(const std::string& value)
{
    WriteFromMemory(value.data(), value.size());
    WriteChar('\0');
}

void BinaryIO::WriteNullTerminatedStringWide(const std::wstring& value)
{
    WriteFromMemory(value.data(), value.size() * sizeof(wchar_t));
    WriteWChar(L'\0');
}

void BinaryIO::WriteFixedLengthString(const std::string& value)
{
    WriteFromMemory(value.data(), value.size());
}

void BinaryIO::WriteFloat(float value)
{
    Write(value);
}

void BinaryIO::WriteDouble(double value)
{
    Write(value);
}

void BinaryIO::WriteFromMemory(const void* source, size_t size)
{    
    if (insertMode) {
        data_.insert(data_.begin() + position_, (uint8_t*)(source), (uint8_t*)(source)+size);
    } else {
        EnsureCapacity(position_ + size);
        std::memcpy(&data_[position_], source, size);
    }
    position_ += size;
}

void BinaryIO::WriteDataWithDynamicSize(const std::function<void(BinaryIO&, size_t)>& writeFunction)
{
    size_t initialPosition = position_;
    writeFunction(*this, initialPosition);
    size_t finalSize = position_ - initialPosition;
    size_t sizePosition = initialPosition - sizeof(int32_t);
    
    SeekBeg(sizePosition);
    WriteInt32(static_cast<int32_t>(finalSize));
    SeekCur(finalSize);
}

void BinaryIO::WriteNullBytes(size_t bytesToWrite)
{
    EnsureCapacity(position_ + bytesToWrite);
    std::memset(&data_[position_], 0, bytesToWrite);
    position_ += bytesToWrite;
}

// === STREAM CONTROL ===

void BinaryIO::SeekBeg(size_t absoluteOffset)
{
    if (absoluteOffset > data_.size())
        throw std::runtime_error("BinaryIO: Seek beyond end of data");
    position_ = absoluteOffset;
}

void BinaryIO::SeekCur(size_t relativeOffset)
{
    if (position_ + relativeOffset > data_.size()) {
        position_ = data_.size();
        return;
    }

    position_ += relativeOffset;
}

void BinaryIO::SeekReverse(size_t relativeOffset)
{
    if (relativeOffset > position_) {
        position_ = 0; 
        return;
    }
    position_ -= relativeOffset;
}

void BinaryIO::Skip(size_t bytesToSkip)
{
    SeekCur(bytesToSkip);
}

size_t BinaryIO::Align(size_t alignmentValue)
{
    size_t remainder = position_ % alignmentValue;
    size_t paddingSize = remainder > 0 ? alignmentValue - remainder : 0;
    Skip(paddingSize);
    return paddingSize;
}

// === POSITION AND SIZE ===

size_t BinaryIO::Position() const
{
    return position_;
}

size_t BinaryIO::Length() const
{
    return data_.size();
}

// === RAW DATA ACCESS ===

uint8_t* BinaryIO::Data()
{
    return data_.data();
}

std::span<const uint8_t> BinaryIO::GetSpan() const
{
    return std::span<const uint8_t>(data_);
}

std::span<uint8_t> BinaryIO::GetSpan()
{
    return std::span<uint8_t>(data_);
}

std::vector<uint8_t>& BinaryIO::GetVector()
{
    return data_;
}

std::vector<uint8_t> BinaryIO::Release()
{
    // reset the position as the buffer is being taken away
    position_ = 0;
    // move the vector out, leaving the internal data_ in a valid but empty state.
    return std::move(data_);
}

void BinaryIO::Resize(size_t newSize)
{
    data_.resize(newSize);
    if (position_ > newSize)
        position_ = newSize;
}

void BinaryIO::Reserve(size_t capacity)
{
    data_.reserve(capacity);
}

void BinaryIO::Clear()
{
    data_.clear();
    position_ = 0;
}

bool BinaryIO::Empty() const
{
    return data_.empty();
}

// === PRIVATE METHODS ===

void BinaryIO::EnsureCapacity(size_t requiredSize)
{
    if (requiredSize > data_.size())
    {
        data_.resize(requiredSize);
    }
}

void BinaryIO::ReadBytes(void* destination, size_t size)
{
    if (position_ + size > data_.size())
        throw std::runtime_error("BinaryIO: Read beyond end of data");
    
    std::memcpy(destination, &data_[position_], size);
    position_ += size;
}

void BinaryIO::WriteBytes(const void* source, size_t size)
{
    EnsureCapacity(position_ + size);
    std::memcpy(&data_[position_], source, size);
    position_ += size;
}

void BinaryIO::SetMode(bool mode) {
    insertMode = mode;
}

bool BinaryIO::GetMode() const {
    return insertMode;
}