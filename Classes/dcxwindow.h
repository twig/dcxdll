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

	DcxWindow( const HWND mHwnd, const UINT mID );
	explicit DcxWindow( const UINT mID );
	virtual ~DcxWindow( );

	BOOL isStyle( const LONG Styles ) const;
	LONG removeStyle( const LONG Styles );
	LONG addStyle( const LONG Styles );
	LONG setStyle( const LONG Styles );
	BOOL isExStyle( const LONG Styles ) const;
	LONG removeExStyle( const LONG Styles );
	LONG addExStyle( const LONG Styles );
	LONG setExStyle( const LONG Styles );

	HWND getHwnd( ) const;
	UINT getID( ) const;

	void redrawWindow( );
	void redrawBufferedWindow( );
	virtual void toXml(TiXmlElement * xml) const = 0;

protected:

	HWND m_Hwnd;
	UINT m_ID;

	virtual void parseCommandRequest( const TString & input ) = 0;
	virtual void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const = 0;

};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _DCXWINDOW_H_
