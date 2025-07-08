#pragma once
#ifndef FWMOD_EI_H
#define FWMOD_EI_H
#include "Common.h"
#include "Globals.h"
#include "Logger.h"
#include "EventManager.h"
#include "CCNParser/Chunks/ObjectHeaders.h"
// TODO: create a global ModStore
// extern std::vector<ObjectHeader>* globalObjectHeaders;
extern "C" {
	DLLCALL Logger* _cdecl CreateLogger();
	DLLCALL EventManager<std::vector<Chunk*>&, BinaryReader&, __int64&>* _cdecl GetGlobalDispatcher();
	DLLCALL EventManager<std::vector<Chunk*>&, BinaryReader&, __int64&>* _cdecl GetGlobalDispatcher();
	DLLCALL std::vector<ObjectHeader>*& _cdecl GetGlobalObjectHeaders();
	DLLCALL BinaryReader* _cdecl CreateBinaryReader(const char* path);
	DLLCALL BinaryWriter* _cdecl CreateBinaryWriter(const char* path, bool truncate = true);
	DLLCALL BinaryReader* _cdecl CreateBinaryReaderEx(char* buffer, uint32_t sizeInBytes);
	DLLCALL BinaryWriter* _cdecl CreateBinaryWriterEx(char* buffer, uint32_t sizeInBytes);
}

#endif
