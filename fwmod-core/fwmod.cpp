#include "fwmod.h"

extern "C" {
	DLLCALL Logger* _cdecl CreateLogger() {
		return new Logger();
	}

	DLLCALL EventManager<std::vector<Chunk*>&, BinaryReader&, __int64&>* _cdecl GetGlobalDispatcher() {
		return &PluginsEventManager;
	}

	DLLCALL std::vector<ObjectHeader>*& _cdecl GetGlobalObjectHeaders() {
		return globalObjectHeaders;
	}
}