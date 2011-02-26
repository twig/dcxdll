/*!
 * \file dcxdivider.h
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

#ifndef _DCXDIVIDER_H_
#define _DCXDIVIDER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/custom/divider.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDivider : public DcxControl {

public:

	DcxDivider( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles );
	virtual ~DcxDivider( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseCommandRequest( const TString & input );
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	LRESULT setPane( const UINT iPaneId, LPDVPANEINFO lpdvpi );
	LRESULT setDivPos( const UINT iDivPos );

	inline TString getType( ) const { return TString( TEXT("divider") ); };
	virtual TString getStyles(void) const;
	void toXml(TiXmlElement * xml) const;

protected:
	HWND m_leftPane;
	HWND m_rightPane;

};

#endif // _DCXDIVIDER_H_
