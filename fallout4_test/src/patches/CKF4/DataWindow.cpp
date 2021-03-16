#include "DataWindow.h"
#include "UIImageList.h"
#include "EditorUIDarkMode.h"
#include "../../../resource.h"

#include <windowsx.h>

#define LVIS_SELFLAG LVIS_FOCUSED | LVIS_SELECTED
#define SIZEBUF 1024

#define OWNCOLUMNNAME "Filename"
#define TWOCOLUMNNAME "Status"

namespace DataWindow
{
	// need static (so not crash)
	NMITEMACTIVATE nmItemFake;

	VOID WINAPI ListView_SetSelectItem(HWND hLV, INT idx)
	{
		ListView_SetItemState(hLV, -1, 0, LVIS_SELFLAG);
		ListView_SetItemState(hLV, idx, LVIS_SELFLAG, LVIS_SELFLAG);
		ListView_EnsureVisible(hLV, idx, FALSE);
	}

	INT WINAPI ListView_GetSelectedItemIndex(HWND hLV)
	{
		// Get the first focused item
		return ListView_GetNextItem(hLV, -1, LVNI_FOCUSED);
	}

	INT WINAPI ListView_FindItemByString(HWND hLV, const std::string str, INT start_idx = 0)
	{
		// The standard search engine is too weak. 
		// Mine allows you to find the first match in the list even if the word is somewhere in the middle. 
		// Standard will only find if there is a match first.

		if (str.length() == 0)
			return -1;

		CHAR szBuf[1024] = { 0 };
		INT nRows = ListView_GetItemCount(hLV);

		if (nRows > start_idx)
		{
			for (INT idx = start_idx; idx < nRows; idx++)
			{
				ListView_GetItemText(hLV, idx, 0, szBuf, sizeof(szBuf));

				if (XUtil::Str::findCaseInsensitive(szBuf, str.c_str()) != std::string::npos)
					return idx;
			}
		}

		return -1;
	}

	Core::Classes::UI::CUICustomWindow DataWindow;

	struct DataWindowControls_t
	{
		Core::Classes::UI::CUIBaseControl ListViewPlugins;
		Core::Classes::UI::CUIBaseControl ListViewPluginsResult;
		Core::Classes::UI::CUIBaseControl ListViewDependences;
		Core::Classes::UI::CUIBaseControl EditSearch;
		Core::Classes::UI::CUIImageList ImageList;
	} DataWindowControls;

	DLGPROC OldDlgProc;

