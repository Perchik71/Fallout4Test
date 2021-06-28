#include "..\..\..\common.h"
#include "..\UIBaseWindow.h"
#include "..\MainWindow.h"
#include "VarCommon.h"
#include "TimeOfDay.h"

#include <CommCtrl.h>

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace TimeOfDay
			{
				UITimeOfDayComponents OldUITimeOfDayComponents;
				UITimeOfDayComponents NewUITimeOfDayComponents;

				static LPSTR lpTimeOfDayClass = "TimeOfDayClass";

				HWND FIXAPI Initialization(HWND hWnd)
				{
					OldUITimeOfDayComponents.hWndToolBar = hWnd;
					NewUITimeOfDayComponents.hWndToolBar = hWnd;

					WNDCLASSEXA wc = { 0 };

					wc.cbSize = sizeof(wc);
					wc.hbrBackground = (HBRUSH)Comctl32GetSysColorBrushEx(COLOR_BTNFACE);
					wc.hInstance = GetModuleHandleA(NULL);
					wc.style = CS_HREDRAW | CS_VREDRAW;
					wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
					wc.lpszClassName = lpTimeOfDayClass;
					wc.lpfnWndProc = (WNDPROC)&TimeOfDayClassWndProc;

					Assert(RegisterClassExA(&wc));

					// To organize the normal operation of another list of components, 
					// you need to group them into a separate window with the processing of messages, 
					// I don't know what the developers were thinking, but not for nothing was the behavior of this thing ambiguous, 
					// not to mention the rendering of the component itself

					HWND hPanel = CreateWindowExA(0, wc.lpszClassName, "", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 1130, 2, 250, 24, hWnd, (HMENU)NULL, wc.hInstance, NULL);

					NewUITimeOfDayComponents.hWndLabel = CreateWindowExA(0, WC_STATICA, "Time of day", WS_CHILD | WS_VISIBLE | SS_CENTER, 0, 5, 90, 20, hPanel, (HMENU)0x16D2, wc.hInstance, NULL);
					NewUITimeOfDayComponents.hWndTrackBar = CreateWindowExA(0, TRACKBAR_CLASSA, "", WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_NOTICKS, 90, 3, 110, 18, hPanel, (HMENU)0x16D3, wc.hInstance, NULL);
					NewUITimeOfDayComponents.hWndEdit = CreateWindowExA(0, WC_STATICA, "0.00", WS_CHILD | WS_VISIBLE | ES_CENTER | ES_READONLY, 200, 5, 50, 20, hPanel, (HMENU)0x16D4, wc.hInstance, NULL);

					Assert((NewUITimeOfDayComponents.hWndLabel.Handle) && (NewUITimeOfDayComponents.hWndTrackBar.Handle) && (NewUITimeOfDayComponents.hWndEdit.Handle));

					ThemeFont->Apply(NewUITimeOfDayComponents.hWndLabel.Handle);
					ThemeFont->Apply(NewUITimeOfDayComponents.hWndEdit.Handle);

					return hPanel;
				}

				LRESULT CALLBACK TimeOfDayClassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
				{
					PAINTSTRUCT ps;
					HDC hdc;

					static NMHDR hdr;

					switch (uMsg)
					{
					case WM_CTLCOLOREDIT:
					case WM_CTLCOLORLISTBOX:
					case WM_CTLCOLORBTN:
					case WM_CTLCOLORDLG:
					case WM_CTLCOLORSTATIC:
					{
						if (hdc = reinterpret_cast<HDC>(wParam); hdc)
						{
							SetTextColor(hdc, GetThemeSysColor(ThemeColor_Text_3));
							SetBkColor(hdc, GetThemeSysColor(ThemeColor_Default));
						}

						return reinterpret_cast<INT_PTR>(Comctl32GetSysColorBrush(COLOR_BTNFACE));
					case WM_HSCROLL:
						switch (LOWORD(wParam)) {
						case SB_ENDSCROLL:
						case SB_LEFT:
						case SB_RIGHT:
						case SB_LINELEFT:
						case SB_LINERIGHT:
						case SB_PAGELEFT:
						case SB_PAGERIGHT:
						case SB_THUMBTRACK:
						case SB_THUMBPOSITION:
							if (NewUITimeOfDayComponents.hWndTrackBar.Handle == (HWND)lParam)
							{
								hdr.code = NM_RELEASEDCAPTURE;
								hdr.hwndFrom = (HWND)OldUITimeOfDayComponents.hWndTrackBar.Handle;
								hdr.idFrom = 0x16D3;

								LONG lPos = (LONG)SendMessageA((HWND)lParam, TBM_GETPOS, 0, 0);
								OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_SETPOS, TRUE, lPos);

								// fake change time of day
								MainWindow::GetWindowObj().Perform(WM_NOTIFY, 0x16D3, (LPARAM)& hdr);
								MainWindow::GetWindowObj().Perform(WM_NOTIFY, NM_RELEASEDCAPTURE, (LPARAM)& hdr);

								POINT Range = {
									(LONG)OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMIN, 0, 0),
									(LONG)OldUITimeOfDayComponents.hWndTrackBar.Perform(TBM_GETRANGEMAX, 0, 0)
								};

								CHAR szBuf[35] = { 0 };
								FLOAT v = (24.0 * lPos) / (Range.y - Range.x);
								sprintf_s(szBuf, "%.2f", v);
								NewUITimeOfDayComponents.hWndEdit.Caption = szBuf;
							}
							break;
						}
						break;
					}
					case WM_PAINT:
						hdc = BeginPaint(hWnd, &ps);
						EndPaint(hWnd, &ps);
						break;
					default:
						return DefWindowProcA(hWnd, uMsg, wParam, lParam);
					}

					return S_OK;
				}
			}
		}
	}
}