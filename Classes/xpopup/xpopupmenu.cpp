/*!
 * \file xpopupmenu.cpp
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
#include "defines.h"
#include "Classes/xpopup/xpopupmenu.h"
#include "Classes/xpopup/xmenubar.h"
#include "Dcx.h"

 // menu class name is "#32768"

 /*!
  * \brief Constructor
  *
  * blah
  */

XPopupMenu::XPopupMenu(const TString& tsMenuName, MenuStyle mStyle)
	: XPopupMenu(tsMenuName, CreatePopupMenu())
{
	m_MenuStyle = mStyle;
}

/*!
 * \brief Constructor
 *
 * Default constructor for testing
 */

XPopupMenu::XPopupMenu(const TString& tsName, HMENU hMenu)
	: m_hMenu(hMenu), m_tsMenuName(tsName), m_menuNameHash(std::hash<TString>{}(tsName)), m_bDestroyHMENU(m_menuNameHash != TEXT("mircbar"_hash) && m_menuNameHash != TEXT("dialog"_hash) && m_menuNameHash != TEXT("scriptpopup"_hash))
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

XPopupMenu::~XPopupMenu()
{
	if (this->m_bAttachedToMenuBar)
		this->detachFromMenuBar(nullptr);

	this->clearAllMenuItems();

	this->m_hBitmap.reset();

	//if (this->m_hBitmap)
	//	DeleteBitmap(this->m_hBitmap);

	//if (m_hMenu && m_menuNameHash != TEXT("mircbar"_hash) && m_menuNameHash != TEXT("dialog"_hash))
	//	DestroyMenu(this->m_hMenu);

	// Ook: possible fix for `Commands` menu failing after dcx is unloaded. (needs looked at more)
	//if (m_hMenu && m_menuNameHash != TEXT("mircbar"_hash) && m_menuNameHash != TEXT("dialog"_hash) && m_menuNameHash != TEXT("scriptpopup"_hash))
	//	DestroyMenu(this->m_hMenu);

	if (m_hMenu && IsHMENUDestrucible())
		DestroyMenu(this->m_hMenu);

	destroyImageList();
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::parseXPopCommand(const TString& input)
{
	const XSwitchFlags flags(input.gettok(2));
	const auto path(input.getfirsttok(1, TSTABCHAR).gettok(3, -1).trim());	// tok 1
	const auto path_toks = path.numtok();

	HMENU hMenu = nullptr;
	if (path_toks == 1)
		hMenu = m_hMenu;
	else {
		hMenu = parsePath(path.gettok(1, gsl::narrow_cast<int>(path_toks - 1)), m_hMenu);

		if (!hMenu)
			throw DcxExceptions::dcxInvalidPath();
	}

	// this should never be null here.
	if (!hMenu)
		throw Dcx::dcxException(TEXT("Menu is NULL."));

	const auto numtok = input.numtok();
	const auto tabtoks = input.numtok(TSTABCHAR);
	const auto tsTabTwo(input.getlasttoks().trim());	// tok 2, -1
	const auto toks_in_tab_two = tsTabTwo.numtok();
	auto nPos = path.gettok(gsl::narrow_cast<int>(path_toks)).to_int() - 1;

	// xpop -a - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS] [ID] [ICON] ItemText (: Command) ([TAB] [TOOLTIP])
	if (flags[TEXT('a')] && tabtoks > 1 && toks_in_tab_two > 3)
	{
		const XSwitchFlags xflags(tsTabTwo.getfirsttok(1));		// tok 1 [+FLAGS]
		const auto mID = tsTabTwo.getnexttok().to_<UINT>();		// tok 2 [ID]
		const auto nIcon = tsTabTwo.getnexttok().to_int() - 1;	// tok 3 [ICON]

		TString tsItemText, tsTooltip;
		//TString itemcom;

		{
			const auto itemtext(tsTabTwo.getlasttoks());			// tok 4, -1 ItemText....

			if (tsTabTwo.numtok(TSTABCHAR) > 1)
			{
				tsItemText = itemtext.gettok(1, TSTABCHAR).trim();
				tsTooltip = itemtext.getlasttoks().trim();
			}
			else
				tsItemText = itemtext;

			// Ook: this never seems to be used....
			//if (tsItemText.numtok(TEXT(':')) > 1)
			//	itemcom = tsItemText.gettok(2, TEXT(':')).trim();
		}

		if (nPos == -1)
			nPos += GetMenuItemCount(hMenu) + 1;

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);

		std::unique_ptr<XPopupMenuItem> p_Item;

		if (tsItemText == TEXT('-'))
		{
			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
			mii.fType = MFT_OWNERDRAW | MFT_SEPARATOR;

			p_Item = std::make_unique<XPopupMenuItem>(this, true);
		}
		else {
			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
			mii.fType = MFT_OWNERDRAW;
			mii.wID = mID;

			if (!xflags[TEXT('+')])
				throw DcxExceptions::dcxInvalidFlag();

			// submenu
			if (xflags[TEXT('s')])
			{
				mii.fMask |= MIIM_SUBMENU;
				if (mii.hSubMenu)
					DestroyMenu(mii.hSubMenu);

				mii.hSubMenu = CreatePopupMenu();

				if (!mii.hSubMenu)
					throw Dcx::dcxException(TEXT("Failed to create submenu."));
			}
			if (xflags[TEXT('c')])
				mii.fState |= MFS_CHECKED;
			if (xflags[TEXT('g')])
				mii.fState |= MFS_GRAYED;

			p_Item = std::make_unique<XPopupMenuItem>(this, tsItemText, tsTooltip, nIcon, (mii.hSubMenu != nullptr));
		}

		mii.dwItemData = reinterpret_cast<ULONG_PTR>(p_Item.get());
		this->m_vpMenuItem.push_back(p_Item.release());
		InsertMenuItem(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	}
	// xpop -c - [MENU] [SWITCH] [PATH]
	else if (flags[TEXT('c')] && numtok > 2)
	{
		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU | MIIM_DATA;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
			throw Dcx::dcxException("Unable to get menu item info");

		if (mii.hSubMenu)
		{
			this->deleteAllItemData(mii.hSubMenu);
			DestroyMenu(mii.hSubMenu);
		}

		mii.hSubMenu = CreatePopupMenu();

		if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			p_Item->setSubMenu(TRUE);

		mii.fMask = MIIM_SUBMENU;

		SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	}
	// xpop -d - [MENU] [SWITCH] [PATH]
	else if (flags[TEXT('d')] && numtok > 2)
	{
		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU | MIIM_DATA;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
			throw Dcx::dcxException("Unable to get menu item info");

		if (mii.hSubMenu)
		{
			this->deleteAllItemData(mii.hSubMenu);
			DestroyMenu(mii.hSubMenu);
		}

		if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			p_Item->setSubMenu(FALSE);

		mii.hSubMenu = nullptr;
		mii.fMask = MIIM_SUBMENU;

		SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	}
	// xpop -f - [MENU] [SWITCH] [PATH]
	else if (flags[TEXT('f')] && numtok > 2)
	{
		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU | MIIM_DATA;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
			throw Dcx::dcxException("Unable to get menu item info");

		if (mii.hSubMenu)
		{
			this->deleteAllItemData(mii.hSubMenu);
			DestroyMenu(mii.hSubMenu);
		}

		if (const auto* const p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			this->deleteMenuItemData(p_Item, nullptr);

		DeleteMenu(hMenu, gsl::narrow_cast<UINT>(nPos), MF_BYPOSITION);
	}
	// xpop -i - [MENU] [SWITCH] [PATH] [TAB] [ICON]
	else if (flags[TEXT('i')] && tabtoks > 1 && toks_in_tab_two > 0)
	{
		const auto nIcon = tsTabTwo.to_int();

		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
			throw Dcx::dcxException("Unable to get menu item info");

		if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			p_Item->setItemIcon(nIcon);
	}
	// xpop -s - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS]
	else if (flags[TEXT('s')] && tabtoks > 1 && toks_in_tab_two > 0)
	{
		const XSwitchFlags xflags(tsTabTwo);

		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_STATE;

		// Flags may be empty, so no error if + is misssing.
		if (xflags[TEXT('+')])
		{
			if (xflags[TEXT('c')])
				mii.fState |= MFS_CHECKED;
			if (xflags[TEXT('g')])
				mii.fState |= MFS_GRAYED;
		}

		SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	}
	// xpop -t - [MENU] [SWITCH] [PATH] [TAB] Itemtext
	else if (flags[TEXT('t')] && tabtoks > 1 && toks_in_tab_two > 0)
	{
		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA | MIIM_STATE | MIIM_SUBMENU | MIIM_FTYPE | MIIM_ID;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
			throw Dcx::dcxException("Unable to get menu item info");

		if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			p_Item->setItemText(tsTabTwo);

		// this is to make sure system resets the measurement of the itemwidth on next display
		DeleteMenu(hMenu, gsl::narrow_cast<UINT>(nPos), MF_BYPOSITION);
		InsertMenuItem(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	}
	// xpop -T - [MENU] [SWITCH] [PATH] [TAB] (text)
	else if (flags[TEXT('T')])
	{
		if (tabtoks < 2)
			throw DcxExceptions::dcxInvalidArguments();

		if (nPos < 0)
			throw DcxExceptions::dcxInvalidPath();

		MENUITEMINFO mii{};
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_DATA | MIIM_STATE | MIIM_SUBMENU | MIIM_FTYPE | MIIM_ID;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
			throw Dcx::dcxException("Unable to get menu item info");

		if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			p_Item->setItemTooltip(tsTabTwo);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

 //void XPopupMenu::parseXPopIdentifier( const TString & input, TCHAR * szReturnValue ) const
 //{
 //	const auto numtok = input.numtok( );
 //	const auto prop(input.getfirsttok(2));		// tok 2
 //	const auto path(input.getlasttoks());		// tok 3, -1
 //
 //	// [NAME] [ID] [PROP] [PATH]
 //	if ( prop == TEXT("num") && numtok > 2 ) {
 //
 //		HMENU hMenu = nullptr;
 //		if ( path == TEXT("root") )
 //			hMenu = m_hMenu;
 //		else
 //			hMenu = parsePath( path, m_hMenu );
 //
 //		if (hMenu == nullptr)
 //			throw Dcx::dcxException("Unable to get menu");
 //
 //		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), GetMenuItemCount( hMenu ) );
 //	}
 //	else if ((prop == TEXT("text") || prop == TEXT("icon")) && numtok > 2) {
 //
 //		//HMENU hMenu;
 //		//
 //		//if ( path.numtok( ) == 1 )
 //		//	hMenu = this->m_hMenu;
 //		//else
 //		//	hMenu = this->parsePath( path.gettok( 1, path.numtok( ) - 1 ), this->m_hMenu );
 //		//
 //		//if (hMenu == nullptr)
 //		//	throw Dcx::dcxException("Unable to get menu");
 //		//
 //		//const auto nPos = path.gettok( path.numtok( ) ).to_int( ) - 1;
 //		//
 //		//if (nPos < 0)
 //		//	throw Dcx::dcxException("Invalid Path");
 //		//
 //		//MENUITEMINFO mii;
 //		//ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
 //		//mii.cbSize = sizeof( MENUITEMINFO );
 //		//mii.fMask = MIIM_DATA;
 //		//
 //		//if ( GetMenuItemInfo( hMenu, (UINT)nPos, TRUE, &mii ) ) {
 //		//
 //		//	const auto p_Item = reinterpret_cast<XPopupMenuItem *>(mii.dwItemData);
 //		//	if (p_Item == nullptr)
 //		//		throw Dcx::dcxException("Unable to get menu data");
 //		//
 //		//	if ( prop == TEXT("text") )
 //		//		dcx_strcpyn( szReturnValue, p_Item->getItemText( ).to_chr( ), MIRC_BUFFER_SIZE_CCH )
 //		//	else if ( prop == TEXT("icon") )
 //		//		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), p_Item->getItemIcon( ) + 1 );
 //		//}
 //
 //		MENUITEMINFO mii;
 //		if (getMenuInfo(MIIM_DATA, path, mii))
 //		{
 //			const auto p_Item = reinterpret_cast<XPopupMenuItem *>(mii.dwItemData);
 //			if (p_Item == nullptr)
 //				throw Dcx::dcxException("Unable to get menu data");
 //
 //			if (prop == TEXT("text"))
 //				dcx_strcpyn(szReturnValue, p_Item->getItemText().to_chr(), MIRC_BUFFER_SIZE_CCH);
 //			else if ( prop == TEXT("icon") )
 //				wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), p_Item->getItemIcon( ) + 1 );
 //		}
 //	}
 //	else if ( ( prop == TEXT("checked") || prop == TEXT("enabled") ) && numtok > 2 ) {
 //
 //		//HMENU hMenu;
 //		//
 //		//if ( path.numtok( ) == 1 )
 //		//	hMenu = this->m_hMenu;
 //		//else
 //		//	hMenu = this->parsePath( path.gettok( 1, path.numtok( ) - 1 ), this->m_hMenu );
 //		//
 //		//if (hMenu == nullptr)
 //		//	throw Dcx::dcxException("Unable to get menu");
 //		//
 //		//const auto nPos = path.gettok(path.numtok()).to_int() - 1;
 //		//
 //		//if (nPos < 0)
 //		//	throw Dcx::dcxException("Invalid Path");
 //		//
 //		//MENUITEMINFO mii;
 //		//ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
 //		//mii.cbSize = sizeof( MENUITEMINFO );
 //		//mii.fMask = MIIM_STATE;
 //		//
 //		//if ( GetMenuItemInfo( hMenu, (UINT)nPos, TRUE, &mii ) ) {
 //		//
 //		//	if (prop == TEXT("checked")) {
 //		//		dcx_Con(dcx_testflag(mii.fState, MFS_CHECKED), szReturnValue);
 //		//	}
 //		//	else if (prop == TEXT("enabled")) {
 //		//		dcx_Con(!dcx_testflag(mii.fState, MFS_GRAYED), szReturnValue);
 //		//	}
 //		//}
 //
 //		MENUITEMINFO mii;
 //		if (getMenuInfo(MIIM_STATE, path, mii))
 //		{
 //			if (prop == TEXT("checked")) {
 //				dcx_Con(dcx_testflag(mii.fState, MFS_CHECKED), szReturnValue);
 //			}
 //			else if (prop == TEXT("enabled")) {
 //				dcx_Con(!dcx_testflag(mii.fState, MFS_GRAYED), szReturnValue);
 //			}
 //
 //		}
 //	}
 //	else if ( prop == TEXT("submenu") && numtok > 2 ) {
 //
 //		//HMENU hMenu;
 //		//
 //		//if ( path.numtok( ) == 1 )
 //		//	hMenu = this->m_hMenu;
 //		//else
 //		//	hMenu = this->parsePath( path.gettok( 1, path.numtok( ) - 1 ), this->m_hMenu );
 //		//
 //		//if (hMenu == nullptr)
 //		//	throw Dcx::dcxException("Unable to get menu");
 //		//
 //		//const auto nPos = path.gettok(path.numtok()).to_int() - 1;
 //		//
 //		//if (nPos < 0)
 //		//	throw Dcx::dcxException("Invalid Path");
 //		//
 //		//MENUITEMINFO mii;
 //		//ZeroMemory( &mii, sizeof( MENUITEMINFO ) );
 //		//mii.cbSize = sizeof( MENUITEMINFO );
 //		//mii.fMask = MIIM_SUBMENU;
 //		//
 //		//if ( GetMenuItemInfo( hMenu, (UINT)nPos, TRUE, &mii ) ) {
 //		//
 //		//	dcx_ConChar(mii.hSubMenu != nullptr, szReturnValue);
 //		//}
 //
 //		MENUITEMINFO mii;
 //		if (getMenuInfo(MIIM_SUBMENU, path, mii))
 //		{
 //			dcx_ConChar(mii.hSubMenu != nullptr, szReturnValue);
 //		}
 //	}
 //}

void XPopupMenu::parseXPopIdentifier(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	const auto numtok = input.numtok();
	const auto propHash = std::hash<TString>{}(input.getfirsttok(2));		// tok 2
	const auto path(input.getlasttoks());		// tok 3, -1

	if (numtok < 3)
		throw DcxExceptions::dcxInvalidArguments();

	// [NAME] [ID] [PROP] [PATH]
	switch (propHash)
	{
	case TEXT("num"_hash):
	{
		HMENU hMenu = nullptr;
		if (path == TEXT("root"))
			hMenu = m_hMenu;
		else
			hMenu = parsePath(path, m_hMenu);

		if (!hMenu)
			throw Dcx::dcxException("Unable to get menu");

		const auto i = GetMenuItemCount(hMenu);
		_ts_snprintf(szReturnValue, TEXT("%d"), i);
	}
	break;
	case TEXT("text"_hash):
	case TEXT("tooltip"_hash):
	case TEXT("icon"_hash):
	{
		if (MENUITEMINFO mii{}; getMenuInfo(MIIM_DATA, path, mii))
		{
			const auto* const p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData);
			if (p_Item == nullptr)
				throw Dcx::dcxException("Unable to get menu data");

			if (propHash == TEXT("text"_hash))
				szReturnValue = p_Item->getItemText().to_chr();
			else if (propHash == TEXT("tooltip"_hash))
				szReturnValue = p_Item->getItemTooltipText().to_chr();
			else if (propHash == TEXT("icon"_hash))
			{
				const auto i = p_Item->getItemIcon() + 1;
				_ts_snprintf(szReturnValue, TEXT("%d"), i);
			}
		}
	}
	break;
	case TEXT("checked"_hash):
	case TEXT("enabled"_hash):
	{
		if (MENUITEMINFO mii{}; getMenuInfo(MIIM_STATE, path, mii))
		{
			if (propHash == TEXT("checked"_hash))
				szReturnValue = dcx_truefalse(dcx_testflag(mii.fState, MFS_CHECKED));
			else if (propHash == TEXT("enabled"_hash))
				szReturnValue = dcx_truefalse(!dcx_testflag(mii.fState, MFS_GRAYED));
		}
	}
	break;
	case TEXT("submenu"_hash):
	{
		if (MENUITEMINFO mii{}; getMenuInfo(MIIM_SUBMENU, path, mii))
		{
			if (mii.hSubMenu)
				szReturnValue = TEXT('1');
			else
				szReturnValue = TEXT('0');
		}
	}
	break;
	default:
		throw Dcx::dcxException("Unknown Property");
	}
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST& XPopupMenu::getImageList() noexcept
{
	if (!m_hImageList)
		m_hImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 0);

	return m_hImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::destroyImageList() noexcept
{
	if (this->m_hImageList)
		ImageList_Destroy(this->m_hImageList);

	this->m_hImageList = nullptr;
}

/*!
* \brief blah
*
* blah
*/

void XPopupMenu::setColor(const MenuColours nColor, const COLORREF clrColor) noexcept
{
	switch (nColor)
	{
	case MenuColours::XPMC_BACKGROUND:
		this->m_MenuColors.m_clrBack = clrColor;
		break;

	case MenuColours::XPMC_ICONBOX:
		m_MenuColors.m_clrBox = clrColor;
		m_MenuColors.m_clrLightBox = XPopupMenuItem::LightenColor(200, m_MenuColors.m_clrBox);
		break;

	case MenuColours::XPMC_CHECKBOX:
		this->m_MenuColors.m_clrCheckBox = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_DISABLED:
		this->m_MenuColors.m_clrDisabledCheckBox = clrColor;
		break;

	case MenuColours::XPMC_SELECTIONBOX_DISABLED:
		this->m_MenuColors.m_clrDisabledSelection = clrColor;
		break;

	case MenuColours::XPMC_TEXT_DISABLED:
		this->m_MenuColors.m_clrDisabledText = clrColor;
		break;

	case MenuColours::XPMC_SELECTIONBOX:
		this->m_MenuColors.m_clrSelection = clrColor;
		break;

	case MenuColours::XPMC_SELECTIONBOX_BORDER:
		this->m_MenuColors.m_clrSelectionBorder = clrColor;
		break;

	case MenuColours::XPMC_SEPARATOR:
		this->m_MenuColors.m_clrSeparatorLine = clrColor;
		break;

	case MenuColours::XPMC_TEXT:
		this->m_MenuColors.m_clrText = clrColor;
		break;

	case MenuColours::XPMC_SELECTEDTEXT:
		this->m_MenuColors.m_clrSelectedText = clrColor;
		break;

	case MenuColours::XPMC_MAX:
	default:
		break;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF XPopupMenu::getColor(const MenuColours nColor) const noexcept
{
	switch (nColor)
	{
	case MenuColours::XPMC_BACKGROUND:
		return this->m_MenuColors.m_clrBack;

	case MenuColours::XPMC_ICONBOX:
		return this->m_MenuColors.m_clrBox;

	case MenuColours::XPMC_CHECKBOX:
		return this->m_MenuColors.m_clrCheckBox;

	case MenuColours::XPMC_CHECKBOX_DISABLED:
		return this->m_MenuColors.m_clrDisabledCheckBox;

	case MenuColours::XPMC_SELECTIONBOX_DISABLED:
		return this->m_MenuColors.m_clrDisabledSelection;

	case MenuColours::XPMC_TEXT_DISABLED:
		return this->m_MenuColors.m_clrDisabledText;

	case MenuColours::XPMC_SELECTIONBOX:
		return this->m_MenuColors.m_clrSelection;

	case MenuColours::XPMC_SELECTIONBOX_BORDER:
		return this->m_MenuColors.m_clrSelectionBorder;

	case MenuColours::XPMC_SEPARATOR:
		return this->m_MenuColors.m_clrSeparatorLine;

	case MenuColours::XPMC_TEXT:
		return this->m_MenuColors.m_clrText;

	case MenuColours::XPMC_SELECTEDTEXT:
		return this->m_MenuColors.m_clrSelectedText;

	case MenuColours::XPMC_MAX:
	default:
		return 0;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

HMENU XPopupMenu::parsePath(const TString& path, const HMENU hParent, const UINT depth)
{
	if (!hParent)
		return nullptr;

	const auto iItem = path.gettok(gsl::narrow_cast<int>(depth)).to_int() - 1;
	auto hMenu = GetSubMenu(hParent, iItem);

	if (depth == path.numtok())
		return hMenu;
	else if (hMenu)
		return parsePath(path, hMenu, depth + 1U);

	return nullptr;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::deleteMenuItemData(const XPopupMenuItem* const p_Item, LPMENUITEMINFO mii) noexcept
{
	//auto itStart = this->m_vpMenuItem.begin();
	//auto itEnd = this->m_vpMenuItem.end();
	//
	//while (itStart != itEnd) {
	//
	//	if (*itStart == p_Item) {
	//		if (mii != nullptr)
	//			mii->dwItemData = (*itStart)->getItemDataBackup();
	//		delete *itStart;
	//		this->m_vpMenuItem.erase(itStart);
	//		return;
	//	}
	//
	//	++itStart;
	//}

	//const auto itEnd = m_vpMenuItem.end();
	//const auto itGot = std::find(m_vpMenuItem.begin(), itEnd, p_Item);
	//if (itGot != itEnd)
	//{
	//	if (mii != nullptr)
	//		mii->dwItemData = (*itGot)->getItemDataBackup();
	//	delete *itGot;
	//	m_vpMenuItem.erase(itGot);
	//}

	if (Dcx::eraseIfFound(m_vpMenuItem, p_Item))
	{
		if (mii)
			mii->dwItemData = p_Item->getItemDataBackup();
		delete p_Item;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::deleteAllItemData(HMENU hMenu)
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA;

	const auto n = GetMenuItemCount(hMenu);

	for (auto i = decltype(n){0}; i < n; ++i)
	{
		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii))
		{
			if (const auto* const p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
			{
				// load the old dwItemData value back to make mIRC happy
				this->deleteMenuItemData(p_Item, &mii);
				mii.fMask = MIIM_DATA;
				SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii);
			}

			if (mii.hSubMenu)
				this->deleteAllItemData(mii.hSubMenu);
		}
	}
}

/*!
 * \brief blah
 *
 * Only called for custom xpopup menus, NOT mIRC menus.
 */

LRESULT CALLBACK XPopupMenu::XPopupWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// for use when debugging.
	//const WindowMessages mm = static_cast<WindowMessages>(uMsg);

	switch (uMsg)
	{

	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, lpms);

		if (lpms->CtlType == ODT_MENU)
			return OnMeasureItem(mHwnd, lpms);
	}
	break;

	case WM_DRAWITEM:
	{
		dcxlParam(LPDRAWITEMSTRUCT, lpdis);

		if (lpdis->CtlType == ODT_MENU)
			return OnDrawItem(mHwnd, lpdis);
	}
	break;

	default:
		break;
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT XPopupMenu::OnMeasureItem(const HWND mHwnd, LPMEASUREITEMSTRUCT lpmis)
{
	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpmis->itemData); p_Item)
	{
		const auto size = p_Item->getItemSize(mHwnd);
		lpmis->itemWidth = gsl::narrow_cast<UINT>(size.cx);
		lpmis->itemHeight = gsl::narrow_cast<UINT>(size.cy);

		if (auto p_Menu = p_Item->getParentMenu(); p_Menu)
		{
			Dcx::m_CurrentMenuAlpha = p_Menu->IsAlpha();
			Dcx::m_CurrentMenuRounded = p_Menu->IsRoundedWindow();
		}
	}
	return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT XPopupMenu::OnDrawItem(const HWND mHwnd, LPDRAWITEMSTRUCT lpdis)
{
	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpdis->itemData); p_Item)
		p_Item->DrawItem(lpdis);

	return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::convertMenu(HMENU hMenu, const BOOL bForce)
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_FTYPE | MIIM_STRING;
	auto string = std::make_unique<TCHAR[]>(MIRC_BUFFER_SIZE_CCH);

	const auto n = GetMenuItemCount(hMenu);

	for (auto i = decltype(n){0}; i < n; ++i)
	{
		mii.dwTypeData = string.get();
		mii.cch = MIRC_BUFFER_SIZE_CCH;

		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii))
		{
			if (!dcx_testflag(mii.fType, MFT_OWNERDRAW) || bForce)
			{
				mii.fType |= MFT_OWNERDRAW;
				std::unique_ptr<XPopupMenuItem> p_Item;

				if (dcx_testflag(mii.fType, MFT_SEPARATOR))
					p_Item = std::make_unique<XPopupMenuItem>(this, true, mii.dwItemData);
				else {
					TString tsItem(string), tsTooltipText;
					int nIcon{ -1 };

					{
						// handle icons
						constexpr TCHAR sepChar = TEXT('\v');	// 11
						if (tsItem.numtok(sepChar) > 1)	// 11
						{
							nIcon = tsItem.getfirsttok(1, sepChar).to_int() - 1;	// tok 1, TEXT("\v")	get embeded icon number if any
							tsItem = tsItem.getlasttoks().trim();				// tok 2, TEXT("\v")	get real item text
						}
					}
					{
						// handles tooltips
						constexpr TCHAR sepChar = TEXT('\t');	// 9
						if (tsItem.numtok(sepChar) > 1)
						{
							const TString tsTmp(tsItem);
							tsItem = tsTmp.getfirsttok(1, sepChar).trim();	// tok 1, get real item text
							tsTooltipText = tsTmp.getlasttoks().trim();		// tok 2-, get tooltip text
						}
					}
					//check if the first char is $chr(12), if so then the text is utf8 (this is kept for compatability with old script only)
					if (tsItem[0] == 12)
					{
						// remove $chr(12) from text and trim whitespaces
						tsItem = tsItem.right(-1).trim();
					}

					// fixes identifiers in the dialog menu not being resolved. 
					// TODO Needs testing to see if it causes any other issues, like double eval's)

					if (bForce && this->getNameHash() == TEXT("dialog"_hash))
						mIRCLinker::eval(tsItem, tsItem); // we can use tsItem for both args as the second arg is copied & used before the first arg is set with the return value.

					p_Item = std::make_unique<XPopupMenuItem>(this, tsItem, tsTooltipText, nIcon, (mii.hSubMenu != nullptr), mii.dwItemData);

					//TString tsItem(string);
					//
					//// fixes identifiers in the dialog menu not being resolved. 
					//// TODO Needs testing to see if it causes any other issues, like double eval's)
					//
					//if (bForce && this->getNameHash() == TEXT("dialog"_hash))
					//	mIRCLinker::eval(tsItem, tsItem); // we can use tsItem for both args as the second arg is copied & used before the first arg is set with the return value.
					//
					//p_Item = std::make_unique<XPopupMenuItem>(this, tsItem, -1, (mii.hSubMenu != nullptr), mii.dwItemData);
				}

				mii.dwItemData = reinterpret_cast<ULONG_PTR>(p_Item.get());
				this->m_vpMenuItem.push_back(p_Item.release());
				SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii);
			}
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::cleanMenu(HMENU hMenu) noexcept
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_FTYPE;

	const auto n = GetMenuItemCount(hMenu);

	for (auto i = decltype(n){0}; i < n; ++i)
	{
		if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii))
		{
			if (mii.hSubMenu)
				XPopupMenu::cleanMenu(mii.hSubMenu);

			if (dcx_testflag(mii.fType, MFT_OWNERDRAW))
			{
				mii.fType &= ~MFT_OWNERDRAW;
				mii.dwItemData = 0;
				SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii);
			}
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenu::clearAllMenuItems() noexcept
{
	for (const auto& a : this->m_vpMenuItem)
		delete a;

	this->m_vpMenuItem.clear();
}

/*!
* \brief blah
*
* blah
*/

void XPopupMenu::setBackBitmap(HBITMAP hBitmap, const TString& tsFilename) noexcept
{
	if (this->m_hBitmap.m_hBitmap)
		DeleteObject(this->m_hBitmap.m_hBitmap);

	this->m_hBitmap.m_hBitmap = hBitmap;
	this->m_hBitmap.m_tsFilename = tsFilename;
}

/**
 * Attaches the XPopupMenu to the mIRC MenuBar.
 */
bool XPopupMenu::attachToMenuBar(HMENU menubar, const TString& label)
{
	// Already attached
	if (this->m_bAttachedToMenuBar)
		return false;

	// Add the menu to the mIRC window menubar
	this->m_bAttachedToMenuBar = Dcx::XMenubar.addToMenuBar(menubar, this, label);
	return this->m_bAttachedToMenuBar;
}

/**
 * Detaches the XPopupMenu from the mIRC MenuBar.
 */
void XPopupMenu::detachFromMenuBar(HMENU menubar) noexcept
{
	// Not attached, dont bother
	if (!this->m_bAttachedToMenuBar)
		return;

	Dcx::XMenubar.removeFromMenuBar(menubar, this);
	this->m_bAttachedToMenuBar = false;
}

bool XPopupMenu::getMenuInfo(const UINT iMask, const TString& path, MENUITEMINFO& mii) const
{
	HMENU hMenu = nullptr;
	const auto path_toks = path.numtok();

	if (path_toks == 1)
		hMenu = this->m_hMenu;
	else
		hMenu = this->parsePath(path.gettok(1, gsl::narrow_cast<int>(path_toks - 1)), this->m_hMenu);

	if (!hMenu)
		throw Dcx::dcxException("Unable to get menu");

	const auto nPos = path.gettok(gsl::narrow_cast<int>(path_toks)).to_int() - 1;

	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = iMask;

	return GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
}

const bool XPopupMenu::isItemValid(const XPopupMenuItem* const pItem) const noexcept
{
	if (!pItem)
		return false;

	for (const auto& a : m_vpMenuItem)
	{
		if (a == pItem)
			return true;
	}
	return false;
}

void XPopupMenu::toXml(const DcxDialog* d, TiXmlElement* const xml) const
{
	{
		TString tsStyle;
		switch (getStyle())
		{
		case XPopupMenu::MenuStyle::XPMS_OFFICE2003:
			tsStyle = L"office2003";
			break;
		case XPopupMenu::MenuStyle::XPMS_OFFICE2003_REV:
			tsStyle = L"office2003rev";
			break;
		case XPopupMenu::MenuStyle::XPMS_OFFICEXP:
			tsStyle = L"officexp";
			break;
		case XPopupMenu::MenuStyle::XPMS_ICY:
			tsStyle = L"icy";
			break;
		case XPopupMenu::MenuStyle::XPMS_ICY_REV:
			tsStyle = L"icyrev";
			break;
		case XPopupMenu::MenuStyle::XPMS_GRADE:
			tsStyle = L"grade";
			break;
		case XPopupMenu::MenuStyle::XPMS_GRADE_REV:
			tsStyle = L"graderev";
			break;
		case XPopupMenu::MenuStyle::XPMS_NORMAL:
			tsStyle = L"normal";
			break;
		case XPopupMenu::MenuStyle::XPMS_CUSTOM:
			tsStyle = L"custom";
			break;
		case XPopupMenu::MenuStyle::XPMS_VERTICAL:
			tsStyle = L"vertical";
			break;
		case XPopupMenu::MenuStyle::XPMS_VERTICAL_REV:
			tsStyle = L"verticalrev";
			break;
		case XPopupMenu::MenuStyle::XPMS_BUTTON:
			tsStyle = L"button";
			break;
		case XPopupMenu::MenuStyle::XPMS_BUTTON_REV:
			tsStyle = L"buttonrev";
			break;
		case XPopupMenu::MenuStyle::XPMS_CUSTOMBIG:
			tsStyle = L"custombig";
			break;
		default:
			break;
		}
		xml->SetAttribute("style", tsStyle.c_str());
	}

	if (auto tsMark(getMarkedText()); !tsMark.empty())
		xml->SetAttribute("mark", tsMark.c_str());

	if (!m_hBitmap.m_tsFilename.empty() && m_hBitmap.m_tsFilename != L"none")
	{
		TiXmlElement xImage("image");

		xImage.SetAttribute("eval", "0"); // diable eval
		xImage.SetAttribute("src", m_hBitmap.m_tsFilename.c_str());

		xml->InsertEndChild(xImage);
	}

	xmlSaveImageList(d, xml);

	xml->LinkEndChild(getColors()->toXml());

	if (this->IsRoundedSelector())
		xml->SetAttribute("roundedselector", "1");
	if (this->IsRoundedWindow())
		xml->SetAttribute("roundedwindow", "1");
	if (this->IsToolTipsEnabled())
		xml->SetAttribute("tooltips", "1");

	if (auto alpha = gsl::narrow_cast<int>(this->IsAlpha()); alpha < 255)
		xml->SetAttribute("alpha", alpha);
}

TiXmlElement* XPopupMenu::toXml(const DcxDialog* d) const
{
	auto xml = std::make_unique<TiXmlElement>("menu");
	toXml(d, xml.get());
	return xml.release();
}

void XPopupMenu::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis, const VectorOfIcons &vIcons)
{
	if (!xThis)
		return;

	if (const TString tsMark(queryAttribute(xThis, "mark")); !tsMark.empty())
		this->setMarkedText(tsMark);

	if (const auto tmp = queryIntAttribute(xThis, "roundedselector"); tmp)
		this->SetRoundedSelector(true);
	if (const auto tmp = queryIntAttribute(xThis, "roundedwindow"); tmp)
		this->SetRoundedWindow(true);
	if (const auto tmp = queryIntAttribute(xThis, "tooltips"); tmp)
		this->setTooltipsState(true);
	//if (const auto tmp = queryIntAttribute(xThis, "alpha"); tmp < 255)
	//	this->SetAlpha(Dcx::numeric_cast<std::byte>(tmp));
	if (const auto tmp = gsl::narrow_cast<BYTE>(queryIntAttribute(xThis, "alpha")); tmp < 255)
		this->SetAlpha(std::byte{ tmp });

	if (auto xml = xThis->FirstChildElement("image"); xml)
	{
		if (TString tsSrc(queryEvalAttribute(xml, "src")); !tsSrc.empty() && tsSrc != L"none")
		{
			if (const auto hBitmap = dcxLoadBitmap(nullptr, tsSrc); hBitmap)
				setBackBitmap(hBitmap, tsSrc);
		}
	}

	if (const TString tsStyle(queryAttribute(xThis, "style")); !tsStyle.empty())
		this->setStyle(this->parseStyle(tsStyle));

	if (auto xml = xThis->FirstChildElement("colours"); xml)
		m_MenuColors.fromXml(xml);

	if (auto himl = this->getImageList(); himl)
	{
		for (auto& a : vIcons)
		{
			if ((a.tsType != L"menu") || (a.tsID != L"dialog"))
				continue;

			TString tsSrc(queryEvalAttribute(a.xIcon, "src"));
			const TString tsIndex(queryAttribute(a.xIcon, "index", "0"));
			const TString tsFlags(queryAttribute(a.xIcon, "flags", "+"));
			Dcx::dcxLoadIconRange(himl, tsSrc, false, tsFlags, tsIndex);
		}
	}
}

void XPopupMenu::xmlSaveImageList(const DcxDialog* d, TiXmlElement* xml) const
{
	if (!m_hImageList || !xml || !d)
		return;

	if (const auto cnt = ImageList_GetImageCount(m_hImageList); cnt > 0)
	{
		//if (!xml->Attribute("iconsize"))
		//{
		//	if (int cx{}, cy{}; ImageList_GetIconSize(m_hImageList, &cx, &cy))
		//	{
		//		xml->SetAttribute("iconsize", cx);
		//	}
		//}

		xmlIcon xIcon;

		xIcon.tsType = L"menu";
		xIcon.tsID = this->getName();
		xIcon.tsFlags = L"+B";

		for (int i{}; i < cnt; ++i)
		{
			if (auto hIcon = ImageList_GetIcon(m_hImageList, i, ILD_TRANSPARENT); hIcon)
			{
				Auto(DestroyIcon(hIcon));

				xIcon.tsSrc = IconToBase64(hIcon);

				d->xmlGetIcons().emplace_back(xIcon);
			}
		}
	}
}

/*
 * Parses a string to a menu style.
 */
XPopupMenu::MenuStyle XPopupMenu::parseStyle(const TString& tsStyle) noexcept
{
	auto style = XPopupMenu::MenuStyle::XPMS_OFFICE2003;

	switch (std::hash<TString>{}(tsStyle))
	{
	case TEXT("office2003rev"_hash):
		style = MenuStyle::XPMS_OFFICE2003_REV;
		break;
	case TEXT("officexp"_hash):
		style = MenuStyle::XPMS_OFFICEXP;
		break;
	case TEXT("icy"_hash):
		style = MenuStyle::XPMS_ICY;
		break;
	case TEXT("icyrev"_hash):
		style = MenuStyle::XPMS_ICY_REV;
		break;
	case TEXT("grade"_hash):
		style = MenuStyle::XPMS_GRADE;
		break;
	case TEXT("graderev"_hash):
		style = MenuStyle::XPMS_GRADE_REV;
		break;
	case TEXT("vertical"_hash):
		style = MenuStyle::XPMS_VERTICAL;
		break;
	case TEXT("verticalrev"_hash):
		style = MenuStyle::XPMS_VERTICAL_REV;
		break;
	case TEXT("normal"_hash):
		style = MenuStyle::XPMS_NORMAL;
		break;
	case TEXT("custom"_hash):
		style = MenuStyle::XPMS_CUSTOM;
		break;
	case TEXT("button"_hash):
		style = MenuStyle::XPMS_BUTTON;
		break;
	case TEXT("buttonrev"_hash):
		style = MenuStyle::XPMS_BUTTON_REV;
		break;
	case TEXT("custombig"_hash):
		style = MenuStyle::XPMS_CUSTOMBIG;
		break;
	default:
		break;
	}
	return style;
}
