/*!
 * \file dcxpanel.h
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

#ifndef _DCXPANEL_H_
#define _DCXPANEL_H_

#include "../defines.h"
#include "dcxcontrol.h"

#include "layout/layoutmanager.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxPanel : public DcxControl {

public:

  //DcxPanel( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxPanel( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxPanel( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "panel" ); };

protected:

  static UINT parseLayoutFlags( TString & flags );

  LayoutManager * m_pLayoutManager; //!< Layout Manager Object

};

#endif // _DCXPANEL_H_