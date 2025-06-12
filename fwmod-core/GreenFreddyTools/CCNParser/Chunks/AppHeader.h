#pragma once
#ifndef APPHEADER_H
#include "Chunks.h"

#include <array>
#include <cstdint>
#include "../../Utils/BitDict.h"
#include "BinaryTools/BinaryReader.h"



static const std::vector<std::string> kFlags = {
    "HeadingWhenMaximized", "HeadingDisabled", "FitInside", "MachineIndependentSpeed",
    "ResizeDisplay", "", "MenuDisplayedDisabled", "MenuBar", "MaximizedOnBoot", "MultiSamples",
    "ChangeResolutionMode", "AllowFullscreenSwitch", "", "", "", "NotEdrt"
};

static const std::vector<std::string> kNewFlags = {
    "PlaySoundsOverFrames", "", "DontMuteOnLostFocus", "NoMinimizeBox", "NoMaximizeBox",
    "NoThickFrame", "DontCenterFrame", "DontStopScreenSaver", "DisableCloseButton", "HiddenAtStart",
    "EnableVisualThemes", "VSync", "RunWhenMinimized", "", "RunWhileResizing"
};


static const std::vector<std::string> kOtherFlags = {
    "DebuggerShortcuts", "", "DontShareSubAppData", "", "", "IncludeExternalFiles", "ShowDebugger",
    "", "", "", "", "Direct3D9or11", "Direct3D8or11"
};


class AppHeader : public Chunk {
public:
    int header = 0;
    BitDict<short> flags;
    BitDict<short> newFlags;
    BitDict<short> otherFlags;

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
        this->flags.SetKeys(kFlags);
        this->flags.SetValue(flags);
        this->newFlags.SetKeys(kNewFlags);
        this->newFlags.SetValue(newflags);
        this->otherFlags.SetKeys(kOtherFlags);
        this->otherFlags.SetValue(otherflags);
        //this->otherFlags.SetFlag("ShowDebugger", true); sad that doesn't work
    }
};

#endif
