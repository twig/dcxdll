/*!
 * \file dcxcontrol.h
 * \brief blah
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * � ScriptsDB.org - 2006
 */

#ifndef _DCXCONTROL_H_
#define _DCXCONTROL_H_

#include "defines.h"
#include "dcxwindow.h"

 //constexpr auto DCC_TEXTCOLOR = 0x01;	//!< Control Text Color;
 //constexpr auto DCC_TEXTBKGCOLOR = 0x02;	//!< Control Text Background Color;
 //constexpr auto DCC_BKGCOLOR = 0x04;	//!< Control Background Color;
 //constexpr auto DCC_BORDERCOLOR = 0x08;	//!< Control Border Color;
 //constexpr auto DCC_GRADSTARTCOLOR = 0x10;	//!< Colour At the start of the gradient;
 //constexpr auto DCC_GRADENDCOLOR = 0x20;	//!< Colour At the end of the gradient;
 //
 //constexpr auto DCCS_FROMRESSOURCE = 0x01;	//!< Cursor from ressource;
 //constexpr auto DCCS_FROMFILE = 0x02;	//!< Cursor from File;

enum class DcxColourFlags
	: UINT
{
	None,
	TEXTCOLOR,				//!< Control Text Color;
	TEXTBKGCOLOR,			//!< Control Text Background Color;
	BKGCOLOR = 0x04,		//!< Control Background Color;
	BORDERCOLOR = 0x08,		//!< Control Border Color;
	GRADSTARTCOLOR = 0x10,	//!< Colour At the start of the gradient;
	GRADENDCOLOR = 0x20		//!< Colour At the end of the gradient;
};

//template <typename T>
//constexpr DcxColourFlags &operator |=(DcxColourFlags &eStyle, const T &dStyle) noexcept
//{
//	return eStyle = static_cast<DcxColourFlags>(static_cast<UINT>(eStyle) | static_cast<UINT>(dStyle));
//}
//template <typename T>
//constexpr DcxColourFlags& operator |=(T& eStyle, const DcxColourFlags& dStyle) noexcept
//{
//	return eStyle = static_cast<DcxColourFlags>(static_cast<UINT>(eStyle) | static_cast<UINT>(dStyle));
//}
//
//template <typename T>
//constexpr DcxColourFlags& operator &=(DcxColourFlags& eStyle, const T& dStyle) noexcept
//{
//	return eStyle = static_cast<DcxColourFlags>(static_cast<UINT>(eStyle) & static_cast<UINT>(dStyle));
//}
//
//template <typename T>
//constexpr DcxColourFlags operator &(const DcxColourFlags& eStyle, const T& dStyle) noexcept
//{
//	return static_cast<DcxColourFlags>(static_cast<UINT>(eStyle) & static_cast<UINT>(dStyle));
//}
//
//template <typename T>
//constexpr DcxColourFlags operator |(const DcxColourFlags& eStyle, const T& dStyle) noexcept
//{
//	return static_cast<DcxColourFlags>(static_cast<UINT>(eStyle) | static_cast<UINT>(dStyle));
//}
//
//constexpr DcxColourFlags operator ~(const DcxColourFlags& eStyle) noexcept
//{
//	return static_cast<DcxColourFlags>(~static_cast<UINT>(eStyle));
//}

