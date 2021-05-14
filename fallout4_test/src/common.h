#pragma once

#include "../resource.h"
#include "config.h"

#include <windows.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <d3d11.h>
#include <d3d11_2.h>
#include <stdexcept>

// TBBMalloc
#include <tbb/scalable_allocator.h>

// Detours
#include <detours/Detours.h>


// INIReader

#if FALLOUT4_MINI_PROJECT
#include "INIReader.h"

extern mINIReader g_INI;
extern mINIReader g_INI_CK;
#else
#define INI_ALLOW_MULTILINE 0
#define INI_USE_STACK 0
#define INI_MAX_LINE 4096
#include "INIReader.h"

extern INIReader g_INI;
#endif // FALLOUT4_MINI_PROJECT

#include "UtfStr.h"
#include "xutil.h"
#include "profiler.h"
#include "patches/offsets.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

extern HINSTANCE g_hModule;

extern uintptr_t g_ModuleBase;
extern uintptr_t g_ModuleSize;

extern uintptr_t g_ScrapSize;
extern uintptr_t g_bhkMemSize;

extern uintptr_t g_CodeBase;	// .text or .textbss
extern uintptr_t g_CodeEnd;
extern uintptr_t g_RdataBase;	// .rdata
extern uintptr_t g_RdataEnd;
extern uintptr_t g_DataBase;	// .data
extern uintptr_t g_DataEnd;

enum class GAME_EXECUTABLE_TYPE
{
	UNKNOWN,

	LAUNCHER_FALLOUT4,
	GAME_FALLOUT4,
	CREATIONKIT_FALLOUT4,
};

extern GAME_EXECUTABLE_TYPE g_LoadType;
extern char g_GitVersion[64];

#define PatchIAT(detour, module, procname) Detours::IATHook(g_ModuleBase, (module), (procname), (uintptr_t)(detour));

#define UI_CUSTOM_MESSAGE	52000

#define FIXAPI __stdcall
#define PROPERTY(read_func, write_func) __declspec(property(get = read_func, put = write_func))
#define READ_PROPERTY(read_func) __declspec(property(get = read_func))