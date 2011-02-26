/*!
 * \file dcxscroll.h
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

#ifndef _DCXSCROLL_H_
#define _DCXSCROLL_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxScroll : public DcxControl {

public:

	DcxScroll( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles );
	virtual ~DcxScroll( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseCommandRequest( const TString & input );
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	inline TString getType( ) const { return TString( TEXT("scroll") ); };
	TString getStyles(void) const;

protected:

	INT m_nPage; //!< Scroll Page Size
	INT m_nLine; //!< Scroll Line Size

};

#endif // _DCXSCROLL_H_