	VOID CKF4Fixes_ListViewResultInitialize(VOID)
	{
		HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;

		// add list columns
		LVCOLUMN lvc = { 0 };
		// Initialize the LVCOLUMN structure.
		// The mask specifies that the format, width, text, and
		// subitem members of the structure are valid.
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cchTextMax = 255;
		// width of column in pixels
		lvc.cx = 150;

		lvc.pszText = OWNCOLUMNNAME;
		ListView_InsertColumn(hWnd, 0, &lvc);

		lvc.iSubItem = 1;
		lvc.pszText = TWOCOLUMNNAME;
		ListView_InsertColumn(hWnd, 1, &lvc);

		ListView_SetExtendedListViewStyle(hWnd, LVS_OWNERDATA | LVS_LIST | LVS_SMALLICON | LVS_SORTDESCENDING);
		ListView_SetExtendedListViewStyleEx(hWnd, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
	}

	VOID CKF4Fixes_ToggleListView(const BOOL bShowResultListView)
	{
		HWND hWnd = DataWindowControls.ListViewPlugins.Handle;
		HWND hWndResult = DataWindowControls.ListViewPluginsResult.Handle;

		DataWindowControls.ListViewPlugins.Visible = !bShowResultListView;
		DataWindowControls.ListViewPluginsResult.Visible = bShowResultListView;

		if (bShowResultListView)
		{
			

			// The width could be changed, will repeat for list
			ListView_SetColumnWidth(hWndResult, 0, ListView_GetColumnWidth(hWnd, 0));
			ListView_SetColumnWidth(hWndResult, 1, ListView_GetColumnWidth(hWnd, 1));
		}
	}

	BOOL CKF4Fixes_AppendToListViewResult(const std::string sFileName, const std::string sType, const BOOL bCheck)
	{
		HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;
		INT iLastIndex = ListView_GetItemCount(hWnd);

		LVITEMA lvi = { 0 };
		lvi.mask = LVIF_TEXT | LVIF_IMAGE;
		lvi.cchTextMax = sFileName.length() + 1;
		lvi.iItem = iLastIndex;
		lvi.pszText = (LPSTR)(&sFileName[0]);
		lvi.iSubItem = 0;
		lvi.iImage = (INT)bCheck;

		if (ListView_InsertItem(hWnd, &lvi) == -1)
			return FALSE;

		ListView_SetItemText(hWnd, iLastIndex, 1, (LPSTR)(&sType[0]));

		return TRUE;
	}

	VOID CKF4Fixes_UpdateListViewResult(VOID)
	{
		INT idx, idx_safe;
		LVITEMA lvi = { 0 };
		HWND hSrchEdit, hListView, hListViewResult;
		CHAR szStrs[2][SIZEBUF];

		hSrchEdit = DataWindowControls.EditSearch.Handle;
		hListView = DataWindowControls.ListViewPlugins.Handle;
		hListViewResult = DataWindowControls.ListViewPluginsResult.Handle;

		ListView_DeleteAllItems(hListViewResult);

		if (Edit_GetTextLength(hSrchEdit) < 2)
		{
			CKF4Fixes_ToggleListView(FALSE);
			return;
		}

		idx_safe = -1;
		idx = -1;

		while ((idx = ListView_FindItemByString(hListView, DataWindowControls.EditSearch.Caption, idx + 1)) != -1)
		{
			if (idx_safe == -1) idx_safe = idx;
			ListView_GetItemText(hListView, idx, 0, szStrs[0], SIZEBUF);
			ListView_GetItemText(hListView, idx, 1, szStrs[1], SIZEBUF);

			lvi.mask = LVIF_IMAGE;
			lvi.iItem = idx;
			ListView_GetItem(hListView, &lvi);

			CKF4Fixes_AppendToListViewResult(szStrs[0], szStrs[1], lvi.iImage != 0);
		}

		if (!ListView_GetItemCount(hListViewResult))
		{
			ListView_SetSelectItem(hListView, 0);
			CKF4Fixes_ToggleListView(FALSE);
			return;
		}

		ListView_SetSelectItem(hListView, (idx_safe >= 0) ? idx_safe : 0);
		CKF4Fixes_ToggleListView(TRUE);
		ListView_SetSelectItem(hListViewResult, 0);

		SendMessageA(hSrchEdit, WM_SETCURSOR, 0, 0);
	}

	BOOL CKF4Fixes_LoadImageFromFileA(HBITMAP &bitmap, const std::string& filename)
	{
		bitmap = (HBITMAP)LoadImageA(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		return bitmap != NULL;
	}

	BOOL CKF4Fixes_LoadImageFromResourceA(HBITMAP &bitmap, const std::string& name)
	{
		bitmap = (HBITMAP)LoadImageA(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, 0);
		return bitmap != NULL;
	}

	BOOL CKF4Fixes_ImageListReplaceA(HIMAGELIST imglst, INT idx, const std::string& filename)
	{
		HBITMAP bmp;
		if (!CKF4Fixes_LoadImageFromFileA(bmp, filename))
			return FALSE;

		BOOL bRes = ImageList_Replace(imglst, idx, NULL, bmp);
		DeleteObject(bmp);

		return bRes;
	}

	HWND GetWindow(void)
	{
		return DataWindow.Handle;
	}

	Core::Classes::UI::CUICustomWindow& GetWindowObj(void)
	{
		return DataWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			DataWindow = DialogHwnd;

			// Set font default
			// This is the default value, but I need an object record to create the missing controls
//			DataWindow.Font = Core::Classes::UI::CFont("Microsoft Sans Serif", 8, {}, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

			DataWindowControls.ListViewPlugins = DataWindow.GetControl(UI_LISTVIEW_PLUGINS);
			DataWindowControls.ListViewPluginsResult = DataWindow.GetControl(UI_NEW_LISTVIEW_CONTROL_TO_RESULT);
			DataWindowControls.ListViewDependences = DataWindow.GetControl(1057);
			DataWindowControls.EditSearch = DataWindow.GetControl(UI_EDIT_SEARCH_PLUGIN_BY_NAME);

			// Eliminate the flicker when changing cells
			ListView_SetExtendedListViewStyleEx(DataWindowControls.ListViewPlugins.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);
			ListView_SetExtendedListViewStyleEx(DataWindowControls.ListViewDependences.Handle, LVS_EX_DOUBLEBUFFER, LVS_EX_DOUBLEBUFFER);

			// Initialize listview for result search
			CKF4Fixes_ListViewResultInitialize();

			INT_PTR nRes = OldDlgProc(DialogHwnd, Message, wParam, lParam);

			// Bethesda probably doesn't know about the existence of Check. 
			// They have created icons that mimic pictorially for the user.
			// I completely take everything from there, although I'm not happy about it, but this is a ready-made mechanism, and I'm just trying to make a search in it.
			HIMAGELIST hImageList = ListView_GetImageList(DataWindowControls.ListViewPlugins.Handle, LVSIL_SMALL);
			
        	DataWindowControls.ImageList.ReCreate(16, 16, FALSE, Core::Classes::UI::ilct24Bit);

			if (EditorUIDarkMode::IsUIDarkMode())
			{
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP4));
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP2));
			}
			else
			{
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP3));
				DataWindowControls.ImageList.AddFromResource(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP1));
			}
			
			ListView_SetImageList(DataWindowControls.ListViewPluginsResult.Handle, DataWindowControls.ImageList.Handle, LVSIL_SMALL);
			ListView_SetImageList(DataWindowControls.ListViewPlugins.Handle, DataWindowControls.ImageList.Handle, LVSIL_SMALL);

			ImageList_Destroy(hImageList);

			return nRes;
		}
		else if (Message == WM_COMMAND)
		{
			switch (LOWORD(wParam))
			{
			case UI_EDIT_SEARCH_PLUGIN_BY_NAME:
			{
				if (HIWORD(wParam) == EN_CHANGE)
				{
					CKF4Fixes_UpdateListViewResult();
				}
			}
			return 0;
			case UI_SETACTIVEPLUGIN_BUTTON:
			{
				if (!DataWindowControls.ListViewPluginsResult.Visible)
					return OldDlgProc(DialogHwnd, Message, wParam, lParam);
				else
				{
					INT_PTR nRes = 0;
					CHAR szStrs[SIZEBUF] = { 0 };
					HWND hListView = DataWindowControls.ListViewPlugins.Handle;
					HWND hListViewResult = DataWindowControls.ListViewPluginsResult.Handle;

					INT idx = ListView_GetSelectedItemIndex(hListViewResult);
					if (idx != -1)
					{
						INT idx_safe = idx;
						ListView_GetItemText(hListViewResult, idx, 0, szStrs, SIZEBUF);
						idx = ListView_FindItemByString(hListView, szStrs);
						ListView_SetSelectItem(hListView, ListView_FindItemByString(hListView, szStrs));

						nRes = OldDlgProc(DialogHwnd, Message, wParam, lParam);
						CKF4Fixes_UpdateListViewResult();

						ListView_SetSelectItem(hListViewResult, idx_safe);
					}

					return nRes;
				}
			}
			return OldDlgProc(DialogHwnd, Message, wParam, lParam);
			}
		}
		else if (Message == WM_NOTIFY)
		{
			if (LOWORD(wParam) == UI_NEW_LISTVIEW_CONTROL_TO_RESULT)
			{
				if (((LPNMHDR)lParam)->code == NM_DBLCLK)
				{
					// Double-click the mouse, just check the box, do the same

					LPNMITEMACTIVATE lpnmItem = (LPNMITEMACTIVATE)lParam;
					HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;
					HWND hWndOld = DataWindowControls.ListViewPlugins.Handle;
					RECT rRectItem;
					CHAR szStrs[SIZEBUF] = { 0 };

					ListView_GetItemText(hWnd, lpnmItem->iItem, 0, szStrs, SIZEBUF);
					INT idx = ListView_FindItemByString(hWndOld, szStrs);

					Assert(idx != -1);
					Assert(ListView_GetItemRect(hWndOld, idx, &rRectItem, LVIR_BOUNDS));

					memset(&nmItemFake, 0, sizeof(nmItemFake));
					nmItemFake.hdr.code = NM_DBLCLK;
					nmItemFake.hdr.hwndFrom = hWndOld;
					nmItemFake.hdr.idFrom = UI_LISTVIEW_PLUGINS;
					nmItemFake.iItem = idx;
					nmItemFake.ptAction.x = rRectItem.left + 5;
					nmItemFake.ptAction.y = rRectItem.top + 5;

					// fake click
					SendMessageA(DialogHwnd, WM_NOTIFY, UI_LISTVIEW_PLUGINS, (LPARAM)&nmItemFake);

					// update short list
					CKF4Fixes_UpdateListViewResult();
					ListView_SetSelectItem(hWnd, lpnmItem->iItem);
				}
				else if (((LPNMHDR)lParam)->code == NM_RDBLCLK)
				{
					// By double - clicking the right mouse button, 
					// I get the active mod installed in the original list of mods.
					// I model this behavior with a short list.

					LPNMITEMACTIVATE lpnmItem = (LPNMITEMACTIVATE)lParam;
					HWND hWnd = DataWindowControls.ListViewPluginsResult.Handle;
					HWND hWndOld = DataWindowControls.ListViewPlugins.Handle;
					RECT rRectItem;
					CHAR szStrs[SIZEBUF] = { 0 };

					ListView_GetItemText(hWnd, lpnmItem->iItem, 0, szStrs, SIZEBUF);
					INT idx = ListView_FindItemByString(hWndOld, szStrs);

					Assert(idx != -1);
					Assert(ListView_GetItemRect(hWndOld, idx, &rRectItem, LVIR_BOUNDS));

					memset(&nmItemFake, 0, sizeof(nmItemFake));
					nmItemFake.hdr.code = NM_RDBLCLK;
					nmItemFake.hdr.hwndFrom = hWndOld;
					nmItemFake.hdr.idFrom = UI_LISTVIEW_PLUGINS;
					nmItemFake.iItem = idx;
					nmItemFake.ptAction.x = rRectItem.left + 5;
					nmItemFake.ptAction.y = rRectItem.top + 5;

					// fake click
					SendMessageA(DialogHwnd, WM_NOTIFY, UI_LISTVIEW_PLUGINS, (LPARAM)&nmItemFake);

					// update short list
					CKF4Fixes_UpdateListViewResult();
					ListView_SetSelectItem(hWnd, lpnmItem->iItem);
				}

				return 0;
			}
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}