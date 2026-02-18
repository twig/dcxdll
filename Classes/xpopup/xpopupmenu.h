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
#include "UAHMenuBar.h"

// uncomment this to enable using unique_ptr
//#define XPOPUP_USE_UNIQUEPTR 1

constexpr auto XPS_ICON3D = 0x01; //!< Icons have a 3D effect
constexpr auto XPS_DISABLEDSEL = 0x02; //!< Disabled Items have a selectionbox
constexpr auto XPS_ICON3DSHADOW = 0x04; //!< Icons have a 3D effect with undershadow
constexpr auto XPS_DOUBLESEP = 0x08; //!< Seperator items have a double line.
constexpr auto XPS_VERTICALSEP = 0x10; //!< Seperator items have a double line.

enum class MainMenuStyle : UINT
{
	XPMS_None,
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
	XPMS_CUSTOMBIG,
	XPMS_THEMED,
	XPMS_PROGRESS,
	XPMS_TRACK,

	XPMS_BUTTON_THEMED,
	XPMS_BUTTON_REV_THEMED,
	XPMS_PROGRESS_THEMED,
	XPMS_TRACK_THEMED
};

struct XPMENUBARDRAWCOLOURS
{
	COLORREF m_clrFill{};
	COLORREF m_clrText{};
	COLORREF m_clrBorder{};
	COLORREF m_clrBack{};
};

struct XPMENUBARITEMSTATEDATA
{
	XPMENUBARDRAWCOLOURS m_Clrs;
	dcxImage		m_Img;
	BARITEMSTATES	m_ThemeState{};
};

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


	XPMENUBARCOLORS() noexcept = default;

	bool operator==(const XPMENUBARCOLORS& other) const = default;

	XPMENUBARCOLORS(const COLORREF& m_clrBack, const COLORREF& m_clrBox, const COLORREF& m_clrSelection, const COLORREF& m_clrHot, const COLORREF& m_clrDisabled, const COLORREF& m_clrBorder, const COLORREF& m_clrHotBorder, const COLORREF& m_clrSelectionBorder, const COLORREF& m_clrText, const COLORREF& m_clrSelectedText, const COLORREF& m_clrHotText, const COLORREF& m_clrDisabledText) noexcept
		: m_clrBack(m_clrBack), m_clrBox(m_clrBox), m_clrSelection(m_clrSelection), m_clrHot(m_clrHot), m_clrDisabled(m_clrDisabled), m_clrBorder(m_clrBorder), m_clrHotBorder(m_clrHotBorder), m_clrSelectionBorder(m_clrSelectionBorder), m_clrText(m_clrText), m_clrSelectedText(m_clrSelectedText), m_clrHotText(m_clrHotText), m_clrDisabledText(m_clrDisabledText)
	{
	}
	void toXml(TiXmlElement* xml) const
	{
		if (!xml)
			return;

		setColourAttribute(xml, "back", m_clrBack);

		setColourAttribute(xml, "box", m_clrBox);
		setColourAttribute(xml, "selection", m_clrSelection);
		setColourAttribute(xml, "hot", m_clrHot);
		setColourAttribute(xml, "disabled", m_clrDisabled);

		setColourAttribute(xml, "border", m_clrBorder);
		setColourAttribute(xml, "hotborder", m_clrHotBorder);
		setColourAttribute(xml, "selectionborder", m_clrSelectionBorder);

		setColourAttribute(xml, "text", m_clrText);
		setColourAttribute(xml, "selectedtext", m_clrSelectedText);
		setColourAttribute(xml, "hottext", m_clrHotText);
		setColourAttribute(xml, "disabledtext", m_clrDisabledText);
	}
	TiXmlElement* toXml() const
	{
		auto xml = std::make_unique<TiXmlElement>("colours");
		toXml(xml.get());
		return xml.release();
	}
	void fromXml(const TiXmlElement* xml) noexcept
	{
		if (const auto tmp = queryColourAttribute(xml, "back"); tmp != CLR_INVALID)
			m_clrBack = tmp;
		if (const auto tmp = queryColourAttribute(xml, "border"); tmp != CLR_INVALID)
			m_clrBorder = tmp;
		if (const auto tmp = queryColourAttribute(xml, "box"); tmp != CLR_INVALID)
			m_clrBox = tmp;
		if (const auto tmp = queryColourAttribute(xml, "disabled"); tmp != CLR_INVALID)
			m_clrDisabled = tmp;
		if (const auto tmp = queryColourAttribute(xml, "disabledtext"); tmp != CLR_INVALID)
			m_clrDisabledText = tmp;
		if (const auto tmp = queryColourAttribute(xml, "hot"); tmp != CLR_INVALID)
			m_clrHot = tmp;
		if (const auto tmp = queryColourAttribute(xml, "hotborder"); tmp != CLR_INVALID)
			m_clrHotBorder = tmp;
		if (const auto tmp = queryColourAttribute(xml, "hottext"); tmp != CLR_INVALID)
			m_clrHotText = tmp;
		if (const auto tmp = queryColourAttribute(xml, "selectedtext"); tmp != CLR_INVALID)
			m_clrSelectedText = tmp;
		if (const auto tmp = queryColourAttribute(xml, "selection"); tmp != CLR_INVALID)
			m_clrSelection = tmp;
		if (const auto tmp = queryColourAttribute(xml, "selectionborder"); tmp != CLR_INVALID)
			m_clrSelectionBorder = tmp;
		if (const auto tmp = queryColourAttribute(xml, "text"); tmp != CLR_INVALID)
			m_clrText = tmp;
	}
};
using LPXPMENUBARCOLORS = XPMENUBARCOLORS*;
struct XPMENUBARITEM
{
	XPMENUBARCOLORS m_Colours;		// colours for menubar item (overridden by m_ItemSettings)
	dcxImage		m_hBkgNormal{};		// bitmap to draw in menubar item. (can be null, if null only colours are used)
	dcxImage		m_hBkgSelected{};		// bitmap to draw in menubar item. (can be null, if null only colours are used)
	dcxImage		m_hBkgHot{};		// bitmap to draw in menubar item. (can be null, if null only colours are used)

