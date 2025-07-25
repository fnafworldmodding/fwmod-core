#pragma once
#ifndef FWGLOBALS_H
#define FWGLOBALS_H
#pragma once

//#define IMAGEOGORDER
#define XLUAEXT
#include "common.h"
#include "CCNParser\CCNPackage.h"
#include "CCNParser\Chunks\Chunks.h"
#include "EventManager.h"
#include "Logger.h"
#include <format>
// common included headers
#include "pch.h"

#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <direct.h> // For _getcwd on Windows
#include <tlhelp32.h>
#include <fstream>
#include <ctime>
#include <functional>


// globals variables
extern DLLCALL Logger CoreLogger;
extern DLLCALL EventManager<std::vector<Chunk*>&, BinaryReader&, __int64&> PluginsEventManager;

struct Config {
	// what to load/modify
	bool loadPlugins = false;
	/*
	bool images = false;
	bool objects = false;
	*/
};

extern DLLCALL Config FwmodConfig;

#endif // !FWGLOBALS_H
