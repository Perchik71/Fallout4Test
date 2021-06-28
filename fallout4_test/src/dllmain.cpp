#include "common.h"
#include "version_info.h"
#include "profiler_internal.h"

#include <filesystem>
#include <shellapi.h>

/*

This file is part of Fallout 4 Fixes source code.

*/


VOID FIXAPI Sys_DumpDisableBreakpoint(VOID);
VOID FIXAPI Sys_DumpEnableBreakpoint(VOID);
VOID FIXAPI MainFix_PatchFallout4CreationKit(VOID);
VOID FIXAPI MainFix_PatchFallout4Game(VOID);

/*
==================
Sys_ApplyPatches

The main function for code injection, called from dump.cpp
==================
*/
VOID FIXAPI Sys_ApplyPatches(VOID)
{
	// The EXE has been unpacked at this point
	strcpy_s(g_GitVersion, VER_CURRENT_COMMIT_ID);
	XUtil::SetThreadName(GetCurrentThreadId(), "Main Thread");

#if FALLOUT4_USE_VTUNE
	// Check if VTune is already active
	const char *libITTPath = getenv("INTEL_LIBITTNOTIFY64");

	if (!libITTPath)
		libITTPath = "libittnotify.dll";

	g_DllVTune = LoadLibraryA(libITTPath);

	__itt_domain_create("MemoryManager");
	ITT_AllocateCallback = __itt_heap_function_create("Allocate", "MemoryManager");
	ITT_ReallocateCallback = __itt_heap_function_create("Reallocate", "MemoryManager");
	ITT_FreeCallback = __itt_heap_function_create("Free", "MemoryManager");
#endif

	switch (g_LoadType)
	{
	case GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4:
		if (g_INI.GetBoolean("Voice", "bRunCK32ForLips", FALSE))
		{
			// To generate .lip files, need creationkit32.exe

			AssertMsg(std::filesystem::exists(std::filesystem::absolute(L"CreationKit32.exe").c_str()),
					  "To generate .lip files, need creationkit32.exe")

			ShellExecuteW(NULL, L"open", L"CreationKit32.exe", NULL, std::filesystem::absolute(L"").c_str(), SW_SHOW);
			exit(0);
		}
		else
			MainFix_PatchFallout4CreationKit();
		break;
	case GAME_EXECUTABLE_TYPE::GAME_FALLOUT4:
		MainFix_PatchFallout4Game();
		break;
	}
}

/*
==================
DllMain

==================
*/
BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
		// Force this dll to be loaded permanently
		HMODULE temp;
		g_hModule = hModule;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)hModule, &temp);

		// Then determine which exe is being loaded
		g_LoadType = GAME_EXECUTABLE_TYPE::UNKNOWN;

		CHAR modulePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandle(NULL), modulePath, MAX_PATH);

		CHAR executableName[MAX_PATH];
		_strlwr_s(modulePath);
		_splitpath_s(modulePath, NULL, 0, NULL, 0, executableName, ARRAYSIZE(executableName), NULL, 0);

		switch (CRC32(executableName))
		{
		case CRC32("fallout4launcher"):
			g_LoadType = GAME_EXECUTABLE_TYPE::LAUNCHER_FALLOUT4;
			break;

		case CRC32("fallout4"):
		case CRC32("fallout4_dump"):
			g_LoadType = GAME_EXECUTABLE_TYPE::GAME_FALLOUT4;
			break;

		case CRC32("creationkit"):
			g_LoadType = GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4;
			break;
		}

		// For now, skip everything except the game and CK
		switch (g_LoadType)
		{
		case GAME_EXECUTABLE_TYPE::GAME_FALLOUT4:
		case GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4:
#if FALLOUT4_CREATIONKIT_ONLY
			if (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4)
				return TRUE;
#else
			if (!g_INI.GetBoolean("Mode", "Extended", FALSE) && (g_LoadType != GAME_EXECUTABLE_TYPE::CREATIONKIT_FALLOUT4))
			{
				return TRUE;
			}
#endif

			Sys_DumpEnableBreakpoint();
			break;
		}
    }

    return TRUE;
}