//#define CTLF_ALLOW_PBAR				0x0000000000000001ULL
//#define CTLF_ALLOW_TRACKBAR			0x0000000000000002ULL
//#define CTLF_ALLOW_COMBOEX			0x0000000000000004ULL
//#define CTLF_ALLOW_COLORCOMBO		0x0000000000000008ULL
//#define CTLF_ALLOW_STATUSBAR		0x0000000000000010ULL
//#define CTLF_ALLOW_TOOLBAR			0x0000000000000020ULL
//#define CTLF_ALLOW_TREEVIEW			0x0000000000000040ULL
//#define CTLF_ALLOW_LISTVIEW			0x0000000000000080ULL
//#define CTLF_ALLOW_REBAR			0x0000000000000100ULL
//#define CTLF_ALLOW_BUTTON			0x0000000000000200ULL
//#define CTLF_ALLOW_RICHEDIT			0x0000000000000400ULL
//#define CTLF_ALLOW_EDIT				0x0000000000000800ULL
//#define CTLF_ALLOW_UPDOWN			0x0000000000001000ULL
//#define CTLF_ALLOW_IPADDRESS		0x0000000000002000ULL
//#define CTLF_ALLOW_WEBCTRL			0x0000000000004000ULL
//#define CTLF_ALLOW_CALANDER			0x0000000000008000ULL
//#define CTLF_ALLOW_DIVIDER			0x0000000000010000ULL
//#define CTLF_ALLOW_PANEL			0x0000000000020000ULL
//#define CTLF_ALLOW_TAB				0x0000000000040000ULL
//#define CTLF_ALLOW_LINE				0x0000000000080000ULL
//#define CTLF_ALLOW_BOX				0x0000000000100000ULL
//#define CTLF_ALLOW_RADIO			0x0000000000200000ULL
//#define CTLF_ALLOW_CHECK			0x0000000000400000ULL
//#define CTLF_ALLOW_TEXT				0x0000000000800000ULL
//#define CTLF_ALLOW_SCROLL			0x0000000001000000ULL
//#define CTLF_ALLOW_LIST				0x0000000002000000ULL
//#define CTLF_ALLOW_LINK				0x0000000004000000ULL
//#define CTLF_ALLOW_IMAGE			0x0000000008000000ULL
//#define CTLF_ALLOW_PAGER			0x0000000010000000ULL
//#define CTLF_ALLOW_DOCK				0x0000000020000000ULL // allows @Window and Dialog docking
//#define CTLF_ALLOW_DATETIME			0x0000000040000000ULL
//#define CTLF_ALLOW_STACKER			0x0000000080000000ULL
//#define CTLF_ALLOW_DIRECTSHOW		0x0000000100000000ULL
//#define CTLF_ALLOW_ALL				0xFFFFFFFFFFFFFFFFULL
//#define CTLF_ALLOW_ALLBUTDOCK		(CTLF_ALLOW_ALL & ~CTLF_ALLOW_DOCK)

enum class DcxAllowControls
	: UINT64
{
	ALLOW_PBAR = 0x000000001ULL,
	ALLOW_TRACKBAR = 0x000000002ULL,
	ALLOW_COMBOEX = 0x000000004ULL,
	ALLOW_COLORCOMBO = 0x000000008ULL,
	ALLOW_STATUSBAR = 0x000000010ULL,
	ALLOW_TOOLBAR = 0x000000020ULL,
	ALLOW_TREEVIEW = 0x000000040ULL,
	ALLOW_LISTVIEW = 0x000000080ULL,
	ALLOW_REBAR = 0x000000100ULL,
	ALLOW_BUTTON = 0x000000200ULL,
	ALLOW_RICHEDIT = 0x000000400ULL,
	ALLOW_EDIT = 0x000000800ULL,
	ALLOW_UPDOWN = 0x000001000ULL,
	ALLOW_IPADDRESS = 0x000002000ULL,
	ALLOW_WEBCTRL = 0x000004000ULL,
	ALLOW_CALANDER = 0x000008000ULL,
	ALLOW_DIVIDER = 0x000010000ULL,
	ALLOW_PANEL = 0x000020000ULL,
	ALLOW_TAB = 0x000040000ULL,
	ALLOW_LINE = 0x000080000ULL,
	ALLOW_BOX = 0x000100000ULL,
	ALLOW_RADIO = 0x000200000ULL,
	ALLOW_CHECK = 0x000400000ULL,
	ALLOW_TEXT = 0x000800000ULL,
	ALLOW_SCROLL = 0x001000000ULL,
	ALLOW_LIST = 0x002000000ULL,
	ALLOW_LINK = 0x004000000ULL,
	ALLOW_IMAGE = 0x008000000ULL,
	ALLOW_PAGER = 0x010000000ULL,
	ALLOW_DOCK = 0x020000000ULL, // allows @Window and Dialog docking
	ALLOW_DATETIME = 0x040000000ULL,
	ALLOW_STACKER = 0x080000000ULL,
	ALLOW_DIRECTSHOW = 0x100000000ULL,
	ALLOW_ALL = 0xFFFFFFFFFFFFFFFFULL,
	ALLOW_ALLBUTDOCK = (UINT64)(ALLOW_ALL & ~ALLOW_DOCK)
};

// Control types...
enum class DcxControlTypes
	: UINT
{
	UNKNOWN = 0, BOX = 1,
	CHECK, EDIT, IMAGE, LINE, LINK, LIST, RADIO, SCROLL, TEXT, BUTTON, CALENDAR,
	COLORCOMBO, COMBOEX, DATETIME, DIRECTSHOW, DIVIDER, IPADDRESS, LISTVIEW, DIALOG,
	WINDOW, PAGER, PANEL, PROGRESSBAR, REBAR, RICHEDIT, STACKER, STATUSBAR, TABB, TOOLBAR,
	TRACKBAR, TREEVIEW, UPDOWN, WEBCTRL
};

