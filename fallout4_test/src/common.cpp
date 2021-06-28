#include "common.h"
#include "version_info.h"

#include "..\resource.h"

#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "Version.lib")

#pragma comment(lib, "tbb.lib")					// Thread Building Blocks
#pragma comment(lib, "libzydis.lib")			// Zydis
#pragma comment(lib, "libmeshoptimizer.lib")	// Meshoptimizer
#pragma comment(lib, "detours.lib")				// Detours

#if SKYRIM64_USE_VTUNE
#pragma comment(lib, "libittnotify.lib")		// Intel Threading Tools

__itt_heap_function ITT_AllocateCallback;
__itt_heap_function ITT_ReallocateCallback;
__itt_heap_function ITT_FreeCallback;
#endif

#pragma comment(lib, "tbbmalloc.lib")			// TBB Allocator

HINSTANCE g_hModule;

#if FALLOUT4_MINI_PROJECT
mINIReader g_INI("fallout4_test.ini");
mINIReader g_INI_CK("CreationKitPrefs.ini");
#else
INIReader g_INI("fallout4_test.ini");
#endif // FALLOUT4_MINI_PROJECT

uintptr_t g_ModuleBase;
uintptr_t g_ModuleSize;

uintptr_t g_ScrapSize = 0x2000000;
uintptr_t g_bhkMemSize = 0x2000000;

uintptr_t g_CodeBase;	// .text or .textbss
uintptr_t g_CodeEnd;
uintptr_t g_RdataBase;	// .rdata
uintptr_t g_RdataEnd;
uintptr_t g_DataBase;	// .data
uintptr_t g_DataEnd;

bool g_UIEnabled = false;
int8_t g_i8DialogMode = 0;

GAME_EXECUTABLE_TYPE g_LoadType;
char g_GitVersion[64] = VER_CURRENT_COMMIT_ID;