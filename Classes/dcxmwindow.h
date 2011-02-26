/*!
 * \file dcxmwindow.h
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

#ifndef _DCXMWINDOW_H_
#define _DCXMWINDOW_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxMWindow : public DcxControl {

public:

	DcxMWindow( const HWND cHwnd, const HWND pHwnd, const UINT ID, DcxDialog * p_Dialog, RECT * rc, const TString & styles );
	virtual ~DcxMWindow( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseCommandRequest( const TString & input );
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	inline TString getType( ) const { return TString( TEXT("window") ); };

protected:

	LONG m_OrigStyles;      //!< Window Original Styles
	LONG m_OrigExStyles;    //!< Window Original Extended Styles
	HWND m_OrigParentHwnd;  //!< Window Original Parent Handle
	UINT m_OrigID;          //!< Window Original Control ID
	TString m_OrigName;     //!< Dialog Original Name
};

#endif // _DCXMWINDOW_H_
