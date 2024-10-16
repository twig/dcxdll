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

constexpr auto XPMI_HEIGHT = 22;	//!< Menu Item Height
constexpr auto XPMI_BOXLPAD = 0;	//!< Padding space between menu left bordure and box left bordure
constexpr auto XPMI_BOXWIDTH = 22;	//!< Box Width (should be no less than 16)
constexpr auto XPMI_BOXRPAD = 5;	//!< Padding space between box right border and text/line
constexpr auto XPMI_ICONSIZE = 16;	//!< Icon Size
constexpr auto XPMI_MINSTRING = 20;	//!< Min space reserved for string
constexpr auto XPMI_SEPHEIGHT = 3;	//!< Seperator item height.
constexpr auto XPMI_SUBARROW_WIDTH = 9;	//!< size of the default subitem arrow
constexpr auto XPMI_SUBARROW_HEIGHT = 10;	//!< size of the default subitem arrow
constexpr auto XPMI_SUBARROW_XPAD = 2;

// dummy class, resolved at runtime
class XPopupMenu;

/*!
* \brief XPopup Menu Item Colors
*
* Structure containing the menu item colors
*/

struct XPMENUCOLORS final
{
	COLORREF m_clrBack{ RGB(255, 255, 255) };				//!< Menu Item BackGround Color
	COLORREF m_clrBox{ RGB(184, 199, 146) };				//!< Menu Item Box Color
	COLORREF m_clrLightBox{ RGB(240,243,231) };				//!< Menu Item Box Color (lighter by 200)
	COLORREF m_clrSelection{ RGB(255, 229, 179) };			//!< Menu Item Selection Box Color
	COLORREF m_clrDisabledSelection{ RGB(255, 255, 255) };	//!< Menu Item Disabled Selection Box Color
	COLORREF m_clrText{ RGB(0, 0, 0) };						//!< Menu Item Text Color
	COLORREF m_clrDisabledText{ RGB(128, 128, 128) };		//!< Menu Item Disabled Text Color
	//COLORREF m_clrCheckBox{ RGB(255, 128, 0) };			//!< Menu Item CheckBox Color
	//COLORREF m_clrDisabledCheckBox{ RGB(200, 200, 200) };	//!< Menu Item Disabled CheckBox Color
	COLORREF m_clrSeparatorLine{ RGB(128, 128, 128) };		//!< Menu Item Separator Line Color
	COLORREF m_clrSelectionBorder{ RGB(0, 0, 0) };			//!< Menu Item Selection Box Border Color
	COLORREF m_clrSelectedText{ RGB(0, 0, 0) };				//!< Menu Item Selected Text Colour
	COLORREF m_clrBorder{ CLR_INVALID };					//!< Menu Border colour.

	clrCheckBox m_clrCheckBox;								//!< Menu Item CheckBox Colors

	void toXml(TiXmlElement* xml) const
	{
		setColourAttribute(xml, "back", m_clrBack);

		setColourAttribute(xml, "box", m_clrBox);
		setColourAttribute(xml, "lightbox", m_clrLightBox);
		setColourAttribute(xml, "selection", m_clrSelection);
		setColourAttribute(xml, "disabledsel", m_clrDisabledSelection);

		setColourAttribute(xml, "text", m_clrText);
		setColourAttribute(xml, "disabledtext", m_clrDisabledText);
		setColourAttribute(xml, "selectedtext", m_clrSelectedText);

		setColourAttribute(xml, "checkbox", m_clrCheckBox.m_clrBackground);
		setColourAttribute(xml, "disabledcheckbox", m_clrCheckBox.m_clrDisabledBackground);

		setColourAttribute(xml, "separator", m_clrSeparatorLine);
		setColourAttribute(xml, "selectionborder", m_clrSelectionBorder);
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

		if (const auto tmp = queryColourAttribute(xml, "box"); tmp != CLR_INVALID)
			m_clrBox = tmp;
		if (const auto tmp = queryColourAttribute(xml, "lightbox"); tmp != CLR_INVALID)
			m_clrLightBox = tmp;
		if (const auto tmp = queryColourAttribute(xml, "selection"); tmp != CLR_INVALID)
			m_clrSelection = tmp;
		if (const auto tmp = queryColourAttribute(xml, "disabledsel"); tmp != CLR_INVALID)
			m_clrDisabledSelection = tmp;

		if (const auto tmp = queryColourAttribute(xml, "text"); tmp != CLR_INVALID)
			m_clrText = tmp;
		if (const auto tmp = queryColourAttribute(xml, "disabledtext"); tmp != CLR_INVALID)
			m_clrDisabledText = tmp;
		if (const auto tmp = queryColourAttribute(xml, "selectedtext"); tmp != CLR_INVALID)
			m_clrSelectedText = tmp;
		if (const auto tmp = queryColourAttribute(xml, "checkbox"); tmp != CLR_INVALID)
			m_clrCheckBox.m_clrBackground = tmp;
		if (const auto tmp = queryColourAttribute(xml, "disabledcheckbox"); tmp != CLR_INVALID)
			m_clrCheckBox.m_clrDisabledBackground = tmp;
		if (const auto tmp = queryColourAttribute(xml, "separator"); tmp != CLR_INVALID)
			m_clrSeparatorLine = tmp;
		if (const auto tmp = queryColourAttribute(xml, "selectionborder"); tmp != CLR_INVALID)
			m_clrSelectionBorder = tmp;
	}
};
using LPXPMENUCOLORS = XPMENUCOLORS*;

