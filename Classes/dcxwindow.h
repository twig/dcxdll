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
	DcxWindow &operator =(const DcxWindow &) = delete;	// No assignments!

	DcxWindow( const HWND mHwnd, const UINT mID );
	explicit DcxWindow( const UINT mID );
	virtual ~DcxWindow( );

	bool isStyle( const WindowStyle Styles ) const noexcept;
	WindowStyle removeStyle( const WindowStyle Styles );
	WindowStyle addStyle( const WindowStyle Styles );
	WindowStyle setStyle( const WindowStyle Styles );
	bool isExStyle( const WindowExStyle Styles ) const noexcept;
	WindowExStyle removeExStyle( const WindowExStyle Styles );
	WindowExStyle addExStyle( const WindowExStyle Styles );
	WindowExStyle setExStyle( const WindowExStyle Styles );

	const HWND &getHwnd( ) const noexcept;
	const UINT &getID( ) const noexcept;

	void redrawWindow( );
	void redrawBufferedWindow( );
	virtual void toXml(TiXmlElement *const xml) const = 0;

	virtual void parseCommandRequest(const TString & input) = 0;
	//virtual void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const = 0;
	virtual void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const = 0;
	static PTCHAR parseCursorType(const TString & cursor);
	static UINT parseCursorFlags(const TString & flags);
	static UINT parseCursorArea(const TString &flags);
	static HIMAGELIST createImageList(bool bBigIcons = false);

	LRESULT CallDefaultProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	WNDPROC m_hDefaultWindowProc; //!< Old Window Procedure
	HWND m_Hwnd;
	UINT m_ID;

	HRGN	m_hZeroRgn;

	static const std::map<std::hash<TString>::result_type, PTCHAR> IDC_map;
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXWINDOW_H_
