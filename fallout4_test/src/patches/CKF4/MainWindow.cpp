#include "MainWindow.h"
#include "LogWindow.h"
#include "CellViewWindow.h"
#include "ObjectWindow.h"
#include "RenderWindow.h"
#include "Editor.h"
#include "EditorUI.h"

#include "TESCellViewScene_CK.h"

#include <commdlg.h>
#include <shellapi.h>
#include <filesystem>

#include <Uxtheme.h>

namespace MainWindow
{
	static BOOL bActiveApp = FALSE;
	static Classes::CUIMainWindow MainWindow;
	static Classes::CUIBaseControl ToolbarPanel_1;

	WNDPROC OldWndProc;
	
	BOOL FIXAPI IsActive(VOID)
	{
		return bActiveApp;
	}

	HWND FIXAPI GetWindow(VOID)
	{
		return MainWindow.Handle;
	}

	Classes::CUIMainWindow& FIXAPI GetWindowObj(VOID)
	{
		return MainWindow;
	}

	Classes::CUIMenu& FIXAPI GetMainMenuObj(VOID)
	{
		return MainWindow.MainMenu;
	}

	bool CreateExtensionMenu()
	{
		BOOL result = TRUE;

		// I tend to write object-oriented

		Classes::CUIMenu* ExtensionSubMenu = new Classes::CUIMenu(Classes::CUIMenu::CreateSubMenu());
		Classes::CUIMenu* LinksSubMenu = new Classes::CUIMenu(Classes::CUIMenu::CreateSubMenu());

		Assert(ExtensionSubMenu);
		Assert(LinksSubMenu);

		// I'll add hiding and showing the log window 

		result = result && ExtensionSubMenu->Append("Show/Hide Log", UI_EXTMENU_SHOWLOG, TRUE, IsWindowVisible(LogWindow::GetWindow()));
		result = result && ExtensionSubMenu->Append("Clear Log", UI_EXTMENU_CLEARLOG);
		result = result && ExtensionSubMenu->Append("Autoscroll Log", UI_EXTMENU_AUTOSCROLL, TRUE, TRUE);
		result = result && ExtensionSubMenu->AppendSeparator();
		result = result && ExtensionSubMenu->Append("Dump Active Forms", UI_EXTMENU_LOADEDESPINFO);
		result = result && ExtensionSubMenu->AppendSeparator();
		result = result && ExtensionSubMenu->Append("Save Hardcoded Forms", UI_EXTMENU_HARDCODEDFORMS);
		result = result && MainWindow.MainMenu.Append("Extensions", *ExtensionSubMenu);

		result = result && LinksSubMenu->Append("Cascadia Wiki", UI_EXTMENU_LINKS_WIKI);
		result = result && LinksSubMenu->Append("Material Editor official page", UI_EXTMENU_LINKS_MATERIALEDITOR);
		result = result && MainWindow.MainMenu.Append("Links", *LinksSubMenu);

		// I don't use DeleteMenu when destroying, I don't need to store a pointer and all that.

		delete LinksSubMenu;
		delete ExtensionSubMenu;

		AssertMsg(result, "Failed to create extension submenus");
		return result;
	}

