#include "LogWindow.h"
#include "UIGraphics.h"
#include "UIDialogManager.h"

#include "..\..\profiler_internal.h"
#include "..\..\..\..\Dependencies\zipper\src\zip.h"

#include <filesystem>
#include <fstream>

#define PATH_DIALOGS_SYSTEM "data/f4ckfixes"
#define PATH_DIALOGS "dia"
#define FONTNAME "Microsoft Sans Serif"


Core::Classes::UI::LPDialogManager g_DialogManager = NULL;

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			namespace fs = std::filesystem;
			using namespace std;

			CDialogFontType WINAPI GetDialogType(const string& text)
			{
				switch (strtol(text.data(), NULL, 10))
				{
				case 8:
					return CDialogFontType::jdt8pt;
			/*	case 9:
					return CDialogFontType::jdt9pt;*/
				case 10:
					return CDialogFontType::jdt10pt;
				default:
					return CDialogFontType::jdtUnknown;
				}
			}

			BOOL CDialogManager::IsDialog(const LONG uid, const CDialogFontType type)
			{
				tbb_map* map = NULL;

				switch (type)
				{
				case Core::Classes::UI::jdt8pt:
					map = &m_items_8pt;
					break;
		/*		case Core::Classes::UI::jdt9pt:
					map = &m_items_9pt;
					break;*/
				case Core::Classes::UI::jdt10pt:
					map = &m_items_10pt;
					break;
				}

				if (!map)
					return FALSE;

				return map->find(uid) != map->end();
			}

			BOOL CDialogManager::AddDialog(const string& json_file, const LONG uid, const CDialogFontType type)
			{
				tbb_map* map = NULL;

				switch (type)
				{
				case Core::Classes::UI::jdt8pt:
					map = &m_items_8pt;
					break;
			/*	case Core::Classes::UI::jdt9pt:
					map = &m_items_9pt;
					break;*/
				case Core::Classes::UI::jdt10pt:
					map = &m_items_10pt;
					break;
				}

				if (!map || (map->find(uid) != map->end()))
					return FALSE;

				jDialog* dialog = new jDialog();
				if (!dialog)
					return FALSE;

				if (!dialog->LoadFromFile(json_file))
				{
					delete dialog;
					return FALSE;
				}

				map->emplace(uid, dialog);

				return TRUE;
			}

			BOOL CDialogManager::AddDialogByCode(const std::string& json_code, const LONG uid, const CDialogFontType type)
			{
				tbb_map* map = NULL;

				switch (type)
				{
				case Core::Classes::UI::jdt8pt:
					map = &m_items_8pt;
					break;
				/*case Core::Classes::UI::jdt9pt:
					map = &m_items_9pt;
					break;*/
				case Core::Classes::UI::jdt10pt:
					map = &m_items_10pt;
					break;
				}

				if (!map || (map->find(uid) != map->end()))
					return FALSE;

				jDialog* dialog = new jDialog();
				if (!dialog)
					return FALSE;

				if (!dialog->ParseJSON(json_code))
				{
					delete dialog;
					return FALSE;
				}

				map->emplace(uid, dialog);

				return TRUE;
			}

			jDialog* CDialogManager::GetDialog(const LONG uid, const CDialogFontType type)
			{
				tbb_map* map = NULL;

				switch (type)
				{
				case Core::Classes::UI::jdt8pt:
					map = &m_items_8pt;
					break;
				/*case Core::Classes::UI::jdt9pt:
					map = &m_items_9pt;
					break;*/
				case Core::Classes::UI::jdt10pt:
					map = &m_items_10pt;
					break;
				}

				if (!map)
					return NULL;

				auto it = map->find(uid);
				if (it == map->end())
					return NULL;

				return it->second;
			}

			BOOL CDialogManager::Empty(VOID) const
			{
				return !m_bInit;
			}

			VOID CDialogManager::Init(VOID)
			{
				if (!fs::exists(PATH_DIALOGS_SYSTEM))
					return;
				
				string path = PATH_DIALOGS_SYSTEM;
				path += "/";
				path += PATH_DIALOGS;

				WIN32_FIND_DATA FindFileData;
				HANDLE hFind;

				if (fs::exists(path))
				{
					if (hFind = FindFirstFileA((path + "/*.json").c_str(), &FindFileData); hFind != INVALID_HANDLE_VALUE)
					{
						string sName, sId, sType;

						do
						{
							if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
								continue;

							// skip hidden files (I prefer to see the files)
							if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN)
								continue;

							// skip encrypted files (Encrypted file, problematic file)
							if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) == FILE_ATTRIBUTE_ENCRYPTED)
								continue;
							
							// skip offline files
							if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) == FILE_ATTRIBUTE_OFFLINE)
								continue;

							sName = FindFileData.cFileName;

							auto AtuID = sName.find_first_of('-');
							if (AtuID == string::npos)
								continue;

							sId = sName.substr(0, AtuID);
							++AtuID;
							sType = sName.substr(AtuID, sName.find_first_of('.') - AtuID);

							AddDialog(path + "/" + FindFileData.cFileName, strtoul(sId.data(), NULL, 10), GetDialogType(sType));
						} while (FindNextFileA(hFind, &FindFileData));

						FindClose(hFind);
					}
				}

				path = PATH_DIALOGS_SYSTEM;

				if (hFind = FindFirstFileA((path + "/*.zip").c_str(), &FindFileData); hFind != INVALID_HANDLE_VALUE)
				{
					string sName, sId, sType, fname;

					// I didn't understand it, but it crashes when calling json::parser().
					// I will make the load via a file.

					CHAR szBuf[MAX_PATH] = { 0 };
					Assert(GetTempPathA(MAX_PATH, szBuf));
					string path_temp = szBuf;

					do
					{
						if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
							continue;

						// skip hidden files (I prefer to see the files)
						if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN)
							continue;

						// skip encrypted files (Encrypted file, problematic file)
						if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) == FILE_ATTRIBUTE_ENCRYPTED)
							continue;

						// skip offline files
						if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) == FILE_ATTRIBUTE_OFFLINE)
							continue;

						// open archive
						struct zip_t* zip = zip_open((path + "/" + FindFileData.cFileName).c_str(), 0, 'r');
						if(!zip)
							LogWindow::Log("DIALOG: Failed open archive ""%s""", FindFileData.cFileName);
						else
							LogWindow::Log("DIALOG: Open archive ""%s""", FindFileData.cFileName);

						INT nCount = zip_entries_total(zip);
						for (INT i = 0; i < nCount; ++i)
						{
							zip_entry_openbyindex(zip, i);
							// skip dirs
							if (zip_entry_isdir(zip) != 0)
								continue;

							sName = zip_entry_name(zip);

							// only .json 
							if (fs::path(sName).extension() != ".json")
								continue;

							if (XUtil::Str::dirnameOf(sName) != PATH_DIALOGS)
								continue;

							auto AtuID = sName.find_first_of('-');
							if (AtuID == string::npos)
								continue;

							auto startindex = sName.find_first_of("\\/") + 1;
							sId = sName.substr(startindex, AtuID - startindex);
							++AtuID;
							sType = sName.substr(AtuID, sName.find_first_of('.') - AtuID);

							fname = path_temp + sId + "_" + sType + ".json";
							if (zip_entry_fread(zip, fname.c_str()))
								LogWindow::Log("DIALOG: Failed read file \"%s\"", fname.c_str());
							else
								AddDialog(fname, strtoul(sId.data(), NULL, 10), GetDialogType(sType));

							DeleteFileA(fname.c_str());
							zip_entry_close(zip);
						}

						zip_close(zip);
					} while (FindNextFileA(hFind, &FindFileData));

					FindClose(hFind);
				}
				
				m_bInit = m_items_8pt.size() > 0 || m_items_9pt.size() > 0 || m_items_10pt.size() > 0;
			}

			VOID CDialogManager::Release(VOID)
			{

			}

			CDialogManager::CDialogManager(VOID) : m_bInit(FALSE)
			{
				Init();
			}

			CDialogManager::~CDialogManager(VOID)
			{
				Release();
			}
		}
	}
}