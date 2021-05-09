#include "ResponseWindow.h"
#include "LogWindow.h"
#include "CellViewWindow.h"
#include "EditorUI.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace ResponseWindow
{
	static BOOL EnableLipGeneration = TRUE;
	Classes::CUICustomWindow ResponseWindow;
	Classes::CUIBaseControl ListViewItems;

	DLGPROC OldDlgProc;

	HWND FIXAPI GetWindow(VOID)
	{
		return ResponseWindow.Handle;
	}

	Classes::CUICustomWindow& FIXAPI GetWindowObj(VOID)
	{
		return ResponseWindow;
	}

	INT_PTR CALLBACK DlgProc(HWND DialogHwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		if (Message == WM_INITDIALOG)
		{
			ResponseWindow = DialogHwnd;
			ListViewItems = ResponseWindow.GetControl(UI_LISTVIEW_ITEMS);

			if (!fs::exists(fs::absolute(L"CreationKit32.exe")) ||
				!fs::exists(fs::absolute(L"GFSDK_GodraysLib.Win32.dll")) ||
				!fs::exists(fs::absolute(L"ssce5532.dll")))
				EnableLipGeneration = FALSE;
			
			if (!EnableLipGeneration)
			{
				LogWindow::Log("LIPGEN: 'CreationKit32.exe', 'GFSDK_GodraysLib.Win32.dll', or 'ssce5532.dll' is missing from your game directory. LIP generation will be disabled.");
			}
		}
		else if (Message == WM_COMMAND)
		{
			switch (const UINT32 param = LOWORD(wParam); param)
			{
				// "Generate Lip File"
				case UI_BUTTON_GENERATE_LIP_FILE: 
				// "From WAV"
				case UI_CHECKBUTTON_WAV_FILE:
				// "From LTF"
			//	case UI_CHECKBUTTON_LTF_FILE:
				{
					auto item = (INT64)EditorUI::ListViewGetSelectedItem(ListViewItems.Handle);

					if (!EnableLipGeneration || !item)
						return S_FALSE;

					if (param == UI_BUTTON_GENERATE_LIP_FILE)
					{
						std::string AudioFilePath = (LPCSTR)(item + 0xC);

						auto data = *(PINT64)OFFSET(0x6D7B480, 0);
						auto topic = ((INT64(__fastcall*)(INT64, UINT32))OFFSET(0x0B99420, 0))(*(PINT64)(data + 0x28), *(PBYTE)(*(PINT64)(data + 0x18) + 0x1A));
	
						std::string InputText = ((LPCSTR(__fastcall*)(INT64))OFFSET(0x0B56920, 0))(topic);

						// only .wav

						std::wstring wcAudioFilePath = XUtil::Conversion::AnsiToWide(AudioFilePath);
						auto pathAudioFile = fs::path(wcAudioFilePath);
						
						// You can often find and .xwm.
						// Replacement by .wav
						pathAudioFile.replace_extension(L".wav");

						if (!fs::exists(pathAudioFile))
						{
							LogWindow::LogWc(L"LIPGEN: File \"%s\" no found. Trying WAV extension fallback.", pathAudioFile.c_str());
							MessageBoxA(DialogHwnd, "Unable to find audio file on disk", "Error", MB_ICONERROR);

							return S_FALSE;
						}

						AudioFilePath = XUtil::Conversion::WideToAnsi(pathAudioFile);
						((BOOL(__fastcall*)(HWND, LPCSTR, LPCSTR))OFFSET(0x0B66BF0, 0))(DialogHwnd, AudioFilePath.c_str(), InputText.c_str());

						fs::path pathLipFile = pathAudioFile;
						pathLipFile.replace_extension(L".lip");
						if (!fs::exists(pathLipFile))
						{
							LogWindow::LogWc(L"LIPGEN: File \"%s\" LIP generation failed.", pathLipFile.c_str());
							return S_FALSE;
						}
					}
				}
			}
		}
		else if (Message == WM_NOTIFY)
		{
			auto notify = (LPNMHDR)lParam;

			if (notify->code == LVN_ITEMCHANGED && notify->idFrom == UI_LISTVIEW_ITEMS)
				ResponseWindow.Perform(WM_COMMAND, UI_CHECKBUTTON_WAV_FILE, 0);
		}

		return OldDlgProc(DialogHwnd, Message, wParam, lParam);
	}
}