/*!
 * \brief blah
 *
 * blah
 */

class XPopupMenuItem final
{
public:
	XPopupMenuItem() = delete;
	XPopupMenuItem(const XPopupMenuItem&) = delete;
	XPopupMenuItem& operator = (const XPopupMenuItem&) = delete;
	XPopupMenuItem(XPopupMenuItem&&) = delete;
	XPopupMenuItem& operator = (XPopupMenuItem&&) = delete;

	XPopupMenuItem(XPopupMenu* Parent, const bool bSep, ULONG_PTR dwDataBackup = 0) noexcept;
	XPopupMenuItem(XPopupMenu* Parent, const TString& tsItemText, const bool bSubMenu, ULONG_PTR dwDataBackup = 0);
	XPopupMenuItem(XPopupMenu* Parent, const TString& tsItemText, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup = 0);
	XPopupMenuItem(XPopupMenu* Parent, const TString& tsItemText, const TString& tsTooltip, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup = 0);
	~XPopupMenuItem() noexcept = default;

	bool operator==(const XPopupMenuItem& other) const = default;

	void DrawItem(const LPDRAWITEMSTRUCT lpdis);
	void DrawItemBackground(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol);
	void DrawItemBox(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol);
	void DrawItemText(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis = false);
	void DrawItemIcon(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const UINT iExStyles, const bool bSel = false, const bool bDis = false) noexcept;
	void DrawItemSeparator(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol) noexcept;

	SIZE getItemSize(const HWND mHwnd);

	static void DrawItemSelection(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis = false, const bool bRounded = false) noexcept;
	static void DrawItemCheckBox(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis = false, const bool bRounded = false) noexcept;
	static void DrawItemRadioCheck(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis = false, const bool bRounded = false) noexcept;
	static void DrawItemSubArrow(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bDis = false) noexcept;
	static bool DrawMenuBitmap(const LPDRAWITEMSTRUCT lpdis, const bool bBigImage, const HBITMAP bmImage);
	static void DrawGradient(const HDC hdc, const RECT* const lprc, const COLORREF clrStart, const COLORREF clrEnd, const bool bHorz = false) noexcept;
	static void DrawVerticalBar(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS* const lpcol, const bool bReversed);

	static COLORREF LightenColor(const UINT iScale, const COLORREF clrColor) noexcept;
	static COLORREF DarkenColor(const UINT iScale, const COLORREF clrColor) noexcept;

