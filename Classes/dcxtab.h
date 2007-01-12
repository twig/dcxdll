/*!
 * \file dcxtab.h
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

#ifndef _DCXTAB_H_
#define _DCXTAB_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXTCITEM {

  TString tsTipText;  //!< Tooltip Text
  HWND mChildHwnd;    //!< Hwnd to child window

} DCXTCITEM, * LPDCXTCITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTab : public DcxControl {

public:

  DcxTab( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxTab( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  static HIMAGELIST createImageList( );

  void deleteLParamInfo( const int nItem );
  void activateSelectedTab( );

  inline TString getType( ) { return TString( "tab" ); };

protected:

};

#endif // _DCXTAB_H_