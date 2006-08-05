/*!
 * \file dcxbox.h
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

#ifndef _DCXBOX_H_
#define _DCXBOX_H_

#include "../../defines.h"
#include "../dcxcontrol.h"

#include "../layout/layoutmanager.h"

#define DCX_BOXTEXTSPACING 10

#define BOXS_LEFT    0x00 //!< Box Left Style
#define BOXS_CENTER  0x01 //!< Box Center Style
#define BOXS_RIGHT   0x02 //!< Box Right Style
#define BOXS_BOTTOM  0x04 //!< Box Bottom Style
#define BOXS_NONE		0x08 //!< Box No Border Style
#define BOXS_ROUNDED 0x10 //!< Box has rounded corners.

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxBox : public DcxControl {

public:

  DcxBox( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxBox( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxBox( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "box" ); };

protected:

  UINT parseLayoutFlags( TString & flags );

  LONG m_iBoxStyles; //!< Custom Box Styles

  LayoutManager * m_pLayoutManager; //!< Layout Manager Object

};

#endif // _DCXBOX_H_