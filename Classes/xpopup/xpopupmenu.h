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

#pragma once
#ifndef _XPOPUPMENU_H_
#define _XPOPUPMENU_H_

#include "defines.h"
#include "Classes/xpopup/xpopupmenuitem.h"

#define XPS_ICON3D        0x01 //!< Icons have a 3D effect
#define XPS_DISABLEDSEL   0x02 //!< Disabled Items have a selectionbox
#define XPS_ICON3DSHADOW  0x04 //!< Icons have a 3D effect with undershadow

using VectorOfXPopupMenu = std::vector<XPopupMenu *>; //!< Vector of XPopupMenu Objects

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
	enum MenuStyle: UINT {
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
		XPMS_VERTICAL_REV,
		XPMS_BUTTON,
		XPMS_CUSTOMBIG
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

	//static const int XPMC_BACKGROUND = 1;
	//static const int XPMC_ICONBOX = 2;
	//static const int XPMC_CHECKBOX = 3;
	//static const int XPMC_CHECKBOX_DISABLED= 4;
	//static const int XPMC_SELECTIONBOX_DISABLED = 5;
	//static const int XPMC_TEXT_DISABLED = 6;
	//static const int XPMC_SELECTIONBOX = 7;
	//static const int XPMC_SELECTIONBOX_BORDER = 8;
	//static const int XPMC_SEPARATOR = 9;
	//static const int XPMC_TEXT = 10;
	//static const int XPMC_SELECTEDTEXT = 11;

	enum MenuColours : UINT {
		XPMC_MIN = 1,
		XPMC_BACKGROUND = 1,
		XPMC_ICONBOX,
		XPMC_CHECKBOX,
		XPMC_CHECKBOX_DISABLED,
		XPMC_SELECTIONBOX_DISABLED,
		XPMC_TEXT_DISABLED,
		XPMC_SELECTIONBOX,
		XPMC_SELECTIONBOX_BORDER,
		XPMC_SEPARATOR,
		XPMC_TEXT,
		XPMC_SELECTEDTEXT,
		XPMC_MAX
	};

	XPopupMenu() = delete;
	XPopupMenu(const XPopupMenu &) = delete;
	XPopupMenu &operator = (const XPopupMenu &) = delete;
	XPopupMenu(XPopupMenu &&) = delete;
	XPopupMenu &operator =(XPopupMenu &&) = delete;

	XPopupMenu(const TString &tsName, HMENU hMenu);
	XPopupMenu( const TString & tsMenuName, MenuStyle mStyle );
	virtual ~XPopupMenu( );

	void parseXPopCommand(const TString & input);
	//void parseXPopIdentifier( const TString & input, TCHAR * szReturnValue ) const;
	void parseXPopIdentifier(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const;
	static XPopupMenu::MenuStyle parseStyle(const TString &style) noexcept;

	static HMENU parsePath( const TString & path, const HMENU hParent, const UINT depth = 1 );

	HIMAGELIST &getImageList( ) noexcept;
	void destroyImageList( ) noexcept;

	const MenuStyle &getStyle() const noexcept
	{
		return this->m_MenuStyle;
	}
	constexpr void setStyle(const MenuStyle style) noexcept
	{
		this->m_MenuStyle = style;
	}
	const UINT &getItemStyle() const noexcept
	{
		return this->m_MenuItemStyles;
	}
	constexpr void setItemStyle(const UINT iExStyles) noexcept
	{
		this->m_MenuItemStyles = iExStyles;
	}

	void deleteMenuItemData(const XPopupMenuItem *const p_Item, LPMENUITEMINFO mii = nullptr);
	void deleteAllItemData( HMENU hMenu );

	const TString &getName() const noexcept
	{
		return this->m_tsMenuName;
	}
	const size_t &getNameHash() const noexcept { return m_menuNameHash;	}

	const inline HMENU &getMenuHandle( ) const noexcept { return this->m_hMenu; };

	const XPMENUCOLORS *getColors() const noexcept
	{
		return &m_MenuColors;
	}
	void setColor( const MenuColours nColor, const COLORREF clrColor ) noexcept;
	COLORREF getColor( const MenuColours nColor ) const noexcept;
	constexpr void setDefaultColor(const MenuColours nColor) noexcept
	{
		switch (nColor)
		{
		case XPMC_BACKGROUND:
			this->m_MenuColors.m_clrBack = RGB(255, 255, 255);
			break;

		case XPMC_ICONBOX:
			m_MenuColors.m_clrBox = RGB(184, 199, 146);
			//m_MenuColors.m_clrLightBox = XPopupMenuItem::LightenColor(200, m_MenuColors.m_clrBox); // == XPopupMenuItem::LightenColor(200, RGB(184, 199, 146)) == RGB(240, 243, 231)
			m_MenuColors.m_clrLightBox = RGB(240, 243, 231);
			break;

		case XPMC_CHECKBOX:
			this->m_MenuColors.m_clrCheckBox = RGB(255, 128, 0);
			break;

		case XPMC_CHECKBOX_DISABLED:
			this->m_MenuColors.m_clrDisabledCheckBox = RGB(200, 200, 200);
			break;

		case XPMC_SELECTIONBOX_DISABLED:
			this->m_MenuColors.m_clrDisabledSelection = RGB(255, 255, 255);
			break;

		case XPMC_TEXT_DISABLED:
			this->m_MenuColors.m_clrDisabledText = RGB(128, 128, 128);
			break;

		case XPMC_SELECTIONBOX:
			this->m_MenuColors.m_clrSelection = RGB(255, 229, 179);
			break;

		case XPMC_SELECTIONBOX_BORDER:
			this->m_MenuColors.m_clrSelectionBorder = RGB(0, 0, 0);
			break;

		case XPMC_SEPARATOR:
			this->m_MenuColors.m_clrSeparatorLine = RGB(128, 128, 128);
			break;

		case XPMC_TEXT:
			this->m_MenuColors.m_clrText = RGB(0, 0, 0);
			break;

		case XPMC_SELECTEDTEXT:
			this->m_MenuColors.m_clrSelectedText = RGB(0, 0, 0);
			break;

		case XPMC_MAX:
		default:
			break;
		}
	}

	static LRESULT CALLBACK XPopupWinProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	static LRESULT OnMeasureItem( const HWND mHwnd, LPMEASUREITEMSTRUCT lpmis );
	static LRESULT OnDrawItem( const HWND mHwnd, LPDRAWITEMSTRUCT lpdis ); 

	void convertMenu( HMENU hMenu, const BOOL bForce );
	static void cleanMenu( HMENU hMenu ) noexcept;
	void clearAllMenuItems( ) noexcept;

	const HBITMAP &getBackBitmap() const noexcept
	{
		return m_hBitmap;
	}
	void setBackBitmap( HBITMAP hBitmap ) noexcept;

	const inline bool &IsRounded(void) const noexcept { return this->m_bRoundedSel; };
	const inline std::byte &IsAlpha(void) const noexcept { return this->m_uiAlpha; };
	constexpr void SetRounded(const bool rounded) noexcept { this->m_bRoundedSel = rounded; };
	constexpr void SetAlpha(const std::byte alpha) noexcept { this->m_uiAlpha = alpha; };

	// Methods to attach and detach from mIRC menu.
	bool attachToMenuBar(HMENU menubar, const TString &label);
	void detachFromMenuBar(HMENU menubar);

	// Methods to access marked text.
	void setMarkedText(const TString &text)
	{
		this->m_tsMarkedText = text;
	}
	const TString &getMarkedText() const noexcept
	{
		return this->m_tsMarkedText;
	}

	bool getMenuInfo(const UINT iMask, const TString &path, MENUITEMINFO &mii) const;

	VectorOfXPopupMenuItem m_vpMenuItem; //!< Vector of XPopupMenuItem Objects

protected:

	HMENU m_hMenu;						//!< Menu Handle
	HIMAGELIST m_hImageList{ nullptr };	//!< Menu ImageList
	MenuStyle m_MenuStyle{ XPMS_OFFICE2003 };//!< Menu Style
	TString m_tsMenuName;				//!< Menu Name
	TString m_tsMarkedText;				//!< Extra field to store custom information
	UINT m_MenuItemStyles{};			//!< Menu Item Styles
	size_t m_menuNameHash;				//!< Hash of tsMenuName

	HBITMAP m_hBitmap{ nullptr };		//!< Menu Item Background Image in Custom Style

	XPMENUCOLORS m_MenuColors;			//!< Menu Colors

	std::byte m_uiAlpha{ 255 };			//!< Menu is alpha blended. 0 -> 255

	bool m_bRoundedSel{ false };		//!< Menu has rounded selection box.
	bool m_bAttachedToMenuBar{ false }; //!< Is the menu attached to the mIRC window menubar?
	bool m_bReserved[2]{};				//!< Reserved for future use.
};
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENU_H_
