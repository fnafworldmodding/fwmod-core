#pragma once
#ifndef FWGLOBALS_H
#define FWGLOBALS_H
#pragma once

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
extern Logger CoreLogger;
extern EventManager<std::vector<Chunk*>&, BinaryReader&, __int64&> PluginsEventManager;

#endif // !FWGLOBALS_H
