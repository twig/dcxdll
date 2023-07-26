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

constexpr auto XPS_ICON3D = 0x01; //!< Icons have a 3D effect
constexpr auto XPS_DISABLEDSEL = 0x02; //!< Disabled Items have a selectionbox
constexpr auto XPS_ICON3DSHADOW = 0x04; //!< Icons have a 3D effect with undershadow

using VectorOfXPopupMenu = std::vector<XPopupMenu *>; //!< Vector of XPopupMenu Objects

struct XPMENUBARCOLORS final
{
	COLORREF m_clrBack{ RGB(184, 199, 146) };				//!< Menu Item BackGround Color

	COLORREF m_clrBox{ RGB(184, 199, 146) };				//!< Menu Item Box Color
	COLORREF m_clrSelection{ RGB(255, 229, 179) };			//!< Menu Item Box Selection Color
	COLORREF m_clrHot{ RGB(240,243,231) };					//!< Menu Item Box Hot Color
	COLORREF m_clrDisabled{ RGB(255, 255, 255) };			//!< Menu Item Box Disabled Color

	COLORREF m_clrBorder{ RGB(184, 199, 146) };				//!< Menu Item Box Border Color
	COLORREF m_clrHotBorder{ RGB(184, 199, 146) };			//!< Menu Item Box Hot Border Color
	COLORREF m_clrSelectionBorder{ RGB(184, 199, 146) };	//!< Menu Item Box Selection Border Color

	COLORREF m_clrText{ RGB(0, 0, 0) };						//!< Menu Item Text Color
	COLORREF m_clrSelectedText{ RGB(0, 0, 0) };				//!< Menu Item Selected Text Colour
	COLORREF m_clrHotText{ RGB(0, 0, 0) };					//!< Menu Item Selected Text Colour
	COLORREF m_clrDisabledText{ RGB(128, 128, 128) };		//!< Menu Item Disabled Text Color


	XPMENUBARCOLORS() = default;

	bool operator==(const XPMENUBARCOLORS& other) const = default;

	XPMENUBARCOLORS(const COLORREF& m_clrBack, const COLORREF& m_clrBox, const COLORREF& m_clrSelection, const COLORREF& m_clrHot, const COLORREF& m_clrDisabled, const COLORREF& m_clrBorder, const COLORREF& m_clrHotBorder, const COLORREF& m_clrSelectionBorder, const COLORREF& m_clrText, const COLORREF& m_clrSelectedText, const COLORREF& m_clrHotText, const COLORREF& m_clrDisabledText) noexcept
		: m_clrBack(m_clrBack), m_clrBox(m_clrBox), m_clrSelection(m_clrSelection), m_clrHot(m_clrHot), m_clrDisabled(m_clrDisabled), m_clrBorder(m_clrBorder), m_clrHotBorder(m_clrHotBorder), m_clrSelectionBorder(m_clrSelectionBorder), m_clrText(m_clrText), m_clrSelectedText(m_clrSelectedText), m_clrHotText(m_clrHotText), m_clrDisabledText(m_clrDisabledText)
	{
	}
};
using LPXPMENUBARCOLORS = XPMENUBARCOLORS*;
struct XPMENUBARITEM
{
	XPMENUBARCOLORS m_Colours;						// colours for whole menubar (overridden by m_ItemSettings)
	HBITMAP m_hBkg{};								// bitmap to draw in menubar. (can be null, if null only colours are used)

	XPMENUBARITEM() = default;

	XPMENUBARITEM(const XPMENUBARCOLORS& m_Colours, const HBITMAP& m_hBkg) noexcept
		: m_Colours(m_Colours), m_hBkg(m_hBkg)
	{
	}

	bool operator==(const XPMENUBARITEM& other) const = default;
};
struct XPMENUBAR
{
	static inline HTHEME g_menuTheme{};

	bool m_bEnable{ false };						// Custom draw the dialogs menu bar.
	bool m_bDrawBorder{ false };					// draw border sound item?
	bool m_bDrawRoundedBorder{ false };				// draw rounded border?
	XPMENUBARITEM m_Default;						// colours for whole menubar (overridden by m_ItemSettings)
	std::map<int, XPMENUBARITEM> m_ItemSettings;	// item specific colour settings.

