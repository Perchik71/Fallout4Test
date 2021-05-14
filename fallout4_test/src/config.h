#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#define FALLOUT4_CREATIONKIT_ONLY	0	// Only build code related to the Creation Kit hooks
#define FALLOUT4_GENERATE_OFFSETS	0	// Dump offset list to disk in codegen.cpp
#define FALLOUT4_USE_VTUNE			0	// Enable VTune instrumentation API
#define FALLOUT4_USE_VFS			0	// Enable virtual file system
#define FALLOUT4_USE_PROFILER		0	// Enable built-in profiler macros / "profiler.h"
#define FALLOUT4_USE_PAGE_HEAP		0	// Treat every memory allocation as a separate page (4096 bytes) for debugging
#define FALLOUT4_LAZ_UNICODE_PLUGIN 1	// Support unicode string (utf-8) convert to ansi and back
#define FALLOUT4_STUDY_CK64_INIFILE 0	// Output of variables from the ini file and their values to the log
#define FALLOUT4_MINI_PROJECT		1	// uses https://github.com/pulzed/mINI