// Search types...
enum class DcxSearchTypes
	: UINT
{
	SEARCH_W = 0x01,	//!< WildCard Search
	SEARCH_R,			//!< Regex Search
	SEARCH_E			//!< Exact Match
};

// icon sizes...
enum class DcxIconSizes
	: int
{
	SmallIcon = 16, MediumIcon = 24, LargeIcon = 32,
	MaxSize = LargeIcon
};

class DcxDialog;

struct ALPHAINFO
{
	HDC ai_hdc{ nullptr };
	HDC* ai_hdcBuffer{ nullptr };
	HDC ai_Oldhdc{ nullptr };
	HBITMAP ai_bitmap{ nullptr };
	HBITMAP ai_bkg{ nullptr };
	HBITMAP ai_oldBM{ nullptr };
	RECT ai_rcClient{ 0,0,0,0 };
	RECT ai_rcWin{ 0,0,0,0 };
	BLENDFUNCTION ai_bf{ AC_SRC_OVER,0, 255, 0 };
	HPAINTBUFFER ai_Buffer{ nullptr };

	ALPHAINFO(HWND hwnd)
	{
		if (!GetClientRect(hwnd, &ai_rcClient))
			throw Dcx::dcxException("Unable to get Client Rect");
		if (!GetWindowRect(hwnd, &ai_rcWin))
			throw Dcx::dcxException("Unable to get Window Rect");
	}
};
using LPALPHAINFO = ALPHAINFO*;

