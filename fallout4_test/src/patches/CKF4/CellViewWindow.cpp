#include "CellViewWindow.h"

#include "LogWindow.h"

namespace CellViewWindow
{
	Classes::CUICustomWindow CellViewWindow;

	struct CellViewWindowControls_t
	{
		BOOL Initialize = FALSE;
		BOOL NowFiltering = FALSE;
		BOOL LockFiltering = FALSE;
		std::string FilterCell;

		Classes::CUIBaseControl LabelWorldSpace;
		Classes::CUIBaseControl LabelFilter;
		Classes::CUIBaseControl NoCellSellected;
		Classes::CUIBaseControl WorldSpaceCombo;
		Classes::CUIBaseControl LoadedAtTop;
		Classes::CUIBaseControl FilteredOnly;
		Classes::CUIBaseControl VisibleOnly;
		Classes::CUIBaseControl SelectedOnly;
		Classes::CUIBaseControl LabelX;
		Classes::CUIBaseControl LabelY;
		Classes::CUIBaseControl EditX;
		Classes::CUIBaseControl EditY;
		Classes::CUIBaseControl EditCellObjsFiltered;
		Classes::CUIBaseControl BtnGo;
		Classes::CUIBaseControl Lst1;
		Classes::CUIBaseControl Lst2;
		Classes::CUIBaseControl EditFilterCell;
		Classes::CUICheckbox ActiveOnly;
		Classes::CUICheckbox ActiveOnlyObjs;
	} CellViewWindowControls;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return CellViewWindow.Handle;
	}

	Classes::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return CellViewWindow;
	}

	VOID FIXAPI SetCellWindowFilter(const BOOL actived)
	{
		CellViewWindowControls.FilterCell = CellViewWindowControls.EditFilterCell.Caption;
		CellViewWindowControls.ActiveOnly.Checked = actived;
		// Fake the dropdown list being activated
		CellViewWindow.Perform(WM_COMMAND, MAKEWPARAM(2083, 1), 0);
	}

	VOID FIXAPI SetCellObjectsWindowFilter(const BOOL actived)
	{
		CellViewWindowControls.ActiveOnlyObjs.Checked = actived;
		// Fake a filter text box change
		CellViewWindow.Perform(WM_COMMAND, MAKEWPARAM(2581, EN_CHANGE), 0);
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			// This message is called a great many times, especially when updating World Space

			if (!CellViewWindowControls.Initialize)
			{
				CellViewWindowControls.Initialize = TRUE;
				CellViewWindow = DialogHwnd;

				CellViewWindowControls.LabelWorldSpace = CellViewWindow.GetControl(1164);
				CellViewWindowControls.NoCellSellected = CellViewWindow.GetControl(1163);
				CellViewWindowControls.LabelFilter = CellViewWindow.GetControl(UI_CELL_VIEW_FILTER_LABEL);
				CellViewWindowControls.WorldSpaceCombo = CellViewWindow.GetControl(UI_CELL_VIEW_WORLDSPACE);
				CellViewWindowControls.LoadedAtTop = CellViewWindow.GetControl(5662);
				CellViewWindowControls.FilteredOnly = CellViewWindow.GetControl(5664);
				CellViewWindowControls.VisibleOnly = CellViewWindow.GetControl(5666);
				CellViewWindowControls.SelectedOnly = CellViewWindow.GetControl(5665);
				CellViewWindowControls.LabelX = CellViewWindow.GetControl(5281);
				CellViewWindowControls.LabelY = CellViewWindow.GetControl(5282);
				CellViewWindowControls.EditX = CellViewWindow.GetControl(5283);
				CellViewWindowControls.EditY = CellViewWindow.GetControl(5099);
				CellViewWindowControls.EditCellObjsFiltered = CellViewWindow.GetControl(2581);
				CellViewWindowControls.BtnGo = CellViewWindow.GetControl(3681);
				CellViewWindowControls.Lst1 = CellViewWindow.GetControl(1155);
				CellViewWindowControls.Lst2 = CellViewWindow.GetControl(1156);
				CellViewWindowControls.EditFilterCell = CellViewWindow.GetControl(UI_CELL_VIEW_FILTER_EDITTEXT);
				CellViewWindowControls.ActiveOnly.CreateWnd(CellViewWindow, CellViewWindow.GetControl(UI_CELL_WINDOW_ADD_ITEM), UI_CELL_WINDOW_ADD_ITEM);
				CellViewWindowControls.ActiveOnlyObjs.CreateWnd(CellViewWindow, CellViewWindow.GetControl(UI_CELL_VIEW_CHECK_ACTIVE_CELL_OBJECTS), UI_CELL_VIEW_CHECK_ACTIVE_CELL_OBJECTS);

				// Eliminate the flicker when changing cells
				ListView_SetExtendedListViewStyleEx(CellViewWindowControls.Lst1.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
				ListView_SetExtendedListViewStyleEx(CellViewWindowControls.Lst2.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

				ShowWindow(GetDlgItem(DialogHwnd, 1007), SW_HIDE);
				SetTimer(DialogHwnd, UI_TIMER_FILTER_CELL, 2000, NULL);
			}
			else
			{
				// Let's use this feature

				CellViewWindowControls.LockFiltering = TRUE;
				CellViewWindowControls.NowFiltering = FALSE;
				CellViewWindowControls.FilterCell = "";
				CellViewWindowControls.EditFilterCell.Caption = "";
			}
		}
		else if (Message == UI_CELL_WINDOW_ADD_ITEM)
		{
			auto form = reinterpret_cast<const TESForm_CK*>(wParam);
			auto allowInsert = reinterpret_cast<BOOL*>(lParam);
			*allowInsert = TRUE;

			if (!form)
				return S_OK;

			// Skip the entry if "Show only active cells" is checked
			if (CellViewWindowControls.ActiveOnly.Checked)
				*allowInsert = (BOOL)form->Active;

			if (*allowInsert && (CellViewWindowControls.FilterCell.length() > 2))
				*allowInsert = (BOOL)(XUtil::Str::findCaseInsensitive(form->EditID.c_str(), CellViewWindowControls.FilterCell.c_str()) != std::string::npos);	

			return S_OK;
		}
		else if (Message == UI_CELL_VIEW_ADD_CELL_OBJECT_ITEM)
		{
			auto form = reinterpret_cast<const TESForm_CK*>(wParam);
			auto allowInsert = reinterpret_cast<BOOL*>(lParam);
			*allowInsert = TRUE;

			if(!form)
				return S_OK;

			// Skip the entry if "Show only active objects" is checked
			if (CellViewWindowControls.ActiveOnlyObjs.Checked)
				*allowInsert = (BOOL)form->Active;

			return S_OK;
		}
		// Don't let us reduce the window too much
		else if (Message == WM_GETMINMAXINFO)
		{
			if (lParam)
			{
				LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
				lpMMI->ptMinTrackSize.x = 738;
				lpMMI->ptMinTrackSize.y = 315;
			}

			return S_OK;
		}
		else if (Message == WM_TIMER)
		{
			if (wParam == UI_TIMER_FILTER_CELL)
			{
				if (CellViewWindowControls.NowFiltering)
				{
					SetCellWindowFilter(CellViewWindowControls.ActiveOnly.Checked);
					CellViewWindowControls.NowFiltering = FALSE;
				}
			}
		}
		else if (Message == WM_COMMAND)
		{
			switch (LOWORD(wParam))
			{
			case UI_CELL_WINDOW_ADD_ITEM:
				SetCellWindowFilter(!CellViewWindowControls.ActiveOnly.Checked);
				return S_OK;
			case UI_CELL_VIEW_CHECK_ACTIVE_CELL_OBJECTS:
				SetCellObjectsWindowFilter(!CellViewWindowControls.ActiveOnlyObjs.Checked);
				return S_OK;
			case UI_CELL_VIEW_FILTER_EDITTEXT:
				if (HIWORD(wParam) == EN_CHANGE)
				{
					UINT32 len = GetWindowTextLengthA(CellViewWindowControls.EditFilterCell.Handle);
					if (len > 2)
					{
						CellViewWindowControls.LockFiltering = FALSE;
						CellViewWindowControls.NowFiltering = TRUE;
					}
					else if (!len && !CellViewWindowControls.LockFiltering)
						SetCellWindowFilter(CellViewWindowControls.ActiveOnly.Checked);
				}
				return S_OK;
			}
		}
		else if (Message == WM_SIZE)
		{
			LRESULT lResult = OldDlgProc(DialogHwnd, Message, wParam, lParam);

			// Fix the "World Space" label positioning on window resize
			CellViewWindowControls.LabelWorldSpace.Width = CellViewWindowControls.WorldSpaceCombo.Width;
			
			CellViewWindowControls.ActiveOnlyObjs.Left = CellViewWindowControls.VisibleOnly.Left;

			Classes::CRECT wnd_rect_e = CellViewWindowControls.EditFilterCell.WindowRect();
			Classes::CRECT wnd_rect_l = CellViewWindowControls.Lst1.WindowRect();

			CellViewWindowControls.EditFilterCell.Width = wnd_rect_l.Right - wnd_rect_e.Left;
			CellViewWindowControls.LabelFilter.Left = CellViewWindowControls.EditFilterCell.Left + 8;
			CellViewWindowControls.LabelFilter.Width = wnd_rect_l.Right - wnd_rect_e.Left - 8;

			return lResult;
		}
		else if (Message == WM_DESTROY)
		{
			KillTimer(DialogHwnd, UI_TIMER_FILTER_CELL);
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}

	VOID FIXAPI hk_7FF70C322BC0(HWND ListViewHandle, TESForm_CK* Form, BOOL UseImage, int32_t ItemIndex)
	{
		BOOL allowInsert = TRUE;
		CellViewWindow.Perform(UI_CELL_WINDOW_ADD_ITEM, (WPARAM)Form, (LPARAM)&allowInsert);

		if (!allowInsert)
			return;

		return ((VOID(__fastcall*)(HWND, TESForm_CK*, BOOL, int32_t))OFFSET(0x562BC0, 0))(ListViewHandle, Form, UseImage, ItemIndex);
	}

	INT32 FIXAPI hk_call_5A43B5(HWND** ListViewHandle, TESForm_CK** Form, INT64 a3)
	{
		BOOL allowInsert = TRUE;
		CellViewWindow.Perform(UI_CELL_VIEW_ADD_CELL_OBJECT_ITEM, (WPARAM)*Form, (LPARAM)&allowInsert);

		if (!allowInsert)
			return 1;

		return ((INT32(__fastcall*)(HWND**, TESForm_CK**, INT64))OFFSET(0x5A4900, 0))(ListViewHandle, Form, a3);
	}
}