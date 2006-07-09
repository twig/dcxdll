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

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxCalendar : public DcxControl {

public:

  DcxCalendar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxCalendar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxCalendar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "calendar" ); };

protected:

	static long SystemTimeToMircTime(SYSTEMTIME st);
};

#endif // _DCXCALENDAR_H_