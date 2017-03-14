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

/*!
* \brief XPopup Menu Item Colors
*
* Structure containing the menu item colors
*/

struct XPMENUCOLORS {
	COLORREF m_clrBack;					//!< Menu Item BackGround Color
	COLORREF m_clrBox;					//!< Menu Item Box Color
	COLORREF m_clrLightBox;				//!< Menu Item Box Color (lighter by 200)
	COLORREF m_clrSelection;			//!< Menu Item Selection Box Color
	COLORREF m_clrDisabledSelection;	//!< Menu Item Disabled Selection Box Color
	COLORREF m_clrText;					//!< Menu Item Text Color
	COLORREF m_clrDisabledText;			//!< Menu Item Disabled Text Color
	COLORREF m_clrCheckBox;				//!< Menu Item CheckBox Color
	COLORREF m_clrDisabledCheckBox;		//!< Menu Item Disabled CheckBox Color
	COLORREF m_clrSeparatorLine;		//!< Menu Item Separator Line Color
	COLORREF m_clrSelectionBorder;		//!< Menu Item Selection Box Border Color
	COLORREF m_clrSelectedText;			//!< Menu Item Selected Text Colour

};
using LPXPMENUCOLORS = XPMENUCOLORS *;

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
	XPopupMenuItem() = delete;
	XPopupMenuItem(const XPopupMenuItem &) = delete;
	XPopupMenuItem &operator = (const XPopupMenuItem &) = delete;

	XPopupMenuItem( XPopupMenu * Parent, const bool bSep, ULONG_PTR dwDataBackup = NULL );
	XPopupMenuItem( XPopupMenu * Parent, const TString &tsItemText, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup = NULL );
	virtual ~XPopupMenuItem( );

	void DrawItem(const LPDRAWITEMSTRUCT lpdis);
	void DrawItemBackground( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol );
	void DrawItemBox( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol );
	void DrawItemText( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const bool bDis = false );
	void DrawItemIcon( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const UINT iExStyles, const bool bSel = false, const bool bDis = false );
	void DrawItemSeparator( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol );

	SIZE getItemSize( const HWND mHwnd );

	static void DrawItemSelection(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const bool bDis = false, const bool bRounded = false);
	static void DrawItemCheckBox(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const bool bDis = false);
	static void DrawItemSubArrow(const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const bool bDis = false);
	static bool DrawMenuBitmap(const LPDRAWITEMSTRUCT lpdis, const bool bBigImage, const HBITMAP bmImage);
	static void DrawGradient(const HDC hdc, const LPRECT lprc, const COLORREF clrStart, const COLORREF clrEnd, const bool bHorz = false);
	static void DrawVerticalBar( const LPDRAWITEMSTRUCT lpdis, const LPXPMENUCOLORS lpcol, const bool bReversed);
	static COLORREF LightenColor( const UINT iScale, const COLORREF clrColor ) noexcept;
	static COLORREF DarkenColor( const UINT iScale, const COLORREF clrColor ) noexcept;

	void setSubMenu( const bool bSubMenu ) noexcept;
	void setItemText( const TString & tsItemText );
	void setItemIcon( const int nIcon ) noexcept;
	const TString &getItemText( ) const noexcept;
	const int &getItemIcon(  ) const noexcept;

	const ULONG_PTR &getItemDataBackup() const noexcept;

protected:

	TString m_tsItemText;			//!< Menu Item Text
	TString m_tsItemCommand;		//!< Menu Item Command
	int m_nIcon;					//!< Menu Item Icon Index
	XPopupMenu * m_pXParentMenu;	//!< Parent XPopupMenu
	ULONG_PTR m_dwItemDataBackup;
	bool m_bBigBitmap;				//!< Single large bitmap image used for whole menu?
	bool m_bSep;					//!< Is Separator ?
	bool m_bSubMenu;				//!< Has A SubMenu ?
	bool m_bReserved;				//!< Reserved for future use.
};

using VectorOfXPopupMenuItem = std::vector<XPopupMenuItem *>; //!< Vector of XPopupMenuItem Objects

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUITEM_H_
