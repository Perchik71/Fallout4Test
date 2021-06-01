#include "JsonDialogGenerator.h"
#include "LogWindow.h"
#include "UIGraphics.h"

#include "..\..\profiler_internal.h"

#include "..\..\..\..\Dependencies\json\single_include\nlohmann\json.hpp"
#include "..\..\..\..\Dependencies\zipper\src\zip.h"

#include <filesystem>

#define ARCHIVE_DIALOGS_FILENAME "data/f4ckfixes/dialogs.zip"
#define FONTNAME "Microsoft Sans Serif"

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			using json = nlohmann::json;
			namespace fs = std::filesystem;

			LPJsonDialogManager g_JsonDialogManager = NULL;

			LPWORD lpwAlign(LPWORD lpIn)
			{
				ULONG ul;
				ul = (ULONG)lpIn;
				ul += 3;
				ul >>= 2;
				ul <<= 2;
				return (LPWORD)ul;
			}

			BOOL CJsonDialogManager::AddDialog(const char* json_code, const LONG uid, const CJsonDialogType type)
			{
				if (type == CJsonDialogType::jdtUnknown)
					return FALSE;

				auto jDialog = json::parse(json_code);
				if (jDialog.empty())
					return FALSE;

				WORD fsize = 8;
				// Font point size.
				switch (type)
				{
				case Core::Classes::UI::jdt10pt:
					fsize = 10;
					break;
				case Core::Classes::UI::jdt12pt:
					fsize = 12;
					break;
				}

				CUICanvas* Canvas = new CUICanvas(GetDC(GetDesktopWindow()));

				Canvas->Font.Name = FONTNAME;
				Canvas->Font.Styles = {};
				Canvas->Font.Size = fsize;

				INT cxDlgUnit;
				INT cyDlgUnit;
				Canvas->TextSize("X", cxDlgUnit, cyDlgUnit);
				cxDlgUnit = cxDlgUnit >> 1;
				cyDlgUnit = cyDlgUnit >> 1;
			
				ReleaseDC(GetDesktopWindow(), Canvas->Handle);
				delete Canvas;

				std::string sTitle = jDialog["title"].get<std::string>();
				std::string sFace = FONTNAME;
				
				const size_t titleSize = sTitle.length() + 1;
				const size_t fontSize = sFace.length() + 1;
				const size_t dataSize = sizeof(DLGTEMPLATE) +
					sizeof(CHAR) +								// Menu array
					sizeof(CHAR) +								// Class array
					titleSize +									// Title array
					sizeof(WORD) +								// Font point size
					fontSize;									// Font array

				LPDLGITEMTEMPLATE lpdit;
				LPDLGTEMPLATE hgbl = (LPDLGTEMPLATE)GlobalAlloc(GMEM_ZEROINIT, dataSize + (512 * jDialog["items"].size()));
				if (!hgbl)
					return FALSE;

				LPDLGTEMPLATE dt = (LPDLGTEMPLATE)GlobalLock(hgbl);
				dt->style = jDialog["style"].get<DWORD>() | DS_SETFONT | WS_VISIBLE;
				dt->dwExtendedStyle = jDialog["styleex"].get<DWORD>();
				dt->cdit = jDialog["items"].size();
				dt->x = (jDialog["x"].get<SHORT>() * 4) / cxDlgUnit;
				dt->y = (jDialog["y"].get<SHORT>() * 8) / cyDlgUnit;
				dt->cx = (jDialog["width"].get<SHORT>() * 4) / cxDlgUnit;
				dt->cy = (jDialog["height"].get<SHORT>() * 8) / cyDlgUnit;

				BYTE* dtExtra = (BYTE*)dt + sizeof(DLGTEMPLATE);

				// Menu array.
				*(CHAR*)dtExtra = '\0';
				dtExtra += sizeof(CHAR);
				// Class array.
				*(CHAR*)dtExtra = '\0';
				dtExtra += sizeof(CHAR);
				// Title array.
				memcpy(dtExtra, sTitle.data(), titleSize);
				dtExtra += titleSize;
				// Font point size.
				*(WORD*)dtExtra = fsize;
				dtExtra += sizeof(WORD);
				// Font array.
				memcpy(dtExtra, sFace.data(), fontSize);

				LPWORD lpw = (LPWORD)dtExtra;
				auto items = jDialog["items"];
				for (json::iterator it = items.begin(); it != items.end(); ++it)
				{
					lpdit = (LPDLGITEMTEMPLATE)lpwAlign(lpw);
					lpdit->x = (it.value()["x"].get<SHORT>() * 4) / cxDlgUnit;
					lpdit->y = (it.value()["y"].get<SHORT>() * 8) / cyDlgUnit;
					lpdit->cx = (it.value()["width"].get<SHORT>() * 4) / cxDlgUnit;
					lpdit->cy = (it.value()["height"].get<SHORT>() * 8) / cyDlgUnit;
					lpdit->id = it.value()["id"].get<WORD>();
					lpdit->style = WS_CHILD | it.value()["style"].get<DWORD>();

					lpw = (LPWORD)(lpdit + 1);
					*lpw++ = 0xFFFF;

					auto item_type = it.value()["type"].get<std::string>();
					if (!stricmp(item_type.data(), "PUSHBUTTON"))
						*lpw++ = 0x0080;
					else if (!stricmp(item_type.data(), "EDITTEXT"))
						*lpw++ = 0x0081;
					else if (!stricmp(item_type.data(), "LTEXT"))
						*lpw++ = 0x0082;
					else if (!stricmp(item_type.data(), "PUSHBUTTON"))
						*lpw++ = 0x0083;
					else if (!stricmp(item_type.data(), "PUSHBUTTON"))
						*lpw++ = 0x0084;
					else if (!stricmp(item_type.data(), "COMBOBOX"))
						*lpw++ = 0x0085;
					else
						*lpw++ = 0x0086;


							/* static class */

					auto caption = it.value()["id"].get<std::string>();
					memcpy((LPWSTR)lpw, caption.data(), caption.length());
					lpw += 1 + caption.length();
				}
				
				/*
				0x0080	Button
	Edit
	Static
	List box
	Scroll bar
0x0085

	        2581,29,16,61,12,ES_AUTOHSCROLL
	CONTROL         "",2093,"SysTreeView32",TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | WS_BORDER | WS_HSCROLL | WS_TABSTOP,0,42,104,191,WS_EX_CLIENTEDGE
	CONTROL         "List2",1041,"SysListView32",LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS | WS_TABSTOP,119,0,245,233,WS_EX_CLIENTEDGE
	           "",2157,105,0,10,233,NOT WS_GROUP
	LTEXT           "Filter",-1,3,18,19,10
	        6024,29,2,61,98,CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_SORT | WS_VSCROLL | WS_TABSTOP
	LTEXT           "Layout",-1,3,4,23,11
	PUSHBUTTON      "...",6025,93,1,11,14
	PUSHBUTTON      "-",6027,93,15,11,14
	CONTROL         "Show only active (*) forms",2579,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,3,30,101,9
				*/


				GlobalUnlock(hgbl);

				return TRUE;
			}

			VOID CJsonDialogManager::Init(VOID)
			{
				// If there is no archive, this will cause an error and can't run the CK.
				AssertMsgVa(fs::exists(fs::absolute(ARCHIVE_DIALOGS_FILENAME)), "File \"%s\" no found", ARCHIVE_DIALOGS_FILENAME);

				LogWindow::Log("Initialize JSON dialog manager");
				// open archive
				struct zip_t* zip = zip_open(ARCHIVE_DIALOGS_FILENAME, 0, 'r');
				AssertMsgVa(zip, "File \"%s\" corrupted or not an archive", ARCHIVE_DIALOGS_FILENAME);
				LogWindow::Log("Initialize JSON dialog manager");
				// reading the archive and retrieving data
				CJsonDialogType jdtType = CJsonDialogType::jdtUnknown;
				std::string sJson, sName;
				INT nCount = zip_entries_total(zip);
				for (INT i = 0; i < nCount; ++i)
				{
					zip_entry_openbyindex(zip, i);
					// skip dirs
					if (zip_entry_isdir(zip) != 0)
						continue;
					
					sName = zip_entry_name(zip);
					if (sName.find_first_of('-') == std::string::npos)
						LogWindow::Log("JSON: Incorrect filename \"%s\"", sName.c_str());
					else
					{
						size_t nSize = zip_entry_size(zip);
						if (!nSize) 
							continue;
						// read
						char* buf = NULL;
						size_t bufsize = 0;
						if (zip_entry_read(zip, (void**)&buf, &bufsize) != nSize)
							LogWindow::Log("JSON: Error read file \"%s\"", sName.c_str());
						//else if (zip_entry_crc32(zip) != CRC32(buf))
						//	LogWindow::Log("JSON: Incorrect CRC32 \"%s\"", sName.c_str());
						else
						{
							auto AtuID = sName.find_first_of('-');
							std::string sID = sName.substr(0, AtuID);
							auto AtExt = sName.find_first_of('.');
							std::string sType = sName.substr(AtuID + 1, AtExt);
							std::string sExt = sName.substr(AtExt + 1);
							
							switch (strtol(sType.data(), NULL, 10))
							{
							case 8:
								jdtType = CJsonDialogType::jdt8pt;
								break;
							case 10:
								jdtType = CJsonDialogType::jdt10pt;
								break;
							case 12:
								jdtType = CJsonDialogType::jdt12pt;
								break;
							default:
								jdtType = CJsonDialogType::jdtUnknown;
								break;
							}

							if (jdtType == CJsonDialogType::jdtUnknown)
								LogWindow::Log("JSON: Unknown dialog type %s \"%s\"", sType.c_str(), sName.c_str());
							else if (strnicmp(sExt.c_str(), "json", 8))
								LogWindow::Log("JSON: file \"%s\" not JSON", sName.c_str());
							else if (!AddDialog(buf, strtol(sID.data(), NULL, 10), jdtType))
								LogWindow::Log("JSON: Incorrect JSON data in file \"%s\"", sName.c_str());
						}

						if (buf)
							free(buf);
					}
				}
			}

			VOID CJsonDialogManager::Release(VOID)
			{
				LogWindow::Log("Shutdown JSON dialog manager");
			}

			CJsonDialogManager::CJsonDialogManager(VOID)
			{
				Init();
			}

			CJsonDialogManager::~CJsonDialogManager(VOID)
			{
				Release();
			}
		}
	}
}