	XPMENUBARITEM() noexcept = default;
	//~XPMENUBARITEM()
	//{
	//	m_hBkg.reset();
	//}

	bool operator==(const XPMENUBARITEM& other) const = default;

	XPMENUBARITEM(const XPMENUBARCOLORS& m_Colours, const dcxImage& m_hBkgNormal, const dcxImage& m_hBkgSelected, const dcxImage& m_hBkgHot)
		: m_Colours(m_Colours), m_hBkgNormal(m_hBkgNormal), m_hBkgSelected(m_hBkgSelected), m_hBkgHot(m_hBkgHot)
	{
	}
	void toXml(TiXmlElement* xml) const
	{
		if (!xml)
			return;

		xml->LinkEndChild(m_Colours.toXml());
		if (!m_hBkgNormal.m_tsFilename.empty() && m_hBkgNormal.m_tsFilename != L"none")
		{
			//xml->LinkEndChild(m_hBkgNormal.toXml());

			TiXmlElement xImg("normalimage");
			m_hBkgNormal.toXml(&xImg);
			xml->InsertEndChild(xImg);
	}
		if (!m_hBkgSelected.m_tsFilename.empty() && m_hBkgSelected.m_tsFilename != L"none")
		{
			TiXmlElement xImg("selimage");
			m_hBkgSelected.toXml(&xImg);
			xml->InsertEndChild(xImg);
		}
		if (!m_hBkgHot.m_tsFilename.empty() && m_hBkgHot.m_tsFilename != L"none")
		{
			TiXmlElement xImg("hotimage");
			m_hBkgHot.toXml(&xImg);
			xml->InsertEndChild(xImg);
		}
	}
	TiXmlElement* toXml() const
	{
		auto xml = std::make_unique<TiXmlElement>("item");
		toXml(xml.get());
		return xml.release();
	}
	void fromXml(const TiXmlElement* xml)
	{
		if (!xml)
			return;

		if (auto xColours = xml->FirstChildElement("colours"); xColours)
			m_Colours.fromXml(xColours);

		// load image.
		if (auto xImage = xml->FirstChildElement("normalimage"); xImage)
			m_hBkgNormal.fromXml(xImage);
		if (auto xImage = xml->FirstChildElement("selimage"); xImage)
			m_hBkgSelected.fromXml(xImage);
		if (auto xImage = xml->FirstChildElement("hotimage"); xImage)
			m_hBkgHot.fromXml(xImage);
	}

	XPMENUBARDRAWCOLOURS GetDrawColours(UINT itemState) const noexcept;
	static BARITEMSTATES GetThemeState(UAHDRAWMENUITEM* pUDMI) noexcept;
	const dcxImage& GetStateImage(UINT itemState) const noexcept;
	XPMENUBARITEMSTATEDATA getStateData(UAHDRAWMENUITEM* pUDMI) const noexcept;
};

struct XPMENUBAR
{
	HTHEME m_menuTheme{};

	HMENU m_hMenuBackup{};

	bool m_bEnable{ false };						// Custom draw the dialogs menu bar.
	bool m_bDrawBorder{ false };					// draw border sound item?
	bool m_bDrawRoundedBorder{ false };				// draw rounded border?
	bool m_bDrawShadowText{ false };				// draw text with shadows?
	bool m_bDrawSysButtons{ false };
	MainMenuStyle m_Style{ MainMenuStyle::XPMS_None };

	XPMENUBARITEM m_Default;						// colours for whole menubar (overridden by m_ItemSettings)
	std::map<int, XPMENUBARITEM> m_ItemSettings;	// item specific colour settings.

	RECT m_rcClose{};
	RECT m_rcMin{};
	RECT m_rcRestore{};

	dcxImage		m_BkgImage{};		// bitmap to draw in menubar. (can be null, if null only colours are used)

	XPMENUBAR() noexcept = default;
	XPMENUBAR(const XPMENUBAR&) = default;
	XPMENUBAR(XPMENUBAR&&) = default;

	XPMENUBAR& operator=(const XPMENUBAR&) = default;
	XPMENUBAR& operator=(XPMENUBAR&&) = default;

	bool operator==(const XPMENUBAR& other) const = default;

