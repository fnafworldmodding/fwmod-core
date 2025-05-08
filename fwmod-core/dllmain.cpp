#include "pch.h"

#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <mutex>
using namespace std;
// ccn reader and writer should be implmented today, look at notes to know what todo

static unordered_map < string, FARPROC > functionCache;
HMODULE realDLL = nullptr;
mutex dllMutex;

static void ShowLastError(const char* context) {
    DWORD errorCode = GetLastError();
    LPSTR messageBuffer = nullptr;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorCode,
        0, // Default language    
        (LPSTR)&messageBuffer,
        0,
        nullptr
    );

    ostringstream errorMsg;
    errorMsg << context;
    errorMsg << " failed with error ";
    errorMsg << errorCode;
    if (messageBuffer) {
        errorMsg << ": ";
        errorMsg << messageBuffer;
        LocalFree(messageBuffer);
    }

    MessageBoxA(nullptr, errorMsg.str().c_str(), "Error", MB_OK | MB_ICONERROR);
}

static void LoadOriginalDLL(const char* dllName) {
    lock_guard < mutex > lock(dllMutex);
    realDLL = LoadLibraryA(dllName);
    if (!realDLL) {
        ShowLastError("LoadLibraryA()");
        ExitProcess(1);
    }
}

static FARPROC ResolveExport(const char* functionName) {
    lock_guard < mutex > lock(dllMutex);
    if (!realDLL) return nullptr;

    auto dosHeader = (PIMAGE_DOS_HEADER)realDLL;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)realDLL + dosHeader->e_lfanew);
    auto exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)realDLL + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    auto names = (DWORD*)((BYTE*)realDLL + exportDirectory->AddressOfNames);
    auto functions = (DWORD*)((BYTE*)realDLL + exportDirectory->AddressOfFunctions);
    auto ordinals = (WORD*)((BYTE*)realDLL + exportDirectory->AddressOfNameOrdinals);

    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        string currentFunctionName = (char*)((BYTE*)realDLL + names[i]);
        if (currentFunctionName == functionName) {
            DWORD functionRVA = functions[ordinals[i]];
            return (FARPROC)((BYTE*)realDLL + functionRVA);
        }
    }

    return nullptr;
}

extern "C"
static FARPROC __stdcall DelayLoadHandler(const char* functionName) {
    lock_guard < mutex > lock(dllMutex);

    auto cacheIt = functionCache.find(functionName);
    if (cacheIt != functionCache.end()) {
        return cacheIt->second;
    }

    FARPROC originalFunction = ResolveExport(functionName);
    if (!originalFunction) {
        MessageBoxA(nullptr, ("Function not found: " + string(functionName)).c_str(), "Error", MB_OK | MB_ICONERROR);
        ExitProcess(1);
    }

    functionCache[functionName] = originalFunction;
    return originalFunction;
}

static BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        LoadOriginalDLL("og_mmf2d3d11.dll");
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        if (realDLL) {
            FreeLibrary(realDLL);
            realDLL = nullptr;
        }
    }
    else if (ul_reason_for_call == DLL_THREAD_ATTACH || ul_reason_for_call == DLL_THREAD_DETACH) {
        // ...
    }
    return TRUE;
}