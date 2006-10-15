/*!
 * \file dcxdatetime.h
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

#ifndef _DCXDATETIMEe_H_
#define _DCXDATETIME_H_

#include "../defines.h"
#include "dcxcontrol.h"

#define BOLDDAY(ds,iDay) if(iDay>0 && iDay<32) (ds)|=(0x00000001<<(iDay-1))

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDateTime : public DcxControl {

public:

  //DcxDateTime( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxDateTime( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxDateTime( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "datetime" ); };

protected:
	static SYSTEMTIME MircTimeToSystemTime(long mircTime);
	static long SystemTimeToMircTime(LPSYSTEMTIME pst);
};

#endif // _DCXDATETIME_H_