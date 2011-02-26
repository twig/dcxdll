/*!
 * \file dcxcalendar.h
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

#ifndef _DCXCALENDAR_H_
#define _DCXCALENDAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

#define BOLDDAY(ds,iDay) if(iDay>0 && iDay<32) (ds)|=(0x00000001<<(iDay-1))

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxCalendar : public DcxControl {

public:

	DcxCalendar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles );
	virtual ~DcxCalendar( );

	LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

	void parseInfoRequest( const TString & input, TCHAR * szReturnValue ) const;
	void parseCommandRequest( const TString & input );
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

	inline TString getType( ) const { return TString( TEXT("calendar") ); };
	void toXml(TiXmlElement * xml) const;
	TString getStyles(void) const;
	TString getValue(void) const;
};

#endif // _DCXCALENDAR_H_
