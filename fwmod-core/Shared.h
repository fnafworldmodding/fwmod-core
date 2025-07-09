#pragma once
#ifndef FWMC_ISHARED_H_
#define FWMC_ISHARED_H_
#include "common.h"
#include <unordered_set>
#include <windows.h>

enum PluginFlags {
    None,
    UnloadDll = 1,
};

constexpr auto pluginsPath = "mods/_core/preload";

class DLLCALL IPlugin {
public:
    HMODULE hModule = 0;
public:
    virtual ~IPlugin() = default; // Important for correct destruction
    virtual void Initialize() = 0;
    virtual PluginFlags Shutdown() = 0;
    virtual const char* GetName() const = 0; // Return C-style string
};

extern DLLCALL std::unordered_set<IPlugin*> Plugins;
void loadPlugins();
void unloadPlugins();

using CreatePluginFunc = IPlugin * (*)();


#endif // !FWMC_ISHARED_H_