	XPMENUBAR() = default;

	bool operator==(const XPMENUBAR& other) const = default;

	XPMENUBAR(bool m_bEnable, bool m_bDrawBorder, bool m_bDrawRoundedBorder, const XPMENUBARITEM& m_Default, const std::map<int, XPMENUBARITEM>& m_ItemSettings)
		: m_bEnable(m_bEnable), m_bDrawBorder(m_bDrawBorder), m_bDrawRoundedBorder(m_bDrawRoundedBorder), m_Default(m_Default), m_ItemSettings(m_ItemSettings)
	{
	}
};

/*!
 * \brief blah
 *
 * blah
 */
#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( push )
#pragma warning( disable : 2292 ) //warning #2292: destructor is declared but copy constructor and assignment operator are not
#endif

class XPopupMenu final
{
public:

	/*!
	* \brief Menu Styles
	*
	* Availbale XPopupMenu Styles
	*/
	enum class MenuStyle : UINT {
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
		XPMS_BUTTON_REV,
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

	enum class MenuColours : UINT {
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
	XPopupMenu(const TString & tsMenuName, MenuStyle mStyle);
	~XPopupMenu();

	bool operator==(const XPopupMenu& other) const = default;

	void parseXPopCommand(const TString & input);
	void parseXPopIdentifier(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const;
	static XPopupMenu::MenuStyle parseStyle(const TString &style) noexcept;

	static HMENU parsePath(const TString & path, const HMENU hParent, const UINT depth = 1);

	HIMAGELIST &getImageList() noexcept;
	void destroyImageList() noexcept;

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

	void deleteMenuItemData(const XPopupMenuItem *const p_Item, LPMENUITEMINFO mii = nullptr) noexcept;
	void deleteAllItemData(HMENU hMenu);

	const TString &getName() const noexcept
	{
		return this->m_tsMenuName;
	}
	const size_t &getNameHash() const noexcept { return m_menuNameHash; }

	const inline HMENU &getMenuHandle() const noexcept { return this->m_hMenu; };

	const XPMENUCOLORS *getColors() const noexcept
	{
		return &m_MenuColors;
	}
	void setColor(const MenuColours nColor, const COLORREF clrColor) noexcept;
	COLORREF getColor(const MenuColours nColor) const noexcept;
	GSL_SUPPRESS(type.4) constexpr void setDefaultColor(const MenuColours nColor) noexcept
	{
		switch (nColor)
		{
		case MenuColours::XPMC_BACKGROUND:
			this->m_MenuColors.m_clrBack = RGB(255, 255, 255);
			break;

		case MenuColours::XPMC_ICONBOX:
			m_MenuColors.m_clrBox = RGB(184, 199, 146);
			//m_MenuColors.m_clrLightBox = XPopupMenuItem::LightenColor(200, m_MenuColors.m_clrBox); // == XPopupMenuItem::LightenColor(200, RGB(184, 199, 146)) == RGB(240, 243, 231)
			m_MenuColors.m_clrLightBox = RGB(240, 243, 231);
			break;

		case MenuColours::XPMC_CHECKBOX:
			this->m_MenuColors.m_clrCheckBox = RGB(255, 128, 0);
			break;

		case MenuColours::XPMC_CHECKBOX_DISABLED:
			this->m_MenuColors.m_clrDisabledCheckBox = RGB(200, 200, 200);
			break;

		case MenuColours::XPMC_SELECTIONBOX_DISABLED:
			this->m_MenuColors.m_clrDisabledSelection = RGB(255, 255, 255);
			break;

		case MenuColours::XPMC_TEXT_DISABLED:
			this->m_MenuColors.m_clrDisabledText = RGB(128, 128, 128);
			break;

		case MenuColours::XPMC_SELECTIONBOX:
			this->m_MenuColors.m_clrSelection = RGB(255, 229, 179);
			break;

		case MenuColours::XPMC_SELECTIONBOX_BORDER:
			this->m_MenuColors.m_clrSelectionBorder = RGB(0, 0, 0);
			break;

		case MenuColours::XPMC_SEPARATOR:
			this->m_MenuColors.m_clrSeparatorLine = RGB(128, 128, 128);
			break;

		case MenuColours::XPMC_TEXT:
			this->m_MenuColors.m_clrText = RGB(0, 0, 0);
			break;

		case MenuColours::XPMC_SELECTEDTEXT:
			this->m_MenuColors.m_clrSelectedText = RGB(0, 0, 0);
			break;

		case MenuColours::XPMC_MAX:
		default:
			break;
		}
	}

	static LRESULT CALLBACK XPopupWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT OnMeasureItem(const HWND mHwnd, LPMEASUREITEMSTRUCT lpmis);
	static LRESULT OnDrawItem(const HWND mHwnd, LPDRAWITEMSTRUCT lpdis);

	void convertMenu(HMENU hMenu, const BOOL bForce);
	static void cleanMenu(HMENU hMenu) noexcept;
	void clearAllMenuItems() noexcept;

	const HBITMAP &getBackBitmap() const noexcept
	{
		return m_hBitmap;
	}
	void setBackBitmap(HBITMAP hBitmap) noexcept;

	const inline bool &IsRoundedSelector(void) const noexcept { return this->m_bRoundedSel; };
	const inline bool& IsRoundedWindow(void) const noexcept { return this->m_bRoundedWindow; };
	const inline std::byte &IsAlpha(void) const noexcept { return this->m_uiAlpha; };
	constexpr void SetRoundedSelector(const bool rounded) noexcept { this->m_bRoundedSel = rounded; };
	constexpr void SetAlpha(const std::byte alpha) noexcept { this->m_uiAlpha = alpha; };
	constexpr void SetRoundedWindow(const bool rounded) noexcept { this->m_bRoundedWindow = rounded; };

	const inline bool& IsHMENUDestrucible(void) const noexcept { return this->m_bDestroyHMENU; };
	//constexpr void SetHMENUDestructible(const bool boom) noexcept { this->m_bDestroyHMENU = boom; };

	// Methods to attach and detach from mIRC menu.
	bool attachToMenuBar(HMENU menubar, const TString &label);
	void detachFromMenuBar(HMENU menubar) noexcept;

	// Methods to access marked text.
	void setMarkedText(const TString &text)
	{
		this->m_tsMarkedText = text;
	}
	const TString &getMarkedText() const noexcept
	{
		return this->m_tsMarkedText;
	}
	const bool& IsToolTipsEnabled() const noexcept { return m_bEnableTooltips; }
	void setTooltipsState(bool a) noexcept { m_bEnableTooltips = a; }

	bool getMenuInfo(const UINT iMask, const TString &path, MENUITEMINFO &mii) const;

	const bool isItemValid(const XPopupMenuItem* const pItem) const noexcept;

	VectorOfXPopupMenuItem m_vpMenuItem; //!< Vector of XPopupMenuItem Objects

protected:

	HMENU m_hMenu{ nullptr };			//!< Menu Handle
	HIMAGELIST m_hImageList{ nullptr };	//!< Menu ImageList
	MenuStyle m_MenuStyle{ MenuStyle::XPMS_OFFICE2003 };//!< Menu Style
	TString m_tsMenuName;				//!< Menu Name
	TString m_tsMarkedText;				//!< Extra field to store custom information
	UINT m_MenuItemStyles{};			//!< Menu Item Styles
	size_t m_menuNameHash{};			//!< Hash of tsMenuName

	HBITMAP m_hBitmap{ nullptr };		//!< Menu Item Background Image in Custom Style

	XPMENUCOLORS m_MenuColors;			//!< Menu Colors

	std::byte m_uiAlpha{ 255 };			//!< Menu is alpha blended. 0 -> 255

	bool m_bRoundedSel{ false };		//!< Menu has rounded selection box.
	bool m_bAttachedToMenuBar{ false }; //!< Is the menu attached to the mIRC window menubar?
	bool m_bEnableTooltips{ false };	//!< are tooltips enabled for this menu?
	bool m_bRoundedWindow{ false };		//!< Menu has rounded window.
	const bool m_bDestroyHMENU{ true };		//!< Should the destructor call DestroyMenu()
};

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENU_H_
