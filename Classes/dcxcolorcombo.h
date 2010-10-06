/*!
 * \file dcxcolorcombo.h
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

#ifndef _DCXCOLORCOMBO_H_
#define _DCXCOLORCOMBO_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXCCOMBOITEM {

  COLORREF clrItem;     //!< Item Color
  TString tsItemText;   //!< Item Text

} DCXCCOMBOITEM,*LPDCXCCOMBOITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxColorCombo : public DcxControl {

public:

  DcxColorCombo( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxColorCombo( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, PTCHAR szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  void setmIRCPalette( );

  LRESULT insertItem( const int nPos, const LPARAM lParam );
  LRESULT getCount( ) const;
  LRESULT setCurSel( const int nPos );
  LRESULT getItemData( const int nItem ) const;
  LRESULT getCurSel( ) const;
  LRESULT deleteItem( const int nItem );
  LRESULT resetContent( );

  inline TString getType( ) { return TString( TEXT("colorcombo") ); };

protected:
  
};

#endif // _DCXCOLORCOMBO_H_
