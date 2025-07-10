#include "pch.h"


#include "Globals.h"
#include "Hooks.h"

// exported needed functions
#pragma comment(linker, "/export:timeBeginPeriod=WINMM.timeBeginPeriod")  
#pragma comment(linker, "/export:joyGetDevCapsW=WINMM.joyGetDevCapsW")  
#pragma comment(linker, "/export:joyGetPosEx=WINMM.joyGetPosEx")  
#pragma comment(linker, "/export:timeEndPeriod=WINMM.timeEndPeriod")

// TODO: move functionality to another file to lower the mess and size of this file

using namespace std;
static std::unordered_map<std::string, FARPROC> functionCache;
HMODULE realDLL = nullptr;
mutex dllMutex;

inline static void SetThreadPriority(std::thread& thread, DWORD priority) {
    HANDLE handle = OpenThread(THREAD_SET_INFORMATION, FALSE, GetThreadId(thread.native_handle()));
    if (handle != NULL) {
        SetThreadPriority(handle, priority);
        CloseHandle(handle);
    }
}


static void ShowLastError(const std::string& context) {
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

    std::string errorMsg = "";
    errorMsg += context;
    errorMsg += " failed with error ";
    errorMsg += std::to_string(errorCode);
    if (messageBuffer) {
        errorMsg += ": ";
        errorMsg += messageBuffer;
        LocalFree(messageBuffer);
    }

    MessageBoxA(nullptr, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
}

inline static void LoadOriginalDLL(const char* dllName) {
    lock_guard<mutex> lock(dllMutex);
    realDLL = LoadLibraryA(dllName);
    if (!realDLL) {
        ShowLastError("LoadLibraryA " + std::string(dllName));
        ExitProcess(1);
    }
}

static FARPROC ResolveExport(const char* functionName) {
    lock_guard<mutex> lock(dllMutex);
    if (!realDLL) return nullptr;

    auto dosHeader = (PIMAGE_DOS_HEADER)realDLL;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)realDLL + dosHeader->e_lfanew);
    auto exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)realDLL + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    auto names = (DWORD*)((BYTE*)realDLL + exportDirectory->AddressOfNames);
    auto functions = (DWORD*)((BYTE*)realDLL + exportDirectory->AddressOfFunctions);
    auto ordinals = (WORD*)((BYTE*)realDLL + exportDirectory->AddressOfNameOrdinals);

    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        std::string currentFunctionName = (char*)((BYTE*)realDLL + names[i]);
        if (currentFunctionName == functionName) {
            DWORD functionRVA = functions[ordinals[i]];
            return (FARPROC)((BYTE*)realDLL + functionRVA);
        }
    }

    return nullptr;
}

extern "C"
static FARPROC __stdcall DelayLoadHandler(const char* functionName) {
    lock_guard<mutex> lock(dllMutex);

    auto cacheIt = functionCache.find(functionName);
    if (cacheIt != functionCache.end()) {
        return cacheIt->second;
    }

    FARPROC originalFunction = ResolveExport(functionName);
    if (!originalFunction) {
        MessageBoxA(nullptr, ("Function not found: " + std::string(functionName)).c_str(), "Error", MB_OK | MB_ICONERROR);
        ExitProcess(1);
    }

    functionCache[functionName] = originalFunction;
    return originalFunction;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        LoadOriginalDLL("WINMM.dll");
#ifdef _DEBUG
        MessageBoxA(nullptr, "Hi, you can debug now!", "Message", MB_OK | MB_ICONINFORMATION);
#endif
        InitializeHooks();
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        if (realDLL) {
            FreeLibrary(realDLL);
            realDLL = nullptr;
        }
        // no need to uninitialize hooks because it meant to be called once anyways 
        // and bc this dll stay for the life time of the executable
    }
    else if (ul_reason_for_call == DLL_THREAD_ATTACH || ul_reason_for_call == DLL_THREAD_DETACH) {
        // ...
    }
    return TRUE;
}