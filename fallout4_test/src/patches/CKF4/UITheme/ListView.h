#pragma once

#include <Uxtheme.h>

#include "..\..\..\common.h"
#include "..\UIGraphics.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace ListView
			{
				namespace Graphics = Core::Classes::UI;

				HTHEME FIXAPI Initialize(HWND hWindow);
				LRESULT CALLBACK ListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

				LRESULT FIXAPI OnCustomDraw(HWND hWindow, LPNMLVCUSTOMDRAW lpListView);
			}
		}
	}
}