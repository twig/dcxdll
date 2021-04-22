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
	COLORREF m_clrBack{ RGB(255, 255, 255) };					//!< Menu Item BackGround Color
	COLORREF m_clrBox{ RGB(184, 199, 146) };					//!< Menu Item Box Color
	COLORREF m_clrLightBox{ RGB(240,243,231) };				//!< Menu Item Box Color (lighter by 200)
	COLORREF m_clrSelection{ RGB(255, 229, 179) };			//!< Menu Item Selection Box Color
	COLORREF m_clrDisabledSelection{ RGB(255, 255, 255) };	//!< Menu Item Disabled Selection Box Color
	COLORREF m_clrText{ RGB(0, 0, 0) };					//!< Menu Item Text Color
	COLORREF m_clrDisabledText{ RGB(128, 128, 128) };			//!< Menu Item Disabled Text Color
	COLORREF m_clrCheckBox{ RGB(255, 128, 0) };				//!< Menu Item CheckBox Color
	COLORREF m_clrDisabledCheckBox{ RGB(200, 200, 200) };		//!< Menu Item Disabled CheckBox Color
	COLORREF m_clrSeparatorLine{ RGB(128, 128, 128) };		//!< Menu Item Separator Line Color
	COLORREF m_clrSelectionBorder{ RGB(0, 0, 0) };		//!< Menu Item Selection Box Border Color
	COLORREF m_clrSelectedText{ RGB(0, 0, 0) };			//!< Menu Item Selected Text Colour
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

class XPopupMenuItem final
{
public:
	XPopupMenuItem() = delete;
	XPopupMenuItem(const XPopupMenuItem &) = delete;
	XPopupMenuItem &operator = (const XPopupMenuItem &) = delete;
	XPopupMenuItem(XPopupMenuItem &&) = delete;
	XPopupMenuItem &operator = (XPopupMenuItem &&) = delete;

	XPopupMenuItem( XPopupMenu * Parent, const bool bSep, ULONG_PTR dwDataBackup = NULL ) noexcept;
	XPopupMenuItem( XPopupMenu * Parent, const TString &tsItemText, const int nIcon, const bool bSubMenu, ULONG_PTR dwDataBackup = NULL );
	~XPopupMenuItem( ) noexcept = default;

	void DrawItem(const LPDRAWITEMSTRUCT lpdis);
	void DrawItemBackground( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol );
	void DrawItemBox( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol );
	void DrawItemText( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis = false );
	void DrawItemIcon( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const UINT iExStyles, const bool bSel = false, const bool bDis = false ) noexcept;
	void DrawItemSeparator( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol ) noexcept;

	SIZE getItemSize( const HWND mHwnd );

	static void DrawItemSelection(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis = false, const bool bRounded = false) noexcept;
	static void DrawItemCheckBox(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis = false) noexcept;
	static void DrawItemSubArrow(const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bDis = false) noexcept;
	static bool DrawMenuBitmap(const LPDRAWITEMSTRUCT lpdis, const bool bBigImage, const HBITMAP bmImage);
	static void DrawGradient(const HDC hdc, const RECT *const lprc, const COLORREF clrStart, const COLORREF clrEnd, const bool bHorz = false) noexcept;
	static void DrawVerticalBar( const LPDRAWITEMSTRUCT lpdis, const XPMENUCOLORS *const lpcol, const bool bReversed);

	static COLORREF LightenColor( const UINT iScale, const COLORREF clrColor ) noexcept;
	static COLORREF DarkenColor( const UINT iScale, const COLORREF clrColor ) noexcept;

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

	void setSubMenu( const bool bSubMenu ) noexcept;
	void setItemText( const TString & tsItemText );
	void setItemIcon( const int nIcon ) noexcept;
	const TString &getItemText( ) const noexcept;
	const int &getItemIcon(  ) const noexcept;

	const ULONG_PTR &getItemDataBackup() const noexcept
	{
		return m_dwItemDataBackup;
	}

protected:

	TString m_tsItemText;					//!< Menu Item Text
	TString m_tsItemCommand;				//!< Menu Item Command
	int m_nIcon{ -1 };						//!< Menu Item Icon Index
	XPopupMenu* m_pXParentMenu{ nullptr };	//!< Parent XPopupMenu
	ULONG_PTR m_dwItemDataBackup{};
	bool m_bBigBitmap{ false };				//!< Single large bitmap image used for whole menu?
	bool m_bSep{ false };					//!< Is Separator ?
	bool m_bSubMenu{ false };				//!< Has A SubMenu ?
	bool m_bReserved{ false };				//!< Reserved for future use.
};

using VectorOfXPopupMenuItem = std::vector<XPopupMenuItem *>; //!< Vector of XPopupMenuItem Objects

#ifdef __INTEL_COMPILER // Defined when using Intel C++ Compiler.
#pragma warning( pop )
#endif

#endif // _XPOPUPMENUITEM_H_