	// oddly the functions below cause VS2017 15.5.1 - 15.6 to crash
	//constexpr COLORREF LightenColor(const UINT iScale, const COLORREF clrColor) noexcept
	//{
	//	const auto nScale = gsl::narrow_cast<int>(iScale);
	//	const auto R = Dcx::dcxMulDiv32(255 - GetRValue(clrColor), nScale, 255) + GetRValue(clrColor);
	//	const auto G = Dcx::dcxMulDiv32(255 - GetGValue((clrColor & 0xFFFF)), nScale, 255) + GetGValue((clrColor & 0xFFFF));
	//	const auto B = Dcx::dcxMulDiv32(255 - GetBValue(clrColor), nScale, 255) + GetBValue(clrColor);
	//	return RGB(R, G, B);
	//}
	//constexpr COLORREF DarkenColor(const UINT iScale, const COLORREF clrColor) noexcept
	//{
	//	const auto nScale = gsl::narrow_cast<int>(iScale);
	//	const auto R = Dcx::dcxMulDiv32(GetRValue(clrColor), (255 - nScale), 255);
	//	const auto G = Dcx::dcxMulDiv32(GetGValue((clrColor & 0xFFFF)), (255 - nScale), 255);
	//	const auto B = Dcx::dcxMulDiv32(GetBValue(clrColor), (255 - nScale), 255);
	//	return RGB(R, G, B);
	//}

	void setSubMenu(const bool bSubMenu) noexcept;
	void setItemText(const TString& tsItemText);
	void setItemIcon(const int nIcon) noexcept;
	void setItemTooltip(const TString& tsText) { m_tsTooltipText = tsText; }
	void setOverrideStyle(UINT uStyle) noexcept;
	void setCheckToggle(bool bState) noexcept { m_bCheckToggle = bState; }
	void setRadioCheck(bool bEnable) noexcept { m_bRadioCheck = bEnable; }

	const TString& getItemText() const noexcept;
	const int& getItemIcon() const noexcept;
	const TString& getItemTooltipText() const noexcept { return m_tsTooltipText; }
	const XPopupMenu* getParentMenu() const noexcept { return m_pXParentMenu; }
	const ULONG_PTR& getItemDataBackup() const noexcept { return m_dwItemDataBackup; }
	auto getStyle() const noexcept;

	bool IsTooltipsEnabled() const noexcept;
	const bool& IsCheckToggle() const noexcept { return m_bCheckToggle; };
	const bool& IsRadioCheck() const noexcept { return m_bRadioCheck; };

	void setCommandID(UINT mID) noexcept { m_CommandID = mID; }
	const UINT& getCommandID() const noexcept { return m_CommandID; }

	bool parseItemText();

	/// <summary>
	/// Convert a menuitem into a dcxml.
	/// NB: Icon information is stored in vIcons to be saved by the dialogs toXml() at the end.
	/// </summary>
	/// <param name="vIcons"></param>
	/// <param name="xml"></param>
	void toXml(VectorOfIcons& vIcons, TiXmlElement* const xml) const;

	/// <summary>
	/// Convert a menuitem into a dcxml.
	/// NB: Icon information is stored in vIcons to be saved by the dialogs toXml() at the end.
	/// </summary>
	/// <param name="vIcons"></param>
	/// <returns></returns>
	TiXmlElement* toXml(VectorOfIcons& vIcons) const;

	/// <summary>
	/// Convert a DCXML into a menuitem.
	/// </summary>
	/// <param name="xDcxml"></param>
	/// <param name="xThis"></param>
	/// <param name="vIcons"></param>
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis, const VectorOfIcons& vIcons);

protected:

	TString m_tsItemText;					//!< Menu Item Text
	//TString m_tsItemCommand;				//!< Menu Item Command
	TString m_tsTooltipText;				//!< Menu Items tooltip text (if any)
	int m_nIcon{ -1 };						//!< Menu Item Icon Index
	XPopupMenu* m_pXParentMenu{ nullptr };	//!< Parent XPopupMenu
	ULONG_PTR m_dwItemDataBackup{};
	UINT m_eStyleOverride{};
	UINT m_CommandID{};
	bool m_bBigBitmap{ false };				//!< Single large bitmap image used for whole menu?
	bool m_bSep{ false };					//!< Is Separator ?
	bool m_bSubMenu{ false };				//!< Has A SubMenu ?
	bool m_bCheckToggle{ false };				//!< Reserved for future use.
	bool m_bRadioCheck{ false };
};

using VectorOfXPopupMenuItem = std::vector<XPopupMenuItem*>; //!< Vector of XPopupMenuItem Objects

#endif // _XPOPUPMENUITEM_H_
