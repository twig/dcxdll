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

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxUpDown : public DcxControl {

public:

  DcxUpDown( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxUpDown( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, TCHAR * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  LRESULT setBuddy( HWND mHwnd );
  LRESULT setRange32( const int iLow, const int iHigh );
  LRESULT getRange32( LPINT iLow, LPINT iHigh ) const;
  LRESULT setBase( const int iBase );
  LRESULT getBase( ) const;
  LRESULT setPos32( const INT nPos );
  LRESULT getPos32( LPBOOL pfError ) const;
  TString getStyles(void);

  inline TString getType( ) { return TString( TEXT("updown") ); };

protected:

};

#endif // _DCXUPDOWN_H_
