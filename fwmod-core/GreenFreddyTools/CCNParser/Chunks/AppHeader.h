#pragma once
#ifndef APPHEADER_H
#include "Chunks.h"

#include <array>
#include <cstdint>
#include "../../Utils/BitDict.h"
#include "BinaryTools/BinaryReader.h"

enum class Flags {
    HeadingWhenMaximized,
    HeadingDisabled,
    FitInside,
    MachineIndependentSpeed,
    ResizeDisplay,
    A, // skip flag
    MenuDisplayedDisabled,
    MenuBar,
    MaximizedOnBoot,
    MultiSamples,
    ChangeResolutionMode,
    AllowFullscreenSwitch,
    B, C, D, // skip 3 flags
    NotEdrt
};

enum class NewFlags {
    PlaySoundsOverFrames,
	A, // skip flag
    DontMuteOnLostFocus,
    NoMinimizeBox,
    NoMaximizeBox,
    NoThickFrame,
    DontCenterFrame,
    DontStopScreenSaver,
    DisableCloseButton,
    HiddenAtStart,
    EnableVisualThemes,
    VSync,
    RunWhenMinimized,
	B, // skip flag
    RunWhileResizing,
    Count // This can be used to get the number of new flags
};

enum class OtherFlags {
    DebuggerShortcuts,
	A, // skip flag
    DontShareSubAppData,
	B, C, // skip 2 flags
    IncludeExternalFiles,
    ShowDebugger,
	D, E, F, G, // skip 4 flags
    Direct3D9or11,
    Direct3D8or11,
};


template class DLLCALL BitDict<Flags, short>;
template class DLLCALL BitDict<NewFlags, short>;
template class DLLCALL BitDict<OtherFlags, short>;

class DLLCALL AppHeader : public Chunk {
public:
    int header = 0;
    BitDict<Flags, short> flags;
    BitDict<NewFlags, short> newFlags;
    BitDict<OtherFlags, short> otherFlags;

    int16_t graphicMode = 4;
    int16_t appWidth = 640;
    int16_t appHeight = 480;
    int32_t initScore = (0 + 1) * -1;
    int32_t initLives = (3 + 1) * -1;
    int16_t controlType[4]{};
    int16_t controlKeys[4][8]{};
    uint32_t borderColor{};
    int32_t frameCount{};
    int32_t frameRate = 60;
    int32_t windowMenu{};


    AppHeader() : Chunk() {
        id = static_cast<short>(ChunksIDs::AppHeader);
    }

    AppHeader(short id, short flag, int size) : Chunk(id, flag, size) {
        id = static_cast<short>(ChunksIDs::AppHeader);
    }

    AppHeader(short flag, int size) : Chunk(flag, size) {
        id = static_cast<short>(ChunksIDs::AppHeader);
    }

    virtual bool Init() override;
    virtual void Write(BinaryWriter& buffer, bool compress = false) override;

private: void InitDictFlags(short flags, short newflags, short otherflags) {
        this->flags.SetValue(flags);
        this->newFlags.SetValue(newflags);
        this->otherFlags.SetValue(otherflags);
        //this->otherFlags.SetFlag("ShowDebugger", true); sad that doesn't work
    }
};

#endif
