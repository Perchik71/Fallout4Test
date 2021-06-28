#include "PreferencesWindow.h"
#include "UITheme/TimeOfDay.h"
#include "Editor.h"

#include <CommCtrl.h>

namespace PreferencesWindow
{
	namespace ToD = Core::UI::Theme::TimeOfDay;
	Core::Classes::UI::CUICustomWindow PreferencesWindow;
	FLOAT TimeOfDayValueChanged = 10.0f;

	DLGPROC OldDlgProc;
	static CHAR szTabName[MAX_PATH];

	HWND FIXAPI GetWindow(VOID)
	{
		return PreferencesWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return PreferencesWindow;
	}

	VOID FIXAPI SetNewValueTimeOfDay(VOID)
	{
		CHAR szBuf[24];
		sprintf_s(szBuf, "%.2f", TimeOfDayValueChanged);

		POINT Range = {
			(LONG)ToD::OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMIN, 0, 0),
			(LONG)ToD::OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMAX, 0, 0)
		};

		INT32 iPos = (INT32)(((Range.y - Range.x) * TimeOfDayValueChanged) / 24.0f);

		ToD::NewUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETPOS, TRUE, (LPARAM)iPos);
		ToD::NewUITimeOfDayComponents.hWndEdit.Caption = szBuf;
	}

	HWND WINAPI CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
	{
		if (lpTemplateName == (LPCSTR)0xA9)
			return hk_CreateDialogParamA(hInstance, lpTemplateName, hWndParent, DlgProc, dwInitParam);
		return hk_CreateDialogParamA(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if ((Message == WM_INITDIALOG) && (!GetWindow()))
		{
			PreferencesWindow = DialogHwnd;
		}
		else if (Message == WM_NCACTIVATE && (!wParam))
		{
			SetNewValueTimeOfDay();
		}
		else if (Message == WM_COMMAND)
		{
			if (LOWORD(wParam) == 0x5C5)
				SetNewValueTimeOfDay();
		}
		else if (WM_CLOSE == Message)
		{
			PreferencesWindow = NULL;
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}

	VOID FIXAPI hk_SetInPreferencesToTimeOfDay(HWND hDlg, INT nIDDlgItem, FLOAT value, INT a4)
	{
		if (nIDDlgItem == 0x3E8)
			TimeOfDayValueChanged = value;

		((VOID(__fastcall*)(HWND, INT, FLOAT, INT))OFFSET(0x565160, 0))(hDlg, nIDDlgItem, value, a4);
	}
}