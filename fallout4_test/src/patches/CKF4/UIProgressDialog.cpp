#include "Editor.h"
#include "EditorUI.h"
#include "UIProgressDialog.h"
#include "LogWindow.h"
#include <CommCtrl.h>

#define MIN_STATUSBAR_VALUE 0
#define MAX_STATUSBAR_VALUE 94						// to 95%
#define UI_CONTROL_ID_LABEL_DLGPROGRESS		2217
#define UI_CONTROL_ID_PROGRESS_DLGPROGRESS	31007

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			CUIProgressDialog* ProgressDialog = NULL;

			LRESULT WINAPI DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{	
				if(ProgressDialog)
					return ProgressDialog->WndProc(hWnd, uMsg, wParam, lParam);

				return TRUE;
			}

			CUIProgressDialog::CUIProgressDialog(CUICustomWindow* parent, const UINT res_id) :
				CUICustomDialog(parent, res_id, DlgProc)
			{
				// Enable message patch
				LogWindow::Log("FIXES: Enable patch 'MessagePool'.");
				bAllowPoolMessage = TRUE;
			}

			CUIProgressDialog::~CUIProgressDialog(VOID)
			{
				SetMarquee(FALSE);
				FreeRelease();

				// Disable message patch
				bAllowPoolMessage = FALSE;
				LogWindow::Log("FIXES: Disable patch 'MessagePool'.");
			}

			VOID CUIProgressDialog::SetMinAndMax(const UINT16 min, const UINT16 max)
			{
				m_Min = std::max(min, (UINT16)0);
				m_Max = std::max(max, (UINT16)1);

				Assert(m_Max != m_Min);

				m_Progress.Perform(PBM_SETRANGE, 0, MAKELPARAM(m_Min, m_Max));
				m_Progress.Perform(PBM_SETPOS, min, 0);
			}

			UINT16 CUIProgressDialog::GetPosition(VOID)
			{
				return (UINT16)m_Progress.Perform(PBM_GETPOS, 0, 0);
			}

			VOID CUIProgressDialog::SetPosition(const UINT16 value)
			{
				if (m_Marquee) return;
				m_Progress.Perform(PBM_SETPOS, value, 0);
				// Fake run message pool
				PatchMessage();
			}

			VOID CUIProgressDialog::SetMarquee(const BOOL value)
			{
				if (m_Marquee == value) return;

				m_Marquee = value;
				if (m_Marquee)
				{
					m_Progress.Style = WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_MARQUEE;
					SafePos = GetPosition();
					SetPosition(0);
					SetTimer(m_hWnd, UI_PROGRESS_CMD_STEPIT, 25, NULL);
				}
				else
				{
					KillTimer(m_hWnd, UI_PROGRESS_CMD_STEPIT);
					m_Progress.Style = WS_CHILD | WS_VISIBLE | PBS_SMOOTH;
					SetPosition(SafePos);
				}

				m_Progress.Perform(PBM_SETMARQUEE, (WPARAM)m_Marquee, NULL);
			}

			BOOL CUIProgressDialog::GetMarquee(VOID)
			{
				return m_Marquee;
			}

			std::string CUIProgressDialog::GetMessageText(VOID)
			{
				return m_Label.Caption;
			}

			VOID CUIProgressDialog::SetMessageText(const std::string &str)
			{
				m_Label.Caption = str;
			}

			UINT16 CUIProgressDialog::GetMin(VOID)
			{
				return m_Min;
			}

			VOID CUIProgressDialog::SetMin(const UINT16 value)
			{
				UINT16 m_newMin = std::max(value, (UINT16)0);
				if (m_newMin == value) 
					return;

				m_Min = m_newMin;
				Assert(m_Max != m_Min);

				m_Progress.Perform(PBM_SETRANGE, 0, MAKELPARAM(m_Min, m_Max));
			}

			UINT16 CUIProgressDialog::GetMax(VOID)
			{
				return m_Max;
			}

			VOID CUIProgressDialog::SetMax(const UINT16 value)
			{
				UINT16 m_newMax = std::max(value, (UINT16)1);
				if (m_newMax == value)
					return;

				m_Max = m_newMax;
				Assert(m_Max != m_Min);

				m_Progress.Perform(PBM_SETRANGE, 0, MAKELPARAM(m_Min, m_Max));
			}

			UINT16 CUIProgressDialog::GetIncrement(VOID)
			{
				return m_Progress.Perform(PBM_GETSTEP, 0, 0);
			}

			VOID CUIProgressDialog::SetIncrement(const UINT16 value)
			{
				m_Progress.Perform(PBM_SETSTEP, value, 0);
			}

			VOID CUIProgressDialog::Step(VOID)
			{
				m_Progress.Perform(PBM_STEPIT, 0, 0);
			}

			VOID FIXAPI CUIProgressDialog::ProcessMessages(VOID)
			{
				if (ProgressDialog)
					// Fake run message pool
					PatchMessage();
				else
					Sleep(1);
			}

			VOID FIXAPI CUIProgressDialog::ProcessMessagesOnlyLoadCellWorld(VOID)
			{
				if (ProgressDialog)
					// Fake run message pool
					PatchMessage();
			}

			LRESULT CUIProgressDialog::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			{
				switch (uMsg)
				{
					case WM_INITDIALOG:
					{
						m_hWnd = hWnd;
						m_Marquee = FALSE;
						m_Progress = GetControl(UI_CONTROL_ID_PROGRESS_DLGPROGRESS);
						m_Label = GetControl(UI_CONTROL_ID_LABEL_DLGPROGRESS);
						SetMinAndMax(MIN_STATUSBAR_VALUE, MAX_STATUSBAR_VALUE);
						MessageText = "";
						Increment = 1;

						break;
					}

					case UI_PROGRESS_CMD_STEPIT:
					{
						Step();

						break;
					}
				}

				return FALSE;
			}
		}
	}
}