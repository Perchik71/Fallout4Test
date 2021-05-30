#include "VarCommon.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			const Core::Classes::UI::CUIFont ThemeFont("Microsoft Sans Serif", 8, {}, DEFAULT_CHARSET, Core::Classes::UI::fqClearTypeNatural, Core::Classes::UI::fpVariable);

			constexpr COLORREF szConstDark[62]
			{
				RGB(52, 52, 52),											// ThemeColor_Default
				RGB(32, 32, 32),											// ThemeColor_ListView_Color
				RGB(32, 32, 32),											// ThemeColor_TreeView_Color
				RGB(32, 32, 32),											// ThemeColor_Edit_Color
				RGB(37, 37, 37),											// ThemeColor_Edit_Color_Disabled
				RGB(137, 137, 137),											// ThemeColor_Text_1
				RGB(156, 156, 156),											// ThemeColor_Text_2
				RGB(200, 200, 200),											// ThemeColor_Text_3
				RGB(220, 220, 220),											// ThemeColor_Text_4
				RGB(16, 16, 16),											// ThemeColor_MDIWindow
				RGB(78, 78, 78),											// ThemeColor_Default_Gradient_Start
				RGB(61, 61, 61),											// ThemeColor_Default_Gradient_End
				RGB(52, 52, 52),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_Start
				RGB(64, 64, 64),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_End
				RGB(20, 20, 20),											// ThemeColor_Divider_Color
				RGB(38, 38, 38),											// ThemeColor_Divider_Color_Disabled
				RGB(105, 105, 105),											// ThemeColor_Divider_Highlighter
				RGB(125, 125, 125),											// ThemeColor_Divider_Highlighter_Hot
				RGB(38, 38, 38),											// ThemeColor_Divider_Color_Ver2
				RGB(45, 45, 45),											// ThemeColor_Divider_Color_Disabled_Ver2
				RGB(73, 73, 73),											// ThemeColor_Divider_Highlighter_Ver2
				RGB(61, 61, 61),											// ThemeColor_Divider_Highlighter_Disabled_Ver2
				RGB(52, 52, 52),											// ThemeColor_Divider_Highlighter_Gradient_Start
				RGB(72, 72, 72),											// ThemeColor_Divider_Highlighter_Gradient_End
				RGB(52, 52, 52),											// ThemeColor_Divider_Highlighter_Hot_Gradient_Start
				RGB(80, 95, 114),											// ThemeColor_Divider_Highlighter_Hot_Gradient_End
				RGB(74, 102, 150),											// ThemeColor_Divider_Highlighter_Pressed
				RGB(38, 38, 38),											// ThemeColor_Button_Pressed_Gradient_Start
				RGB(29, 29, 29),											// ThemeColor_Button_Pressed_Gradient_End
				RGB(98, 98, 98),											// ThemeColor_Button_Hot_Gradient_Start
				RGB(78, 78, 78),											// ThemeColor_Button_Hot_Gradient_End
				RGB(31, 31, 31),											// ThemeColor_Button_Pressed_Divider
				RGB(79, 79, 79),											// ThemeColor_Button_Light_Disabled_Divider
				RGB(65, 65, 65),											// ThemeColor_Button_Disabled_Gradient_Start
				RGB(57, 57, 57),											// ThemeColor_Button_Disabled_Gradient_End
				RGB(51, 51, 51),											// ThemeColor_CheckBox_Gradient_Start
				RGB(20, 20, 20),											// ThemeColor_CheckBox_Gradient_End
				RGB(39, 39, 39),											// ThemeColor_ScrollBar_Gradient_Start
				RGB(27, 27, 27),											// ThemeColor_ScrollBar_Gradient_End
				RGB(78, 78, 78),											// ThemeColor_ScrollBar_Thumb_Gradient_Start
				RGB(61, 61, 61),											// ThemeColor_ScrollBar_Thumb_Gradient_End
				RGB(98, 98, 98),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_Start
				RGB(78, 78, 78),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_End
				RGB(105, 105, 105),											// ThemeColor_ScrollBar_Thumb_Highlighter
				RGB(125, 125, 125),											// ThemeColor_ScrollBar_Thumb_Highlighter_Hot
				RGB(190, 190, 190),											// ThemeColor_Shape
				RGB(131, 162, 194),											// ThemeColor_Shape_Hot
				RGB(220, 220, 220),											// ThemeColor_Shape_Pressed
				RGB(110, 110, 110),											// ThemeColor_Shape_Disabled
				RGB(20, 20, 20),											// ThemeColor_Shape_Shadow
				RGB(38, 38, 38),											// ThemeColor_Shape_Shadow_Disabled
				RGB(70, 149, 205),											// ThemeColor_Progress_Fill_Gradient_Start
				RGB(42, 101, 143),											// ThemeColor_Progress_Fill_Gradient_End
				RGB(87, 159, 209),											// ThemeColor_Progress_Fill_Highlighter
				RGB(103, 175, 225),											// ThemeColor_Progress_Fill_Highlighter_Up
				RGB(0, 122, 204),											// ThemeColor_Border_Window,
				RGB(255, 255, 255),											// ThemeColor_StatusBar_Text
				RGB(255, 255, 255),											// ThemeColor_Caption_Text
				RGB(34, 34, 34),											// ThemeColor_Header_Normal_Gradient_Start
				RGB(27, 27, 27),											// ThemeColor_Header_Normal_Gradient_End
				RGB(44, 44, 44),											// ThemeColor_Header_Hot_Gradient_Start
				RGB(37, 37, 37)												// ThemeColor_Header_Hot_Gradient_End
			};

			constexpr COLORREF szConstDarkGray[62]
			{
				RGB(83, 83, 83),											// ThemeColor_Default
				RGB(58, 58, 58),											// ThemeColor_ListView_Color
				RGB(58, 58, 58),											// ThemeColor_TreeView_Color
				RGB(58, 58, 58),											// ThemeColor_Edit_Color
				RGB(37, 37, 37),											// ThemeColor_Edit_Color_Disabled
				RGB(137, 137, 137),											// ThemeColor_Text_1
				RGB(156, 156, 156),											// ThemeColor_Text_2
				RGB(200, 200, 200),											// ThemeColor_Text_3
				RGB(220, 220, 220),											// ThemeColor_Text_4
				RGB(48, 48, 48),											// ThemeColor_MDIWindow
				RGB(117, 117, 117),											// ThemeColor_Default_Gradient_Start
				RGB(98, 98, 98),											// ThemeColor_Default_Gradient_End
				RGB(83, 83, 83),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_Start
				RGB(94, 94, 94),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_End
				RGB(40, 40, 40),											// ThemeColor_Divider_Color
				RGB(61, 61, 61),											// ThemeColor_Divider_Color_Disabled
				RGB(145, 145, 145),											// ThemeColor_Divider_Highlighter
				RGB(165, 165, 165),											// ThemeColor_Divider_Highlighter_Hot
				RGB(56, 56, 56),											// ThemeColor_Divider_Color_Ver2
				RGB(96, 96, 96),											// ThemeColor_Divider_Color_Disabled_Ver2
				RGB(124, 124, 124),											// ThemeColor_Divider_Highlighter_Ver2
				RGB(114, 114, 114),											// ThemeColor_Divider_Highlighter_Disabled_Ver2
				RGB(83, 83, 83),											// ThemeColor_Divider_Highlighter_Gradient_Start
				RGB(106, 106, 106),											// ThemeColor_Divider_Highlighter_Gradient_End
				RGB(83, 83, 83),											// ThemeColor_Divider_Highlighter_Hot_Gradient_Start
				RGB(100, 125, 154),											// ThemeColor_Divider_Highlighter_Hot_Gradient_End
				RGB(83, 147, 190),											// ThemeColor_Divider_Highlighter_Pressed
				RGB(63, 63, 63),											// ThemeColor_Button_Pressed_Gradient_Start
				RGB(48, 48, 48),											// ThemeColor_Button_Pressed_Gradient_End
				RGB(127, 127, 127),											// ThemeColor_Button_Hot_Gradient_Start
				RGB(108, 108, 108),											// ThemeColor_Button_Hot_Gradient_End
				RGB(51, 51, 51),											// ThemeColor_Button_Pressed_Divider
				RGB(116, 116, 116),											// ThemeColor_Button_Light_Disabled_Divider
				RGB(100, 100, 100),											// ThemeColor_Button_Disabled_Gradient_Start
				RGB(91, 91, 91),											// ThemeColor_Button_Disabled_Gradient_End
				RGB(71, 71, 71),											// ThemeColor_CheckBox_Gradient_Start
				RGB(40, 40, 40),											// ThemeColor_CheckBox_Gradient_End
				RGB(65, 65, 65),											// ThemeColor_ScrollBar_Gradient_Start
				RGB(54, 54, 54),											// ThemeColor_ScrollBar_Gradient_End
				RGB(139, 139, 139),											// ThemeColor_ScrollBar_Thumb_Gradient_Start
				RGB(107, 107, 107),											// ThemeColor_ScrollBar_Thumb_Gradient_End
				RGB(149, 149, 149),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_Start
				RGB(117, 117, 117),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_End
				RGB(154, 154, 154),											// ThemeColor_ScrollBar_Thumb_Highlighter
				RGB(164, 164, 164),											// ThemeColor_ScrollBar_Thumb_Highlighter_Hot
				RGB(190, 190, 190),											// ThemeColor_Shape
				RGB(123, 198, 225),											// ThemeColor_Shape_Hot
				RGB(220, 220, 220),											// ThemeColor_Shape_Pressed
				RGB(140, 140, 140),											// ThemeColor_Shape_Disabled
				RGB(40, 40, 40),											// ThemeColor_Shape_Shadow
				RGB(58, 58, 58),											// ThemeColor_Shape_Shadow_Disabled
				RGB(73, 157, 216),											// ThemeColor_Progress_Fill_Gradient_Start
				RGB(50, 120, 169),											// ThemeColor_Progress_Fill_Gradient_End
				RGB(87, 159, 209),											// ThemeColor_Progress_Fill_Highlighter
				RGB(103, 175, 225),											// ThemeColor_Progress_Fill_Highlighter_Up
				RGB(0, 129, 224),											// ThemeColor_Border_Window,
				RGB(255, 255, 255),											// ThemeColor_StatusBar_Text
				RGB(255, 255, 255),											// ThemeColor_Caption_Text
				RGB(60, 60, 60),											// ThemeColor_Header_Normal_Gradient_Start
				RGB(55, 55, 55),											// ThemeColor_Header_Normal_Gradient_End
				RGB(70, 70, 70),											// ThemeColor_Header_Hot_Gradient_Start
				RGB(65, 65, 65)												// ThemeColor_Header_Hot_Gradient_End
			};

			constexpr COLORREF szConstGray[62]
			{
				RGB(184, 184, 184),											// ThemeColor_Default
				RGB(238, 238, 238),											// ThemeColor_ListView_Color
				RGB(238, 238, 238),											// ThemeColor_TreeView_Color
				RGB(238, 238, 238),											// ThemeColor_Edit_Color
				RGB(238, 238, 238),											// ThemeColor_Edit_Color_Disabled
				RGB(141, 141, 141),											// ThemeColor_Text_1
				RGB(96, 96, 96),											// ThemeColor_Text_2
				RGB(32, 32, 32),											// ThemeColor_Text_3
				RGB(16, 16, 16),											// ThemeColor_Text_4
				RGB(123, 123, 123),											// ThemeColor_MDIWindow
				RGB(229, 229, 229),											// ThemeColor_Default_Gradient_Start
				RGB(190, 190, 190),											// ThemeColor_Default_Gradient_End
				RGB(184, 184, 184),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_Start
				RGB(192, 192, 192),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_End
				RGB(81, 81, 81),											// ThemeColor_Divider_Color
				RGB(140, 140, 140),											// ThemeColor_Divider_Color_Disabled
				RGB(246, 246, 246),											// ThemeColor_Divider_Highlighter
				RGB(246, 246, 246),											// ThemeColor_Divider_Highlighter_Hot
				RGB(153, 153, 153),											// ThemeColor_Divider_Color_Ver2
				RGB(196, 196, 196),											// ThemeColor_Divider_Color_Disabled_Ver2
				RGB(212, 212, 212),											// ThemeColor_Divider_Highlighter_Ver2
				RGB(164, 164, 164),											// ThemeColor_Divider_Highlighter_Disabled_Ver2
				RGB(184, 184, 184),											// ThemeColor_Divider_Highlighter_Gradient_Start
				RGB(199, 199, 199),											// ThemeColor_Divider_Highlighter_Gradient_End
				RGB(184, 184, 184),											// ThemeColor_Divider_Highlighter_Hot_Gradient_Start
				RGB(142, 180, 203),											// ThemeColor_Divider_Highlighter_Hot_Gradient_End
				RGB(75, 156, 245),											// ThemeColor_Divider_Highlighter_Pressed
				RGB(139, 139, 139),											// ThemeColor_Button_Pressed_Gradient_Start
				RGB(130, 130, 130),											// ThemeColor_Button_Pressed_Gradient_End
				RGB(239, 239, 239),											// ThemeColor_Button_Hot_Gradient_Start
				RGB(200, 200, 200),											// ThemeColor_Button_Hot_Gradient_End
				RGB(121, 121, 121),											// ThemeColor_Button_Pressed_Divider
				RGB(223, 223, 223),											// ThemeColor_Button_Light_Disabled_Divider
				RGB(207, 207, 207),											// ThemeColor_Button_Disabled_Gradient_Start
				RGB(187, 187, 187),											// ThemeColor_Button_Disabled_Gradient_End
				RGB(111, 111, 111),											// ThemeColor_CheckBox_Gradient_Start
				RGB(81, 81, 81),											// ThemeColor_CheckBox_Gradient_End
				RGB(221, 221, 221),											// ThemeColor_ScrollBar_Gradient_Start
				RGB(189, 189, 189),											// ThemeColor_ScrollBar_Gradient_End
				RGB(161, 161, 161),											// ThemeColor_ScrollBar_Thumb_Gradient_Start
				RGB(129, 129, 129),											// ThemeColor_ScrollBar_Thumb_Gradient_End
				RGB(171, 171, 171),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_Start
				RGB(139, 139, 139),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_End
				RGB(174, 174, 174),											// ThemeColor_ScrollBar_Thumb_Highlighter
				RGB(184, 184, 184),											// ThemeColor_ScrollBar_Thumb_Highlighter_Hot
				RGB(76, 76, 76),											// ThemeColor_Shape
				RGB(123, 198, 225),											// ThemeColor_Shape_Hot
				RGB(55, 55, 55),											// ThemeColor_Shape_Pressed
				RGB(129, 129, 129),											// ThemeColor_Shape_Disabled
				RGB(220, 220, 220),											// ThemeColor_Shape_Shadow
				RGB(190, 190, 190),											// ThemeColor_Shape_Shadow_Disabled
				RGB(70, 149, 205),											// ThemeColor_Progress_Fill_Gradient_Start
				RGB(42, 101, 143),											// ThemeColor_Progress_Fill_Gradient_End
				RGB(87, 159, 209),											// ThemeColor_Progress_Fill_Highlighter
				RGB(103, 175, 225),											// ThemeColor_Progress_Fill_Highlighter_Up
				RGB(0, 137, 245),											// ThemeColor_Border_Window,
				RGB(255, 255, 255),											// ThemeColor_StatusBar_Text
				RGB(20, 20, 20),											// ThemeColor_Caption_Text
				RGB(166, 166, 166),											// ThemeColor_Header_Normal_Gradient_Start
				RGB(147, 147, 147),											// ThemeColor_Header_Normal_Gradient_End
				RGB(176, 176, 176),											// ThemeColor_Header_Hot_Gradient_Start
				RGB(157, 157, 157)											// ThemeColor_Header_Hot_Gradient_End
			};

			constexpr COLORREF szConstLight[62]
			{
				RGB(214, 214, 214),											// ThemeColor_Default
				RGB(255, 255, 255),											// ThemeColor_ListView_Color
				RGB(255, 255, 255),											// ThemeColor_TreeView_Color
				RGB(255, 255, 255),											// ThemeColor_Edit_Color
				RGB(235, 235, 235),											// ThemeColor_Edit_Color_Disabled
				RGB(158, 158, 158),											// ThemeColor_Text_1
				RGB(96, 96, 96),											// ThemeColor_Text_2
				RGB(32, 32, 32),											// ThemeColor_Text_3
				RGB(16, 16, 16),											// ThemeColor_Text_4
				RGB(153, 153, 153),											// ThemeColor_MDIWindow
				RGB(253, 253, 253),											// ThemeColor_Default_Gradient_Start
				RGB(233, 233, 233),											// ThemeColor_Default_Gradient_End
				RGB(214, 214, 214),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_Start
				RGB(223, 223, 223),											// ThemeColor_Divider_Highlighter_Disabled_Gradient_End
				RGB(100, 100, 100),											// ThemeColor_Divider_Color
				RGB(172, 172, 172),											// ThemeColor_Divider_Color_Disabled
				RGB(255, 255, 255),											// ThemeColor_Divider_Highlighter
				RGB(255, 255, 255),											// ThemeColor_Divider_Highlighter_Hot
				RGB(153, 153, 153),											// ThemeColor_Divider_Color_Ver2
				RGB(196, 196, 196),											// ThemeColor_Divider_Color_Disabled_Ver2
				RGB(232, 232, 232),											// ThemeColor_Divider_Highlighter_Ver2
				RGB(200, 200, 200),											// ThemeColor_Divider_Highlighter_Disabled_Ver2
				RGB(214, 214, 214),											// ThemeColor_Divider_Highlighter_Gradient_Start
				RGB(232, 232, 232),											// ThemeColor_Divider_Highlighter_Gradient_End
				RGB(214, 214, 214),											// ThemeColor_Divider_Highlighter_Hot_Gradient_Start
				RGB(162, 200, 223),											// ThemeColor_Divider_Highlighter_Hot_Gradient_End
				RGB(123, 183, 249),											// ThemeColor_Divider_Highlighter_Pressed
				RGB(169, 169, 169),											// ThemeColor_Button_Pressed_Gradient_Start
				RGB(160, 160, 160),											// ThemeColor_Button_Pressed_Gradient_End
				RGB(243, 243, 243),											// ThemeColor_Button_Hot_Gradient_Start
				RGB(223, 223, 223),											// ThemeColor_Button_Hot_Gradient_End
				RGB(121, 121, 121),											// ThemeColor_Button_Pressed_Divider
				RGB(240, 240, 240),											// ThemeColor_Button_Light_Disabled_Divider
				RGB(233, 233, 233),											// ThemeColor_Button_Disabled_Gradient_Start
				RGB(224, 224, 224),											// ThemeColor_Button_Disabled_Gradient_End
				RGB(131, 131, 131),											// ThemeColor_CheckBox_Gradient_Start
				RGB(100, 100, 100),											// ThemeColor_CheckBox_Gradient_End
				RGB(231, 231, 231),											// ThemeColor_ScrollBar_Gradient_Start
				RGB(201, 201, 201),											// ThemeColor_ScrollBar_Gradient_End
				RGB(179, 179, 179),											// ThemeColor_ScrollBar_Thumb_Gradient_Start
				RGB(142, 142, 142),											// ThemeColor_ScrollBar_Thumb_Gradient_End
				RGB(189, 189, 189),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_Start
				RGB(152, 152, 152),											// ThemeColor_ScrollBar_Thumb_Gradient_Hot_End
				RGB(190, 190, 190),											// ThemeColor_ScrollBar_Thumb_Highlighter
				RGB(200, 200, 200),											// ThemeColor_ScrollBar_Thumb_Highlighter_Hot
				RGB(94, 94, 94),											// ThemeColor_Shape
				RGB(131, 162, 194),											// ThemeColor_Shape_Hot
				RGB(71, 71, 71),											// ThemeColor_Shape_Pressed
				RGB(149, 149, 149),											// ThemeColor_Shape_Disabled
				RGB(240, 240, 240),											// ThemeColor_Shape_Shadow
				RGB(210, 210, 210),											// ThemeColor_Shape_Shadow_Disabled
				RGB(70, 149, 205),											// ThemeColor_Progress_Fill_Gradient_Start
				RGB(42, 101, 143),											// ThemeColor_Progress_Fill_Gradient_End
				RGB(87, 159, 209),											// ThemeColor_Progress_Fill_Highlighter
				RGB(103, 175, 225),											// ThemeColor_Progress_Fill_Highlighter_Up
				RGB(0, 150, 250),											// ThemeColor_Border_Window,
				RGB(255, 255, 255),											// ThemeColor_StatusBar_Text
				RGB(20, 20, 20),											// ThemeColor_Caption_Text
				RGB(196, 196, 196),											// ThemeColor_Header_Normal_Gradient_Start
				RGB(177, 177, 177),											// ThemeColor_Header_Normal_Gradient_End
				RGB(206, 206, 206),											// ThemeColor_Header_Hot_Gradient_Start
				RGB(187, 187, 187)											// ThemeColor_Header_Hot_Gradient_End
			};

			HBRUSH hThemeDefaultBrush = NULL;
			HBRUSH hThemeText3Brush = NULL;
			HBRUSH hThemeEditBrush = NULL;
			HBRUSH hThemeDividerBrush = NULL;
			HBRUSH hThemeText4Brush = NULL;
			HBRUSH hThemeBorderWindowBrush = NULL;
			COLORREF* szCurrentScheme = NULL;

			static Theme generalCurentTheme = Theme_Dark;

			Theme FIXAPI GetTheme(VOID)
			{
				return generalCurentTheme;
			}

			VOID FIXAPI SetTheme(Theme theme)
			{
				generalCurentTheme = theme;

				switch (theme)
				{
				case Theme_Light:
					szCurrentScheme = const_cast<COLORREF*>(szConstLight);
					break;
				case Theme_Gray:
					szCurrentScheme = const_cast<COLORREF*>(szConstGray);
					break;
				case Theme_DarkGray:
					szCurrentScheme = const_cast<COLORREF*>(szConstDarkGray);
					break;
				case Theme_Dark:
					szCurrentScheme = const_cast<COLORREF*>(szConstDark);
					break;
				}

				DeleteObject(hThemeDefaultBrush);
				DeleteObject(hThemeText3Brush);
				DeleteObject(hThemeEditBrush);
				DeleteObject(hThemeDividerBrush);
				DeleteObject(hThemeText4Brush);
				DeleteObject(hThemeBorderWindowBrush);

				hThemeDefaultBrush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Default));
				hThemeText3Brush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Text_3));
				hThemeEditBrush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Edit_Color));
				hThemeDividerBrush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Divider_Color));
				hThemeText4Brush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Text_4));
				hThemeBorderWindowBrush = CreateSolidBrush(GetThemeSysColor(ThemeColor_Border_Window));
			}

			COLORREF FIXAPI GetThemeSysColor(const ThemeColor color)
			{
				if (szCurrentScheme)
					return szCurrentScheme[color - 1];
				else
					return 0;
			}

			DWORD FIXAPI Comctl32GetSysColor(INT nIndex)
			{
				switch (nIndex)
				{
				case COLOR_BTNFACE: return GetThemeSysColor(ThemeColor_Default);
				case COLOR_BTNTEXT: return GetThemeSysColor(ThemeColor_Text_3);
				case COLOR_WINDOW: return GetThemeSysColor(ThemeColor_Edit_Color);
				case COLOR_WINDOWFRAME: return GetThemeSysColor(ThemeColor_Divider_Color);
				case COLOR_WINDOWTEXT: return GetThemeSysColor(ThemeColor_Text_4);
				case COLOR_ACTIVEBORDER: return GetThemeSysColor(ThemeColor_Border_Window);
				case COLOR_INACTIVEBORDER: return GetThemeSysColor(ThemeColor_Divider_Color);
				default:
					return ::GetSysColor(nIndex);
				}
			}

			HBRUSH FIXAPI Comctl32GetSysColorBrushEx(INT nIndex, BOOL reCreate)
			{
				if (!reCreate)
					return Comctl32GetSysColorBrush(nIndex);

				return CreateSolidBrush(Comctl32GetSysColor(nIndex));
			}

			HBRUSH FIXAPI Comctl32GetSysColorBrush(INT nIndex)
			{
				switch (nIndex)
				{
				case COLOR_BTNFACE: return hThemeDefaultBrush;
				case COLOR_BTNTEXT: return hThemeText3Brush;
				case COLOR_WINDOWFRAME: return hThemeDividerBrush;
				case COLOR_WINDOWTEXT: return hThemeText4Brush;
				case COLOR_ACTIVEBORDER: return hThemeBorderWindowBrush;
				case COLOR_INACTIVEBORDER: return hThemeDividerBrush;
				default:
					return ::GetSysColorBrush(nIndex);
				}
			}
		}
	}
}