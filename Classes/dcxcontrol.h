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
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXCONTROL_H_
#define _DCXCONTROL_H_

#include "defines.h"
#include "dcxwindow.h"
#include "CtrlHelper.h"

 //#include "DcxTextRender.h"

enum class DcxColourFlags
	: UINT
{
	None,								// No styles

	TEXTCOLOR,							// Text Color;
	TEXTBKGCOLOR,						// Text Background Color;

	BKGCOLOR = 0x0004,					// Control Background Color;
	BORDERCOLOR = 0x0008,				// Control Border Color;
	GRADSTARTCOLOR = 0x0010,			// Control gradient background start colour.
	GRADENDCOLOR = 0x0020,				// Control gradient background end colour.

	CHECKBOXFRAMECOLOR = 0x0040,		// Checkbox frame colour
	CHECKBOXBGCOLOR = 0x0080,			// CheckBox Background colour
	CHECKBOXTICKCOLOR = 0x0100,			// CheckBox Tick colour
	CHECKBOXHOT = 0x0200,				// CheckBox Background colour
	CHECKBOXDISABLED = 0x0400,			// CheckBox Background colour

	TEXTGRADSTARTCOLOR = 0x0800,		// Text Colour At the start of the gradient;
	TEXTGRADENDCOLOR = 0x1000,			// Text Colour At the end of the gradient;
	TEXTOUTLINECOLOR = 0x2000,			// Text outline colour.
	TEXTOUTLINEGRADSTARTCOLOR = 0x4000,	// Text gradient outline start colour.
	TEXTOUTLINEGRADENDCOLOR = 0x8000,	// Text gradient outline end colour.
	TEXTSHADOWCOLOR = 0x10000,			// Text shadow colour.
	TEXTGLOWCOLOR = 0x20000				// Text glow colour.
};

