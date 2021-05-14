#include "../../common.h"
#include <CommCtrl.h>
#include <math.h>

#include "EditorUI.h"
#include "UIThemeMode.h"
#include "LogWindow.h"
#include "MainWindow.h"

namespace EditorUI
{
	WNDPROC OldWndProc;
	DLGPROC OldResponseWindowProc;
	BOOL bReplaceTips = FALSE;

	static UINT64 timeInterval = 0;
	static UINT64 timeStartTick = 0;
	static UINT64 timeCurrentTick = 0;

	LRESULT FIXAPI hk_SetSettingsPartStatusBar(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// https://github.com/Nukem9/SkyrimSETest/commit/d1181a346ccfe6a655fa9da6544ee1b808f89d2f
		// Scale the status bar segments to fit the window size

		auto scale = [&](INT32 Width)
		{
			return static_cast<INT32>(Width * (LOWORD(lParam) / 1024.0f));
		};

		std::array<INT32, 4> spacing
		{
			scale(150),
			scale(300),
			scale(600),
			-1,
		};

		return SendMessageA(hWnd, SB_SETPARTS, spacing.size(), (LPARAM)spacing.data());
	}

	VOID FIXAPI hk_SpamFPSToStatusBar(INT64 a1)
	{
		timeCurrentTick = GetTickCount64();
		if ((timeCurrentTick - timeStartTick) >= timeInterval)
		{
			((VOID(__fastcall*)(INT64))OFFSET(0x45EBB0, 0))(a1);
			
			timeStartTick = GetTickCount64();
			timeCurrentTick = timeStartTick;
		}
	}

	VOID FIXAPI Initialize(VOID)
	{
		InitCommonControls();
		bReplaceTips = g_INI.GetBoolean("CreationKit", "UI", FALSE) && g_INI.GetBoolean("CreationKit", "ReplacingTipsWithProgressBar", FALSE);
		timeInterval = std::min(std::max(g_INI.GetInteger("CreationKit", "FPSSpamInterval", 1000), (INI_INT)42), (INI_INT)1000);
		timeStartTick = GetTickCount64();
		UITheme::InitializeThread();

		if (!LogWindow::Initialize())
			MessageBoxA(NULL, "Failed to create console log window", "Error", MB_ICONERROR);
	}

	BOOL FIXAPI ListViewCustomSetItemState(HWND ListViewHandle, WPARAM Index, UINT Data, UINT Mask)
	{
		// Microsoft's implementation of this define is broken (ListView_SetItemState)
		LVITEMA item = { 0 };
		item.mask = LVIF_STATE;
		item.state = Data;
		item.stateMask = Mask;

		return (BOOL)SendMessageA(ListViewHandle, LVM_SETITEMSTATE, Index, (LPARAM)&item);
	}

	VOID FIXAPI ListViewSelectItem(HWND ListViewHandle, INT32 ItemIndex, bool KeepOtherSelections)
	{
		if (!KeepOtherSelections)
			ListViewCustomSetItemState(ListViewHandle, -1, 0, LVIS_SELECTED);

		if (ItemIndex != -1)
		{
			ListView_EnsureVisible(ListViewHandle, ItemIndex, FALSE);
			ListViewCustomSetItemState(ListViewHandle, ItemIndex, LVIS_SELECTED, LVIS_SELECTED);
		}
	}

	VOID FIXAPI ListViewFindAndSelectItem(HWND ListViewHandle, LPVOID Parameter, bool KeepOtherSelections)
	{
		if (!KeepOtherSelections)
			ListViewCustomSetItemState(ListViewHandle, -1, 0, LVIS_SELECTED);

		LVFINDINFOA findInfo = { 0 };
		findInfo.flags = LVFI_PARAM;
		findInfo.lParam = (LPARAM)Parameter;

		int index = ListView_FindItem(ListViewHandle, -1, &findInfo);

		if (index != -1)
			ListViewSelectItem(ListViewHandle, index, KeepOtherSelections);
	}

	LPVOID FIXAPI ListViewGetSelectedItem(HWND ListViewHandle)
	{
		if (!ListViewHandle)
			return NULL;

		int index = ListView_GetNextItem(ListViewHandle, -1, LVNI_SELECTED);

		if (index == -1)
			return NULL;

		LVITEMA item = { 0 };
		item.mask = LVIF_PARAM;
		item.iItem = index;

		ListView_GetItem(ListViewHandle, &item);
		return (LPVOID)item.lParam;
	}

	VOID FIXAPI ListViewDeselectItem(HWND ListViewHandle, LPVOID Parameter)
	{
		LVFINDINFOA findInfo = { 0 };
		findInfo.flags = LVFI_PARAM;
		findInfo.lParam = (LPARAM)Parameter;

		int index = ListView_FindItem(ListViewHandle, -1, &findInfo);

		if (index != -1)
			ListViewCustomSetItemState(ListViewHandle, index, 0, LVIS_SELECTED);
	}

	VOID FIXAPI TabControlDeleteItem(HWND TabControlHandle, uint32_t TabIndex)
	{
		TCITEMA itemInfo = {};

		if (TabCtrl_GetItem(TabControlHandle, TabIndex, &itemInfo))
			TabCtrl_DeleteItem(TabControlHandle, TabIndex);
	}
}