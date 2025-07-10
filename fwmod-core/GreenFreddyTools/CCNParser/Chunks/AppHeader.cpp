#include "AppHeader.h"
#include "../../Utils/Decompressor.h"
class Decompressor;

template class BitDict<Flags, short>;
template class BitDict<NewFlags, short>;
template class BitDict<OtherFlags, short>;


bool AppHeader::Init() {
    BinaryReader reader(this->data.data(), this->data.size());

    this->header = reader.ReadInt32();

    short flags = reader.ReadInt16();

    short newflags = reader.ReadInt16();

    this->graphicMode = reader.ReadInt16();

    short otherFlags = reader.ReadInt16();

    InitDictFlags(flags, newflags, otherFlags);

    this->appWidth = reader.ReadInt16();
    this->appHeight = reader.ReadInt16();

    this->initScore = reader.ReadInt32();
    this->initLives = reader.ReadInt32();


    for (int i = 0; i < 4; ++i)
        this->controlType[i] = reader.ReadInt16();

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j)
            this->controlKeys[i][j] = reader.ReadInt16();
    }

    this->borderColor = reader.ReadInt32(); // TODO: change type from int32 to a color struct
    this->frameCount = reader.ReadInt32();

    this->frameRate = reader.ReadInt32();
    this->windowMenu = reader.ReadInt32();

    return true;
}

void AppHeader::Write(BinaryWriter& writer, bool compress) {
    this->WriteHeader(writer);
    writer.WriteInt32(this->header);

    writer.WriteInt16(this->flags.Value());
    writer.WriteInt16(this->newFlags.Value());
    writer.WriteInt16(this->graphicMode);
    writer.WriteInt16(this->otherFlags.Value());
    writer.WriteInt16(this->appWidth);
    writer.WriteInt16(this->appHeight);
    writer.WriteInt32(this->initScore);
    writer.WriteInt32(this->initLives);

    for (int i = 0; i < 4; ++i)
        writer.WriteInt16(this->controlType[i]);

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 8; ++j)
            writer.WriteInt16(this->controlKeys[i][j]);

    writer.WriteInt32(this->borderColor);
    writer.WriteInt32(this->frameCount);

    writer.WriteInt32(this->frameRate);
    writer.WriteInt32(this->windowMenu);
}