enum class DcxAllowControls
	: UINT64
{
	ALLOW_PBAR = 0x0000000000000001ULL,
	ALLOW_TRACKBAR = 0x0000000000000002ULL,
	ALLOW_COMBOEX = 0x0000000000000004ULL,
	ALLOW_COLORCOMBO = 0x0000000000000008ULL,
	ALLOW_STATUSBAR = 0x0000000000000010ULL,
	ALLOW_TOOLBAR = 0x0000000000000020ULL,
	ALLOW_TREEVIEW = 0x0000000000000040ULL,
	ALLOW_LISTVIEW = 0x0000000000000080ULL,
	ALLOW_REBAR = 0x0000000000000100ULL,
	ALLOW_BUTTON = 0x0000000000000200ULL,
	ALLOW_RICHEDIT = 0x0000000000000400ULL,
	ALLOW_EDIT = 0x0000000000000800ULL,
	ALLOW_UPDOWN = 0x0000000000001000ULL,
	ALLOW_IPADDRESS = 0x0000000000002000ULL,
	ALLOW_WEBCTRL = 0x0000000000004000ULL,
	ALLOW_CALANDER = 0x0000000000008000ULL,
	ALLOW_DIVIDER = 0x0000000000010000ULL,
	ALLOW_PANEL = 0x0000000000020000ULL,
	ALLOW_TAB = 0x0000000000040000ULL,
	ALLOW_LINE = 0x0000000000080000ULL,
	ALLOW_BOX = 0x0000000000100000ULL,
	ALLOW_RADIO = 0x0000000000200000ULL,
	ALLOW_CHECK = 0x0000000000400000ULL,
	ALLOW_TEXT = 0x0000000000800000ULL,
	ALLOW_SCROLL = 0x0000000001000000ULL,
	ALLOW_LIST = 0x0000000002000000ULL,
	ALLOW_LINK = 0x0000000004000000ULL,
	ALLOW_IMAGE = 0x0000000008000000ULL,
	ALLOW_PAGER = 0x0000000010000000ULL,
	ALLOW_DOCK = 0x0000000020000000ULL, // allows @Window and Dialog docking
	ALLOW_DATETIME = 0x0000000040000000ULL,
	ALLOW_STACKER = 0x0000000080000000ULL,
	ALLOW_DIRECTSHOW = 0x0000000100000000ULL,
	ALLOW_GRID = 0x0000000200000000ULL,
	ALLOW_MULTIBUTTON = 0x0000000400000000ULL,
	ALLOW_MULTICOMBO = 0x0000000800000000ULL,
	ALLOW_WEB2CTRL = 0x0000001000000000ULL,
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
	TRACKBAR, TREEVIEW, UPDOWN, WEBCTRL, MULTIBUTTON, MULTICOMBO, GRID, WEB2CTRL
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
		if (!hwnd)
			throw Dcx::dcxException("Invalid HWND");

		if (!GetClientRect(hwnd, &ai_rcClient))
			throw Dcx::dcxException("Unable to get Client Rect");
		if (!GetWindowRect(hwnd, &ai_rcWin))
			throw Dcx::dcxException("Unable to get Window Rect");
	}

	ALPHAINFO() = default;

	bool operator==(const ALPHAINFO& other) const = default;

	ALPHAINFO(const HDC& ai_hdc, HDC* ai_hdcBuffer, const HDC& ai_Oldhdc, const HBITMAP& ai_bitmap, const HBITMAP& ai_bkg, const HBITMAP& ai_oldBM, const RECT& ai_rcClient, const RECT& ai_rcWin, const BLENDFUNCTION& ai_bf, const HPAINTBUFFER& ai_Buffer) noexcept
		: ai_hdc(ai_hdc), ai_hdcBuffer(ai_hdcBuffer), ai_Oldhdc(ai_Oldhdc), ai_bitmap(ai_bitmap), ai_bkg(ai_bkg), ai_oldBM(ai_oldBM), ai_rcClient(ai_rcClient), ai_rcWin(ai_rcWin), ai_bf(ai_bf), ai_Buffer(ai_Buffer)
	{
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
GSL_SUPPRESS(c.128) xclass &operator =(const xclass &) = delete; \
xclass(xclass &&other) noexcept = delete; \
GSL_SUPPRESS(c.128) xclass &operator =(xclass &&) noexcept = delete; \
bool operator==(const xclass& other) const = delete;

template <class T, std::size_t N>
GSL_SUPPRESS(bounds)
consteval bool CheckFreeCommand(T cmd, const T(&global_cmds)[N], std::size_t offset)
{
	if (offset < N)
		return cmd == global_cmds[offset] ? false : CheckFreeCommand(cmd, global_cmds, offset + 1);

	return true;
}
// check if cmd is in a list of already used global commands.
consteval bool CheckFreeCommand(TCHAR cmd)
{
	// list of command chars in use by parseGlobalCommandRequest()
	const TCHAR global_cmds[] = TEXT("bCefFhJMpxURsTz");
	return CheckFreeCommand(cmd, global_cmds, 0);
}
// check if this command is valid & not in use globally.
template <TCHAR c>
GSL_SUPPRESS(bounds)
inline bool IsThisCommand(const XSwitchFlags& xflags) noexcept
{
	static_assert(CheckFreeCommand(c), "Command in use!");

	return xflags[c];
}

inline RECT TSToRect(const TString& tsInput)
{
	RECT rc{};

	rc.left = tsInput.getnexttokas<LONG>();
	rc.top = tsInput.getnexttokas<LONG>();
	rc.right = rc.left + tsInput.getnexttokas<LONG>();
	rc.bottom = rc.top + tsInput.getnexttokas<LONG>();

	return rc;
}

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
	GSL_SUPPRESS(c.128) DcxControl& operator =(const DcxControl&) = delete;	// No copy assignments!
	DcxControl(DcxControl&& other) = delete;	// no move constructor
	GSL_SUPPRESS(c.128) DcxControl& operator =(DcxControl&&) = delete;	// No move assignments!
	GSL_SUPPRESS(c.128) bool operator==(const DcxControl& other) const = delete;

	DcxControl(const UINT mID, gsl::strict_not_null<DcxDialog* const> p_Dialog) noexcept;
	~DcxControl() noexcept;

	virtual dcxWindowStyles parseControlStyles(const TString& tsStyles) = 0;

	[[nodiscard]] dcxWindowStyles parseGeneralControlStyles(const TString& styles, dcxWindowStyles& ws);
	[[nodiscard]] dcxWindowStyles parseGeneralControlStyles(const TString& styles);

	bool evalAliasEx(_Out_writes_z_(maxlen) TCHAR* const szReturn, _In_ const int maxlen, _In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const;

	//template <typename Format, typename Value, typename... Arguments>
	//bool evalAliasEx(TCHAR *const szReturn, const int maxlen, const Format &fmt, const Value val, Arguments&&... args) const
	//{
	//	TString tsBuf;
	//	getParentDialog()->evalAlias(szReturn, maxlen, _ts_sprintf(tsBuf, fmt, val, args...).to_chr());
	//}

	bool execAliasEx(_In_z_ _Printf_format_string_ const TCHAR* const szFormat, ...) const;

	//template <typename Format, typename Value, typename... Arguments>
	//bool execAliasEx(const Format &fmt, const Value val, Arguments&&... args) const
	//{
	//	TString tsBuf;
	//	getParentDialog()->execAlias(_ts_sprintf(tsBuf, fmt, val, args...).to_chr());
	//}

	[[nodiscard]] const UINT& getUserID() const noexcept { return m_UserID; }

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	virtual LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) = 0;
	virtual LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) = 0;

	virtual const TString getType() const = 0;
	virtual const DcxControlTypes getControlType() const noexcept = 0;

	virtual const TString getStyles(void) const;
	void toXml(TiXmlElement* const xml) const override;
	TiXmlElement* toXml(void) const override;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) override;

	void xmlParseElements(const TString& tsPath, const TiXmlElement* xParent, const TiXmlElement* xTemplate);
	void xmlAddPane(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xElement, const TiXmlElement* xTemplate);
	bool xmlAddControl(const TString& tsParentPath, const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCtrl);
	void xmlCallTemplate(const TString& tsCurrentPath, const TiXmlElement* xParent, const TiXmlElement* xCallTemplate);

	/// <summary>
	/// Sets the dialogs style, as in font and colours
	/// </summary>
	/// <param name="xStyle">- Pointer to the &lt;style&gt; tag to apply or to the &lt;dialog&gt; its self if no &lt;style&gt; applies.</param>
	void xmlSetStyle(const TiXmlElement* xStyle) override;

	void xmlSetStyles();
	void xmlLoadIcons(const TiXmlElement* xThis);
	void xmlSaveImageList(HIMAGELIST himl, TiXmlElement* xml, const TString& tsFlags) const;

	virtual LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	LRESULT setFont(const HFONT hFont, const BOOL fRedraw) noexcept;
	void setControlFont(const HFONT hFont, const BOOL fRedraw) noexcept;
	LRESULT setRedraw(const BOOL fView) noexcept;
	void setBackClrBrush(const HBRUSH c) noexcept { m_hBackBrush = c; }
	void setTextBackColor(const COLORREF c) noexcept { m_TextOptions.m_clrTextBackground = c; }
	void setTextColor(const COLORREF c) noexcept { m_TextOptions.m_clrText = c; }
	void setStartGradientColor(const COLORREF c) noexcept { m_clrStartGradient = c; }
	void setEndGradientColor(const COLORREF c) noexcept { m_clrEndGradient = c; }

	//void setToolTipHWND(const HWND hwnd) noexcept { m_ToolTipHWND = hwnd; }
	void setNoThemed(const bool b) noexcept { m_bNoThemed = b; }
	void setAlphaBlended(const bool b) noexcept { m_bAlphaBlend = b; }
	void setControlCursor(const HCURSOR c) noexcept { m_hCursor.cursor = c; }
	void setShadowTextState(const bool b) noexcept { m_TextOptions.m_bShadow = b; }
	void setControlCodeTextState(const bool b) noexcept { m_TextOptions.m_bCtrlCodes = b; }
	void setCursor(const TString& tsFlags, TString& tsFilename);
	void setRegion(const TString& tsFlags, const TString& tsArgs);

	[[nodiscard]] HFONT getFont() const noexcept;
	[[nodiscard]] const HFONT& getControlFont() const noexcept
	{
		return this->m_hFont;
	}
	[[nodiscard]] const HBRUSH& getBackClrBrush() const noexcept
	{
		return this->m_hBackBrush;
	}
	[[nodiscard]] const COLORREF& getBackColor() const noexcept
	{
		return this->m_clrBackground;
	}
	[[nodiscard]] const COLORREF& getBackTextColor() const noexcept
	{
		return this->m_TextOptions.m_clrTextBackground;
	}
	[[nodiscard]] const COLORREF& getTextColor() const noexcept
	{
		return this->m_TextOptions.m_clrText;
	}
	[[nodiscard]] const COLORREF& getStartGradientColor() const noexcept
	{
		return m_clrStartGradient;
	};
	[[nodiscard]] const COLORREF& getEndGradientColor() const noexcept
	{
		return m_clrEndGradient;
	};
	[[nodiscard]] const RECT getWindowPosition() const noexcept;
	//[[nodiscard]] DcxDialog* const getParentDialog() const noexcept
	//{
	//	return m_pParentDialog;
	//}
	[[nodiscard]] gsl::strict_not_null<DcxDialog* const> getParentDialog() const noexcept
	{
		return m_pParentDialog;
	}
	[[nodiscard]] const HWND& getParentHWND() const noexcept
	{
		return m_pParentHWND;
	}
	//[[nodiscard]] const HWND& getToolTipHWND() const noexcept
	//{
	//	return m_ToolTipHWND;
	//}
	[[nodiscard]] const HCURSOR& getControlCursor() const noexcept
	{
		return m_hCursor.cursor;
	}
	TString FontToCommand() const;

	[[nodiscard]] const inline bool& IsAlphaBlend() const noexcept
	{
		return m_bAlphaBlend;
	}
	[[nodiscard]] const inline bool IsThemed() const noexcept
	{
		return !m_bNoThemed;
	}

	// text options
	[[nodiscard]] const inline bool& IsShadowTextEnabled() const noexcept
	{
		return m_TextOptions.m_bShadow;
	}
	[[nodiscard]] const inline bool& IsControlCodeTextEnabled() const noexcept
	{
		return m_TextOptions.m_bCtrlCodes;
	}
	[[nodiscard]] const inline bool& IsFilledOutlineTextEnabled() const noexcept
	{
		return m_TextOptions.m_bFilledOutline;
	}
	[[nodiscard]] const inline bool& IsOutlineTextEnabled() const noexcept
	{
		return m_TextOptions.m_bOutline;
	}
	[[nodiscard]] const inline bool& IsDoubleOutlineTextEnabled() const noexcept
	{
		return m_TextOptions.m_bDoubleOutline;
	}
	[[nodiscard]] const inline bool& IsGradientTextEnabled() const noexcept
	{
		return m_TextOptions.m_bGradientFill;
	}
	[[nodiscard]] const inline bool& IsGlowTextEnabled() const noexcept
	{
		return m_TextOptions.m_bGlow;
	}
	[[nodiscard]] const inline bool& IsGradientOutlineTextEnabled() const noexcept
	{
		return m_TextOptions.m_bGradientOutline;
	}
	[[nodiscard]] const inline bool& IsHorizontalGradientTextEnabled() const noexcept
	{
		return m_TextOptions.m_bHorizGradientFill;
	}
	[[nodiscard]] const inline bool& IsHorizontalGradientOutlineTextEnabled() const noexcept
	{
		return m_TextOptions.m_bHorizGradientOutline;
	}
	[[nodiscard]] const inline bool& IsTransparentTextEnabled() const noexcept
	{
		return m_TextOptions.m_bTransparent;
	}

	// background options
	[[nodiscard]] const inline bool& IsGradientFillEnabled() const noexcept
	{
		return m_bGradientFill;
	}
	[[nodiscard]] const inline bool& IsGradientFillVertical() const noexcept
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
	[[nodiscard]] inline const UINT& getRefCount() const noexcept
	{
		return m_iRefCount;
	};

	void updateParentCtrl() noexcept; //!< updates controls host control pointers, MUST be called before these pointers are used.
	void DrawParentsBackground(const HDC hdc, const RECT* const rcBounds = nullptr, const HWND dHwnd = nullptr);
	[[nodiscard("Memory Leak")]] LPALPHAINFO SetupAlphaBlend(HDC* hdc, const bool DoubleBuffer = false);
	void FinishAlphaBlend(LPALPHAINFO ai) noexcept;
	void MoveFocusToNext() const noexcept
	{
		PostMessage(m_Hwnd, WM_NEXTDLGCTL, FALSE, FALSE);
	};
	void MoveFocusToPrev() const noexcept
	{
		PostMessage(m_Hwnd, WM_NEXTDLGCTL, FALSE, TRUE);
	};

	void showError(const TCHAR* const prop, const TCHAR* const cmd, const TCHAR* const err) const;
	template <typename Format, typename Value, typename... Arguments>
	void showError(const TCHAR* const prop, const TCHAR* const cmd, const Format& fmt, const Value& val, Arguments&& ... args) const
	{
		TString tsErr;
		showError(prop, cmd, _ts_sprintf(tsErr, fmt, val, args...).to_chr());
	}

	virtual void HandleDragDrop(int x, int y) noexcept;
	virtual void HandleDragMove(int x, int y) noexcept;

	static LRESULT CALLBACK WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	[[nodiscard("Memory Leak")]] static DcxControl* controlFactory(gsl::strict_not_null<DcxDialog* const> p_Dialog, const UINT mID, const TString& input, const UINT offset, const DcxAllowControls mask, HWND hParent = nullptr);
	static void DrawCtrlBackground(const HDC hdc, const DcxControl* const p_this, const RECT* const rwnd = nullptr, HTHEME hTheme = nullptr, const int iPartId = 0, const int iStateId = 0) noexcept;
	[[nodiscard]] static HBITMAP resizeBitmap(HBITMAP srcBM, const RECT* const rc) noexcept;
	[[nodiscard]] static DcxControlTypes TSTypeToControlType(const TString& t);
	// Convert a number into the closest icon size
	[[nodiscard]] static DcxIconSizes NumToIconSize(const int& num) noexcept;

	static void InitializeDcxControls();
	static void UnInitializeDcxControls() noexcept;

