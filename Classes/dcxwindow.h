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

enum class DcxResourceFlags : UINT {
	None,
	FROMRESSOURCE,			//!< Cursor from ressource;
	FROMFILE				//!< Cursor from File;
};

using NoTheme = bool;

struct dcxWindowStyles {
	WindowStyle		m_Styles{ WindowStyle::None };
	WindowExStyle	m_ExStyles{ WindowExStyle::None };
	NoTheme			m_NoTheme{ false };

	//dcxWindowStyles(HWND hwnd) noexcept
	//	: m_ExStyles{ dcxGetWindowExStyle(hwnd) }
	//	, m_Styles{ dcxGetWindowStyle(hwnd) }
	//	, m_NoTheme{ Dcx::UXModule.dcxGetWindowTheme(hwnd) == nullptr }
	//{			
	//}

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

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class DcxWindow {

public:
	DcxWindow() = delete;
	DcxWindow(const DcxWindow &other) = delete;
	DcxWindow &operator =(const DcxWindow &) = delete;
	DcxWindow(DcxWindow &&other) = delete;
	DcxWindow &operator =(DcxWindow &&) = delete;

	DcxWindow( const HWND mHwnd, const UINT mID ) noexcept;
	explicit DcxWindow( const UINT mID ) noexcept;
	virtual ~DcxWindow( ) noexcept;

	bool isStyle( const WindowStyle Styles ) const noexcept;
	WindowStyle removeStyle( const WindowStyle Styles ) noexcept;
	WindowStyle addStyle( const WindowStyle Styles ) noexcept;
	WindowStyle setStyle( const WindowStyle Styles ) noexcept;
	bool isExStyle( const WindowExStyle Styles ) const noexcept;
	WindowExStyle removeExStyle( const WindowExStyle Styles ) noexcept;
	WindowExStyle addExStyle( const WindowExStyle Styles ) noexcept;
	WindowExStyle setExStyle( const WindowExStyle Styles ) noexcept;

	const HWND &getHwnd( ) const noexcept;
	const UINT &getID( ) const noexcept;

	void redrawWindow( ) noexcept;
	void redrawBufferedWindow( );

	virtual void toXml(TiXmlElement *const xml) const = 0;
	virtual void parseCommandRequest(const TString & input) = 0;
	//virtual void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const = 0;
	virtual void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const = 0;

	static PTCHAR parseCursorType(const TString & cursor);
	static DcxResourceFlags parseCursorFlags(const TString & flags) noexcept;
	static UINT parseCursorArea(const TString &flags) noexcept;
	static HIMAGELIST createImageList(bool bBigIcons = false) noexcept;
	static dcxWindowStyles parseBorderStyles(const TString& tsFlags) noexcept;

	LRESULT CallDefaultProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

protected:
	WNDPROC m_hDefaultWindowProc{ nullptr }; //!< Old Window Procedure
	HWND m_Hwnd{ nullptr };
	UINT m_ID{};

	HRGN	m_hZeroRgn{ nullptr };
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXWINDOW_H_