	LRESULT CALLBACK WndProc(HWND Hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		Classes::CUIMenuItem MenuItem;

		if (Message == WM_CREATE)
		{
			auto createInfo = (const CREATESTRUCT*)lParam;

			if (!_stricmp(createInfo->lpszName, "Creation Kit") && !_stricmp(createInfo->lpszClass, "Creation Kit"))
			{
				// Initialize the original window before adding anything

				// The entire API is in the Creation Kit ANSI, so it's better to use A
				LRESULT status = CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
				MainWindow = Hwnd;

				// Create custom menu controls
				MainWindow.MainMenu = createInfo->hMenu;
				CreateExtensionMenu();

				// All main menus change to uppercase letters
				for (UINT i = 0; i < MainWindow.MainMenu.Count(); i++)
				{
					MenuItem = MainWindow.MainMenu.GetItemByPos(i);
					MenuItem.Text = XUtil::Str::UpperCase(MenuItem.Text);
				}

				Classes::CUIMenu ViewMenu = MainWindow.MainMenu.GetSubMenuItem(2);

				// How annoying is this window Warnings, delete from the menu.
				ViewMenu.RemoveByPos(34);

				MenuItem = ViewMenu.GetItem(UI_FOG_CMD);
				// 459F228 - address bFogEnabled
				bFogToggle = (*(bool*)(OFFSET(0x459F228, 0)));
				MenuItem.Checked = bFogToggle;

				// Fix show/hide object & cell view windows
				MenuItem = ViewMenu.GetItemByPos(2);
				MenuItem.ID = UI_CMD_SHOWHIDE_OBJECTWINDOW;
				MenuItem.Checked = TRUE;
				MenuItem = ViewMenu.GetItemByPos(3);
				MenuItem.ID = UI_CMD_SHOWHIDE_CELLVIEWWINDOW;
				MenuItem.Checked = TRUE;

				if (!g_INI_CK.GetBoolean("General", "bViewFog", TRUE))
					ViewMenu.GetItem(UI_FOG_CMD).Click();

				return status;
			}
		}
		else if (Message == WM_SIZE && Hwnd == GetWindow())
		{
			LRESULT lResult = CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
			EditorUI::hk_SetSettingsPartStatusBar(MainWindow::GetWindowObj().Statusbar.Handle, Message, wParam, lParam);
			return lResult;
		}
		else if (Message == WM_NCACTIVATE)
		{
			bActiveApp = (BOOL)wParam;
		}
		else if (Message == WM_COMMAND)
		{
			const uint32_t param = LOWORD(wParam);

			switch (param)
			{
			case UI_EDITOR_OPENFORMBYID:
			{
				auto form = TESForm_CK::GetFormByNumericID((uint32_t)lParam);

				if (form)
					(*(void(__fastcall **)(TESForm_CK*, HWND, __int64, __int64))(*(__int64*)form + 0x340))(form, Hwnd, 0, 1);
			}
			return S_OK;

			case UI_EXTMENU_SHOWLOG:
			{
				// I already have a class that describes the basic functions of Windows, 
				// but there is no point in rewriting it too much, so I'll add hiding and showing the log window.

				if (IsWindowVisible(LogWindow::GetWindow()))
					ShowWindow(LogWindow::GetWindow(), SW_HIDE);
				else
				{
					ShowWindow(LogWindow::GetWindow(), SW_SHOW);
					SetForegroundWindow(LogWindow::GetWindow());
				}

				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_EXTMENU_SHOWLOG);
				MenuItem.Checked = !MenuItem.Checked;
			}
			return S_OK;

			case UI_EXTMENU_CLEARLOG:
			{
				PostMessageA(LogWindow::GetWindow(), UI_LOG_CMD_CLEARTEXT, 0, 0);
			}
			return S_OK;

			case UI_EXTMENU_AUTOSCROLL:
			{
				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_EXTMENU_AUTOSCROLL);
				MenuItem.Checked = !MenuItem.Checked;

				PostMessageA(LogWindow::GetWindow(), UI_LOG_CMD_AUTOSCROLL, (WPARAM)MenuItem.Checked, 0);
			}
			return S_OK;

			case UI_EXTMENU_LOADEDESPINFO:
			{
				char filePath[MAX_PATH] = {};
				OPENFILENAME ofnData = { 0 };
				ofnData.lStructSize = sizeof(OPENFILENAME);
				ofnData.lpstrFilter = "Text Files (*.txt)\0*.txt\0\0";
				ofnData.lpstrFile = filePath;
				ofnData.nMaxFile = ARRAYSIZE(filePath);
				ofnData.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
				ofnData.lpstrDefExt = "txt";

				if (FILE* f; GetSaveFileName(&ofnData) && fopen_s(&f, filePath, "w") == 0)
				{
					struct VersionControlListItem
					{
						const char* EditorId;
						uint32_t FileOffset;
						char Type[4];
						uint32_t FileLength;
						char GroupType[4];
						uint32_t FormId;
						uint32_t VersionControlId;
						char _pad0[0x8];
					};
					static_assert_offset(VersionControlListItem, EditorId, 0x0);
					static_assert_offset(VersionControlListItem, FileOffset, 0x8);
					static_assert_offset(VersionControlListItem, Type, 0xC);
					static_assert_offset(VersionControlListItem, FileLength, 0x10);
					static_assert_offset(VersionControlListItem, GroupType, 0x14);
					static_assert_offset(VersionControlListItem, FormId, 0x18);
					static_assert_offset(VersionControlListItem, VersionControlId, 0x1C);
					static_assert(sizeof(VersionControlListItem) == 0x28);

					static std::vector<VersionControlListItem> formList;

					// Invoke the dialog, building form list
					void(*callback)(void*, void*, void*, __int64) = [](void*, void*, void*, __int64 Item)
					{
						auto data = *(VersionControlListItem **)(Item + 0x28);

						formList.push_back(*data);
						formList.back().EditorId = _strdup(data->EditorId);
					};

					XUtil::PatchMemoryNop(OFFSET(0x5A5D51, 0), 6);
					XUtil::DetourCall(OFFSET(0x5A5D51, 0), callback);
					CallWindowProcA((WNDPROC)OFFSET(0x5A8250, 0), Hwnd, WM_COMMAND, 1185, 0);

					// Sort by: type, editor id, form id, then file offset
					std::sort(formList.begin(), formList.end(),
						[](const auto & A, const auto & B) -> bool
						{
							int ret = memcmp(A.Type, B.Type, sizeof(VersionControlListItem::Type));

							if (ret != 0)
								return ret < 0;

							ret = _stricmp(A.EditorId, B.EditorId);

							if (ret != 0)
								return ret < 0;

							if (A.FormId != B.FormId)
								return A.FormId > B.FormId;

							return A.FileOffset > B.FileOffset;
						});

					// Dump it to the log
					fprintf(f, "Type, Editor Id, Form Id, File Offset, File Length, Version Control Id\n");

					for (auto& item : formList)
					{
						fprintf(f, "%c%c%c%c,\"%s\",%08X,%u,%u,-%08X-\n",
							item.Type[0], item.Type[1], item.Type[2], item.Type[3],
							item.EditorId,
							item.FormId,
							item.FileOffset,
							item.FileLength,
							item.VersionControlId);

						free((void*)item.EditorId);
					}

					formList.clear();
					fclose(f);
				}
			}
			return S_OK;

			case UI_EXTMENU_HARDCODEDFORMS:
			{
				for (uint32_t i = 0; i < 2048; i++)
				{
					auto form = TESForm_CK::GetFormByNumericID(i);

					if (form)
					{
						(*(void(__fastcall **)(TESForm_CK*, __int64))(*(__int64*)form + 0x1A0))(form, 1);
						LogWindow::Log("SetFormModified(%08X)", i);
					}
				}

				// Fake the click on "Save"
				PostMessageA(Hwnd, WM_COMMAND, 40127, 0);
			}
			return S_OK;

			case UI_EXTMENU_LINKS_WIKI:
			{
				ShellExecuteA(NULL, "open", "https://wiki.falloutcascadia.com/index.php?title=Main_Page", "", "", SW_SHOW);
			}
			return S_OK;

			case UI_EXTMENU_LINKS_MATERIALEDITOR:
			{
				ShellExecuteA(NULL, "open", "https://www.nexusmods.com/fallout4/mods/3635", "", "", SW_SHOW);
			}
			return S_OK;

			case UI_SHOW_MATERIALEDITOR:
			{
				constexpr LPSTR lpMaterialEditorPath = ".\\Tools\\MaterialEditor\\Material Editor.exe";
				if (std::filesystem::exists(lpMaterialEditorPath))
					ShellExecuteA(NULL, "open", lpMaterialEditorPath, "", "", SW_SHOW);
				else
					MainWindow::GetWindowObj().MessageWarningDlg(std::string("File: \"") + lpMaterialEditorPath + "\" no found.");
			}
			return S_OK;

			case UI_CMD_SHOWHIDE_OBJECTWINDOW:
			{
				ObjectWindow::OBJWNDS Wnds = ObjectWindow::GetAllWindowObj();

				for each (auto Wnd in Wnds)
				{
					Wnd.second->ObjectWindow.Visible = !Wnd.second->ObjectWindow.Visible;
					if (Wnd.second->ObjectWindow.Visible)
						Wnd.second->ObjectWindow.Foreground();
				}

				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_CMD_SHOWHIDE_OBJECTWINDOW);
				MenuItem.Checked = !MenuItem.Checked;
			}
			return S_OK;

