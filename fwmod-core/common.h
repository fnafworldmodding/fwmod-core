#ifndef COMMON_H
#define COMMON_H
#pragma once

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

// (de)compressor for headers
#include <cstdint>  
#include <stdexcept>




// ctf runtime doesn't seem to ever have suspended threads, so this should be safe

inline static void SuspendAllThreadsExceptThis() {
    DWORD currentThreadId = GetCurrentThreadId();
    DWORD currentProcessId = GetCurrentProcessId();

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == currentProcessId &&
                threadEntry.th32ThreadID != currentThreadId) {

                HANDLE threadHandle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry.th32ThreadID);
                if (threadHandle != NULL) {
                    SuspendThread(threadHandle);
                    CloseHandle(threadHandle);
                }
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }

    CloseHandle(snapshot);
}

inline static void UnsuspendAllThreads() {
    DWORD currentProcessId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;
    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == currentProcessId) {
                HANDLE threadHandle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry.th32ThreadID);
                if (threadHandle != NULL) {
                    ResumeThread(threadHandle);
                    CloseHandle(threadHandle);
                }
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }
    CloseHandle(snapshot);
}

#endif // COMMON_H