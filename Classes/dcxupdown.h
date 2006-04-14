/*!
 * \file dcxupdown.h
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

#ifndef _DCXUPDOWN_H_
#define _DCXUPDOWN_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxUpDown : public DcxControl {

public:

  DcxUpDown( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxUpDown( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxUpDown( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  LRESULT setBuddy( HWND mHwnd );
  LRESULT setRange32( int iLow, int iHigh );
  LRESULT getRange32( LPINT iLow, LPINT iHigh );
  LRESULT setBase( int iBase );
  LRESULT getBase( );
  LRESULT setPos32( INT nPos );
  LRESULT getPos32( LPBOOL pfError );

  inline TString getType( ) { return TString( "updown" ); };

protected:

};

#endif // _DCXUPDOWN_H_