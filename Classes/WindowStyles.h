#pragma once

// v1.4 By Ook
//

#include <gsl/gsl>

#include "EnumConcepts.h"

// Window Styles Helper enums.

enum class WindowStyle
	: DWORD
{
	None,
	Child = WS_CHILD,
	Border = WS_BORDER,
	Caption = WS_CAPTION,
	ClipChildren = WS_CLIPCHILDREN,
	TabStop = WS_TABSTOP,
	LBS_MultiSel = LBS_MULTIPLESEL,
	LBS_ExtendedSel = LBS_EXTENDEDSEL,
	LBS_UseTabStops = LBS_USETABSTOPS,
	BS_Bitmap = BS_BITMAP,
	BS_OwnerDraw = BS_OWNERDRAW,
	SS_EndEllipsis = SS_ENDELLIPSIS,
	SS_PathEllipsis = SS_PATHELLIPSIS,
	SS_NoPrefix = SS_NOPREFIX,
	SS_LeftNoWordWrap = SS_LEFTNOWORDWRAP,
	SS_Center = SS_CENTER,
	SS_Right = SS_RIGHT,
	ES_MultiLine = ES_MULTILINE,
	ES_Password = ES_PASSWORD,
	ES_ReadOnly = ES_READONLY,
	TVS_CheckBoxes = TVS_CHECKBOXES,
	CCS_NoParentAlign = CCS_NOPARENTALIGN,
	CCS_NoResize = CCS_NORESIZE,
	TCS_ForceLeftAlign = TCS_FORCELABELLEFT,
	PBS_Vertical = PBS_VERTICAL,
	MCS_MultiSelect = MCS_MULTISELECT,
	LVS_SingleSelect = LVS_SINGLESEL,
	LVS_ShowSelAlways = LVS_SHOWSELALWAYS,
	CBS_DropDown = CBS_DROPDOWN,
	CBS_Simple = CBS_SIMPLE,
	DTS_ShowNone = DTS_SHOWNONE
};

enum class WindowExStyle
	: DWORD
{
	None,
	DialogModalFrame = WS_EX_DLGMODALFRAME,
	ParentNotify = WS_EX_NOPARENTNOTIFY,
	TopMost = WS_EX_TOPMOST,
	AcceptFiles = WS_EX_ACCEPTFILES,
	Transparent = WS_EX_TRANSPARENT,
	MDIChild = WS_EX_MDICHILD,
	ToolWindow = WS_EX_TOOLWINDOW,
	WindowEdge = WS_EX_WINDOWEDGE,
	ControlParent = WS_EX_CONTROLPARENT,
	ClientEdge = WS_EX_CLIENTEDGE,
	Composited = WS_EX_COMPOSITED,
	Layered = WS_EX_LAYERED
};

enum class WindowAnimStyle
	: DWORD
{
	None
};

template <EnumConcepts::IsNumeric T>
constexpr WindowStyle to_WindowStyle(T other) noexcept
{
	return gsl::narrow_cast<WindowStyle>(other);
}
template <EnumConcepts::IsNumeric T>
constexpr WindowExStyle to_WindowExStyle(T other) noexcept
{
	return gsl::narrow_cast<WindowExStyle>(other);
}

inline WindowStyle dcxGetWindowStyle(HWND Hwnd) noexcept
{
	return to_WindowStyle(GetWindowLong(Hwnd, GWL_STYLE));
}

inline WindowExStyle dcxGetWindowExStyle(HWND Hwnd) noexcept
{
	return to_WindowExStyle(GetWindowLong(Hwnd, GWL_EXSTYLE));
}

inline WindowStyle dcxSetWindowStyle(HWND Hwnd, WindowStyle style) noexcept
{
	return to_WindowStyle(SetWindowLongPtr(Hwnd, GWL_STYLE, gsl::narrow_cast<LONG>(style)));
}

inline WindowExStyle dcxSetWindowExStyle(HWND Hwnd, WindowExStyle style) noexcept
{
	return to_WindowExStyle(SetWindowLongPtr(Hwnd, GWL_EXSTYLE, gsl::narrow_cast<LONG>(style)));
}

inline HWND dcxCreateWindow(const WindowExStyle ExStyles, const TCHAR *const szClass, const WindowStyle Styles, const RECT *const rc, HWND hParent, const UINT uID, const void *const pthis = nullptr) noexcept
{
	return CreateWindowEx(
		gsl::narrow_cast<DWORD>(ExStyles),
		szClass,
		nullptr,
		gsl::narrow_cast<DWORD>(Styles),
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		hParent,
		(HMENU)uID,
		GetModuleHandle(nullptr),
		(LPVOID)pthis);
}

inline UINT dcxSetWindowID(HWND Hwnd, const UINT uID) noexcept
{
	return gsl::narrow_cast<UINT>(SetWindowLongPtr(Hwnd, GWLP_ID, gsl::narrow_cast<LONG>(uID)));
}
