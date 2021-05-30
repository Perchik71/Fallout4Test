#include "../../common.h"

#define THEME_DEBUG 0

#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <unordered_set>
#include <vssym32.h>
#include <Richedit.h>

#include <stdlib.h>

#include "UIThemeMode.h"
#include "UIBaseWindow.h"
#include "MainWindow.h"

/// THEMES

#include "UITheme/VarCommon.h"
#include "UITheme/MDIClient.h"
#include "UITheme/PopupMenu.h"
#include "UITheme/ListView.h"
#include "UITheme/TreeView.h"
#include "UITheme/ToolBar.h"
#include "UITheme/PushButton.h"
#include "UITheme/CheckBox.h"
#include "UITheme/RadioButton.h"
#include "UITheme/GroupBox.h"
#include "UITheme/EditText.h"
#include "UITheme/ComboBox.h"
#include "UITheme/StatusBar.h"
#include "UITheme/ScrollBar.h"
#include "UITheme/TrackBar.h"
#include "UITheme/UpDown.h"
#include "UITheme/ListBox.h"
#include "UITheme/ProgressBar.h"
#include "UITheme/CustomBox.h"
#include "UITheme/TimeOfDay.h"
#include "UITheme/Header.h"
#include "UITheme/PageControl.h"
#include "UITheme/RichEdit20.h"
#include "UITheme/Memo.h"

/*

This file is part of Fallout 4 Fixes source code.

*/

namespace Theme = Core::UI::Theme;
namespace Classes = Core::Classes::UI;

#if THEME_DEBUG
#include <fstream>
#endif

namespace UITheme
{
	struct string_equal_to
	{
		inline bool operator()(const std::string_view& lhs, const std::string_view& rhs) const
		{
			return !_stricmp(lhs.data(), rhs.data());
		}
	};

	const std::unordered_set<std::string_view> PermanentWindowSubclasses
	{
		"Creation Kit",
		"ActivatorClass",
		"AlchemyClass",
		"ArmorClass",
		"CreatureClass",
		"LockPickClass",
		"NPCClass",
		"WeaponClass",
		"FaceClass",
		"PlaneClass",
		"MonitorClass",
		"ViewerClass",
		"SpeakerClass",
		"LandClass",
		"RenderWindow",
		"#32770"
		// "BABYGRID",
		// "NiTreeCtrl",
	};

	tbb::concurrent_unordered_map<HTHEME, ThemeType> ThemeHandles;
	BOOL EnableThemeHooking;

#if THEME_DEBUG
	std::ofstream ofs;
#endif

	static WNDPROC OldPopupMenuWndClass = NULL;
	static Classes::CUIFont* listFont = NULL;

	VOID FIXAPI Initialize(Theme::Theme ThemeID)
	{
		Theme::SetTheme(ThemeID);
		EnableThemeHooking = TRUE;

		// I will new it once and forget about its existence
		// I have no general idea where to destroy it. Yes, and it is not necessary, it will die along with the process.
		listFont = new Classes::CUIFont("Microsoft Sans Serif", 8, {}, g_INI.GetInteger("Font", "Charset", DEFAULT_CHARSET));

#if THEME_DEBUG
		ofs.open("__theme_debug.log");
#endif
	}

	VOID FIXAPI InitializeThread(VOID)
	{
		if (EnableThemeHooking)
			SetWindowsHookExA(WH_CALLWNDPROC, CallWndProcCallback, NULL, GetCurrentThreadId());
	}

	BOOL FIXAPI IsEnabledMode(VOID)
	{
		return EnableThemeHooking;
	}