	XPMENUBAR(const HTHEME& m_menuTheme, const HMENU& m_hMenuBackup, bool m_bEnable, bool m_bDrawBorder, bool m_bDrawRoundedBorder, bool m_bDrawShadowText, bool m_bDrawSysButtons, const MainMenuStyle& m_Style, const XPMENUBARITEM& m_Default, const std::map<int, XPMENUBARITEM>& m_ItemSettings, const RECT& m_rcClose, const RECT& m_rcMin, const RECT& m_rcRestore, const dcxImage& m_BkgImage)
		: m_menuTheme(m_menuTheme), m_hMenuBackup(m_hMenuBackup), m_bEnable(m_bEnable), m_bDrawBorder(m_bDrawBorder), m_bDrawRoundedBorder(m_bDrawRoundedBorder), m_bDrawShadowText(m_bDrawShadowText), m_bDrawSysButtons(m_bDrawSysButtons), m_Style(m_Style), m_Default(m_Default), m_ItemSettings(m_ItemSettings), m_rcClose(m_rcClose), m_rcMin(m_rcMin), m_rcRestore(m_rcRestore), m_BkgImage(m_BkgImage)
	{
	}

	~XPMENUBAR()
	{
		m_BkgImage.reset();
	}

	static MainMenuStyle StyleFromString(const TString& tsStyle) noexcept
	{
		auto style = MainMenuStyle::XPMS_OFFICE2003;

		switch (std::hash<TString>{}(tsStyle))
		{
		case TEXT("office2003rev"_hash):
			style = MainMenuStyle::XPMS_OFFICE2003_REV;
			break;
		case TEXT("officexp"_hash):
			style = MainMenuStyle::XPMS_OFFICEXP;
			break;
		case TEXT("icy"_hash):
			style = MainMenuStyle::XPMS_ICY;
			break;
		case TEXT("icyrev"_hash):
			style = MainMenuStyle::XPMS_ICY_REV;
			break;
		case TEXT("grade"_hash):
			style = MainMenuStyle::XPMS_GRADE;
			break;
		case TEXT("graderev"_hash):
			style = MainMenuStyle::XPMS_GRADE_REV;
			break;
		case TEXT("vertical"_hash):
			style = MainMenuStyle::XPMS_VERTICAL;
			break;
		case TEXT("verticalrev"_hash):
			style = MainMenuStyle::XPMS_VERTICAL_REV;
			break;
		case TEXT("normal"_hash):
			style = MainMenuStyle::XPMS_NORMAL;
			break;
		case TEXT("custom"_hash):
			style = MainMenuStyle::XPMS_CUSTOM;
			break;
		case TEXT("button"_hash):
			style = MainMenuStyle::XPMS_BUTTON;
			break;
		case TEXT("buttonrev"_hash):
			style = MainMenuStyle::XPMS_BUTTON_REV;
			break;
		case TEXT("button_themed"_hash):
			style = MainMenuStyle::XPMS_BUTTON_THEMED;
			break;
		case TEXT("buttonrev_themed"_hash):
			style = MainMenuStyle::XPMS_BUTTON_REV_THEMED;
			break;
		case TEXT("custombig"_hash):
			style = MainMenuStyle::XPMS_CUSTOMBIG;
			break;
		case TEXT("progress"_hash):
			style = MainMenuStyle::XPMS_PROGRESS;
			break;
		case TEXT("track"_hash):
			style = MainMenuStyle::XPMS_TRACK;
			break;
		case TEXT("progress_themed"_hash):
			style = MainMenuStyle::XPMS_PROGRESS_THEMED;
			break;
		case TEXT("track_themed"_hash):
			style = MainMenuStyle::XPMS_TRACK_THEMED;
			break;
		case TEXT("themed"_hash):
			style = MainMenuStyle::XPMS_THEMED;
			break;
		default:
			break;
		}
		return style;
	}

	static TString StringFromStyle(MainMenuStyle ms)
	{
		TString tsRes(L"normal");

		switch (ms)
		{
		case MainMenuStyle::XPMS_OFFICE2003:
			tsRes = L"office2003";
			break;
		case MainMenuStyle::XPMS_OFFICE2003_REV:
			tsRes = L"office2003rev";
			break;
		case MainMenuStyle::XPMS_OFFICEXP:
			tsRes = L"officexp";
			break;
		case MainMenuStyle::XPMS_ICY:
			tsRes = L"icy";
			break;
		case MainMenuStyle::XPMS_ICY_REV:
			tsRes = L"icyrev";
			break;
		case MainMenuStyle::XPMS_GRADE:
			tsRes = L"grade";
			break;
		case MainMenuStyle::XPMS_GRADE_REV:
			tsRes = L"graderev";
			break;
		case MainMenuStyle::XPMS_CUSTOM:
			tsRes = L"custom";
			break;
		case MainMenuStyle::XPMS_VERTICAL:
			tsRes = L"vertical";
			break;
		case MainMenuStyle::XPMS_VERTICAL_REV:
			tsRes = L"verticalrev";
			break;
		case MainMenuStyle::XPMS_BUTTON:
			tsRes = L"button";
			break;
		case MainMenuStyle::XPMS_BUTTON_REV:
			tsRes = L"buttonrev";
			break;
		case MainMenuStyle::XPMS_CUSTOMBIG:
			tsRes = L"custombig";
			break;
		case MainMenuStyle::XPMS_THEMED:
			tsRes = L"themed";
			break;
		case MainMenuStyle::XPMS_BUTTON_THEMED:
			tsRes = L"button_themed";
			break;
		case MainMenuStyle::XPMS_BUTTON_REV_THEMED:
			tsRes = L"buttonrev_themed";
			break;
		case MainMenuStyle::XPMS_None:
		case MainMenuStyle::XPMS_NORMAL:
		default:
			tsRes = L"normal";
			break;
		}
		return tsRes;
	}

