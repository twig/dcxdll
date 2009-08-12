/*!
 * \file xpopupmenu.h
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

#ifndef _XPOPUPMENU_H_
#define _XPOPUPMENU_H_

#include "defines.h"
#include "Classes/xpopup/xpopupmenuitem.h"

#define XPS_ICON3D        0x01 //!< Icons have a 3D effect
#define XPS_DISABLEDSEL   0x02 //!< Disabled Items have a selectionbox
#define XPS_ICON3DSHADOW  0x04 //!< Icons have a 3D effect with undershadow

/*!
 * \brief XPopup Menu Item Colors
 *
 * Structure containing the menu item colors
 */

typedef struct tagXPMENUCOLORS {

	COLORREF m_clrBack;               //!< Menu Item BackGround Color
	COLORREF m_clrBox;                //!< Menu Item Box Color
	COLORREF m_clrSelection;          //!< Menu Item Selection Box Color
	COLORREF m_clrDisabledSelection;  //!< Menu Item Disabled Selection Box Color
	COLORREF m_clrText;               //!< Menu Item Text Color
	COLORREF m_clrDisabledText;       //!< Menu Item Disabled Text Color
	COLORREF m_clrCheckBox;           //!< Menu Item CheckBox Color
	COLORREF m_clrDisabledCheckBox;   //!< Menu Item Disabled CheckBox Color
	COLORREF m_clrSeparatorLine;      //!< Menu Item Separator Line Color
	COLORREF m_clrSelectionBorder;    //!< Menu Item Selection Box Border Color
	COLORREF m_clrSelectedText;				//!< Menu Item Selected Text Colour

} XPMENUCOLORS, * LPXPMENUCOLORS;

typedef std::vector<XPopupMenu *> VectorOfXPopupMenu; //!< Vector of XPopupMenu Objects

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class XPopupMenu {

public:

	/*!
	* \brief Menu Styles
	*
	* Availbale XPopupMenu Styles
	*/
	enum MenuStyle {
		XPMS_OFFICE2003,
		XPMS_OFFICE2003_REV,
		XPMS_OFFICEXP,
		XPMS_ICY,
		XPMS_ICY_REV,
		XPMS_GRADE,
		XPMS_GRADE_REV,
		XPMS_NORMAL,
		XPMS_CUSTOM,
		XPMS_VERTICAL,
		XPMS_VERTICAL_REV
	};

	// Menu Colors
	/*
	1  	Menu background color
	2 	Icon box color
	3 	Checkbox color
	4 	Disabled checkbox color
	5 	Disabled selection box color
	6 	Disabled text color
	7 	Selection box color
	8 	Selection box border color
	9 	Separator line color
	10 	Text color
	11 	Selected text color
	*/

	static const int XPMC_BACKGROUND = 1;
	static const int XPMC_ICONBOX = 2;
	static const int XPMC_CHECKBOX = 3;
	static const int XPMC_CHECKBOX_DISABLED= 4;
	static const int XPMC_SELECTIONBOX_DISABLED = 5;
	static const int XPMC_TEXT_DISABLED = 6;
	static const int XPMC_SELECTIONBOX = 7;
	static const int XPMC_SELECTIONBOX_BORDER = 8;
	static const int XPMC_SEPARATOR = 9;
	static const int XPMC_TEXT = 10;
	static const int XPMC_SELECTEDTEXT = 11;

	XPopupMenu( const TString tsName, HMENU hMenu );
	XPopupMenu( const TString & tsMenuName, MenuStyle mStyle );
	virtual ~XPopupMenu( );

	void parseXPopCommand( const TString & input );
	void parseXPopIdentifier( const TString & input, char * szReturnValue );
	static XPopupMenu::MenuStyle parseStyle(const TString &style);

	static HMENU parsePath( const TString & path, const HMENU hParent, const int depth = 1 );

	HIMAGELIST getImageList( );
	void destroyImageList( );

	MenuStyle getStyle( ) const;
	void setStyle( MenuStyle style );
	UINT getItemStyle( ) const;
	void setItemStyle( const UINT iExStyles );

	void deleteMenuItemData( XPopupMenuItem * p_Item, LPMENUITEMINFO mii = NULL);
	void deleteAllItemData( HMENU hMenu );

	TString getName( ) const;

	HMENU getMenuHandle( ) const { return this->m_hMenu; };

	LPXPMENUCOLORS getColors( ) const;
	void setColor( const int nColor, const COLORREF clrColor );
	COLORREF getColor( const int nColor ) const;
	void setDefaultColor(const int nColor);

	static LRESULT CALLBACK XPopupWinProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	static LRESULT OnMeasureItem( const HWND mHwnd, LPMEASUREITEMSTRUCT lpmis );
	static LRESULT OnDrawItem( const HWND mHwnd, LPDRAWITEMSTRUCT lpdis ); 

	void convertMenu( HMENU hMenu, const BOOL bForce );
	void cleanMenu( HMENU hMenu );
	void clearAllMenuItems( );

	HBITMAP getBackBitmap( ) const;
	void setBackBitmap( HBITMAP hBitmap );

	bool IsRounded(void) const { return this->m_bRoundedSel; };
	BYTE IsAlpha(void) const { return this->m_uiAlpha; };
	void SetRounded(bool rounded) { this->m_bRoundedSel = rounded; };
	void SetAlpha(BYTE alpha) { this->m_uiAlpha = alpha; };

	// Methods to attach and detach from mIRC menu.
	bool attachToMenuBar(HMENU menubar, TString label);
	void detachFromMenuBar(HMENU menubar);

	// Methods to access marked text.
	void setMarkedText(TString text);
	TString getMarkedText();

	VectorOfXPopupMenuItem m_vpMenuItem; //!< Vector of XPopupMenuItem Objects

protected:

	HMENU m_hMenu; //!< Menu Handle
	HIMAGELIST m_hImageList; //!< Menu ImageList
	MenuStyle m_MenuStyle; //!< Menu Style
	TString m_tsMenuName; //!< Menu Name
	TString m_tsMarkedText; //!< Extra field to store custom information
	UINT m_MenuItemStyles; //!< Menu Item Styles

	HBITMAP m_hBitmap; //!< Menu Item Background Image in Custom Style

	XPMENUCOLORS m_MenuColors; //!< Menu Colors

	bool m_bRoundedSel; //!< Menu has rounded selection box.
	BYTE m_uiAlpha;			//!< Menu is alpha blended. 0 -> 255

	bool m_bAttachedToMenuBar; //!< Is the menu attached to the mIRC window menubar?
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENU_H_
