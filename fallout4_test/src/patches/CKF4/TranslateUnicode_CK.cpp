#define _CRT_SECURE_NO_WARNINGS

#include "TranslateUnicode_CK.h"

#include <CommCtrl.h>

#define UI_RESID_AUTHOR_PLUGIN			1025
#define UI_RESID_DESCRIPTION_PLUGIN		1024

namespace Core
{
	namespace Patch
	{
		namespace CreationKit
		{
			CTranslateUnicodeMediator Translator;

			CTranslateUnicodeMediator::CTranslateUnicodeMediator(VOID)
			{
				m_Mode = FALSE;
				m_preaddres = NULL;
			}

			CTranslateUnicodeMediator::~CTranslateUnicodeMediator(VOID)
			{
				heap.clear();
			}

			VOID CTranslateUnicodeMediator::SetMode(const BOOL mode)
			{
				m_Mode = mode;

				if (!m_Mode)
				{
					heap.clear();
					m_preaddres = NULL;
				}
			}

			BOOL CTranslateUnicodeMediator::IsValid(LPCSTR addr, const BOOL no_ansi)
			{
				if (no_ansi)
				{
					BOOL bRes = (addr != NULL) && (addr != LPSTR_TEXTCALLBACKA);
					if (bRes)
					{
						// Don't check the entire line, it's very long and unnecessary.
						// Yes, I admit, there will be omissions, but extremely short lines like F0 to F12 and some small things

						size_t l = strlen(addr), limit = 5, lim = 0;
						if (l == 0) return FALSE;

						if (limit > l) limit = l;

						for (size_t i = 0; i < l; i++)
						{
							if ((addr[i] >= 0x41) && (addr[i] <= 0x7E))
								lim++;

							if (limit == lim) return FALSE;
						}

						return TRUE;
					}
				}

				return ((addr != NULL) && (addr != LPSTR_TEXTCALLBACKA) && (strlen(addr) > 0));
			}

			LPSTR CTranslateUnicodeMediator::Utf8ToWinCP(LPCSTR src)
			{
				// Ansi verification is necessary because there are a lot of strings, especially short and system strings. 
				// The debug file without this option was more than 70 mb, compared to 2604 kb.
				// Translation of fallout4.esm has become significantly faster.

				if ((src == m_preaddres) || !IsValid(src, FALSE) || !XUtil::Conversion::IsUtf8Valid(src))
					return const_cast<LPSTR>(src);

				std::string wincp_str = XUtil::Conversion::Utf8ToAnsi(src);

#ifdef __DBG_TRAN_UNICODE_READ
				out.open(L"dbg_tu_read.txt", std::ios::app);
				out << wincp_str << std::endl;
				out.close();
#endif // __DBG_TRAN_UNICODE_READ

				// utf-8 takes up more memory than ansi, so I can use active memory
				m_preaddres = const_cast<LPSTR>(src);
				strcpy(m_preaddres, wincp_str.c_str());

				return m_preaddres;
			}

			LPSTR CTranslateUnicodeMediator::WinCPToUtf8(LPCSTR src)
			{
				// Not all strings are translated during loading and remain in Utf-8. 
				// They are loaded after opening the dialog. As an example "Description".

				if (!IsValid(src) || XUtil::Conversion::IsUtf8Valid(src))
					return const_cast<LPSTR>(src);

				// in the Creation Kit code, the request to return a string occurs twice in a row.
				if ((m_preaddres == src) && (heap.size() > 0))
					return &heap.back()[0];

				// convert
				std::string utf8_str = XUtil::Conversion::AnsiToUtf8(src);

#ifdef __DBG_TRAN_UNICODE_WRITE
				out.open("dbg_tu_write.txt", std::ios::app);
				out << utf8_str << " " << src << std::endl;
				out.close();
#endif // __DBG_TRAN_UNICODE_WRITE

				// Unicode initially takes up more memory than ansi. 
				// Therefore, a heap is created that will store memory for the duration of saving.
				// Lists work quickly on adding items.
				heap.push_back(utf8_str);

				// pointer to the memory of the contents of the last line, it is of course unique
				m_preaddres = const_cast<LPSTR>(src);
				return &heap.back()[0];
			}

			LPSTR CTranslateUnicodeMediator::Translate(LPCSTR src)
			{
				// Back to utf-8 (temporarily)
				if (m_Mode)
					return WinCPToUtf8(src);
				// All APIs are implemented as ANSI. In this regard, utf-8 must be converted to the specified ANSI.
				else
					return Utf8ToWinCP(src);
			}
		}
	}
}

namespace Experimental
{
	INT32 FIXAPI BeginPluginSave(VOID)
	{
		Core::Patch::CreationKit::Translator.SetMode(TRUE);

		// need return 1 in rax
		return 1;
	}

	LPVOID FIXAPI EndPluginSave(LPVOID __arg1)
	{
		LPVOID ret = ((LPVOID(__cdecl*)(LPVOID))OFFSET(0x2A81F, 0))(__arg1);
		Core::Patch::CreationKit::Translator.SetMode(FALSE);

		// return HCURSOR
		return ret;
	}

	LPSTR FIXAPI Translate(LPSTR str)
	{
		return Core::Patch::CreationKit::Translator.Translate(str);
	}

	BOOL WINAPI hk_SetDlgItemTextA(HWND hDlg, INT32 nIDDlgItem, LPCSTR lpString)
	{
		switch (nIDDlgItem)
		{
		case UI_RESID_AUTHOR_PLUGIN:
		case UI_RESID_DESCRIPTION_PLUGIN:
		{
			if (!lpString || !XUtil::Conversion::IsUtf8Valid(lpString))
				goto SetTextDef;

			std::string wincp_str = XUtil::Conversion::Utf8ToAnsi(lpString);
			return SetDlgItemTextA(hDlg, nIDDlgItem, wincp_str.c_str());
		}
		default:
		SetTextDef:
			return SetDlgItemTextA(hDlg, nIDDlgItem, lpString);
		}
	}

	LRESULT WINAPI hk_SendDlgItemMessageA(HWND hDlg, INT32 nIDDlgItem, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg != WM_GETTEXT && Msg != WM_GETTEXTLENGTH)
			MsgTextDef:
			return SendDlgItemMessageA(hDlg, nIDDlgItem, Msg, wParam, lParam);

		HWND hCtrlWnd;

		switch (nIDDlgItem)
		{
		case UI_RESID_AUTHOR_PLUGIN:
		case UI_RESID_DESCRIPTION_PLUGIN:
		{
			hCtrlWnd = GetDlgItem(hDlg, nIDDlgItem);
			INT32 maxlen = GetWindowTextLengthA(hCtrlWnd) << 2;

			if (maxlen <= 0)
				goto MsgTextDef;

			std::string ansi_str;
			ansi_str.resize(maxlen);
			ansi_str.resize(GetWindowTextA(hCtrlWnd, &ansi_str[0], maxlen));

			if (XUtil::Conversion::IsUtf8Valid(ansi_str))
				goto MsgTextDef;

			std::string utf8_str = XUtil::Conversion::AnsiToUtf8(ansi_str);

			if (Msg == WM_GETTEXT)
				strncpy((LPSTR)(lParam), utf8_str.c_str(), wParam);

			return utf8_str.length() + 1;
		}
		default:
			goto MsgTextDef;
		}
	}
}