// no default constructor
// no copy constructor
// No copy assignments!
// no move constructor
// No move assignments!
#define DCX_DELETE_CONTROL_METHODS(xclass) \
xclass() = delete; \
xclass(const xclass &other) = delete; \
xclass &operator =(const xclass &) = delete; \
xclass(xclass &&other) = delete; \
xclass &operator =(xclass &&) = delete;

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxControl
	: public DcxWindow
{
public:
	//DCX_DELETE_CONTROL_METHODS(DcxControl);

	DcxControl() = delete;	// no default constructor
	DcxControl(const DcxControl& other) = delete;	// no copy constructor
	DcxControl& operator =(const DcxControl&) = delete;	// No copy assignments!
	DcxControl(DcxControl&& other) = delete;	// no move constructor
	DcxControl& operator =(DcxControl&&) = delete;	// No move assignments!

	DcxControl(const UINT mID, DcxDialog* const p_Dialog) noexcept;
	~DcxControl() noexcept;

	//virtual void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const = 0;
	//virtual void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const = 0;
	//virtual void parseCommandRequest( const TString & input ) = 0;
	//virtual void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) = 0;

	virtual dcxWindowStyles parseControlStyles(const TString& tsStyles) = 0;

	dcxWindowStyles parseGeneralControlStyles(const TString& styles, dcxWindowStyles& ws);
	dcxWindowStyles parseGeneralControlStyles(const TString& styles);

	bool evalAliasEx(TCHAR* const szReturn, const int maxlen, const TCHAR* const szFormat, ...) const;

	//template <typename Format, typename Value, typename... Arguments>
	//bool evalAliasEx(TCHAR *const szReturn, const int maxlen, const Format &fmt, const Value val, Arguments&&... args) const
	//{
	//	TString tsBuf;
	//	getParentDialog()->evalAlias(szReturn, maxlen, _ts_sprintf(tsBuf, fmt, val, args...).to_chr());
	//}

	bool execAliasEx(const TCHAR* const szFormat, ...) const;

	//template <typename Format, typename Value, typename... Arguments>
	//bool execAliasEx(const Format &fmt, const Value val, Arguments&&... args) const
	//{
	//	TString tsBuf;
	//	getParentDialog()->execAlias(_ts_sprintf(tsBuf, fmt, val, args...).to_chr());
	//}

	const UINT& getUserID() const noexcept
	{
		return m_UserID;
	}

	virtual LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) = 0;
	virtual LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) = 0;

	virtual const TString getType() const = 0;
	virtual const DcxControlTypes getControlType() const noexcept = 0;

	virtual const TString getStyles(void) const;
	virtual const TString getBorderStyles(void) const;
	void toXml(TiXmlElement* const xml) const override;
	virtual TiXmlElement* toXml(void) const;

	virtual LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	LRESULT setFont(const HFONT hFont, const BOOL fRedraw) noexcept;
	void setControlFont(const HFONT hFont, const BOOL fRedraw) noexcept;
	LRESULT setRedraw(const BOOL fView) noexcept;
	void setBackClrBrush(const HBRUSH c) noexcept { m_hBackBrush = c; }
	void setBackColor(const COLORREF c) noexcept { m_clrBackText = c; }
	void setTextColor(const COLORREF c) noexcept { m_clrText = c; }
	void setStartGradientColor(const COLORREF c) noexcept { m_clrStartGradient = c; }
	void setEndGradientColor(const COLORREF c) noexcept { m_clrEndGradient = c; }

	void setToolTipHWND(const HWND hwnd) noexcept { m_ToolTipHWND = hwnd; }
	void setNoThemed(const bool b) noexcept { m_bThemed = b; }
	void setAlphaBlended(const bool b) noexcept { m_bAlphaBlend = b; }
	void setControlCursor(const HCURSOR c) noexcept { m_hCursor = c; }
	void setShadowTextState(const bool b) noexcept { m_bShadowText = b; }
	void setControlCodeTextState(const bool b) noexcept { m_bCtrlCodeText = b; }

	HFONT getFont() const noexcept;
	const HFONT& getControlFont() const noexcept
	{
		return this->m_hFont;
	}
	const HBRUSH& getBackClrBrush() const noexcept
	{
		return this->m_hBackBrush;
	}
	const COLORREF& getBackColor() const noexcept
	{
		return this->m_clrBackText;
	}
	const COLORREF& getTextColor() const noexcept
	{
		return this->m_clrText;
	}
	const COLORREF& getStartGradientColor() const noexcept
	{
		return m_clrStartGradient;
	};
	const COLORREF& getEndGradientColor() const noexcept
	{
		return m_clrEndGradient;
	};
	const RECT getWindowPosition() const noexcept;
	DcxDialog* const getParentDialog() const noexcept
	{
		return m_pParentDialog;
	}
	const HWND& getParentHWND() const noexcept
	{
		return m_pParentHWND;
	}
	const HWND& getToolTipHWND() const noexcept
	{
		return m_ToolTipHWND;
	}
	const HCURSOR& getControlCursor() const noexcept
	{
		return m_hCursor;
	}

	const inline bool& IsAlphaBlend() const noexcept
	{
		return m_bAlphaBlend;
	}
	const inline bool& IsThemed() const noexcept
	{
		return m_bThemed;
	}
	const inline bool& IsShadowTextEnabled() const noexcept
	{
		return m_bShadowText;
	}
	const inline bool& IsControlCodeTextEnabled() const noexcept
	{
		return m_bCtrlCodeText;
	}
	const inline bool& IsGradientFillEnabled() const noexcept
	{
		return m_bGradientFill;
	}
	const inline bool& IsGradientFillVertical() const noexcept
	{
		return m_bGradientVertical;
	}

	inline void incRef() noexcept
	{
		++m_iRefCount;
	};
	inline void decRef() noexcept
	{
		--m_iRefCount;
	};
	inline const UINT& getRefCount() const noexcept
	{
		return m_iRefCount;
	};

	void updateParentCtrl() noexcept; //!< updates controls host control pointers, MUST be called before these pointers are used.
	void DrawParentsBackground(const HDC hdc, const RECT* const rcBounds = nullptr, const HWND dHwnd = nullptr);
	[[nodiscard("Memory Leak")]] LPALPHAINFO SetupAlphaBlend(HDC* hdc, const bool DoubleBuffer = false);
	void FinishAlphaBlend(LPALPHAINFO ai) noexcept;
	void showError(const TCHAR* const prop, const TCHAR* const cmd, const TCHAR* const err) const;
	template <typename Format, typename Value, typename... Arguments>
	void showError(const TCHAR* const prop, const TCHAR* const cmd, const Format& fmt, const Value& val, Arguments&& ... args) const
	{
		TString tsErr;
		showError(prop, cmd, _ts_sprintf(tsErr, fmt, val, args...).to_chr());
	}

	static LRESULT CALLBACK WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	[[nodiscard("Memory Leak")]] static DcxControl* controlFactory(DcxDialog* const p_Dialog, const UINT mID, const TString& input, const UINT offset, const DcxAllowControls mask, HWND hParent = nullptr);
	static void DrawCtrlBackground(const HDC hdc, const DcxControl* const p_this, const RECT* const rwnd = nullptr, HTHEME hTheme = nullptr, const int iPartId = 0, const int iStateId = 0) noexcept;
	static HBITMAP resizeBitmap(HBITMAP srcBM, const RECT* const rc) noexcept;
	static DcxControlTypes TSTypeToControlType(const TString& t);
	// Convert a number into the closest icon size
	static DcxIconSizes NumToIconSize(const int& num) noexcept;

	static void InitializeDcxControls();
	static void UnInitializeDcxControls() noexcept;

