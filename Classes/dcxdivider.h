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

#include "../defines.h"
#include "dcxcontrol.h"

#include "custom/divider.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDivider : public DcxControl {

public:

  DcxDivider( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxDivider( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxDivider( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  LRESULT setPane( UINT iPaneId, LPDVPANEINFO lpdvpi );
  LRESULT setDivPos( UINT iDivPos );

  inline TString getType( ) { return TString( "divider" ); };

protected:

};

#endif // _DCXDIVIDER_H_