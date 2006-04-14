/*!
 * \file xpopupmenuitem.h
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

#ifndef _XPOPUPMENUITEM_H_
#define _XPOPUPMENUITEM_H_

#include "../../defines.h"

#define XPMI_HEIGHT   22 //!< Menu Item Height
#define XPMI_BOXLPAD  0  //!< Padding space between menu left bordure and box left bordure
#define XPMI_BOXWIDTH 22 //!< Box Width (should be no less than 16)
#define XPMI_BOXRPAD  5  //!< Padding space between box right border and text/line
#define XPMI_ICONSIZE 16  //!< Icon Size

// dummy class, resolved at runtime
class XPopupMenu;
typedef struct tagXPMENUCOLORS XPMENUCOLORS, * LPXPMENUCOLORS;

/*!
 * \brief blah
 *
 * blah
 */

class XPopupMenuItem {

public:

  XPopupMenuItem( XPopupMenu * Parent, BOOL bSep );
  XPopupMenuItem( XPopupMenu * Parent, TString tsItemText, int nIcon, BOOL bSubMenu );
  virtual ~XPopupMenuItem( );

  void DrawItem( LPDRAWITEMSTRUCT lpdis );
  void DrawItemBackground( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol );
  void DrawItemBox( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol );
  void DrawItemSelection( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol, BOOL bDis = FALSE );
  void DrawItemCheckBox( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol, BOOL bDis = FALSE );
  void DrawItemText( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol, BOOL bDis = FALSE );
  void DrawItemIcon( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol, UINT iExStyles, BOOL bSel = FALSE, BOOL bDis = FALSE );
  void DrawItemSubArrow( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol, BOOL bDis = FALSE );
  void DrawItemSeparator( LPDRAWITEMSTRUCT lpdis, LPXPMENUCOLORS lpcol );

  SIZE getItemSize( HWND mHwnd );

  void executeCommand( );

  static void DrawGradient( HDC hdc, LPRECT lprc, COLORREF clrStart, COLORREF clrEnd, BOOL bHorz = FALSE );
  static COLORREF LightenColor( unsigned int iScale, COLORREF clrColor );
  static COLORREF DarkenColor( unsigned int iScale, COLORREF clrColor );

  void setSubMenu( BOOL bSubMenu );
  void setItemText( TString & tsItemText );
  void setItemIcon( int nIcon );
  TString * getItemText( );
  int getItemIcon(  );

protected:

  BOOL m_bSep; //!< Is Separator ?
  BOOL m_bSubMenu; //!< Has A SubMenu ?
  TString m_tsItemText; //!< Menu Item Text
  TString m_tsItemCommand; //!< Menu Item Command
  int m_nIcon; //!< Menu Item Icon Index
  XPopupMenu * m_pXParentMenu; //!< Parent XPopupMenu

};

typedef std::vector<XPopupMenuItem *> VectorOfXPopupMenuItem; //!< Vector of XPopupMenuItem Objects

#endif // _XPOPUPMENUITEM_H_