	void toXml(TiXmlElement* xml) const
	{
		if (!xml)
			return;

		if (m_bEnable)
			xml->SetAttribute("enable", "1");
		if (m_bDrawBorder)
			xml->SetAttribute("drawborder", "1");
		if (m_bDrawRoundedBorder)
			xml->SetAttribute("roundedborder", "1");
		if (m_bDrawShadowText)
			xml->SetAttribute("shadowtext", "1");

		xml->SetAttribute("style", StringFromStyle(m_Style).c_str());

		m_Default.toXml(xml);	// defaults NOT in an item.

		// menubar item specific settings...
		if (!m_ItemSettings.empty())
		{
			for (const auto& a : m_ItemSettings)
			{
				TiXmlElement xItem("item");
				xItem.SetAttribute("id", a.first);

				a.second.toXml(&xItem);

				xml->InsertEndChild(xItem);
			}
		}
	}
	TiXmlElement* toXml() const
	{
		auto xml = std::make_unique<TiXmlElement>("menubar");
		toXml(xml.get());
		return xml.release();
	}
	void fromXml(const TiXmlElement* xml)
	{
		if (!xml)
			return;

		m_bEnable = (queryIntAttribute(xml, "enable") > 0);
		m_bDrawBorder = (queryIntAttribute(xml, "drawborder") > 0);
		m_bDrawRoundedBorder = (queryIntAttribute(xml, "roundedborder") > 0);
		m_bDrawShadowText = (queryIntAttribute(xml, "shadowtext") > 0);

		if (const TString tsStyle(queryAttribute(xml, "style", "normal")); !tsStyle.empty())
			m_Style = StyleFromString(tsStyle);

		m_Default.fromXml(xml);

		// load item specific settings
		for (auto xItem = xml->FirstChildElement("item"); xItem; xItem = xItem->NextSiblingElement("item"))
		{
			XPMENUBARITEM item;
			int id{};

			if (const auto tmp = queryIntAttribute(xItem, "id"); tmp)	// NB: ID is always a number here.
				id = tmp;

			item.fromXml(xItem);

			m_ItemSettings[id] = item;
		}
	}

	void dcxDrawMenuIcon(HMENU hMenu, int iPos, HDC hdc, LPRECT prc) noexcept;
	void dcxDrawSystemButton(HTHEME hTheme, int iStateID, HDC hdc, LPRECT prc, HBITMAP hBm) noexcept;

	XPMENUBARITEM UAHGetMenuBarItemData(int iPosition) noexcept;
	//XPMENUBARDRAWCOLOURS UAHGetMenuBarColours(const XPMENUBARITEM & mbi, UINT itemState) noexcept;
	//XPMENUBARDRAWCOLOURS UAHGetMenuBarColours(int iPosition, UINT itemState) noexcept;
	//XPMENUBARDRAWCOLOURS UAHGetMenuBarColours(UAHDRAWMENUITEM* pUDMI) noexcept;
	//BARITEMSTATES UAHGetThemeState(UAHDRAWMENUITEM* pUDMI) noexcept;

	void UAHDrawMenuBar(HWND mHwnd, UAHMENU* pUDM) noexcept;
	void UAHDrawMenuBarItem(HWND mHwnd, UAHDRAWMENUITEM* pUDMI) noexcept;
	void UAHDrawMenuNCBottomLine(HWND hWnd) const noexcept;
	void UAHDrawUpdateSysButtons(HWND hWnd, POINT pos) noexcept;

	// [+FLAGS] [ARGS]
	void Setup(HWND mHwnd, const XSwitchFlags &xflags, TString tsArgs);
};

using VectorOfGroupIDs = VectorOfInts;

struct DcxMenuItemGroup
{
	UINT m_ID{};					// ID of this group.
	VectorOfGroupIDs m_GroupIDs;	// these IDs are the command ids of items in the group. IDs can be in more than one group.

	operator bool() const noexcept {
		return (m_ID > 0 && !m_GroupIDs.empty());
	}
};