protected:
	//private:
	static inline bool m_bInitialized{ false };

	//DcxDialog* m_pParentDialog{ nullptr };	//!< Parent DcxDialog object
	gsl::strict_not_null<DcxDialog*> m_pParentDialog;	//!< Parent DcxDialog object

	HFONT m_hFont{ nullptr };					//!< Control Font

	TString m_tsMark;				//!< Mark Information (see /xdid -M)
	TString m_tsToolTip;			//!< This controls tooltip text (if any).

	//DcxTextRender m_Render;

	//COLORREF m_clrText{ CLR_INVALID };				//!< Font color
	//COLORREF m_clrBackText{ CLR_INVALID };			//!< Font Back Color (not supported)
	COLORREF m_colTransparentBg{ CLR_INVALID };		//!< Colour thats transparent when doing colour key.
	COLORREF m_clrBackground{ CLR_INVALID };		//!< Background Colour. (used to make m_hBackBrush)
	COLORREF m_clrStartGradient{ CLR_INVALID };		//!< Start colour for gradient fill background.
	COLORREF m_clrEndGradient{ CLR_INVALID };		//!< End colour for gradient fill background.

	dcxTextOptions m_TextOptions;

	HBRUSH m_hBackBrush{ nullptr };			//!< Background control color
	HBRUSH m_hBorderBrush{ nullptr };		//!< Controls Border Colour.

	//HBITMAP m_bitmapBg{ nullptr };			//!< Background bitmap

	CursorPair m_hCursor;

	//HWND m_ToolTipHWND{ nullptr };			//!< Tooltip window (if any) (this is a copy of the dialogs tooltip window, dont free)
	HWND m_pParentHWND{ nullptr };			// temp storage, dont free

	UINT m_iRefCount{};						//!< Controls reference counter
	UINT m_UserID{};						//!< controls User ID (ID - mIRC_ID_OFFSET)

	BYTE m_iAlphaLevel{ 0x7f };				//!< The amount the control is alpha blended.

	bool m_bAlphaBlend{ false };			//!< Control is alpha blended.
	bool m_bGradientFill{ false };			//!< Gradient fill the background.
	bool m_bGradientVertical{ false };		//!< Draw gradient vertically.
	bool m_bInPrint{ false };				//!< Are we in the middle of a WM_PRINTCLIENT ?
	//bool m_bShadowText{ false };			//!< Text is drawn with a shadow.
	//bool m_bCtrlCodeText{ true };			//!< mIRC's ctrl codes are used to change the text's appearance.
	bool m_bNoThemed{ true };				//!< Is Control themed.

	/* ***** */
