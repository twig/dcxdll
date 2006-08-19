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

#include "../../defines.h"
#include "../dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxScroll : public DcxControl {

public:

  DcxScroll( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxScroll( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxScroll( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "scroll" ); };

protected:

  INT m_nPage; //!< Scroll Page Size
  INT m_nLine; //!< Scroll Line Size

};

#endif // _DCXSCROLL_H_