using VectorOfMenuItemGroups = std::vector<DcxMenuItemGroup>;

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
	* Available XPopupMenu Styles
	*/
	using MenuStyle = MainMenuStyle;

	//enum class MenuStyle : UINT
	//{
	//	XPMS_None,
	//	XPMS_OFFICE2003,
	//	XPMS_OFFICE2003_REV,
	//	XPMS_OFFICEXP,
	//	XPMS_ICY,
	//	XPMS_ICY_REV,
	//	XPMS_GRADE,
	//	XPMS_GRADE_REV,
	//	XPMS_NORMAL,
	//	XPMS_CUSTOM,
	//	XPMS_VERTICAL,
	//	XPMS_VERTICAL_REV,
	//	XPMS_BUTTON,
	//	XPMS_BUTTON_REV,
	//	XPMS_CUSTOMBIG
	//};

	// Menu Colors
	/*
	1  	Menu background color
	2 	Icon box color
	3 	Checkbox background color
	4 	Disabled checkbox background color
	5 	Disabled selection box color
	6 	Disabled text color
	7 	Selection box color
	8 	Selection box border color
	9 	Separator line color
	10 	Text color
	11 	Selected text color
	12	CheckBox Tick color
	13	CheckBox Frame color
	14	Disabled CheckBox Tick color
	15	Disabled CheckBox Frame color
	16	Hot CheckBox background color
	17	Hot CheckBox Tick color
	18	Hot CheckBox Frame color
	19	Menu Border color
	20	Vertical separator color.
	*/

	enum class MenuColours : UINT
	{
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
		XPMC_CHECKBOX_TICK,
		XPMC_CHECKBOX_FRAME,
		XPMC_CHECKBOX_TICK_DISABLED,
		XPMC_CHECKBOX_FRAME_DISABLED,
		XPMC_CHECKBOX_HOT,
		XPMC_CHECKBOX_TICK_HOT,
		XPMC_CHECKBOX_FRAME_HOT,
		XPMC_BORDER,
		XPMC_VSEPARATOR,
		XPMC_MAX
	};

	XPopupMenu() = delete;
	XPopupMenu(const XPopupMenu&) = delete;
	XPopupMenu& operator = (const XPopupMenu&) = delete;
	XPopupMenu(XPopupMenu&&) = delete;
	XPopupMenu& operator =(XPopupMenu&&) = delete;

	XPopupMenu(const TString& tsName, HMENU hMenu);
	XPopupMenu(const TString& tsMenuName, MenuStyle mStyle);
	XPopupMenu(const TString& tsMenuName, MenuStyle mStyle, const TString& tsCallback);
	~XPopupMenu();

	bool operator==(const XPopupMenu& other) const = default;

	/// <summary>
	/// Execute menu specific commands.
	/// </summary>
	/// <param name="input"></param>
	void parseXPopCommand(const TString& input);

	/// <summary>
	/// Get information about this menu.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="szReturnValue"></param>
	void parseXPopIdentifier(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const;

	/// <summary>
	/// Get a menu style from a string.
	/// </summary>
	/// <param name="style"></param>
	/// <returns></returns>
	static XPopupMenu::MenuStyle parseStyle(const TString& style) noexcept;

	/// <summary>
	/// Get a menu from a path.
	/// </summary>
	/// <param name="path"></param>
	/// <param name="hParent"></param>
	/// <param name="depth"></param>
	/// <returns></returns>
	static HMENU parsePath(const TString& path, const HMENU hParent, const UINT depth = 1);

	/// <summary>
	/// Get the menus image list.
	/// </summary>
	/// <returns></returns>
	HIMAGELIST& getImageList() noexcept;

	/// <summary>
	/// Destroy the menus image list.
	/// </summary>
	void destroyImageList() noexcept;

	/// <summary>
	/// Get the menus current style.
	/// </summary>
	/// <returns></returns>
	const MenuStyle& getStyle() const noexcept { return this->m_MenuStyle; }

	TString getStyleString() const;

	/// <summary>
	/// Set the menus current style.
	/// </summary>
	/// <param name="style"></param>
	constexpr void setStyle(const MenuStyle style) noexcept { this->m_MenuStyle = style; }

	/// <summary>
	/// Get additional item styles.
	/// </summary>
	/// <returns></returns>
	const UINT& getItemStyle() const noexcept { return this->m_MenuItemStyles; }

	/// <summary>
	/// Set additional item styles.
	/// </summary>
	/// <param name="iExStyles"></param>
	constexpr void setItemStyle(const UINT iExStyles) noexcept { this->m_MenuItemStyles = iExStyles; }

	/// <summary>
	/// Parse a flags string into additional item styles.
	/// </summary>
	/// <param name="tsFlags"></param>
	void setItemStyleString(const TString& tsFlags);

	/// <summary>
	/// Get additional item styles as a string.
	/// </summary>
	/// <returns></returns>
	TString getItemStyleString() const;

	/// <summary>
	/// Delete an items custom data.
	/// </summary>
	/// <param name="p_Item"></param>
	/// <param name="mii"></param>
	void deleteMenuItemData(const XPopupMenuItem* const p_Item, LPMENUITEMINFO mii = nullptr) noexcept;

	/// <summary>
	/// Delete the custom data from all a menus items.
	/// </summary>
	/// <param name="hMenu"></param>
	void deleteAllItemData(HMENU hMenu) noexcept;

	/// <summary>
	/// Get this menus name.
	/// </summary>
	/// <returns></returns>
	const TString& getName() const noexcept { return this->m_tsMenuName; }

	/// <summary>
	/// Get the hash of this menus name.
	/// </summary>
	/// <returns></returns>
	const size_t& getNameHash() const noexcept { return m_menuNameHash; }

	/// <summary>
	/// Get this menus HMENU handle.
	/// </summary>
	/// <returns></returns>
	const inline HMENU& getMenuHandle() const noexcept { return this->m_hMenu; };

	/// <summary>
	/// Get all colour data.
	/// </summary>
	/// <returns>XPMENUCOLORS</returns>
	const XPMENUCOLORS& getColors() const noexcept { return m_MenuColors; }

	TString getColorsString() const;

	/// <summary>
	/// Set specified colours value.
	/// </summary>
	/// <param name="nColor"></param>
	/// <param name="clrColor"></param>
	void setColor(const MenuColours nColor, const COLORREF clrColor) noexcept;

	/// <summary>
	/// Get specified colours current setting.
	/// </summary>
	/// <param name="nColor"></param>
	/// <returns></returns>
	COLORREF getColor(const MenuColours nColor) const noexcept;

	TString getColorString(MenuColours nColor) const;

	/// <summary>
	/// Set specified colour to default setting.
	/// </summary>
	/// <param name="nColor"></param>
	GSL_SUPPRESS(type.4) void setDefaultColor(_In_ const MenuColours nColor) noexcept;

	static LRESULT CALLBACK XPopupWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT OnMeasureItem(const HWND mHwnd, LPMEASUREITEMSTRUCT lpmis);
	static LRESULT OnDrawItem(const HWND mHwnd, LPDRAWITEMSTRUCT lpdis);

	/// <summary>
	/// Convert an mIRC menu to an XPopupMenu.
	/// </summary>
	/// <param name="hMenu"></param>
	/// <param name="bForce"></param>
	void convertMenu(HMENU hMenu, const BOOL bForce);

	/// <summary>
	/// Removes custom data & sets menu as non-ownerdraw.
	/// </summary>
	/// <param name="hMenu"></param>
	static void cleanMenu(HMENU hMenu) noexcept;

	/// <summary>
	/// Deletes all menu items.
	/// </summary>
	void clearAllMenuItems() noexcept;

	/// <summary>
	/// Get the menus background image.
	/// </summary>
	/// <returns></returns>
	const HBITMAP& getBackBitmap() const noexcept { return m_hBitmap.m_hBitmap; }

	/// <summary>
	/// Get the filename of the menus current background image.
	/// </summary>
	/// <returns></returns>
	const TString& getBackBitmapFilename() const noexcept { return m_hBitmap.m_tsFilename; }

	/// <summary>
	/// Set the menus background image.
	/// </summary>
	/// <param name="hBitmap"></param>
	/// <param name="tsFilename"></param>
	void setBackBitmap(HBITMAP hBitmap, const TString& tsFilename);

	/// <summary>
	/// Check if menu is using a rounded selector.
	/// </summary>
	/// <param name=""></param>
	/// <returns>true/false</returns>
	const inline bool& IsRoundedSelector(void) const noexcept { return this->m_bRoundedSel; };

	/// <summary>
	/// Check if menu is using a rounded window.
	/// </summary>
	/// <param name=""></param>
	/// <returns>true/false</returns>
	const inline bool& IsRoundedWindow(void) const noexcept { return this->m_bRoundedWindow; };

	/// <summary>
	/// Check if menu alpha blended when inactive.
	/// </summary>
	/// <param name=""></param>
	/// <returns>0 - 255 alpha value</returns>
	const inline std::byte& IsAlphaInactive(void) const noexcept { return this->m_uiAlphaInactive; };

	/// <summary>
	/// Check if menu alpha blended by default.
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const inline std::byte& IsAlphaDefault(void) const noexcept { return this->m_uiAlphaDefault; };

	/// <summary>
	/// Set the menu to use a rounded selector.
	/// </summary>
	/// <param name="rounded"></param>
	constexpr void SetRoundedSelector(const bool rounded) noexcept { this->m_bRoundedSel = rounded; };

	/// <summary>
	/// Set the menus alpha value.
	/// </summary>
	/// <param name="alpha"></param>
	constexpr void SetAlphaInactive(const std::byte alpha) noexcept { this->m_uiAlphaInactive = alpha; };

	/// <summary>
	/// Set the menus alpha value.
	/// </summary>
	/// <param name="alpha"></param>
	constexpr void SetAlphaDefault(const std::byte alpha) noexcept { this->m_uiAlphaDefault = alpha; };

	/// <summary>
	/// Set the menu to use a rounded window.
	/// </summary>
	/// <param name="rounded"></param>
	constexpr void SetRoundedWindow(const bool rounded) noexcept { this->m_bRoundedWindow = rounded; };

	/// <summary>
	/// Check if menu is destrucible.
	/// </summary>
	/// <param name=""></param>
	/// <returns>true/false</returns>
	const inline bool& IsHMENUDestrucible(void) const noexcept { return this->m_bDestroyHMENU; };
	//constexpr void SetHMENUDestructible(const bool boom) noexcept { this->m_bDestroyHMENU = boom; };

	/// <summary>
	/// Attach menu to mIRC's menubar.
	/// </summary>
	/// <param name="menubar"></param>
	/// <param name="label"></param>
	/// <returns>true/false</returns>
	bool attachToMenuBar(HMENU menubar, const TString& label);

	/// <summary>
	/// Detach menu from mIRC's menubar.
	/// </summary>
	/// <param name="menubar"></param>
	void detachFromMenuBar(HMENU menubar) noexcept;

	/// <summary>
	/// Set the menus marked text.
	/// </summary>
	/// <param name="text"></param>
	void setMarkedText(const TString& text) { this->m_tsMarkedText = text; }

	/// <summary>
	/// Get the menus marked text.
	/// </summary>
	/// <returns></returns>
	const TString& getMarkedText() const noexcept { return this->m_tsMarkedText; }

	/// <summary>
	/// Set the callback alias.
	/// </summary>
	/// <param name="tsCallback"></param>
	void setCallback(const TString& tsCallback) { m_tsCallback = tsCallback; }

	/// <summary>
	/// Get the callback alias.
	/// </summary>
	/// <returns>The current callback alias if any.</returns>
	const TString& getCallback() const noexcept { return this->m_tsCallback; }

	/// <summary>
	/// Check if tooltips are enabled.
	/// </summary>
	/// <returns>true/false</returns>
	const bool& IsToolTipsEnabled() const noexcept { return m_bEnableTooltips; }

	/// <summary>
	/// Set tooltips enabled/disabled.
	/// </summary>
	/// <param name="a"></param>
	void setTooltipsState(bool a) noexcept { m_bEnableTooltips = a; }

	/// <summary>
	/// Get the info on a menu item at the supplied path.
	/// </summary>
	/// <param name="iMask"></param>
	/// <param name="path"></param>
	/// <param name="mii"></param>
	/// <returns>true/false</returns>
	bool getMenuInfo(_In_ const UINT iMask, _In_ const TString& path, _In_ MENUITEMINFO& mii) const;

	/// <summary>
	/// Get a XPopupMenuItem based on an item position.
	/// </summary>
	/// <param name="hMenu">- The menu to search.</param>
	/// <param name="nPos">- The position to look for.</param>
	/// <returns>XPopupMenuItem*</returns>
	XPopupMenuItem* getMenuItem(_In_ HMENU hMenu, _In_ int nPos) const;

	/// <summary>
	/// Get a XPopupMenuItem based on a path.
	/// </summary>
	/// <param name="path"></param>
	/// <returns>XPopupMenuItem*</returns>
	XPopupMenuItem* getMenuItem(_In_ const TString& path) const;

	/// <summary>
	/// Get a XPopupMenuItem based on a CommandID.
	/// </summary>
	/// <param name="mID">- CommandID to look for.</param>
	/// <returns>XPopupMenuItem*</returns>
	XPopupMenuItem* getMenuItem(_In_ UINT mID) const noexcept;

	/// <summary>
	/// Get the specified menu item's rect.
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <param name="hMenu"></param>
	/// <param name="mID"></param>
	/// <returns></returns>
	static RECT getMenuItemRect(_In_opt_ HWND mHwnd, _In_ HMENU hMenu, _In_ UINT mID) noexcept;

	/// <summary>
	/// Get the specified menu item's rect.
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <param name="mID"></param>
	/// <returns></returns>
	RECT getMenuItemRect(_In_opt_ HWND mHwnd, _In_ UINT mID) const noexcept
	{
		return getMenuItemRect(mHwnd, this->getMenuHandle(), mID);
	}

	/// <summary>
	/// Get the specified menu item's text rect.
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <param name="hMenu"></param>
	/// <param name="mID"></param>
	/// <returns></returns>
	static RECT getMenuItemTextRect(_In_opt_ HWND mHwnd, _In_ HMENU hMenu, _In_ UINT mID) noexcept;

	/// <summary>
	/// Get the specified menu item's text rect.
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <param name="mID"></param>
	/// <returns></returns>
	RECT getMenuItemTextRect(_In_opt_ HWND mHwnd, _In_ UINT mID) const noexcept
	{
		return getMenuItemTextRect(mHwnd, this->getMenuHandle(), mID);
	}

	/// <summary>
	/// Get the specified menu item's value rect.
	/// This is the area used to display progressbars & trackbars
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <param name="hMenu"></param>
	/// <param name="mID"></param>
	/// <returns></returns>
	RECT getMenuItemValueRect(_In_opt_ HWND mHwnd, _In_ HMENU hMenu, _In_ UINT mID) const noexcept;

	/// <summary>
	/// Get an items possible value.
	/// </summary>
	/// <param name="mHwnd"></param>
	/// <param name="hMenu"></param>
	/// <param name="mID"></param>
	/// <returns></returns>
	int getMenuItemPossibleValue(_In_opt_ HWND mHwnd, _In_ HMENU hMenu, _In_ UINT mID) const noexcept;

	/// <summary>
	/// Gets groups data.
	/// </summary>
	/// <returns>VectorOfMenuItemGroups&amp;</returns>
	VectorOfMenuItemGroups& getGroups() noexcept { return m_Groups; }

	/// <summary>
	/// Gets groups data.
	/// </summary>
	/// <returns>const VectorOfMenuItemGroups&amp;</returns>
	const VectorOfMenuItemGroups& getGroups() const noexcept { return m_Groups; }

	/// <summary>
	/// Get an item group from a group id.
	/// </summary>
	/// <param name="nID"></param>
	/// <returns>The item group requested or an empty group.</returns>
	const DcxMenuItemGroup &getGroup(UINT nID) const noexcept;

	/// <summary>
	/// Converts a CommandID to a menu &amp; path.
	/// </summary>
	/// <param name="mID">- CommandID to convert.</param>
	/// <param name="tsPath">- Place to save the new path.</param>
	/// <param name="hMenu">- The menu to start searching. (maybe null)</param>
	/// <returns>a matching menu or nullptr</returns>
	HMENU CommandIDToPath(_In_ UINT mID, _Out_ TString& tsPath, _In_opt_ HMENU hMenu = nullptr) const;

	/// <summary>
	/// Set an items Check state. (unused atm)
	/// </summary>
	/// <param name="nPos"></param>
	/// <param name="bEnable"></param>
	void setItemCheckToggle(UINT nPos, bool bEnable) const;

	/// <summary>
	/// Check if a menu item is valid.
	/// </summary>
	/// <param name="pItem"></param>
	/// <returns></returns>
	const bool isItemValid(const XPopupMenuItem* const pItem) const noexcept;

	/// <summary>
	/// Convert a menu into a dcxml.
	/// NB: Icon information is stored in vIcons to be saved by the dialogs toXml() at the end.
	/// </summary>
	/// <param name="vIcons"></param>
	/// <param name="xml"></param>
	void toXml(VectorOfIcons& vIcons, TiXmlElement* const xml) const;

	/// <summary>
	/// Convert a menu into a dcxml.
	/// NB: Icon information is stored in vIcons to be saved by the dialogs toXml() at the end.
	/// </summary>
	/// <param name="vIcons"></param>
	/// <returns></returns>
	TiXmlElement* toXml(VectorOfIcons& vIcons) const;

	/// <summary>
	/// Convert a DCXML into a menu.
	/// </summary>
	/// <param name="xDcxml"></param>
	/// <param name="xThis"></param>
	/// <param name="vIcons"></param>
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis, const VectorOfIcons& vIcons);

	/// <summary>
	/// Save the menus image list to vIcons.
	/// </summary>
	/// <param name="vIcons"></param>
	/// <param name="xml"></param>
	void xmlSaveImageList(VectorOfIcons& vIcons, TiXmlElement* xml) const;

	/// <summary>
	/// Draw the menu border on the topmost menu window.
	/// </summary>
	/// <returns></returns>
	bool DrawBorder() const;

	/// <summary>
	/// Draw the menu border.
	/// </summary>
	/// <param name="hWnd">- Menus HWND</param>
	/// <returns></returns>
	bool DrawBorder(_In_opt_ HWND hWnd) const noexcept;

	/// <summary>
	/// Draw the menu border.
	/// </summary>
	/// <param name="hWnd">- Menus HWND</param>
	/// <param name="hdc">- Windows HDC</param>
	/// <returns></returns>
	bool DrawBorder(_In_opt_ HWND hWnd, _In_opt_ HDC hdc) const noexcept;

	/// <summary>
	/// A list of all menu items attached to this menu.
	/// </summary>
	VectorOfXPopupMenuItem m_vpMenuItem; //!< Vector of XPopupMenuItem Objects

