#include "ActorWindow.h"

namespace ActorWindow
{
	Core::Classes::UI::CUICustomWindow ActorWindow;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return ActorWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return ActorWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			ActorWindow = DialogHwnd;
		}
		else if (Message == WM_MOVE)
		{
			// fix flick
			return S_OK;
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}