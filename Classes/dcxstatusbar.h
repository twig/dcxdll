/*!
 * \file dcxstatusbar.h
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

#ifndef _DCXSTATUSBAR_H_
#define _DCXSTATUSBAR_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxStatusBar : public DcxControl {

public:

  DcxStatusBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxStatusBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  static HIMAGELIST createImageList( );

  static UINT parseItemFlags( TString & flags );
  void cleanPartIcons( );
  
  LRESULT setParts( const int nParts, const LPINT aWidths );
  LRESULT getParts( const int nParts, LPINT aWidths ) const;
  LRESULT setBkColor( const COLORREF clrBk );
  LRESULT setText( const int iPart, const int Style, const LPSTR lpstr );
  LRESULT getText( const int iPart, LPSTR lpstr ) const;
  LRESULT setTipText( const int iPart, const LPSTR lpstr );
  LRESULT getTipText( const int iPart, const int nSize, LPSTR lpstr ) const;
  LRESULT getRect( const int iPart, LPRECT lprc ) const;
  LRESULT setIcon( const int iPart, const HICON hIcon );
  LRESULT getIcon( const int iPart ) const;

  int hitTest( const POINT & pt ) const;

  inline TString getType( ) { return TString( "statusbar" ); };

protected:

  HIMAGELIST m_hImageList; //!< Internal Image List
  
};

#endif // _DCXSTATUSBAR_H_