	HWND FIXAPI Comctl32CreateToolbarEx_1(HWND hwnd, DWORD ws, UINT wID, INT nBitmaps, HINSTANCE hBMInst, UINT_PTR wBMID, LPCTBBUTTON lpButtons,
		INT iNumButtons, INT dxButton, INT dyButton, INT dxBitmap, INT dyBitmap, UINT uStructSize)
	{
		HIMAGELIST hImageList;

		if ((Theme::GetTheme() != Theme::Theme_Light) && (Theme::GetTheme() != Theme::Theme_Gray))
			hImageList = ImageList_LoadImageA(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP5), 16, 0, RGB(56, 56, 56), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
		else
			hImageList = ImageList_LoadImageA(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP7), 16, 0, RGB(192, 192, 192), IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
		
		HWND ret = CreateToolbarEx(hwnd, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, wID, nBitmaps,
			NULL, NULL, lpButtons, iNumButtons - 2 /*delete two divider*/, dxButton, dyButton, dxBitmap, dyBitmap, uStructSize);

		SendMessageA(ret, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
		SendMessageA(ret, TB_SETBITMAPSIZE, 0, MAKELPARAM(16, 16));
		ShowWindow(ret, SW_SHOWNORMAL);
		SetWindowPos(ret, NULL, 0, 24, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

		Theme::TimeOfDay::Initialization(ret);
		
		return ret; 
	}

	HIMAGELIST FIXAPI Comctl32ImageList_LoadImageA_1(HINSTANCE hi, LPCSTR lpbmp, INT cx, INT cGrow, COLORREF crMask, UINT uType, UINT uFlags)
	{
		if ((Theme::GetTheme() != Theme::Theme_Light) && (Theme::GetTheme() != Theme::Theme_Gray))
			return ImageList_LoadImageA(g_hModule, MAKEINTRESOURCEA(IDB_BITMAP6), cx, cGrow, crMask, IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
		else
			return ImageList_LoadImageA(hi, lpbmp, cx, cGrow, crMask, uType, uFlags);
	}

	HWND FIXAPI Comctl32CreateWindowEx_1(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, INT x, INT y,
		INT nWidth, INT nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND ret = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | SS_LEFT, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

		return ret;
	}

	VOID FIXAPI HideOldTimeOfDayComponents(VOID)
	{
		// I will hide the old ones, but I will rely on them when sending messages, however, in the end, I will fake the event to change the time of day

		Theme::TimeOfDay::OldUITimeOfDayComponents.hWndLabel = GetDlgItem(Theme::TimeOfDay::OldUITimeOfDayComponents.hWndToolBar.Handle, 0x16D2);
		Theme::TimeOfDay::OldUITimeOfDayComponents.hWndTrackBar = GetDlgItem(Theme::TimeOfDay::OldUITimeOfDayComponents.hWndToolBar.Handle, 0x16D3);
		Theme::TimeOfDay::OldUITimeOfDayComponents.hWndEdit = GetDlgItem(Theme::TimeOfDay::OldUITimeOfDayComponents.hWndToolBar.Handle, 0x16D4);

		ShowWindow(Theme::TimeOfDay::OldUITimeOfDayComponents.hWndLabel.Handle, SW_HIDE);
		ShowWindow(Theme::TimeOfDay::OldUITimeOfDayComponents.hWndTrackBar.Handle, SW_HIDE);
		ShowWindow(Theme::TimeOfDay::OldUITimeOfDayComponents.hWndEdit.Handle, SW_HIDE);

		POINT Range = {
			(LONG)Theme::TimeOfDay::OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMIN, 0, 0),
			(LONG)Theme::TimeOfDay::OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMAX, 0, 0)
		};

		Theme::TimeOfDay::NewUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETRANGE, (WPARAM)TRUE, MAKELPARAM(Range.x, Range.y));
		Theme::TimeOfDay::NewUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETPOS, (WPARAM)TRUE, (LONG)((10 * (Range.y - Range.x)) / 24));
		Theme::TimeOfDay::NewUITimeOfDayComponents.hWndEdit.Caption = "10.00";
	}

	// Returns a valid visual theme type, depending on the window class
	ThemeType FIXAPI GetThemeTypeFromWindow(HWND hWindow)
	{
		static std::unordered_map<std::string_view, ThemeType, std::hash<std::string_view>, string_equal_to> TargetWindowThemes
		{
			{ STATUSCLASSNAMEA, ThemeType::StatusBar },
			{ "mdiclient", ThemeType::MDIClient },
			{ WC_STATICA, ThemeType::Static },
			{ UPDOWN_CLASS, ThemeType::Spin },
			{ WC_EDITA, ThemeType::Edit },
			{ RICHEDIT_CLASSA, ThemeType::RichEdit20 },
			{ "RICHEDIT50W", ThemeType::RichEdit50 },
			{ PROGRESS_CLASSA, ThemeType::ProgressBar },
			{ WC_BUTTONA, ThemeType::Button },
			{ WC_LISTBOXA, ThemeType::ListBox },
			{ WC_COMBOBOXA, ThemeType::ComboBox },
			{ WC_COMBOBOXEXA, ThemeType::ComboBox },
			{ WC_HEADERA, ThemeType::Header },
			{ WC_LISTVIEWA, ThemeType::ListView },
			{ WC_TREEVIEWA, ThemeType::TreeView },
			{ WC_TABCONTROLA, ThemeType::TabControl },
			{ TOOLBARCLASSNAMEA, ThemeType::ToolBar },
			{ TRACKBAR_CLASSA, ThemeType::TrackBar },
			{ "#32768", ThemeType::PopupMenu },
		};

		auto themeType = ThemeType::None;

		Core::Classes::UI::CUICustomWindow Window = hWindow;

		if (auto itr = TargetWindowThemes.find(Window.Name); itr != TargetWindowThemes.end())
			themeType = itr->second;

		return themeType;
	}

	// Binds the specified class type to the visual theme. hWindow takes only HTHEME
	// Returns TRUE if successful
	BOOL FIXAPI RegisterThemeHandle(HWND hWindow, ThemeType eTheme)
	{
		HTHEME windowTheme = GetWindowTheme(hWindow);
		if (!windowTheme)
			return FALSE;

		return RegisterThemeHandle(windowTheme, eTheme);
	}

	// Binds the specified class type to the visual theme
	// Returns TRUE if successful
	BOOL FIXAPI RegisterThemeHandle(HTHEME hTheme, ThemeType eTheme)
	{
		if (ThemeType::None == eTheme)
			return FALSE;

		for (auto it = ThemeHandles.begin(); it != ThemeHandles.end(); it++)
		{
			if ((*it).second == eTheme)
				return FALSE;
		}

		ThemeHandles.emplace(hTheme, eTheme);

		return TRUE;
	}


	LRESULT CALLBACK CallWndProcCallback(INT nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			if (auto messageData = reinterpret_cast<CWPSTRUCT*>(lParam); (messageData->message == WM_CREATE)/* || messageData->message == WM_INITDIALOG*/)
			{
				LPCREATESTRUCTA lpCreateStruct = (LPCREATESTRUCTA)messageData->lParam;
				if (lpCreateStruct)
				{
#if THEME_DEBUG		
					ofs << "WM_CREATE: " << std::endl
						<< "cx: " << lpCreateStruct->cx << " "
						<< "cy: " << lpCreateStruct->cy << " "
						<< "hInstance: " << lpCreateStruct->hInstance << " "
						<< "dwExStyle: " << lpCreateStruct->dwExStyle << " "
						<< "hMenu: " << lpCreateStruct->hMenu << " "
						<< "hwndParent: " << lpCreateStruct->hwndParent << " "
						<< "lpCreateParams: " << lpCreateStruct->lpCreateParams << " ";

						/*			if (lpCreateStruct->lpszClass)
										ofs << "lpszClass: " << (std::string(lpCreateStruct->lpszClass)).c_str() << " ";
									else
										ofs << "lpszClass: <NULL> ";*/

					if (lpCreateStruct->lpszName)
						ofs << "lpszName: " << (std::string(lpCreateStruct->lpszName)).c_str() << " ";
					else
						ofs << "lpszName: <NULL> ";

					ofs << "style: " << lpCreateStruct->style << " "
						<< "x: " << lpCreateStruct->x << " "
						<< "y: " << lpCreateStruct->y << std::endl;
#endif
					if ((lpCreateStruct->hInstance) && (lpCreateStruct->hInstance != GetModuleHandleA("comdlg32.dll")))
						SetWindowSubclass(messageData->hwnd, WindowSubclass, 0, reinterpret_cast<DWORD_PTR>(WindowSubclass));
				}
			}
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
	
	LRESULT CALLBACK WindowSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		switch (uMsg)
		{
		case WM_CTLCOLOREDIT:
		{
			if (HDC hdc = reinterpret_cast<HDC>(wParam); hdc)
				return Theme::EditText::OnCtlColorEdit(hdc);
		}

		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		{
			if (HDC hdc = reinterpret_cast<HDC>(wParam); hdc)
			{
				SetTextColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Text_3));
				SetBkColor(hdc, Theme::GetThemeSysColor(Theme::ThemeColor_Default));
			}

			return reinterpret_cast<INT_PTR>(Theme::Comctl32GetSysColorBrush(COLOR_BTNFACE));
		}

		case WM_CTLCOLORLISTBOX:
		{
			if (HDC hdc = reinterpret_cast<HDC>(wParam); hdc)
				return Theme::ListBox::OnCtlColorListBox(hWnd, hdc);
		}
		}
		
		switch (uMsg)
		{
		case WM_INITDIALOG:
		case WM_CREATE:
		{
			DefSubclassProc(hWnd, uMsg, wParam, lParam);

			// Theme settings are initialized after WM_CREATE is processed

			HTHEME scrollBarTheme = NULL;
			auto themeType = GetThemeTypeFromWindow(hWnd);
			Core::Classes::UI::CUICustomWindow Window = hWnd;

			UINT uStyles = Window.Style;
			UINT uStylesEx = Window.StyleEx;
			std::string className = Window.Name;

			switch (themeType)
			{
			case ThemeType::MDIClient:
				Theme::MDIClient::Initialize(hWnd);
				break;
			case ThemeType::RichEdit20:
			case ThemeType::RichEdit50:
				{
					CHARFORMATA format = { 0 };
					format.cbSize = sizeof(CHARFORMATA);
					format.dwMask = CFM_COLOR;
					format.crTextColor = Theme::GetThemeSysColor(Theme::ThemeColor_Text_4);
					SendMessageA(hWnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&format);
					SendMessageA(hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format);
					SendMessageA(hWnd, EM_SETBKGNDCOLOR, FALSE, Theme::GetThemeSysColor(Theme::ThemeColor_Edit_Color));

					if (themeType == ThemeType::RichEdit20)
						scrollBarTheme = Theme::RichEdit::Initialize(hWnd);
				}
				break;
			case ThemeType::Button:
				if (((uStylesEx & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) && ((uStylesEx & WS_EX_STATICEDGE) == WS_EX_STATICEDGE))
				{
					// IMAGE (DirectX window)
					Theme::CustomBox::Initialize(hWnd, Theme::CustomBox::abColor);
				}
				break;
			case ThemeType::Edit:
				if (((uStyles & WS_VSCROLL) == WS_VSCROLL) && ((uStyles & WS_HSCROLL) == WS_HSCROLL))
					// this memo control
					scrollBarTheme = Theme::Memo::Initialize(hWnd);
				break;
			case ThemeType::ListBox:
				{
				scrollBarTheme = Theme::ListBox::Initialize(hWnd);

				LONG_PTR uID = GetWindowLongPtrA(hWnd, GWLP_ID);
				if (uID != 0x7D0 && uID != 0x7D1)
					PostMessageA(hWnd, WM_SETFONT, (WPARAM)listFont->Handle, TRUE);
				}
				break;
			case ThemeType::ListView:
				scrollBarTheme = Theme::ListView::Initialize(hWnd);
				PostMessageA(hWnd, WM_SETFONT, (WPARAM)listFont->Handle, TRUE);
				break;
			case ThemeType::TreeView:
				scrollBarTheme = Theme::TreeView::Initialize(hWnd);
				PostMessageA(hWnd, WM_SETFONT, (WPARAM)listFont->Handle, TRUE);
				break;
			case ThemeType::TabControl:
				Theme::PageControl::Initialize(hWnd); 
				break;
			case ThemeType::ToolBar:
				// Magic numbers to exclude paint of the standard dialog for opening and saving files
				if ((0x80 != uStylesEx) && (0x5600B84D != uStyles))
					Theme::ToolBar::Initialize(hWnd);
				break;
			case ThemeType::ProgressBar:
				Theme::ProgressBar::Initialize(hWnd);
				break;
			case ThemeType::PopupMenu:
				{
					Classes::CUIMonitor Monitor = Classes::Screen.MonitorFromWindow(hWnd);
					// Avoid the up and down arrows in PopupMenu
					if (Monitor.WorkAreaRect.Height > 768)
						Theme::PopupMenu::Initialize(hWnd);
				}
				break;
			case ThemeType::Spin:
				Theme::UpDown::Initialize(hWnd);
				break;
			default:
				// fix slowdown render window... exclude from subclassing
				if ((ThemeType::Static == themeType) && ((uStyles & SS_BLACKRECT) == SS_BLACKRECT) && (GetWindowLongPtrA(hWnd, GWLP_ID) == 0xA3B))
				{
					RemoveWindowSubclass(hWnd, WindowSubclass, 0);
					break;
				}
				else if ((uStyles & SS_SUNKEN) == SS_SUNKEN)
				{
					// 1. Label with frame
					// 2. ColorBox (and DialogColor)
					// 3. Edit (Disabled) (don't understand)	--- SS_EDITCONTROL
					// 4. IconBox (CK deprecated control)

					if ((uStyles & SS_BLACKFRAME) == SS_BLACKFRAME)
						// CK deprecated control (Icon)
						Theme::CustomBox::Initialize(hWnd, Theme::CustomBox::abNormal);
					else
					{
						// skip edit control 
						if (((uStyles & SS_EDITCONTROL) == SS_EDITCONTROL) || ((uStyles & ES_AUTOHSCROLL) == ES_AUTOHSCROLL) ||
							((uStyles & ES_AUTOVSCROLL) == ES_AUTOVSCROLL))
							break;

						if (((uStyles & SS_OWNERDRAW) == SS_OWNERDRAW) || ((uStyles & SS_BLACKRECT) == SS_BLACKRECT))
							// CK defined everywhere this component so, except for light (SS_BLACKRECT)
							Theme::CustomBox::Initialize(hWnd, Theme::CustomBox::abColor);
					}

					break;
				}
				else if (((uStyles & SS_CENTERIMAGE) == SS_CENTERIMAGE) && ((uStylesEx & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE))
				{
					// CK deprecated control (material count)
					Theme::CustomBox::Initialize(hWnd, Theme::CustomBox::abNormal);
				}
				else if ((ThemeType::Static == themeType) && (((uStylesEx & WS_EX_CLIENTEDGE) == WS_EX_CLIENTEDGE) || ((uStylesEx & WS_EX_STATICEDGE) == WS_EX_STATICEDGE)))
				{
					Theme::CustomBox::Initialize(hWnd, Theme::CustomBox::abNormal);
				}
				else if (((uStyles & WS_CAPTION) == WS_CAPTION) && ((uStyles & WS_CHILD) != WS_CHILD))
				{
					if ((uStyles & WS_SYSMENU) == WS_SYSMENU)
					{
						SetClassLongPtrA(hWnd, GCLP_HICON, (LONG)LoadIconA((HINSTANCE)g_ModuleBase, MAKEINTRESOURCEA(318)));
						SetClassLongPtrA(hWnd, GCLP_HICONSM, (LONG)LoadIconA((HINSTANCE)g_ModuleBase, MAKEINTRESOURCEA(318)));
					}
				}
				break;
			}
			
			if (scrollBarTheme)
				RegisterThemeHandle(scrollBarTheme, ThemeType::ScrollBar);
			else
				RegisterThemeHandle(hWnd, themeType);

			if (!PermanentWindowSubclasses.count(className))
				RemoveWindowSubclass(hWnd, WindowSubclass, 0);
		}
		return S_OK;

		case WM_INITMENUPOPUP:
		{
			Classes::CUIMonitor Monitor = Classes::Screen.MonitorFromWindow(hWnd);
			// Avoid the up and down arrows in PopupMenu
			if (Monitor.WorkAreaRect.Height > 768)
			{
				// The message about the initialization of the pop-up menu, set all its elements as owner draw
				Theme::PopupMenu::Event::OnInitPopupMenu(hWnd, (HMENU)wParam);

				return S_OK;
			}
		}
		break;

		case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;

			if (lpmis->CtlType == ODT_MENU)
			{
				// Calc size menu item
				Theme::PopupMenu::Event::OnMeasureItem(hWnd, lpmis);

				return TRUE;
			}
		}
		break;
		
		// WHAT? They use this message to render the preview
		// Fixed
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

			if (lpdis->CtlType == ODT_MENU)
			{
				// Paint menu item
				Theme::PopupMenu::Event::OnDrawItem(hWnd, lpdis);

				return TRUE;
			}		
		}
		break;

		case WM_NOTIFY:
		{
			LPNMHDR nmhdr = (LPNMHDR)lParam;

			// Custom drawing (ToolBar)
			if (nmhdr->code == NM_CUSTOMDRAW)
			{
				auto themeType = GetThemeTypeFromWindow(nmhdr->hwndFrom);

				switch (themeType)
				{
				case UITheme::ThemeType::ListView:
					return Theme::ListView::OnCustomDraw(hWnd, (LPNMLVCUSTOMDRAW)lParam);
				case UITheme::ThemeType::ToolBar:
					return Theme::ToolBar::OnCustomDraw(hWnd, (LPNMTBCUSTOMDRAW)lParam);
				}
			}
		}
		break;

		}

		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	DWORD FIXAPI Comctl32GetSysColor(INT nIndex)
	{
		return Theme::Comctl32GetSysColor(nIndex);
	}

	HBRUSH FIXAPI Comctl32GetSysColorBrush(INT nIndex)
	{
		return Theme::Comctl32GetSysColorBrush(nIndex);
	}

	HRESULT FIXAPI Comctl32DrawThemeText(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCWSTR pszText, INT cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect)
	{
		Classes::CUICanvas Canvas(hdc);
		Canvas.TransparentMode = TRUE;
		Canvas.Font.Assign(Theme::ThemeFont);

		if ((Theme::GetTheme() == Theme::Theme_Dark) || (Theme::GetTheme() == Theme::Theme_DarkGray))
		{
			// detected standart OS theme (comdlg32)
			COLORREF clTest = GetPixel(hdc, pRect->left + 2, pRect->top + 2);
			if (((GetRValue(clTest) + GetGValue(clTest) + GetBValue(clTest)) / 3) > 128)
				return DrawThemeText(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect);
		}

		auto themeType = ThemeType::None;
		if (auto itr = ThemeHandles.find(hTheme); itr != ThemeHandles.end())
			themeType = itr->second;

		if (ThemeType::StatusBar != themeType)
		{
			// no shadow
			Canvas.Font.Name = "MS Sans Serif";
		}

		switch (themeType)
		{
		case ThemeType::StatusBar:
			Theme::StatusBar::Event::OnBeforeDrawText(Canvas, dwTextFlags);
			break;
		case ThemeType::TabControl:
			Theme::PageControl::Event::OnBeforeDrawText(Canvas, dwTextFlags);
			break;
		case ThemeType::Button:
			{
			if (iPartId == BP_PUSHBUTTON)
				Theme::PushButton::Event::OnBeforeDrawText(Canvas, dwTextFlags, iStateId);
			else if (iPartId == BP_CHECKBOX)
				Theme::CheckBox::Event::OnBeforeDrawText(Canvas, dwTextFlags, iStateId);
			else if (iPartId == BP_RADIOBUTTON)
				Theme::RadioButton::Event::OnBeforeDrawText(Canvas, dwTextFlags, iStateId);
			else if (iPartId == BP_GROUPBOX)
				Theme::GroupBox::Event::OnBeforeDrawText(Canvas, dwTextFlags);
			}
			break;		
		case ThemeType::ComboBox:
			{
				Theme::ComboBox::Event::OnBeforeDrawText(Canvas, dwTextFlags, iStateId);
			}
			break;
		default:
			Canvas.ColorText = Theme::GetThemeSysColor(Theme::ThemeColor_Text_4);
			break;
		}			

		RECT rc = *pRect;
		Canvas.TextRect(rc, pszText, dwTextFlags);
		Canvas.TransparentMode = FALSE;

		return S_OK;
	}

	HRESULT FIXAPI Comctl32DrawThemeBackground(HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId, LPCRECT pRect, LPCRECT pClipRect)
	{
		auto themeType = ThemeType::None;

		if (auto itr = ThemeHandles.find(hTheme); itr != ThemeHandles.end())
			themeType = itr->second;

		Classes::CUICanvas Canvas(hdc);

		if (themeType == ThemeType::ScrollBar)
		{
			switch (iPartId)
			{
			case SBP_THUMBBTNHORZ:	// Horizontal drag bar
			{
				if (iStateId == SCRBS_HOT || iStateId == SCRBS_PRESSED)
					Theme::ScrollBar::Render::DrawSliderHorz_Focused(Canvas, pRect);
				else
					Theme::ScrollBar::Render::DrawSliderHorz_Normal(Canvas, pRect);
			}
			return S_OK;
			case SBP_THUMBBTNVERT:	// Vertical drag bar
			{
				if (iStateId == SCRBS_HOT || iStateId == SCRBS_PRESSED)
					Theme::ScrollBar::Render::DrawSliderVert_Focused(Canvas, pRect);
				else
					Theme::ScrollBar::Render::DrawSliderVert_Normal(Canvas, pRect);
			}
			return S_OK;

			case SBP_LOWERTRACKHORZ:// Horizontal background
			case SBP_UPPERTRACKHORZ:// Horizontal background
			{
				Theme::ScrollBar::Render::DrawBackgroundHorz(Canvas, pRect);
			}
			return S_OK;

			case SBP_LOWERTRACKVERT:// Vertical background
			case SBP_UPPERTRACKVERT:// Vertical background
			{
				Theme::ScrollBar::Render::DrawBackgroundVert(Canvas, pRect);
			}
			return S_OK;

			case SBP_ARROWBTN:		// Arrow button
			{
				// Assume the perspective of the arrow pointing upward ( /\ ) in GDI coordinates. NOTE: (0, 0) is the
				// top left corner of the screen. Awful code, but it works.
				const INT arrowWidth = std::ceil(std::abs(pRect->left - pRect->right) * 0.4f);
				const INT arrowHeight = std::ceil(std::abs(pRect->top - pRect->bottom) * 0.35f);

				std::array<DWORD, 6> counts{ 2, 2, 2, 2, 2, 2 };
				std::array<POINT, 12> verts
				{ {
						// Left segment
						{ 0, -0 },
						{ (arrowWidth / 2) + 1, -arrowHeight + 2 },

						{ 0, -1 },
						{ (arrowWidth / 2) + 1, -arrowHeight + 1 },

						{ 0, -2 },
						{ (arrowWidth / 2) + 1, -arrowHeight + 0 },

						// Right segment (final vertex Y adjusted to avoid a stray pixel)
						{ arrowWidth - 1, -0 },
						{ arrowWidth / 2, -arrowHeight + 2 },

						{ arrowWidth - 1, -1 },
						{ arrowWidth / 2, -arrowHeight + 1 },

						{ arrowWidth - 1, -2 },
						{ arrowWidth / 2, -arrowHeight + 1 },
					} };

				BOOL isHot = FALSE;
				BOOL isDisabled = FALSE;
				BOOL isHorz = FALSE;

				for (auto& vert : verts)
				{
					switch (iStateId)
					{
					case ABS_UPHOT:// Up
					case ABS_UPPRESSED:
						isHot = TRUE;
					case ABS_UPDISABLED:
						isDisabled = TRUE;
					case ABS_UPNORMAL:
					case ABS_UPHOVER:
						vert.x += pRect->left + arrowHeight - 1;
						vert.y += pRect->bottom - arrowHeight;
						break;

					case ABS_DOWNHOT:// Down
					case ABS_DOWNPRESSED:
						isHot = TRUE;
					case ABS_DOWNDISABLED:
						isDisabled = TRUE;
					case ABS_DOWNNORMAL:
					case ABS_DOWNHOVER:
						vert.x += pRect->left + arrowHeight - 1;
						vert.y = -vert.y + pRect->top + arrowHeight - 1;
						break;

					case ABS_LEFTHOT:// Left
					case ABS_LEFTPRESSED:
						isHot = TRUE;
					case ABS_LEFTDISABLED:
						isDisabled = TRUE;
					case ABS_LEFTNORMAL:
					case ABS_LEFTHOVER:
						isHorz = TRUE;
						std::swap(vert.x, vert.y);
						vert.x += pRect->right - arrowHeight;
						vert.y += pRect->top + arrowHeight - 1;
						break;

					case ABS_RIGHTHOT:// Right
					case ABS_RIGHTPRESSED:
						isHot = TRUE;
					case ABS_RIGHTDISABLED:
						isDisabled = TRUE;
					case ABS_RIGHTNORMAL:
					case ABS_RIGHTHOVER:
						isHorz = TRUE;
						std::swap(vert.x, vert.y);
						vert.x = -vert.x + pRect->left + arrowHeight - 1;
						vert.y += pRect->top + arrowHeight - 1;
						break;
					}
				}

				if (isHot)
				{
					Theme::ScrollBar::Render::DrawButton_Hot(Canvas, pRect);

					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);
				}
				else if (isDisabled)
				{
					Theme::ScrollBar::Render::DrawButton_Disabled(Canvas, pRect);

					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Disabled);
				}
				else
				{
					if (isHorz)
						Theme::ScrollBar::Render::DrawBackgroundHorz(Canvas, pRect);
					else
						Theme::ScrollBar::Render::DrawBackgroundVert(Canvas, pRect);

					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);
				}

				if (isDisabled)
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Shadow_Disabled);
				else
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Shadow);

				PolyPolyline(Canvas.Handle, verts.data(), counts.data(), counts.size());
				
				if (isHot)
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);
				else if (isDisabled)
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape_Disabled);
				else
					Canvas.Pen.Color = Theme::GetThemeSysColor(Theme::ThemeColor_Shape);

				for (auto i = verts.begin(); i != verts.end(); i++)
					(*i).y++;

				PolyPolyline(Canvas.Handle, verts.data(), counts.data(), counts.size());
			}
			return S_OK;

			case SBP_GRIPPERHORZ:	// Horizontal resize scrollbar
			case SBP_GRIPPERVERT:	// Vertical resize scrollbar
				break;
			case SBP_SIZEBOX:		// Resize box, bottom right
			case SBP_SIZEBOXBKGND:	// Resize box, background, unused
				Canvas.Fill(*pRect, Theme::GetThemeSysColor(Theme::ThemeColor_Default));
				return S_OK;
			}
		}
		else if (themeType == ThemeType::StatusBar)
		{
			switch (iPartId)
			{
			case 0:
			{
				// Outside border (top, right)
				Theme::StatusBar::Render::DrawBorder(Canvas, pRect);
			}
			return S_OK;

			case SP_PANE:
			case SP_GRIPPERPANE:
			case SP_GRIPPER:
			{
				// Everything else
				Theme::StatusBar::Render::DrawBackground(Canvas, pRect);
			}
			return S_OK;
			}
		}
		else if (themeType == ThemeType::Spin)
		{
			switch (iPartId)
			{
				case SPNP_UP:
				{
					switch (iStateId)
					{
					case DNS_HOT:
						Theme::UpDown::Render::DrawUp_Hot(Canvas, pRect);
						break;
					case DNS_PRESSED:
						Theme::UpDown::Render::DrawUp_Pressed(Canvas, pRect);
						break;
					case DNS_DISABLED:
						Theme::UpDown::Render::DrawUp_Disabled(Canvas, pRect);
						break;
					default:
						Theme::UpDown::Render::DrawUp_Normal(Canvas, pRect);
						break;
					}
				}
				return S_OK;

				case SPNP_DOWN:
				{
					switch (iStateId)
					{
					case UPS_HOT:
						Theme::UpDown::Render::DrawDown_Hot(Canvas, pRect);
						break;
					case UPS_PRESSED:
						Theme::UpDown::Render::DrawDown_Pressed(Canvas, pRect);
						break;
					case UPS_DISABLED:
						Theme::UpDown::Render::DrawDown_Disabled(Canvas, pRect);
						break;
					default:
						Theme::UpDown::Render::DrawDown_Normal(Canvas, pRect);
						break;
					}
				}
				return S_OK;
				
				case SPNP_UPHORZ:
				{
					switch (iStateId)
					{
					case UPHZS_HOT:
						Theme::UpDown::Render::DrawRight_Hot(Canvas, pRect);
						break;
					case UPHZS_PRESSED:
						Theme::UpDown::Render::DrawRight_Pressed(Canvas, pRect);
						break;
					case UPHZS_DISABLED:
						Theme::UpDown::Render::DrawRight_Disabled(Canvas, pRect);
						break;
					default:
						Theme::UpDown::Render::DrawRight_Normal(Canvas, pRect);
						break;
					}
				}
				return S_OK;

				case SPNP_DOWNHORZ:
				{
					switch (iStateId)
					{
					case DNHZS_HOT:
						Theme::UpDown::Render::DrawLeft_Hot(Canvas, pRect);
						break;
					case DNHZS_PRESSED:
						Theme::UpDown::Render::DrawLeft_Pressed(Canvas, pRect);
						break;
					case DNHZS_DISABLED:
						Theme::UpDown::Render::DrawLeft_Disabled(Canvas, pRect);
						break;
					default:
						Theme::UpDown::Render::DrawLeft_Normal(Canvas, pRect);
						break;
					}
				}
				return S_OK;
			}
		}
		else if (themeType == ThemeType::Edit)
		{
			switch (iPartId)
			{
			case EP_EDITBORDER_NOSCROLL:
			{
				switch (iStateId)
				{
				case ETS_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case ETS_SELECTED:
				case ETS_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case ETS_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case EP_EDITBORDER_VSCROLL:
			{
				switch (iStateId)
				{
				case EPSV_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case EPSV_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case EPSV_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case EP_EDITBORDER_HSCROLL:
			{
				switch (iStateId)
				{
				case EPSH_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case EPSH_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case EPSH_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case EP_EDITBORDER_HVSCROLL:
			{
				switch (iStateId)
				{
				case EPSHV_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case EPSHV_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				case EPSHV_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;
			}
		}
		else if (themeType == ThemeType::ProgressBar)
		{
			switch (iPartId)
			{
			case PP_BAR:
			case PP_TRANSPARENTBAR:
			{
				Theme::ProgressBar::Render::DrawBar(Canvas, pRect);
			}
			return S_OK;

			case PP_FILL:
			{
				Theme::ProgressBar::Render::DrawFill(Canvas, pRect);
			}
			return S_OK;
			}

			// skip other
			return S_OK;
		}
		else if (themeType == ThemeType::Button)
		{
			switch (iPartId)
			{
			case BP_PUSHBUTTON:
			{
				switch (iStateId)
				{
				case PBS_HOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case PBS_DISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case PBS_PRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				default:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case BP_GROUPBOX:
			{
				switch (iStateId)
				{
				case GBS_DISABLED:
					Theme::GroupBox::Render::DrawGroupBox_Disabled(Canvas, pRect);
					break;
				default:
					Theme::GroupBox::Render::DrawGroupBox_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case BP_CHECKBOX:
			{
				switch (iStateId)
				{
				case CBS_UNCHECKEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case CBS_UNCHECKEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				case CBS_UNCHECKEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case CBS_UNCHECKEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				case CBS_CHECKEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Disabled(Canvas, pRect);
					break;
				case CBS_CHECKEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Normal(Canvas, pRect);
					break;
				case CBS_CHECKEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Hot(Canvas, pRect);
					break;
				case CBS_CHECKEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					Theme::CheckBox::Render::DrawCheck_Pressed(Canvas, pRect);
					break;
				case CBS_MIXEDDISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					Theme::CheckBox::Render::DrawIdeterminate_Disabled(Canvas, pRect);
					break;
				case CBS_MIXEDNORMAL:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					Theme::CheckBox::Render::DrawIdeterminate_Normal(Canvas, pRect);
					break;
				case CBS_MIXEDHOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					Theme::CheckBox::Render::DrawIdeterminate_Hot(Canvas, pRect);
					break;
				case CBS_MIXEDPRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					Theme::CheckBox::Render::DrawIdeterminate_Pressed(Canvas, pRect);
					break;
				default:
					return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
				}
			}
			return S_OK;

			case BP_RADIOBUTTON:
			{
				switch (iStateId)
				{
				case RBS_UNCHECKEDDISABLED:
					Theme::RadioButton::Render::DrawPushButtonR_Disabled(Canvas, pRect);
					break;
				case RBS_UNCHECKEDNORMAL:
					Theme::RadioButton::Render::DrawPushButtonR_Normal(Canvas, pRect);
					break;
				case RBS_UNCHECKEDHOT:
					Theme::RadioButton::Render::DrawPushButtonR_Hot(Canvas, pRect);
					break;
				case RBS_UNCHECKEDPRESSED:
					Theme::RadioButton::Render::DrawPushButtonR_Pressed(Canvas, pRect);
					break;
				case RBS_CHECKEDDISABLED:
					Theme::RadioButton::Render::DrawPushButtonR_Disabled(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Disabled(Canvas, pRect);
					break;
				case RBS_CHECKEDNORMAL:
					Theme::RadioButton::Render::DrawPushButtonR_Normal(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Normal(Canvas, pRect);
					break;
				case RBS_CHECKEDHOT:
					Theme::RadioButton::Render::DrawPushButtonR_Hot(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Hot(Canvas, pRect);
					break; 
				case RBS_CHECKEDPRESSED:
					Theme::RadioButton::Render::DrawPushButtonR_Pressed(Canvas, pRect);
					Theme::RadioButton::Render::DrawRadioButton_Pressed(Canvas, pRect);
					break;
				default:
					return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
				}
			}
			return S_OK;

			}
		}
		else if (themeType == ThemeType::TrackBar)
		{
			switch (iPartId)
			{
			case TKP_TRACK:
			case TKP_TRACKVERT:
			{
				Theme::TrackBar::Render::DrawTrack(Canvas, pRect);
			}
			return S_OK;
			case TKP_THUMB:
			case TKP_THUMBVERT:
			case TKP_THUMBBOTTOM:
			case TKP_THUMBTOP:
			case TKP_THUMBLEFT:
			case TKP_THUMBRIGHT:
			{
				switch (iStateId)
				{
				case TUS_HOT:
					Theme::TrackBar::Render::DrawSlider_Hot(Canvas, pRect);
					break;
				case TUS_PRESSED:
					Theme::TrackBar::Render::DrawSlider_Pressed(Canvas, pRect);
					break;
				case TUS_FOCUSED:
					Theme::TrackBar::Render::DrawSlider_Focused(Canvas, pRect);
					break;
				case TUS_DISABLED:
					Theme::TrackBar::Render::DrawSlider_Disabled(Canvas, pRect);
					break;
				default:
					Theme::TrackBar::Render::DrawSlider_Normal(Canvas, pRect);
					break;
				}
			}
			}

			return S_OK;
		}
		else if (themeType == ThemeType::ComboBox)
		{
			switch (iPartId)
			{
			case CP_READONLY:			// Main control
			{
				switch (iStateId)
				{
				case CBRO_HOT:
					Theme::PushButton::Render::DrawPushButton_Hot(Canvas, pRect);
					break;
				case CBRO_DISABLED:
					Theme::PushButton::Render::DrawPushButton_Disabled(Canvas, pRect);
					break;
				case CBRO_PRESSED:
					Theme::PushButton::Render::DrawPushButton_Pressed(Canvas, pRect);
					break;
				default:
					Theme::PushButton::Render::DrawPushButton_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case CP_BORDER:				// Main control with text edit
			{
				// Special case: dropdown arrow needs to be drawn
				DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

				switch (iStateId)
				{
				case CBB_HOT:
					Theme::EditText::Render::DrawEditText_Hot(Canvas, pRect);
					break;
				case CBB_DISABLED:
					Theme::EditText::Render::DrawEditText_Disabled(Canvas, pRect);
					break;
				case CBB_FOCUSED:
					Theme::EditText::Render::DrawEditText_Focused(Canvas, pRect);
					break;
				default:
					Theme::EditText::Render::DrawEditText_Normal(Canvas, pRect);
					break;
				}
			}
			return S_OK;

			case CP_DROPDOWNBUTTONRIGHT:// Dropdown arrow
			case CP_DROPDOWNBUTTONLEFT:	// Dropdown arrow
				if (iStateId == CBXS_DISABLED)
					Theme::ComboBox::Render::DrawArrow_Disabled(Canvas, pRect);
				else
					Theme::ComboBox::Render::DrawArrow_Normal(Canvas, pRect);
				return S_OK;

			case CP_DROPDOWNBUTTON:
				Canvas.Fill(*pRect, RGB(255, 0, 0));
				return S_OK;
			case CP_BACKGROUND:
			case CP_TRANSPARENTBACKGROUND:
			case CP_CUEBANNER:
			case CP_DROPDOWNITEM:
				return S_OK;
			}
		}
		else if (themeType == ThemeType::Header)
		{
			switch (iPartId)
			{
			case 0:
			case HP_HEADERITEM:
			case HP_HEADERITEMLEFT:
			case HP_HEADERITEMRIGHT:
			{
				if ((iPartId == 0 && iStateId == HIS_NORMAL) ||
					(iPartId == HP_HEADERITEM && iStateId == HIS_NORMAL) ||
					(iPartId == HP_HEADERITEMLEFT && iStateId == HILS_NORMAL) ||
					(iPartId == HP_HEADERITEMRIGHT && iStateId == HIRS_NORMAL))
					Theme::Header::Render::DrawBack_Normal(Canvas, pRect);
				else if ((iPartId == 0 && iStateId == HIS_HOT) ||
					(iPartId == HP_HEADERITEM && iStateId == HIS_HOT) ||
					(iPartId == HP_HEADERITEMLEFT && iStateId == HILS_HOT) ||
					(iPartId == HP_HEADERITEMRIGHT && iStateId == HIRS_HOT))
					Theme::Header::Render::DrawBack_Hot(Canvas, pRect);
				else if ((iPartId == 0 && iStateId == HIS_PRESSED) ||
					(iPartId == HP_HEADERITEM && iStateId == HIS_PRESSED) ||
					(iPartId == HP_HEADERITEMLEFT && iStateId == HILS_PRESSED) ||
					(iPartId == HP_HEADERITEMRIGHT && iStateId == HIRS_PRESSED))
					Theme::Header::Render::DrawBack_Pressed(Canvas, pRect);
				else if ((iPartId == HP_HEADERITEM && iStateId == HIS_SORTEDNORMAL) ||
					(iPartId == HP_HEADERITEM && iStateId == HIS_SORTEDHOT) ||
					(iPartId == HP_HEADERITEM && iStateId == HIS_SORTEDPRESSED))
					Theme::Header::Render::DrawBack_Selected(Canvas, pRect);

				Theme::Header::Render::DrawDiv(Canvas, pRect);
			}
			return S_OK;

			case HP_HEADERSORTARROW:
			case HP_HEADERDROPDOWN:
			case HP_HEADERDROPDOWNFILTER:
				return S_OK;

			}
		}
		else if (themeType == ThemeType::TabControl)
		{
			switch (iPartId)
			{

			case TABP_PANE:
				Theme::PageControl::Render::DrawBorder(Canvas, pRect);
				break;
			case TABP_TABITEM:				// TCS_MULTILINE middle buttons
			case TABP_TABITEMLEFTEDGE:		// TCS_MULTILINE leftmost button
			case TABP_TABITEMRIGHTEDGE:		// TCS_MULTILINE rightmost button
			case TABP_TABITEMBOTHEDGE:		// TCS_MULTILINE ???
			case TABP_TOPTABITEM:			// Middle buttons
			case TABP_TOPTABITEMLEFTEDGE:	// Leftmost button
			case TABP_TOPTABITEMRIGHTEDGE:	// Rightmost button
			case TABP_TOPTABITEMBOTHEDGE:	// ???
				{
					Classes::CRECT rc = *pRect;

					BOOL isHover = (iPartId == TABP_TABITEM && iStateId == TIS_HOT) ||
						(iPartId == TABP_TABITEMLEFTEDGE && iStateId == TILES_HOT) ||
						(iPartId == TABP_TABITEMRIGHTEDGE && iStateId == TIRES_HOT) ||
						(iPartId == TABP_TABITEMBOTHEDGE && iStateId == TIBES_HOT) ||
						(iPartId == TABP_TOPTABITEM && iStateId == TTIS_HOT) ||
						(iPartId == TABP_TOPTABITEMLEFTEDGE && iStateId == TTILES_HOT) ||
						(iPartId == TABP_TOPTABITEMRIGHTEDGE && iStateId == TTIRES_HOT) ||
						(iPartId == TABP_TOPTABITEMBOTHEDGE && iStateId == TTIBES_HOT);

					BOOL isSelected = (iPartId == TABP_TABITEM && iStateId == TIS_SELECTED) ||
						(iPartId == TABP_TABITEMLEFTEDGE && iStateId == TILES_SELECTED) ||
						(iPartId == TABP_TABITEMRIGHTEDGE && iStateId == TIRES_SELECTED) ||
						(iPartId == TABP_TABITEMBOTHEDGE && iStateId == TIBES_SELECTED) ||
						(iPartId == TABP_TOPTABITEM && iStateId == TTIS_SELECTED) ||
						(iPartId == TABP_TOPTABITEMLEFTEDGE && iStateId == TTILES_SELECTED) ||
						(iPartId == TABP_TOPTABITEMRIGHTEDGE && iStateId == TTIRES_SELECTED) ||
						(iPartId == TABP_TOPTABITEMBOTHEDGE && iStateId == TTIBES_SELECTED);

					BOOL isDisabled = (iPartId == TABP_TABITEM && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TABITEMLEFTEDGE && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TABITEMRIGHTEDGE && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TABITEMBOTHEDGE && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TOPTABITEM && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TOPTABITEMLEFTEDGE && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TOPTABITEMRIGHTEDGE && iStateId == TIS_DISABLED) ||
						(iPartId == TABP_TOPTABITEMBOTHEDGE && iStateId == TIS_DISABLED);

					BOOL isFocused = (iPartId == TABP_TABITEM && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TABITEMLEFTEDGE && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TABITEMRIGHTEDGE && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TABITEMBOTHEDGE && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TOPTABITEM && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TOPTABITEMLEFTEDGE && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TOPTABITEMRIGHTEDGE && iStateId == TIS_FOCUSED) ||
						(iPartId == TABP_TOPTABITEMBOTHEDGE && iStateId == TIS_FOCUSED);

					if (isSelected || isFocused)
						Theme::PageControl::Render::DrawButton_SelectedAndFocused(Canvas, pRect);
					else if (isHover)
						Theme::PageControl::Render::DrawButton_Hot(Canvas, pRect);
					else if (isDisabled)
						Theme::PageControl::Render::DrawButton_Disabled(Canvas, pRect);
					else
						Theme::PageControl::Render::DrawButton_Normal(Canvas, pRect);
				}
				break;
			}

			return S_OK;
		}

		return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
	}
}