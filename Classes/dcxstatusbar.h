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

  DcxStatusBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxStatusBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxStatusBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  HIMAGELIST createImageList( );

  UINT parseItemFlags( TString & flags );
  void cleanPartIcons( );
  
  LRESULT setParts( int nParts, LPINT aWidths );
  LRESULT getParts( int nParts, LPINT aWidths );
  LRESULT setBkColor( COLORREF clrBk );
  LRESULT setText( int iPart, int Style, LPSTR lpstr );
  LRESULT getText( int iPart, LPSTR lpstr );
  LRESULT setTipText( int iPart, LPSTR lpstr );
  LRESULT getTipText( int iPart, int nSize, LPSTR lpstr );
  LRESULT getRect( int iPart, LPRECT lprc );
  LRESULT setIcon( int iPart, HICON hIcon );
  LRESULT getIcon( int iPart );

  int hitTest( POINT & pt );

  inline TString getType( ) { return TString( "statusbar" ); };

protected:

  HIMAGELIST m_hImageList; //!< Internal Image List
  
};

#endif // _DCXSTATUSBAR_H_