			case UI_CMD_SHOWHIDE_CELLVIEWWINDOW:
			{
				Classes::CUICustomWindow Wnd = CellViewWindow::GetWindowObj();

				Wnd.Visible = !Wnd.Visible;
				if (Wnd.Visible)
					Wnd.Foreground();

				// Change the checkbox
				MenuItem = MainWindow.MainMenu.GetItem(UI_CMD_SHOWHIDE_CELLVIEWWINDOW);
				MenuItem.Checked = !MenuItem.Checked;
			}
			return S_OK;

			case UI_COLLISION_GEOM_CMD:
			{
				// CheckMenuItem is called, however, it always gets zero, but eight is written on top, which is equal to MFS_CHECKED.
				// So I'll create a variable. By the way, when loading a new process, the flag is not set, as is the function itself.

				RenderWindow::SetCollisionView(!RenderWindow::IsCollisionView());
				MainWindow.MainMenu.GetItem(UI_COLLISION_GEOM_CMD).Checked = RenderWindow::IsCollisionView();
			}
			return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);

			case UI_FOG_CMD:
			{
				MenuItem = MainWindow.MainMenu.GetItem(UI_FOG_CMD);
				MenuItem.Checked = !MenuItem.Checked;
				bFogToggle = MenuItem.Checked;

				if (TESCellViewScene_CK* view = TESCellViewScene_CK::GetCellViewScene(); view)
				{
					if (TESForm_CK* form = view->CellInteriosInfo; form)
					{
						// Fake update scene
						((VOID(__stdcall*)(TESCellViewScene_CK*, TESForm_CK*))OFFSET(0x7B1E80, 0))(view, form);
					}
				}
			}
			return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
			}
		}
		else if (Message == WM_SHOWWINDOW)
		{
			// Getting additional child Windows
			MainWindow.FindToolWindow();
		}
		else if (Message == WM_SETTEXT && Hwnd == GetWindow())
		{
			// Continue normal execution but with a custom string
			char customTitle[1024];
			sprintf_s(customTitle, "%s [CK64Fixes Rev. F4-%s]", (LPCSTR)lParam, g_GitVersion);

			return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, (LPARAM)customTitle);
		}
		else if (Message == WM_GETMINMAXINFO)
		{
			// https://social.msdn.microsoft.com/Forums/vstudio/en-US/fb4de52d-66d4-44da-907c-0357d6ba894c/swmaximize-is-same-as-fullscreen?forum=vcgeneral

			RECT WorkArea;
			SystemParametersInfoA(SPI_GETWORKAREA, 0, &WorkArea, 0);
			((MINMAXINFO*)lParam)->ptMaxSize.x = (WorkArea.right - WorkArea.left);
			((MINMAXINFO*)lParam)->ptMaxSize.y = (WorkArea.bottom - WorkArea.top);
			((MINMAXINFO*)lParam)->ptMaxPosition.x = WorkArea.left;
			((MINMAXINFO*)lParam)->ptMaxPosition.y = WorkArea.top;

			return S_OK;
		}
		else if (Message == WM_CLOSE)
		{
			g_INI_CK.SetBoolean("General", "bViewFog", MainWindow.MainMenu.GetItem(UI_FOG_CMD).Checked);
			g_INI_CK.Save();
		}

		return CallWindowProcA(OldWndProc, Hwnd, Message, wParam, lParam);
	}
}