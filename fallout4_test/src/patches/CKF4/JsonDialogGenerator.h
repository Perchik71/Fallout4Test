#pragma once

/*
An apple fell on my head :D
I came up with the idea of using simple languages to generate dialogs.
This should solve the problem of bad dialogs in the future and support 10pt and 12pt text in dialogs.
*/

#include "..\..\common.h"

#include <tbb/concurrent_vector.h>

namespace Core
{
	namespace Classes
	{
		namespace UI
		{
			enum CJsonDialogType
			{
				jdtUnknown,
				jdt8pt,
				jdt10pt,
				jdt12pt
			};

			// 
			typedef class CJsonDialog
			{

			} *PJsonDialog, *LPJsonDialog;


			//
			typedef class CJsonDialogManager
			{
			private:
				tbb::concurrent_vector<LPJsonDialog> m_items;
			public:
				BOOL AddDialog(const char* json_code, const LONG uid, const CJsonDialogType type);
			protected:
				virtual VOID Init(VOID);
				VOID Release(VOID);
			public:
				CJsonDialogManager(VOID);
				virtual ~CJsonDialogManager(VOID);
			} *PJsonDialogManager, *LPJsonDialogManager;

			extern LPJsonDialogManager g_JsonDialogManager;
		}
	}
}