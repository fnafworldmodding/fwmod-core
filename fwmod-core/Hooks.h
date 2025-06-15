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

bool InstallInlineHook(void* func2hook, void* payloadFunction);
bool InstallRawHook(void* func2hook, std::vector<uint8_t> payload, int NOPsCount = 0);
bool InstallEdiHook(void* func2hook, void* payloadFunction);
void InitializeHooks();

#endif // !FWHOOKS_H
