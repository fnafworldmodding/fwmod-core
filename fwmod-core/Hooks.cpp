#include "Globals.h"
#include "Hooks.h"
#include "Preload.h"

constexpr auto TARGET_OPENDAT_OFFSET = 0xB9EC;
constexpr auto TARGET_LOADEXT_OFFSET = 0x4BF12;
#pragma warning(push)
#pragma warning(disable : 4297)

extern "C" static
HANDLE WINAPI CreateFileWHook(
    LPCWSTR               lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile
) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::Status gdiplusStatus = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    if (gdiplusStatus != Gdiplus::Ok) {
        throw std::runtime_error("Failed to initialize GDI+");
    }
    try {
        StartPreloadProcess();
    }
    catch (const std::exception& e) {
        CoreLogger.Error(e.what());
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    // forwards all the received parameters to the actual CreateFileW function
    return ::CreateFileW(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile
    );
}
#pragma warning(pop)

bool InstallInlineHook(void* func2hook, void* payloadFunction)
{
    // TODO: use InstallRawHook?
    constexpr size_t INSTRUCTION_LENGTH = 6;
    if (!func2hook || !payloadFunction)
    {
        CoreLogger.Error("InstallHook: Invalid parameters.");
        return 0;
    }

    DWORD oldProtect;
    if (!VirtualProtect(
        func2hook,
        INSTRUCTION_LENGTH, // Change protection for exactly 6 bytes
        PAGE_EXECUTE_READWRITE,
        &oldProtect
    ))
    {
        CoreLogger.Error("InstallHook: VirtualProtect failed. Error: " + std::to_string(GetLastError()));
        return 0;
    }

    // Construct the 5-byte relative CALL instruction
    PBYTE pInstruction = (PBYTE)func2hook;
    pInstruction[0] = 0xE8; // CALL opcode (changed from 0xE9 (JMP))

    // Calculate the 4-byte relative offset
    // offset = TargetAddress - (CurrentInstructionAddress + LengthOfCallInstruction)
    *(DWORD*)(pInstruction + 1) = (DWORD)((ULONG_PTR)payloadFunction - ((ULONG_PTR)pInstruction + 5));

    // NOP out the 6th byte.
    // The original instruction is 6 bytes. Our CALL is 5 bytes.
    // We fill the remaining byte with a NOP (No Operation) to avoid partial instructions.
    pInstruction[5] = 0x90; // NOP opcode

    // Restore original memory protection
    if (!VirtualProtect(
        func2hook,
        INSTRUCTION_LENGTH,
        oldProtect,
        &oldProtect
    ))
    {
        CoreLogger.Error("InstallHook: VirtualProtect failed to restore protection. Error: " + std::to_string(GetLastError()));
    }

    // Flush instruction cache to ensure the CPU sees the new instructions
    FlushInstructionCache(GetCurrentProcess(), func2hook, INSTRUCTION_LENGTH);

    CoreLogger.Error("Minimal inline hook installed successfully at " + std::format("0x{:X}", (ULONG_PTR)func2hook));
    return 1;
}


bool InstallRawHook(void* func2hook, std::vector<uint8_t> payload, int NOPsCount) {
    // TODO: make use of NOPsCount, it is supposed to be something of overwrite x nops than write payload
    if (!func2hook || payload.empty())
    {
        CoreLogger.Error("InstallHook: Invalid parameters.");
        return 0;
    }

    DWORD oldProtect;
    if (!VirtualProtect(
        func2hook,
        payload.size(), // Change protection for exactly 6 bytes
        PAGE_EXECUTE_READWRITE,
        &oldProtect
    ))
    {
        CoreLogger.Error("InstallHook: VirtualProtect failed. Error: " + std::to_string(GetLastError()));
        return 0;
    }


    // Install hook
    memcpy(func2hook, payload.data(), payload.size());

    // Restore original memory protection
    if (!VirtualProtect(
        func2hook,
        payload.size(),
        oldProtect,
        &oldProtect
    ))
    {
        CoreLogger.Error("InstallHook: VirtualProtect failed to restore protection. Error: " + std::to_string(GetLastError()));
    }

    // Flush instruction cache to ensure the CPU sees the new instructions
    FlushInstructionCache(GetCurrentProcess(), func2hook, payload.size());

    CoreLogger.Error("Minimal inline hook installed successfully at " + std::format("0x{:X}", (ULONG_PTR)func2hook));
    return 1;
}

extern "C" static
HMODULE WINAPI LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    // TODO: implement
    return LoadLibraryExW(lpLibFileName, hFile, dwFlags);
}


bool InstallEdiHook(void* func2hook, void* payloadFunction) {
    const uint8_t* offsetBytes = reinterpret_cast<const uint8_t*>(&payloadFunction);
    std::vector<uint8_t> vInstruction = {
        0xBF,                      // Opcode for MOV EDI, imm32
        offsetBytes[0],            // 32-bit address
        offsetBytes[1],
        offsetBytes[2],
        offsetBytes[3],
        0x90
    };
    return InstallRawHook(func2hook, vInstruction);
}



void InitializeHooks() {
    PBYTE pModuleBase = (PBYTE)GetModuleHandle(NULL); // Get base address of the main executable
    PVOID pTargetOpenDatCallAddress = pModuleBase + TARGET_OPENDAT_OFFSET;
    PVOID pTargetLoadExtCallAddress = pModuleBase + TARGET_LOADEXT_OFFSET;
    PVOID pTargetLoadExtCallAddres2 = pModuleBase + TARGET_LOADEXT_OFFSET + 0x9a; // 0x9a offset to the other `mov edi,dword ptr ds:[<LoadLibraryExW>]`


    if (!InstallInlineHook(pTargetOpenDatCallAddress, (PVOID)CreateFileWHook)) {
        CoreLogger.Error("failed installing hook CreateFileWHook");
        return;
    }
    if (!InstallEdiHook(pTargetLoadExtCallAddress, (PVOID)LoadLibraryExWHook) || !InstallEdiHook(pTargetLoadExtCallAddres2, (PVOID)LoadLibraryExWHook)) {
        CoreLogger.Error("failed installing hook LoadLibraryExWHook");
        return;
    }
}