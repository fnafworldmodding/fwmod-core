#ifndef FWHOOKS_H
#define FWHOOKS_H
#pragma once 
#include "common.h"
#include "preload.h"
#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

extern "C" static
HANDLE WINAPI CreateFileWHook(
    LPCWSTR               lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile
);

DLLCALL bool InstallInlineHook(void* func2hook, void* payloadFunction, int NOPCOUNT);
DLLCALL bool InstallRawHook(void* func2hook, std::vector<uint8_t> payload, int NOPsCount = 0);
DLLCALL bool InstallEdiHook(void* func2hook, void* payloadFunction);
DLLCALL void InitializeHooks();

#endif // !FWHOOKS_H
