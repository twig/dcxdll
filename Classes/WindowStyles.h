#pragma once

// Window Styles Helper enums.

enum class WindowStyle : DWORD {
	None,
	Child = WS_CHILD,
	Border = WS_BORDER,
	Caption = WS_CAPTION,
	ClipChildren = WS_CLIPCHILDREN,
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

//struct ButtonStyle {
//	enum class Style: DWORD {
//		Bitmap = BS_BITMAP,
//		OwnerDraw = BS_OWNERDRAW
//	};
//
//	operator WindowStyle() const noexcept { return static_cast<WindowStyle>(m_buttonStyle); }
//
//	Style m_buttonStyle;
//};

template <typename T>
constexpr WindowStyle &operator |=(WindowStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowStyle &operator &=(WindowStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowStyle operator &(const WindowStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowStyle operator |(const WindowStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

constexpr WindowStyle operator ~(const WindowStyle &eStyle)
{
	return static_cast<WindowStyle>(~static_cast<DWORD>(eStyle));
}

//template <typename T>
//constexpr operator WindowStyle (const T &eStyle)
//{
//	return static_cast<WindowStyle>(static_cast<DWORD>(eStyle));
//}

enum class WindowExStyle : DWORD {
	None,
	ControlParent = WS_EX_CONTROLPARENT,
	ClientEdge = WS_EX_CLIENTEDGE,
	Transparent = WS_EX_TRANSPARENT,
	Composited = WS_EX_COMPOSITED,
	Layered = WS_EX_LAYERED
};

template <typename T>
constexpr WindowExStyle &operator |=(WindowExStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowExStyle &operator &=(WindowExStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowExStyle operator &(const WindowExStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowExStyle operator |(const WindowExStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowExStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

constexpr WindowExStyle operator ~(const WindowExStyle &eStyle)
{
	return static_cast<WindowExStyle>(~static_cast<DWORD>(eStyle));
}

enum class WindowAnimStyle : DWORD {
	None
};

template <typename T>
constexpr WindowAnimStyle &operator |=(WindowAnimStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowAnimStyle &operator &=(WindowAnimStyle &eStyle, const T &dStyle)
{
	return eStyle = static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowAnimStyle operator &(const WindowAnimStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) & static_cast<DWORD>(dStyle));
}

template <typename T>
constexpr WindowAnimStyle operator |(const WindowAnimStyle &eStyle, const T &dStyle)
{
	return static_cast<WindowAnimStyle>(static_cast<DWORD>(eStyle) | static_cast<DWORD>(dStyle));
}
template <typename T>
constexpr WindowAnimStyle operator |(const T &dStyle, const WindowAnimStyle &eStyle)
{
	return static_cast<WindowAnimStyle>(static_cast<DWORD>(dStyle) | static_cast<DWORD>(eStyle));
}

template <typename T>
constexpr inline WindowStyle to_WindowStyle(T other)
{
	return static_cast<WindowStyle>(other);
}
template <typename T>
constexpr inline WindowExStyle to_WindowExStyle(T other)
{
	return static_cast<WindowExStyle>(other);
}

inline WindowStyle dcxGetWindowStyle(HWND Hwnd)
{
	return static_cast<WindowStyle>(GetWindowStyle(Hwnd));
}

inline WindowExStyle dcxGetWindowExStyle(HWND Hwnd)
{
	return static_cast<WindowExStyle>(GetWindowExStyle(Hwnd));
}

inline WindowStyle dcxSetWindowStyle(HWND Hwnd, WindowStyle style)
{
	return static_cast<WindowStyle>(SetWindowLongPtr(Hwnd, GWL_STYLE, static_cast<LONG>(style)));
}

inline WindowExStyle dcxSetWindowExStyle(HWND Hwnd, WindowExStyle style)
{
	return static_cast<WindowExStyle>(SetWindowLongPtr(Hwnd, GWL_EXSTYLE, static_cast<LONG>(style)));
}

inline HWND dcxCreateWindow(const WindowExStyle ExStyles, const TCHAR *const szClass, const WindowStyle Styles, const RECT *const rc, HWND hParent, const UINT uID, const void *const pthis = nullptr)
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

inline UINT dcxSetWindowID(HWND Hwnd, const UINT uID)
{
	return static_cast<UINT>(SetWindowLongPtr(Hwnd, GWLP_ID, static_cast<LONG>(uID)));
}
