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

#include "defines.h"

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
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class XPopupMenuItem {

public:

	XPopupMenuItem( XPopupMenu * Parent, const BOOL bSep, ULONG_PTR dwDataBackup = NULL );
	XPopupMenuItem( XPopupMenu * Parent, const TString &tsItemText, const int nIcon, const BOOL bSubMenu, ULONG_PTR dwDataBackup = NULL );
	virtual ~XPopupMenuItem( );

	void DrawItem( const LPDRAWITEMSTRUCT lpdis );
	void DrawItemBackground( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol );
	void DrawItemBox( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol );
	static void DrawItemSelection( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis = FALSE, const BOOL bRounded = FALSE );
	static void DrawItemCheckBox( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis = FALSE );
	void DrawItemText( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis = FALSE );
	void DrawItemIcon( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const UINT iExStyles, const BOOL bSel = FALSE, const BOOL bDis = FALSE );
	static void DrawItemSubArrow( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOL bDis = FALSE );
	void DrawItemSeparator( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol );

	SIZE getItemSize( const HWND mHwnd );

	//void executeCommand( ); // doesnt exist...

	static void DrawGradient( const HDC hdc, const LPRECT lprc, const COLORREF clrStart, const COLORREF clrEnd, const BOOL bHorz = FALSE );
	static void DrawVerticalBar( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const BOOLEAN bReversed);
	static COLORREF LightenColor( const unsigned int iScale, const COLORREF clrColor );
	static COLORREF DarkenColor( const unsigned int iScale, const COLORREF clrColor );

	void setSubMenu( const BOOL bSubMenu );
	void setItemText( const TString & tsItemText );
	void setItemIcon( const int nIcon );
	const TString * getItemText( ) const;
	int getItemIcon(  ) const;

	ULONG_PTR getItemDataBackup() const;

protected:

	BOOL m_bSep; //!< Is Separator ?
	BOOL m_bSubMenu; //!< Has A SubMenu ?
	TString m_tsItemText; //!< Menu Item Text
	TString m_tsItemCommand; //!< Menu Item Command
	int m_nIcon; //!< Menu Item Icon Index
	XPopupMenu * m_pXParentMenu; //!< Parent XPopupMenu
	ULONG_PTR m_dwItemDataBackup;

};

typedef std::vector<XPopupMenuItem *> VectorOfXPopupMenuItem; //!< Vector of XPopupMenuItem Objects

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUITEM_H_
