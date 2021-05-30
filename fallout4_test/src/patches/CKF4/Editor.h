#pragma once

#include "../../common.h"
#include "../TES/BSTArray.h"

extern BOOL bFogToggle;
extern BOOL bAllowPoolMessage;

struct z_stream_s
{
	LPCVOID next_in;
	uint32_t avail_in;
	uint32_t total_in;
	LPVOID next_out;
	uint32_t avail_out;
	uint32_t total_out;
	LPCSTR msg;
	struct internal_state *state;
};
static_assert_offset(z_stream_s, state, 0x28);

INT_PTR CALLBACK DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND WINAPI hk_CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam);
BOOL WINAPI hk_EndDialog(HWND hDlg, INT_PTR nResult);
LRESULT WINAPI hk_SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

INT32 FIXAPI hk_inflateInit(z_stream_s *Stream, LPCSTR Version, INT32 Mode);
INT32 FIXAPI hk_inflate(z_stream_s *Stream, INT32 Flush);

uint32_t FIXAPI sub_1405B31C0(BSTArray<LPVOID>& Array, LPCVOID &Target);
uint32_t FIXAPI sub_1405B31C0_SSE41(BSTArray<LPVOID>& Array, LPCVOID &Target);

BOOL FIXAPI OpenPluginSaveDialog(HWND ParentWindow, LPCSTR BasePath, BOOL IsESM, LPSTR Buffer, uint32_t BufferSize, LPCSTR Directory);
VOID FIXAPI UpdateObjectWindowTreeView(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown);
VOID FIXAPI UpdateCellViewCellList(LPVOID Thisptr, HWND ControlHandle, INT64 Unknown);
VOID FIXAPI UpdateCellViewObjectList(LPVOID Thisptr, HWND *ControlHandle);
VOID FIXAPI BeginUIDefer(VOID);
VOID FIXAPI EndUIDefer(VOID);
VOID FIXAPI InsertComboBoxItem(HWND ComboBoxHandle, LPCSTR DisplayText, LPVOID Value, BOOL AllowResize);
VOID FIXAPI InsertListViewItem(HWND ListViewHandle, LPVOID Parameter, BOOL UseImage, INT32 ItemIndex);

VOID FIXAPI QuitHandler(VOID);

VOID FIXAPI hk_call_140906407(INT64 a1, INT64 a2, INT64 a3);
BOOL FIXAPI hk_call_12E852C(HWND RichEditControl, LPCSTR Text);
VOID FIXAPI PatchTemplatedFormIterator(VOID);
VOID FIXAPI HiddenMovableStatic(BOOL Value);



/*
==================
PatchSky

Fixes the display of fog in the render. 
The bFogEnabled option must be TRUE otherwise there will be a crash.
This option is set to TRUE by default, but you can override.
The patch sets its variable to bypass.
==================
*/
VOID FIXAPI PatchSky(VOID);


/*
==================
PatchLip

The developers have cut out all the creation functionality .lip files.
But this patch will make the button available.
==================
*/
VOID FIXAPI PatchLip(VOID);


/*
==================
PatchMessage

Most often, CK freezes when loading something large. 
But otherwise it works quite well. 
I limit the impact of the message processing patch by setting the permission flag.

I will give permission when loading with ProgressDialog enabled.
==================
*/
VOID FIXAPI PatchMessage(VOID);


/*
==================
hk_call_2511176

For some reason, an object whose address to the function is garbage gets here.

CONTEXT:  (.ecxr)
rax=000002db7f90f850 rbx=000000f780cfea40 rcx=000002db7f90ed10
rdx=000000f780cfea40 rsi=000002db299aa800 rdi=000002db7f90ed10
rip=00007ff77f381176 rsp=000000f780cfe9f0 rbp=000000f780cfeb80
 r8=0000000000000000  r9=0000000000000016 r10=000000000000000b
r11=000000f780cfea18 r12=0000000000000001 r13=00000000001506ec
r14=0000000000000000 r15=0000000000000000
iopl=0         nv up ei pl zr na po nc
cs=0033  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00010246
CreationKit+0x2511176:
00007ff7`7f381176 ff9090010000    call    qword ptr [rax+190h] ds:000002db`7f90f9e0=bd8a591342b6b52e

This happens when you close the application
==================
*/
VOID FIXAPI hk_call_2511176(LPVOID a1, LPVOID a2, LPVOID a3);

/*
==================
Fixed_IncorrectSmoothnessValueToMaterialNif

The program simply writes about an erroneous value and passes it into the calculation, 
the function will now correct the value from 0.0 to 1.0
==================
*/
FLOAT FIXAPI Fixed_IncorrectSmoothnessValueToMaterialNif(FLOAT a1, FLOAT a2);


/*
==================
Fixed_DeleteTintingRace

The function removes from lists without looping
==================
*/
VOID FIXAPI Fixed_DeleteTintingRace(INT64 count, INT64 item_id, HWND listview);

template<typename T, BOOL Stable = FALSE>
VOID FIXAPI ArrayQuickSortRecursive(BSTArray<T>& Array, INT32(*SortFunction)(LPCVOID, LPCVOID))
{
	auto compare = [SortFunction](const T& A, const T& B)
	{
		return SortFunction(A, B) == -1;
	};

	if constexpr (Stable)
		std::stable_sort(&Array[0], &Array[Array.QSize()], compare);
	else
		std::sort(&Array[0], &Array[Array.QSize()], compare);
}