private:
	void xpop_a(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo);
	void xpop_c(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo);
	void xpop_d(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo);
	void xpop_f(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo);
	void xpop_i(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo) const;
	void xpop_s(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo) const;
	void xpop_t(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo);
	void xpop_T(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo);

	/// <summary>
	/// Create a submenu for this menu.
	/// </summary>
	/// <returns>The new submenu or nullptr</returns>
	HMENU AddSubMenu();

	/// <summary>
	/// Deletes a submenu. Including all items & items custom data.
	/// </summary>
	/// <param name="hSubMenu"></param>
	void DeleteSubMenu(HMENU hSubMenu) noexcept;

protected:

	HMENU m_hMenu{ nullptr };			//!< Menu Handle
	HIMAGELIST m_hImageList{ nullptr };	//!< Menu ImageList
	MenuStyle m_MenuStyle{ MenuStyle::XPMS_OFFICE2003 };//!< Menu Style
	TString m_tsMenuName;				//!< Menu Name
	TString m_tsMarkedText;				//!< Extra field to store custom information
	TString m_tsCallback;
	UINT m_MenuItemStyles{};			//!< Menu Item Styles
	size_t m_menuNameHash{};			//!< Hash of tsMenuName

	//HBITMAP m_hBitmapCache{ nullptr };	//!< Menu Item Background Image cache in Custom(Big) Style, pre-rendered image sized for menu.
	dcxImage m_hBitmap;					//!< Menu Item Background Image in Custom(Big) Style

	XPMENUCOLORS m_MenuColors;			//!< Menu Colors

	VectorOfMenuItemGroups m_Groups;

	std::byte m_uiAlphaInactive{ 255 };			//!< Menus alpha level when inactive. 0 -> 255
	std::byte m_uiAlphaDefault{ 255 };			//!< Menu alpha level when in its default state. 0 -> 255

	bool m_bRoundedSel{ false };		//!< Menu has rounded selection box.
	bool m_bAttachedToMenuBar{ false }; //!< Is the menu attached to the mIRC window menubar?
	bool m_bEnableTooltips{ false };	//!< are tooltips enabled for this menu?
	bool m_bRoundedWindow{ false };		//!< Menu has rounded window.
	const bool m_bDestroyHMENU{ true };		//!< Should the destructor call DestroyMenu()
};

#if defined(XPOPUP_USE_UNIQUEPTR)
using VectorOfXPopupMenu = std::vector<std::unique_ptr<XPopupMenu>>; //!< Vector of XPopupMenu Objects
#else
using VectorOfXPopupMenu = std::vector<XPopupMenu*>; //!< Vector of XPopupMenu Objects
#endif

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENU_H_