//protected:

	/// <summary>
	/// General commands
	/// </summary>
	/// <param name="input"> - The input for the specified command.</param>
	/// <param name="flags"> - a supported command - b/e/h/s/U/T/R/f/F/p/x/C/J/M/Z</param>
	void parseGlobalCommandRequest(const TString& input, const XSwitchFlags& flags);

	/// <summary>
	/// General info request.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="szReturnValue"></param>
	/// <returns></returns>
	bool parseGlobalInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const;

	TString parseGlobalInfoRequest(const TString& input) const;

	LRESULT CommonMessage(const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed);
	void HandleChildControlSize();

	void DrawControl(HDC hDC, HWND hwnd) const;
	void ctrlDrawText(HDC hdc, const TString& txt, const LPRECT rc, const UINT style) const;
	void calcTextRect(HDC hdc, const TString& txt, LPRECT rc, const UINT style) const;

	static void InvalidateParentRect(HWND hwnd) noexcept;
	[[nodiscard]] static const DcxColourFlags parseColorFlags(const TString& flags) noexcept;
};

template <class T>
concept HasWinProc = requires(T t)
{
	T::m_hDefaultClassProc;
};

template <HasWinProc pClassObj>
void dcxRegisterNewClass(const TCHAR* const szDcxClass, WNDPROC pDefProc) noexcept
{
	if (!szDcxClass || !pDefProc)
		return;

	const WNDCLASSEX wc{ sizeof(WNDCLASSEX),0, DcxControl::WindowProc, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1), nullptr, szDcxClass, nullptr };
	pClassObj::m_hDefaultClassProc = pDefProc;
	RegisterClassEx(&wc);
}
template <HasWinProc pClassObj>
void dcxRegisterClass(const TCHAR* const szClass, const TCHAR* const szDcxClass) noexcept
{
	if (!szClass || !szDcxClass)
		return;

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
