#include "..\UIBaseWindow.h"
#include "VarCommon.h"
#include "RadioButton.h"

namespace Core
{
	namespace UI
	{
		namespace Theme
		{
			namespace RadioButton
			{
				namespace Render
				{
					VOID FIXAPI DrawPushButtonR_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect,
						COLORREF clGradientColorStart, COLORREF clGradientColorEnd,
						COLORREF clGradientHighlighterColorStart, COLORREF clGradientHighlighterColorEnd,
						COLORREF clDividerColor, COLORREF clDividerHighlighterColor)
					{
						canvas.Fill(*pRect, GetThemeSysColor(ThemeColor::ThemeColor_Default));
						canvas.Pen.Style = Graphics::psSolid;
						canvas.Pen.Color = clDividerHighlighterColor;
						canvas.Brush.Assign(Graphics::CreateGradientBrush(clGradientColorStart, clGradientColorEnd, 
							pRect->bottom - pRect->top, Core::Classes::UI::gdVert));
						canvas.Ellipse(*pRect);
					}

					VOID FIXAPI DrawPushButtonR_Normal(Graphics::CUICanvas & canvas, LPCRECT pRect)
					{
						DrawPushButtonR_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Default_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Default_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color));
					}

					VOID FIXAPI DrawPushButtonR_Hot(Graphics::CUICanvas & canvas, LPCRECT pRect)
					{
						DrawPushButtonR_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Button_Hot_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Hot_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Hot_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Hot_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color));
					}

					VOID FIXAPI DrawPushButtonR_Pressed(Graphics::CUICanvas & canvas, LPCRECT pRect)
					{
						DrawPushButtonR_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Pressed),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Pressed), GetThemeSysColor(ThemeColor::ThemeColor_Button_Pressed_Divider),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color));
					}

					VOID FIXAPI DrawPushButtonR_Disabled(Graphics::CUICanvas & canvas, LPCRECT pRect)
					{
						DrawPushButtonR_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Button_Disabled_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Button_Disabled_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Disabled_Gradient_Start),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Highlighter_Disabled_Gradient_End), GetThemeSysColor(ThemeColor::ThemeColor_Button_Light_Disabled_Divider),
							GetThemeSysColor(ThemeColor::ThemeColor_Divider_Color_Disabled));
					}

					VOID FIXAPI DrawRadioButton_Stylesheet(Graphics::CUICanvas& canvas, LPCRECT pRect, COLORREF clColor)
					{
						Core::Classes::UI::CRECT rc_temp[2];

						rc_temp[0] = *pRect;
						rc_temp[0].Inflate(-2, -2);
						rc_temp[0].Width++;
						rc_temp[0].Height++;
						rc_temp[1] = rc_temp[0];
						rc_temp[1].Inflate(-1, -1);

						canvas.Pen.Style = Graphics::psClear;
						canvas.Brush.Assign(Graphics::CreateGradientBrush(GetThemeSysColor(ThemeColor::ThemeColor_CheckBox_Gradient_Start), 
							GetThemeSysColor(ThemeColor::ThemeColor_CheckBox_Gradient_End),
							pRect->bottom - pRect->top, Core::Classes::UI::gdVert));
						canvas.Ellipse(rc_temp[0]);
						canvas.EllipseFill(rc_temp[1], clColor);
					}

					VOID FIXAPI DrawRadioButton_Normal(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape));
					}

					VOID FIXAPI DrawRadioButton_Hot(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Hot));
					}

					VOID FIXAPI DrawRadioButton_Pressed(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Pressed));
					}

					VOID FIXAPI DrawRadioButton_Disabled(Graphics::CUICanvas& canvas, LPCRECT pRect)
					{
						DrawRadioButton_Stylesheet(canvas, pRect, GetThemeSysColor(ThemeColor::ThemeColor_Shape_Disabled));
					}
				}

				namespace Event
				{
					VOID FIXAPI OnBeforeDrawText(Graphics::CUICanvas& canvas, DWORD& flags, INT iStateId)
					{
						flags |= DT_END_ELLIPSIS;
						canvas.ColorText = GetThemeSysColor(ThemeColor_Text_3);
					}
				}
			}
		}
	}
}