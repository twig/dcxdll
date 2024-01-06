/*!
 * \file dcxwindow.h
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

#ifndef _DCXWINDOW_H_
#define _DCXWINDOW_H_

#include <map>
#include "Classes/tinyxml/tinyxml.h"
#include "EnumConcepts.h"

 // Event mask defines.
#define DCX_EVENT_MOUSE				0x00000001
#define DCX_EVENT_FOCUS				0x00000002
#define DCX_EVENT_THEME				0x00000004
#define DCX_EVENT_MOVE				0x00000008
#define DCX_EVENT_CLOSE				0x00000010
#define DCX_EVENT_SIZE				0x00000020
#define DCX_EVENT_CLICK				0x00000040
#define DCX_EVENT_DRAG				0x00000080
#define DCX_EVENT_HELP				0x00000100
#define DCX_EVENT_EDIT				0x00000200
#define DCX_EVENT_ALL				0xFFFFFFFF

enum class DcxResourceFlags
	: UINT
{
	None,
	FROMRESSOURCE,			//!< Cursor from ressource;
	FROMFILE				//!< Cursor from File;
};

using NoTheme = bool;

struct dcxWindowStyles
{
	WindowStyle		m_Styles{ WindowStyle::None };
	WindowExStyle	m_ExStyles{ WindowExStyle::None };
	NoTheme			m_NoTheme{ false };

	TString ToString() const
	{
		TString tsStyles;

		if (m_NoTheme)
			tsStyles = _T("notheme");

		if (dcx_testflag(m_Styles, WS_VSCROLL))
			tsStyles.addtok(TEXT("vsbar"));
		if (dcx_testflag(m_Styles, WS_HSCROLL))
			tsStyles.addtok(TEXT("hsbar"));
		if (dcx_testflag(m_Styles, WS_TABSTOP))
			tsStyles.addtok(TEXT("tabstop"));
		if (dcx_testflag(m_Styles, WS_GROUP))
			tsStyles.addtok(TEXT("group"));
		if (dcx_testflag(m_Styles, WS_DISABLED))
			tsStyles.addtok(TEXT("disabled"));
		if (dcx_testflag(m_ExStyles, WS_EX_TRANSPARENT))
			tsStyles.addtok(TEXT("transparent"));
		if (!dcx_testflag(m_Styles, WS_VISIBLE))
			tsStyles.addtok(TEXT("hidden"));

		//if (dcx_testflag(m_Styles, ES_MULTILINE))
		//	tsStyles.addtok(TEXT("multi"));
		//if (dcx_testflag(m_Styles, ES_CENTER))
		//	tsStyles.addtok(TEXT("center"));
		//if (dcx_testflag(m_Styles, ES_RIGHT))
		//	tsStyles.addtok(TEXT("right"));
		//if (dcx_testflag(m_Styles, ES_AUTOHSCROLL))
		//	tsStyles.addtok(TEXT("autohs"));
		//if (dcx_testflag(m_Styles, ES_AUTOVSCROLL))
		//	tsStyles.addtok(TEXT("autovs"));
		//if (dcx_testflag(m_Styles, ES_LOWERCASE))
		//	tsStyles.addtok(TEXT("lowercase"));
		//if (dcx_testflag(m_Styles, ES_NUMBER))
		//	tsStyles.addtok(TEXT("number"));
		//if (dcx_testflag(m_Styles, ES_PASSWORD))
		//	tsStyles.addtok(TEXT("password"));
		//if (dcx_testflag(m_Styles, ES_UPPERCASE))
		//	tsStyles.addtok(TEXT("uppercase"));
		//if (dcx_testflag(m_Styles, ES_WANTRETURN))
		//	tsStyles.addtok(TEXT("return"));
		//if (dcx_testflag(m_Styles, ES_READONLY))
		//	tsStyles.addtok(TEXT("readonly"));
		//if (dcx_testflag(m_Styles, ES_NOHIDESEL))
		//	tsStyles.addtok(TEXT("showsel"));
	}
};

enum class SizingTypes
	: UINT
{
	ReBar = 1,
	Status = 2,
	Panel = 4,
	Toolbar = 8
};

//using CursorValue = Dcx::CodeValue<HCURSOR, bool >;

struct CursorPair
{
	HCURSOR	cursor{ nullptr };
	bool	enabled{ false };
	TString src;
	TString flags;

	explicit operator bool() const noexcept
	{
		return enabled && cursor;
	}
};

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

 //class DcxBase
 //{
 //	DcxBase() = delete;
 //	DcxBase(const DcxBase& other) = delete;
 //	DcxBase& operator =(const DcxBase&) = delete;
 //	DcxBase(DcxBase&& other) = delete;
 //	DcxBase& operator =(DcxBase&&) = delete;
 //
 //	virtual void toXml(TiXmlElement* const xml) const = 0;
 //	virtual void parseCommandRequest(const TString& input) = 0;
 //	virtual TString parseInfoRequest(const TString& input) const = 0;
 //
 //};

class DcxWindow
{
public:
	DcxWindow() = delete;
	DcxWindow(const DcxWindow& other) = delete;
	DcxWindow& operator =(const DcxWindow&) = delete;
	DcxWindow(DcxWindow&& other) = delete;
	DcxWindow& operator =(DcxWindow&&) = delete;

	DcxWindow(const HWND mHwnd, const UINT mID) noexcept;
	explicit DcxWindow(const UINT mID) noexcept;
	virtual ~DcxWindow() noexcept;

	[[nodiscard]] bool isStyle(const WindowStyle Styles) const noexcept;
	WindowStyle removeStyle(const WindowStyle Styles) noexcept;
	WindowStyle addStyle(const WindowStyle Styles) noexcept;
	WindowStyle setStyle(const WindowStyle Styles) noexcept;
	[[nodiscard]] bool isExStyle(const WindowExStyle Styles) const noexcept;
	WindowExStyle removeExStyle(const WindowExStyle Styles) noexcept;
	WindowExStyle addExStyle(const WindowExStyle Styles) noexcept;
	WindowExStyle setExStyle(const WindowExStyle Styles) noexcept;

	[[nodiscard]] const HWND& getHwnd() const noexcept;
	[[nodiscard]] const UINT& getID() const noexcept;
	[[nodiscard]] inline const HWND& getToolTipHWND(void) const noexcept { return m_HwndTooltip; };
	[[nodiscard]] const bool IsValidWindow() const noexcept { return (m_Hwnd && IsWindow(m_Hwnd)); }

	inline void setToolTipHWND(HWND hwnd) noexcept { m_HwndTooltip = hwnd; }

	TString getBorderStyles() const;

	/// <summary>
	/// Opens tooltip window, if it exists.
	/// </summary>
	void OpenToolTip() const noexcept;

	/// <summary>
	/// Closes tooltip window, if open.
	/// </summary>
	void CloseToolTip() const noexcept;

	void redrawWindow() const noexcept;
	void redrawBufferedWindow();
	void redrawBufferedWindowClient();

	void HandleChildSizing(SizingTypes sz) const noexcept;

	virtual void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) = 0;
	virtual void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) = 0;
	virtual void toXml(TiXmlElement* const xml) const = 0;
	virtual TiXmlElement* toXml() const = 0;
	virtual void parseCommandRequest(const TString& input) = 0;
	virtual void xmlSetStyle(const TiXmlElement* xStyle) = 0;
	virtual void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const = 0;
	virtual TString parseInfoRequest(const TString& input) const
	{
		return TString();
	}
	
	inline const DWORD &getEventMask() const noexcept { return m_dEventMask; }
	void setEventMask(const TString& tspFlags, const TString& tsnFlags);

	bool TrackMouseEvents(DWORD events) noexcept;

	[[nodiscard]] static PTCHAR parseCursorType(const TString& cursor);
	[[nodiscard]] static DcxResourceFlags parseCursorFlags(const TString& flags) noexcept;
	[[nodiscard]] static UINT parseCursorArea(const TString& flags) noexcept;
	[[nodiscard("Memory Leak")]] static HIMAGELIST createImageList(bool bBigIcons = false) noexcept;
	[[nodiscard]] static dcxWindowStyles parseBorderStyles(const TString& tsFlags) noexcept;

protected:
	HWND m_Hwnd{ nullptr };
	UINT m_ID{};

	DWORD m_dEventMask{ DCX_EVENT_ALL };

	static inline HRGN m_hZeroRgn{ nullptr };
	HWND m_HwndTooltip{ nullptr };
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXWINDOW_H_