protected:
	//private:
	static inline bool m_bInitialized{ false };

	DcxDialog* m_pParentDialog{ nullptr };	//!< Parent DcxDialog object

	HFONT m_hFont{ nullptr };					//!< Control Font

	TString m_tsMark;				//!< Mark Information (see /xdid -M)
	TString m_tsToolTip;			//!< This controls tooltip text (if any).

	COLORREF m_clrText{ CLR_INVALID };				//!< Font color
	COLORREF m_clrBackText{ CLR_INVALID };			//!< Font Back Color (not supported)
	COLORREF m_colTransparentBg{ CLR_INVALID };
	COLORREF m_clrBackground{ CLR_INVALID };		//!< Background Colour. (used to make m_hBackBrush)
	COLORREF m_clrStartGradient{ CLR_INVALID };
	COLORREF m_clrEndGradient{ CLR_INVALID };

	HBRUSH m_hBackBrush{ nullptr };			//!< Background control color
	HBRUSH m_hBorderBrush{ nullptr };		//!< Controls Border Colour.

	HBITMAP m_bitmapBg{ nullptr };			//!< Background bitmap

	HCURSOR m_hCursor{ nullptr };			//!< Cursor Handle

	HWND m_ToolTipHWND{ nullptr };			//!< Tooltip window (if any)
	HWND m_pParentHWND{ nullptr };

	UINT m_iRefCount{};						//!< Controls reference counter
	UINT m_UserID{};						//!< controls User ID (ID - mIRC_ID_OFFSET)

	DWORD m_dEventMask{};

	BYTE m_iAlphaLevel{ 0x7f };				//!< The amount the control is alpha blended.

	bool m_bCursorFromFile{ false };		//!< Cursor comes from a file?
	bool m_bAlphaBlend{ false };			//!< Control is alpha blended.
	bool m_bGradientFill{ false };			//!< Gradient fill the background.
	bool m_bGradientVertical{ false };		//!< Draw gradient vertically.
	bool m_bInPrint{ false };				//!< Are we in the middle of a WM_PRINTCLIENT ?
	bool m_bShadowText{ false };			//!< Text is drawn with a shadow.
	bool m_bCtrlCodeText{ true };			//!< mIRC's ctrl codes are used to change the text's appearance.
	bool m_bThemed{ true };					//!< Is Control themed.

	/* ***** */
//protected:
	void parseGlobalCommandRequest(const TString& input, const XSwitchFlags& flags);
	bool parseGlobalInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const;

	LRESULT CommonMessage(const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed);
	void HandleChildControlSize();

	void DrawControl(HDC hDC, HWND hwnd);
	void ctrlDrawText(HDC hdc, const TString& txt, const LPRECT rc, const UINT style);
	void calcTextRect(HDC hdc, const TString& txt, LPRECT rc, const UINT style);

	//static std::pair<WindowStyle, WindowExStyle> parseBorderStyles(const TString & flags) noexcept;
	static void InvalidateParentRect(HWND hwnd) noexcept;
	static const DcxColourFlags parseColorFlags(const TString& flags) noexcept;
};

template <class T>
concept HasWinProc = requires(T t)
{
	T::m_hDefaultClassProc;
};

template <HasWinProc pClassObj>
void dcxRegisterClassEx(const TCHAR* const szClass, const TCHAR* const szDcxClass) noexcept
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);

	if (GetClassInfoEx(nullptr, szClass, &wc) != 0)
	{
		wc.lpszClassName = szDcxClass;
		pClassObj::m_hDefaultClassProc = wc.lpfnWndProc;
		wc.lpfnWndProc = DcxControl::WindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.style &= ~CS_GLOBALCLASS;
		RegisterClassEx(&wc);
	}
}
template <HasWinProc pClassObj>
void dcxRegisterClass(const TCHAR* const szClass, const TCHAR* const szDcxClass) noexcept
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);

	if (GetClassInfoEx(nullptr, szClass, &wc) != 0)
	{
		wc.lpszClassName = szDcxClass;
		pClassObj::m_hDefaultClassProc = wc.lpfnWndProc;
		wc.lpfnWndProc = DcxControl::WindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.style &= ~CS_GLOBALCLASS;
		RegisterClassEx(&wc);
	}
};

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXCONTROL_H_
