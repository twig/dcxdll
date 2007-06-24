/*!
 * \file dcxcomboex.h
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

#ifndef _DCXCOMBOEX_H_
#define _DCXCOMBOEX_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

#define CBEXSEARCH_W 0x01 //!< ComboEx WildCard Search
#define CBEXSEARCH_R 0x02 //!< ComboEx Regex Search

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXCOMBOEXEDIT {

  WNDPROC OldProc; //!< Subclassed Window Procedure of Combo
  HWND cHwnd;      //!< Parent ComboEx Handle
  HWND pHwnd;      //!< Dialog Handle

} DCXCOMBOEXEDIT, *LPDCXCOMBOEXEDIT;

/*!
 * \brief blah
 *
 * blah
 */

class DcxComboEx : public DcxControl {

public:

  DcxComboEx( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxComboEx( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  static HIMAGELIST createImageList( );

  BOOL matchItemText( int nItem, TString * search, UINT SearchType );

  LRESULT insertItem( const PCOMBOBOXEXITEM lpcCBItem );
  LRESULT getItem( PCOMBOBOXEXITEM lpcCBItem ) const;
  LRESULT getEditControl( ) const;
  LRESULT deleteItem( const int iIndex );
  LRESULT setCurSel( const int iIndex );
  LRESULT getCurSel( ) const;
  LRESULT getLBText( const int iIndex, LPSTR lps );
  LRESULT resetContent( );
  LRESULT getCount( ) const;
  LRESULT limitText( const int iLimit );

  static LRESULT CALLBACK ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

  inline TString getType( ) { return TString( "comboex" ); };

protected:

  HWND m_EditHwnd;  //!< Combo's Edit Control Handle
};

#endif // _DCXCOMBOEX_H_
