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

XPopupMenu::XPopupMenu(const TString& tsMenuName, MenuStyle mStyle)
	: XPopupMenu(tsMenuName, CreatePopupMenu())
{
	m_MenuStyle = mStyle;

	if (!XPopupMenuManager::m_vpAllMenus.contains(m_hMenu))
		XPopupMenuManager::m_vpAllMenus[m_hMenu] = this;
}

XPopupMenu::XPopupMenu(const TString& tsMenuName, MenuStyle mStyle, const TString& tsCallback)
	: XPopupMenu(tsMenuName, mStyle)
{
	m_tsCallback = tsCallback;
}

XPopupMenu::XPopupMenu(const TString& tsName, HMENU hMenu)
	: m_hMenu(hMenu), m_tsMenuName(tsName), m_menuNameHash(std::hash<TString>{}(tsName)), m_bDestroyHMENU(m_menuNameHash != TEXT("mircbar"_hash) && m_menuNameHash != TEXT("dialog"_hash) && m_menuNameHash != TEXT("scriptpopup"_hash))
{
	if (!XPopupMenuManager::m_vpAllMenus.contains(m_hMenu))
		XPopupMenuManager::m_vpAllMenus[m_hMenu] = this;
}

XPopupMenu::~XPopupMenu()
{
	if (this->m_bAttachedToMenuBar)
		this->detachFromMenuBar(nullptr);

	this->clearAllMenuItems();

	this->m_hBitmap.reset();

	//if (this->m_hBitmap)
	//	DeleteBitmap(this->m_hBitmap);

	XPopupMenuManager::m_vpAllMenus.erase(m_hMenu);

	//if (m_hMenu && m_menuNameHash != TEXT("mircbar"_hash) && m_menuNameHash != TEXT("dialog"_hash))
	//	DestroyMenu(this->m_hMenu);

	// Ook: possible fix for `Commands` menu failing after dcx is unloaded. (needs looked at more)
	//if (m_hMenu && m_menuNameHash != TEXT("mircbar"_hash) && m_menuNameHash != TEXT("dialog"_hash) && m_menuNameHash != TEXT("scriptpopup"_hash))
	//	DestroyMenu(this->m_hMenu);

	if (m_hMenu && IsHMENUDestrucible())
		DestroyMenu(this->m_hMenu);

	destroyImageList();
}

void XPopupMenu::parseXPopCommand(const TString& input)
{
	//const XSwitchFlags flags(input.gettok(2));
	//const auto path(input.getfirsttok(1, TSTABCHAR).gettok(3, -1).trim());	// tok 1
	//const auto path_toks = path.numtok();
	//
	//HMENU hMenu = nullptr;
	//
	// Item Positions
	//if (path_toks == 1)
	//	hMenu = m_hMenu;
	//else {
	//	hMenu = parsePath(path.gettok(1, gsl::narrow_cast<ptrdiff_t>(path_toks - 1)), m_hMenu);
	//
	//	if (!hMenu)
	//		throw DcxExceptions::dcxInvalidPath();
	//}
	//// this should never be null here.
	//if (!hMenu)
	//	throw Dcx::dcxException(TEXT("Menu is NULL."));
	//
	//const auto numtok = input.numtok();
	//const auto tabtoks = input.numtok(TSTABCHAR);
	//const auto tsTabTwo(input.getlasttoks().trim());	// tok 2, -1
	//const auto toks_in_tab_two = tsTabTwo.numtok();
	//auto nPos = path.gettok(gsl::narrow_cast<ptrdiff_t>(path_toks)).to_int() - 1;
	//
	//// xpop -a - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS] [ID] [ICON] ItemText (: Command) ([TAB] [TOOLTIP])
	//if (flags[TEXT('a')])
	//{
	//	if ((tabtoks < 2) || (toks_in_tab_two < 4))
	//		throw DcxExceptions::dcxInvalidArguments();
	//
	//	const XSwitchFlags xflags(tsTabTwo.getfirsttok(1));		// tok 1 [+FLAGS]
	//	const auto mID = tsTabTwo.getnexttok().to_<UINT>();		// tok 2 [ID]
	//	const auto nIcon = tsTabTwo.getnexttok().to_int() - 1;	// tok 3 [ICON]
	//
	//	TString tsItemText, tsTooltip;
	//	//TString itemcom;
	//
	//	{
	//		const auto itemtext(tsTabTwo.getlasttoks());			// tok 4, -1 ItemText....
	//
	//		if (tsTabTwo.numtok(TSTABCHAR) > 1)
	//		{
	//			tsItemText = itemtext.getfirsttok(1, TSTABCHAR).trim();
	//			tsTooltip = itemtext.getlasttoks().trim();
	//		}
	//		else
	//			tsItemText = itemtext;
	//
	//		// Ook: this never seems to be used....
	//		//if (tsItemText.numtok(TEXT(':')) > 1)
	//		//{
	//		//	const TString tsTmp(tsItemText.getfirsttok(1, TEXT(':')).trim());
	//		//	itemcom = tsItemText.getnexttok(TEXT(':')).trim();
	//		//	tsItemText = tsTmp;
	//		//}
	//	}
	//
	//	if (nPos == -1)
	//		nPos += GetMenuItemCount(hMenu) + 1;
	//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//
	//	std::unique_ptr<XPopupMenuItem> p_Item;
	//
	//	if (tsItemText == TEXT('-'))
	//	{
	//		mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
	//		mii.fType = MFT_OWNERDRAW | MFT_SEPARATOR;
	//
	//		p_Item = std::make_unique<XPopupMenuItem>(this, true);
	//	}
	//	else {
	//		mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
	//		mii.fType = MFT_OWNERDRAW;
	//		mii.wID = mID;
	//
	//		if (!xflags[TEXT('+')])
	//			throw DcxExceptions::dcxInvalidFlag();
	//
	//		// submenu
	//		if (xflags[TEXT('s')])
	//		{
	//			mii.fMask |= MIIM_SUBMENU;
	//			if (mii.hSubMenu)
	//				DestroyMenu(mii.hSubMenu);
	//
	//			mii.hSubMenu = CreatePopupMenu();
	//
	//			if (!mii.hSubMenu)
	//				throw Dcx::dcxException(TEXT("Failed to create submenu."));
	//		}
	//		if (xflags[TEXT('c')])
	//			mii.fState |= MFS_CHECKED;
	//		if (xflags[TEXT('g')])
	//			mii.fState |= MFS_GRAYED;
	//
	//		p_Item = std::make_unique<XPopupMenuItem>(this, tsItemText, tsTooltip, nIcon, (mii.hSubMenu != nullptr));
	//
	//		if (xflags[TEXT('r')])
	//		{
	//			mii.fType |= MFT_RADIOCHECK;
	//			p_Item->setRadioCheck(true);
	//		}
	//
	//		p_Item->setCheckToggle(xflags[TEXT('C')]);
	//	}
	//
	//	mii.dwItemData = reinterpret_cast<ULONG_PTR>(p_Item.get());
	//	this->m_vpMenuItem.push_back(p_Item.release());
	//	InsertMenuItem(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	//}
	//// xpop -c - [MENU] [SWITCH] [PATH]
	//else if (flags[TEXT('c')])
	//{
	//	if (numtok < 3)
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_SUBMENU | MIIM_DATA;
//
	//	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
	//		throw Dcx::dcxException("Unable to get menu item info");
//
	//	if (mii.hSubMenu)
	//	{
	//		this->deleteAllItemData(mii.hSubMenu);
	//		DestroyMenu(mii.hSubMenu);
	//	}
//
	//	mii.hSubMenu = CreatePopupMenu();
//
	//	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
	//		p_Item->setSubMenu(TRUE);
//
	//	mii.fMask = MIIM_SUBMENU;
//
	//	SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	//}
	//// xpop -d - [MENU] [SWITCH] [PATH]
	//else if (flags[TEXT('d')])
	//{
	//	if (numtok < 3)
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_SUBMENU | MIIM_DATA;
//
	//	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
	//		throw Dcx::dcxException("Unable to get menu item info");
//
	//	if (mii.hSubMenu)
	//	{
	//		this->deleteAllItemData(mii.hSubMenu);
	//		DestroyMenu(mii.hSubMenu);
	//	}
//
	//	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
	//		p_Item->setSubMenu(FALSE);
//
	//	mii.hSubMenu = nullptr;
	//	mii.fMask = MIIM_SUBMENU;
//
	//	SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	//}
	//// xpop -f - [MENU] [SWITCH] [PATH]
	//else if (flags[TEXT('f')])
	//{
	//	if (numtok < 3)
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_SUBMENU | MIIM_DATA;
//
	//	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
	//		throw Dcx::dcxException("Unable to get menu item info");
//
	//	if (mii.hSubMenu)
	//	{
	//		this->deleteAllItemData(mii.hSubMenu);
	//		DestroyMenu(mii.hSubMenu);
	//	}
//
	//	if (const auto* const p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
	//		this->deleteMenuItemData(p_Item, nullptr);
//
	//	DeleteMenu(hMenu, gsl::narrow_cast<UINT>(nPos), MF_BYPOSITION);
	//}
	//// xpop -i - [MENU] [SWITCH] [PATH] [TAB] [ICON]
	//else if (flags[TEXT('i')])
	//{
	//	if ((tabtoks < 2) || (toks_in_tab_two < 1))
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	const auto nIcon = tsTabTwo.to_int();
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_DATA;
//
	//	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
	//		throw Dcx::dcxException("Unable to get menu item info");
//
	//	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
	//		p_Item->setItemIcon(nIcon);
	//}
	//// xpop -s - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS]
	//// xpop -s - [MENU] [SWITCH] [PATH] [TAB] +r [FIRST] [LAST]
	//else if (flags[TEXT('s')])
	//{
	//	if ((tabtoks < 2) || (toks_in_tab_two < 1))
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	const XSwitchFlags xflags(tsTabTwo.getfirsttok(1));
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	const auto p_Item = getMenuItem(hMenu, nPos);
	//	if (!p_Item)
	//		throw DcxExceptions::dcxInvalidItem();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_STATE;
//
	//	// Flags may be empty, so no error if + is misssing.
	//	if (xflags[TEXT('+')])
	//	{
	//		if (xflags[TEXT('c')])
	//			mii.fState |= MFS_CHECKED;
	//		if (xflags[TEXT('g')])
	//			mii.fState |= MFS_GRAYED;
//
	//		p_Item->setCheckToggle(xflags[TEXT('C')]);
//
	//		if (xflags[TEXT('r')])
	//		{
	//			const auto nFirst = tsTabTwo.getnexttok().to_<UINT>() - 1;
	//			const auto nLast = tsTabTwo.getnexttok().to_<UINT>() - 1;
	//			CheckMenuRadioItem(hMenu, nFirst, nLast, nPos, MF_BYPOSITION);
	//		}
	//		if (xflags[TEXT('R')])
	//			Dcx::XPopups.RedrawMenuIfOpen();
	//	}
	//	else
	//		p_Item->setCheckToggle(false);
//
	//	SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	//}
	//// xpop -t - [MENU] [SWITCH] [PATH] [TAB] Itemtext
	//else if (flags[TEXT('t')])
	//{
	//	if ((tabtoks < 2) || (toks_in_tab_two < 1))
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_DATA | MIIM_STATE | MIIM_SUBMENU | MIIM_FTYPE | MIIM_ID;
//
	//	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
	//		throw Dcx::dcxException("Unable to get menu item info");
//
	//	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
	//		p_Item->setItemText(tsTabTwo);
//
	//	// this is to make sure system resets the measurement of the itemwidth on next display
	//	DeleteMenu(hMenu, gsl::narrow_cast<UINT>(nPos), MF_BYPOSITION);
	//	InsertMenuItem(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
	//}
	//// xpop -T - [MENU] [SWITCH] [PATH] [TAB] (text)
	//else if (flags[TEXT('T')])
	//{
	//	if (tabtoks < 2)
	//		throw DcxExceptions::dcxInvalidArguments();
//
	//	if (nPos < 0)
	//		throw DcxExceptions::dcxInvalidPath();
//
	//	MENUITEMINFO mii{};
	//	mii.cbSize = sizeof(MENUITEMINFO);
	//	mii.fMask = MIIM_DATA;
//
	//	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
	//		throw Dcx::dcxException("Unable to get menu item info");
//
	//	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
	//		p_Item->setItemTooltip(tsTabTwo);
	//}

	// new version
	// all commands have the general syntax
	// [MENU] [SWITCH] [PATH] [TAB] [DATA]
	// [PATH] could be:
	//	1: a normal path - 3 4 3
	//		same as the old version, menu is gotten by parsePath()
	//	2: a command id - :455
	//		menu is gotten bu CommandIDToPath()
	//		path var is adjusted to be normal 3 4 3 style
	//	3: a group id - =4
	//		menu can have many diff values depending on the grouped items
	//		groups are created by /xpopup -g command.
	//	4: a selection of these - 3 4 3,:455,=4
	//

	const auto fullpath(input.getfirsttok(1, TSTABCHAR).gettok(3, -1).trim());	// tok 1

	for (TString path(fullpath.getfirsttok(1, TSCOMMACHAR)); !path.empty(); path = fullpath.getnexttok(TSCOMMACHAR))
	{
		auto path_toks = path.numtok();
		HMENU hMenu = nullptr;

		switch (path[0])
		{
		case L':':
		{
			// CommandID
			const auto mID = path.right(-1).to_<UINT>();
			path.clear();
			hMenu = CommandIDToPath(mID, path);
			path_toks = path.numtok();
		}
		break;

		case L'=':
		{
			// GID
			const auto gid = path.right(-1).to_<UINT>();
			for (const auto& a : getGroups())
			{
				if (gid == a.m_ID)
				{
					// found matching group
					// proccess all id's in group.
					for (const auto& id : a.m_GroupIDs)
					{
						TString tsNewInput;
						_ts_sprintf(tsNewInput, L"% :%\t%", input.gettok(1, 2), id, input.getlasttoks());
						parseXPopCommand(tsNewInput);
					}
					return;
				}
			}
			throw DcxExceptions::dcxInvalidPath();
		}
		break;

		default:
		{
			// Item Positions
			if (path_toks == 1)
				hMenu = m_hMenu;
			else {
				hMenu = parsePath(path.gettok(1, gsl::narrow_cast<ptrdiff_t>(path_toks - 1)), m_hMenu);

				if (!hMenu)
					throw DcxExceptions::dcxInvalidPath();
			}
		}
		break;
		}

		// this should never be null here.
		if (!hMenu)
			throw Dcx::dcxException(TEXT("Menu is NULL."));

		const XSwitchFlags flags(input.gettok(2));
		const auto numtok = input.numtok();
		const auto tabtoks = input.numtok(TSTABCHAR);
		const auto tsTabTwo(input.getlasttoks().trim());	// tok 2, -1
		const auto toks_in_tab_two = tsTabTwo.numtok();
		const auto nPos = path.gettok(gsl::narrow_cast<ptrdiff_t>(path_toks)).to_int() - 1;

		// xpop -a - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS] [ID] [ICON] ItemText (: Command) ([TAB] [TOOLTIP])
		if (flags[TEXT('a')])
		{
			// [PATH] cant be GID for this command.
			if ((tabtoks < 2) || (toks_in_tab_two < 4))
				throw DcxExceptions::dcxInvalidArguments();

			xpop_a(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -c - [MENU] [SWITCH] [PATH]
		else if (flags[TEXT('c')])
		{
			if (numtok < 3)
				throw DcxExceptions::dcxInvalidArguments();

			xpop_c(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -d - [MENU] [SWITCH] [PATH]
		else if (flags[TEXT('d')])
		{
			if (numtok < 3)
				throw DcxExceptions::dcxInvalidArguments();

			xpop_d(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -f - [MENU] [SWITCH] [PATH]
		else if (flags[TEXT('f')])
		{
			if (numtok < 3)
				throw DcxExceptions::dcxInvalidArguments();

			xpop_f(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -i - [MENU] [SWITCH] [PATH] [TAB] [ICON]
		else if (flags[TEXT('i')])
		{
			if ((tabtoks < 2) || (toks_in_tab_two < 1))
				throw DcxExceptions::dcxInvalidArguments();

			xpop_i(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -s - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS]
		// xpop -s - [MENU] [SWITCH] [PATH] [TAB] +r [FIRST] [LAST]
		else if (flags[TEXT('s')])
		{
			if ((tabtoks < 2) || (toks_in_tab_two < 1))
				throw DcxExceptions::dcxInvalidArguments();

			xpop_s(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -t - [MENU] [SWITCH] [PATH] [TAB] Itemtext
		else if (flags[TEXT('t')])
		{
			if ((tabtoks < 2) || (toks_in_tab_two < 1))
				throw DcxExceptions::dcxInvalidArguments();

			xpop_t(hMenu, nPos, path, tsTabTwo);
		}
		// xpop -T - [MENU] [SWITCH] [PATH] [TAB] (text)
		else if (flags[TEXT('T')])
		{
			if (tabtoks < 2)
				throw DcxExceptions::dcxInvalidArguments();

			xpop_T(hMenu, nPos, path, tsTabTwo);
		}
	}
}

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

HIMAGELIST& XPopupMenu::getImageList() noexcept
{
	if (!m_hImageList)
		m_hImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 0);

	return m_hImageList;
}

void XPopupMenu::destroyImageList() noexcept
{
	if (this->m_hImageList)
		ImageList_Destroy(this->m_hImageList);

	this->m_hImageList = nullptr;
}

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
		this->m_MenuColors.m_clrCheckBox.m_clrBackground = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_FRAME:
		this->m_MenuColors.m_clrCheckBox.m_clrFrame = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_TICK:
		this->m_MenuColors.m_clrCheckBox.m_clrTick = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_DISABLED:
		this->m_MenuColors.m_clrCheckBox.m_clrDisabledBackground = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_FRAME_DISABLED:
		this->m_MenuColors.m_clrCheckBox.m_clrDisabledFrame = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_TICK_DISABLED:
		this->m_MenuColors.m_clrCheckBox.m_clrDisabledTick = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_HOT:
		this->m_MenuColors.m_clrCheckBox.m_clrHotBackground = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_FRAME_HOT:
		this->m_MenuColors.m_clrCheckBox.m_clrHotFrame = clrColor;
		break;

	case MenuColours::XPMC_CHECKBOX_TICK_HOT:
		this->m_MenuColors.m_clrCheckBox.m_clrHotTick = clrColor;
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

	case MenuColours::XPMC_VSEPARATOR:
		this->m_MenuColors.m_clrVerticalSeparatorLine = clrColor;
		break;

	case MenuColours::XPMC_TEXT:
		this->m_MenuColors.m_clrText = clrColor;
		break;

	case MenuColours::XPMC_SELECTEDTEXT:
		this->m_MenuColors.m_clrSelectedText = clrColor;
		break;

	case MenuColours::XPMC_BORDER:
		this->m_MenuColors.m_clrBorder = clrColor;
		break;

	case MenuColours::XPMC_MAX:
	default:
		break;
	}
}

COLORREF XPopupMenu::getColor(const MenuColours nColor) const noexcept
{
	switch (nColor)
	{
	case MenuColours::XPMC_BACKGROUND:
		return this->m_MenuColors.m_clrBack;

	case MenuColours::XPMC_ICONBOX:
		return this->m_MenuColors.m_clrBox;

	case MenuColours::XPMC_CHECKBOX:
		return this->m_MenuColors.m_clrCheckBox.m_clrBackground;

	case MenuColours::XPMC_CHECKBOX_FRAME:
		return this->m_MenuColors.m_clrCheckBox.m_clrFrame;

	case MenuColours::XPMC_CHECKBOX_TICK:
		return this->m_MenuColors.m_clrCheckBox.m_clrTick;

	case MenuColours::XPMC_CHECKBOX_DISABLED:
		return this->m_MenuColors.m_clrCheckBox.m_clrDisabledBackground;

	case MenuColours::XPMC_CHECKBOX_FRAME_DISABLED:
		return this->m_MenuColors.m_clrCheckBox.m_clrFrame;

	case MenuColours::XPMC_CHECKBOX_TICK_DISABLED:
		return this->m_MenuColors.m_clrCheckBox.m_clrTick;

	case MenuColours::XPMC_CHECKBOX_HOT:
		return this->m_MenuColors.m_clrCheckBox.m_clrHotBackground;

	case MenuColours::XPMC_CHECKBOX_FRAME_HOT:
		return this->m_MenuColors.m_clrCheckBox.m_clrFrame;

	case MenuColours::XPMC_CHECKBOX_TICK_HOT:
		return this->m_MenuColors.m_clrCheckBox.m_clrTick;

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

	case MenuColours::XPMC_VSEPARATOR:
		return this->m_MenuColors.m_clrVerticalSeparatorLine;

	case MenuColours::XPMC_TEXT:
		return this->m_MenuColors.m_clrText;

	case MenuColours::XPMC_SELECTEDTEXT:
		return this->m_MenuColors.m_clrSelectedText;

	case MenuColours::XPMC_BORDER:
		return this->m_MenuColors.m_clrBorder;

	case MenuColours::XPMC_MAX:
	default:
		return 0;
	}
}

GSL_SUPPRESS(type.4)
void XPopupMenu::setDefaultColor(_In_ const MenuColours nColor) noexcept
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
		this->m_MenuColors.m_clrCheckBox.m_clrBackground = RGB(255, 128, 0);
		break;

	case MenuColours::XPMC_CHECKBOX_FRAME:
		this->m_MenuColors.m_clrCheckBox.m_clrFrame = RGB(0, 0, 0);
		break;

	case MenuColours::XPMC_CHECKBOX_TICK:
		this->m_MenuColors.m_clrCheckBox.m_clrTick = RGB(0, 0, 0);
		break;

	case MenuColours::XPMC_CHECKBOX_DISABLED:
		this->m_MenuColors.m_clrCheckBox.m_clrDisabledBackground = RGB(200, 200, 200);
		break;

	case MenuColours::XPMC_CHECKBOX_FRAME_DISABLED:
		this->m_MenuColors.m_clrCheckBox.m_clrDisabledBackground = RGB(200, 200, 200);
		break;

	case MenuColours::XPMC_CHECKBOX_TICK_DISABLED:
		this->m_MenuColors.m_clrCheckBox.m_clrDisabledBackground = RGB(128, 128, 128);
		break;

	case MenuColours::XPMC_CHECKBOX_HOT:
		this->m_MenuColors.m_clrCheckBox.m_clrHotBackground = RGB(255, 128, 123);
		break;

	case MenuColours::XPMC_CHECKBOX_FRAME_HOT:
		this->m_MenuColors.m_clrCheckBox.m_clrHotFrame = RGB(0, 0, 255);
		break;

	case MenuColours::XPMC_CHECKBOX_TICK_HOT:
		this->m_MenuColors.m_clrCheckBox.m_clrHotTick = RGB(0, 0, 255);
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

	case MenuColours::XPMC_VSEPARATOR:
		this->m_MenuColors.m_clrVerticalSeparatorLine = RGB(128, 128, 128);
		break;

	case MenuColours::XPMC_TEXT:
		this->m_MenuColors.m_clrText = RGB(0, 0, 0);
		break;

	case MenuColours::XPMC_SELECTEDTEXT:
		this->m_MenuColors.m_clrSelectedText = RGB(0, 0, 0);
		break;

	case MenuColours::XPMC_BORDER:
		this->m_MenuColors.m_clrBorder = CLR_INVALID;
		//this->m_MenuColors.m_clrBorder = this->m_MenuColors.m_clrBack;
		break;

	case MenuColours::XPMC_MAX:
	default:
		break;
	}
}

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

void XPopupMenu::setItemStyleString(const TString& tsFlags)
{
	const XSwitchFlags xflags(tsFlags);

	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	UINT iStyles = 0;
	if (xflags[TEXT('i')])
		iStyles |= XPS_ICON3D;
	if (xflags[TEXT('d')])
		iStyles |= XPS_DISABLEDSEL;
	if (xflags[TEXT('p')])
		iStyles |= XPS_ICON3DSHADOW;
	if (xflags[TEXT('D')])
		iStyles |= XPS_DOUBLESEP;
	if (xflags[TEXT('v')])
		iStyles |= XPS_VERTICALSEP;

	setItemStyle(iStyles);
}

TString XPopupMenu::getItemStyleString() const
{
	TString szStyles(TEXT('+'));
	const UINT iExStyles = getItemStyle();

	if (dcx_testflag(iExStyles, XPS_ICON3D))
		szStyles += TEXT('i');
	if (dcx_testflag(iExStyles, XPS_DISABLEDSEL))
		szStyles += TEXT('d');
	if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
		szStyles += TEXT('p');
	if (dcx_testflag(iExStyles, XPS_DOUBLESEP))
		szStyles += TEXT('D');
	if (dcx_testflag(iExStyles, XPS_VERTICALSEP))
		szStyles += TEXT('v');

	return szStyles;
}

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

void XPopupMenu::deleteAllItemData(HMENU hMenu) noexcept
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
	case WM_INITMENU:
	{
		Dcx::XPopups.setOwnerWindow(mHwnd);
	}
	break;

	case WM_MENUCOMMAND:
	{
		auto hMenu = reinterpret_cast<HMENU>(lParam);
		if (!hMenu)
			break;

		auto xItem = Dcx::XPopups.getMenuItemByID(hMenu, gsl::narrow_cast<int>(wParam));
		if (!xItem)
			break;

		// xMenu should never be null
		if (auto xMenu = xItem->getParentMenu(); xMenu)
			mIRCLinker::exec(TEXT("//.signal -n XPopup-% %"), xMenu->getName(), xItem->getCommandID());
	}
	break;

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

LRESULT XPopupMenu::OnDrawItem(const HWND mHwnd, LPDRAWITEMSTRUCT lpdis)
{
	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(lpdis->itemData); p_Item)
		p_Item->DrawItem(lpdis);

	return TRUE;
}

void XPopupMenu::convertMenu(HMENU hMenu, const BOOL bForce)
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA | MIIM_FTYPE | MIIM_STRING | MIIM_ID;
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
					TString tsItem(string);

					//TString tsItem(string), tsTooltipText, tsStyle;
					//int nIcon{ -1 };
					//bool bCheckToggle{}, bRadioCheck{};
					//UINT nSpecialID{};

					// fixes identifiers in the dialog menu not being resolved. 
					// do this BEFORE checking for special chars as these are often $chr()'s
					// TODO Needs testing to see if it causes any other issues, like double eval's)

					if (bForce && this->getNameHash() == TEXT("dialog"_hash))
						mIRCLinker::eval(tsItem, tsItem); // we can use tsItem for both args as the second arg is copied & used before the first arg is set with the return value.

					//{
					//	// handle icons
					//	if (constexpr TCHAR sepChar = TEXT('\v'); tsItem.numtok(sepChar) > 1)	// 11
					//	{
					//		nIcon = tsItem.getfirsttok(1, sepChar).to_int() - 1;	// tok 1, TEXT('\v')	get embeded icon number if any
					//
					//		if (tsItem.numtok(sepChar) > 2)	// 11
					//		{
					//			// second \v token taken to be style info for item. Overrides the menus style.
					//			tsStyle = tsItem.getnexttok(sepChar).trim();
					//		}
					//		tsItem = tsItem.getlasttoks().trim();				// tok last, TEXT('\v')	get real item text
					//	}
					//}
					//{
					//	// handles tooltips
					//	if (constexpr TCHAR sepChar = TEXT('\t'); tsItem.numtok(sepChar) > 1)
					//	{
					//		const TString tsTmp(tsItem);
					//		tsItem = tsTmp.getfirsttok(1, sepChar).trim();	// tok 1, get real item text
					//		tsTooltipText = tsTmp.getlasttoks().trim();		// tok 2-, get tooltip text
					//	}
					//}
					////check if the first char is $chr(12), if so then the text is utf8 (this is kept for compatability with old script only)
					//if (tsItem[0] == 12)
					//{
					//	// remove $chr(12) from text and trim whitespaces
					//	tsItem = tsItem.right(-1).trim();
					//	bRadioCheck = true;
					//}
					//if (const auto nPos = tsItem.find(L'\x0e', 1); nPos != -1)	// $chr(14)
					//{
					//	ptrdiff_t nEnd{ nPos + 1 };
					//	while (tsItem[nEnd] >= L'0' && tsItem[nEnd] <= L'9')
					//	{
					//		++nEnd;
					//	}
					//	if ((nEnd - nPos) > 1)
					//	{
					//		auto tsID(tsItem.sub(nPos, nEnd));
					//		tsItem.remove(tsID.to_chr());
					//		tsID.remove(L'\x0e');
					//		if (!tsID.empty())
					//			nSpecialID = tsID.to_<UINT>();
					//	}
					//	else
					//		tsItem.remove(L'\x0e');
					//	tsItem.trim();
					//	bCheckToggle = true;
					//}
					//
					//p_Item = std::make_unique<XPopupMenuItem>(this, tsItem, tsTooltipText, nIcon, (mii.hSubMenu != nullptr), mii.dwItemData);
					//
					//if (p_Item)
					//{
					//	if (!tsStyle.empty())
					//		p_Item->setOverrideStyle(gsl::narrow_cast<UINT>(parseStyle(tsStyle)));
					//	p_Item->setCheckToggle(bCheckToggle);
					//	if (nSpecialID)
					//		p_Item->setCommandID(nSpecialID);
					//	else
					//		p_Item->setCommandID(mii.wID);
					//	p_Item->setRadioCheck(bRadioCheck);
					//}

					p_Item = std::make_unique<XPopupMenuItem>(this, tsItem, (mii.hSubMenu != nullptr), mii.dwItemData);

					// If command id wasnt extracted from item text, set it here.
					if (p_Item->getCommandID() == 0)
						p_Item->setCommandID(mii.wID);
				}

				const auto lpItem = p_Item.release();
				mii.dwItemData = reinterpret_cast<ULONG_PTR>(lpItem);
				this->m_vpMenuItem.push_back(lpItem);

				//mii.dwItemData = reinterpret_cast<ULONG_PTR>(p_Item.get());
				//this->m_vpMenuItem.push_back(p_Item.release());
				SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(i), TRUE, &mii);
			}
		}
	}
}

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

void XPopupMenu::clearAllMenuItems() noexcept
{
	for (const auto& a : this->m_vpMenuItem)
		delete a;

	this->m_vpMenuItem.clear();
}

void XPopupMenu::setBackBitmap(HBITMAP hBitmap, const TString& tsFilename) noexcept
{
	this->m_hBitmap.reset();

	this->m_hBitmap.m_hBitmap = hBitmap;
	this->m_hBitmap.m_tsFilename = tsFilename;
}

bool XPopupMenu::attachToMenuBar(HMENU menubar, const TString& label)
{
	// Already attached
	if (this->m_bAttachedToMenuBar)
		return false;

	// Add the menu to the mIRC window menubar
	this->m_bAttachedToMenuBar = Dcx::XMenubar.addToMenuBar(menubar, this, label);
	return this->m_bAttachedToMenuBar;
}

void XPopupMenu::detachFromMenuBar(HMENU menubar) noexcept
{
	// Not attached, dont bother
	if (!this->m_bAttachedToMenuBar)
		return;

	Dcx::XMenubar.removeFromMenuBar(menubar, this);
	this->m_bAttachedToMenuBar = false;
}

bool XPopupMenu::getMenuInfo(_In_ const UINT iMask, _In_ const TString& path, _In_ MENUITEMINFO& mii) const
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

XPopupMenuItem* XPopupMenu::getMenuItem(_In_ HMENU hMenu, _In_ int nPos) const
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;

	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
		throw Dcx::dcxException("Unable to get menu item info");

	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		return p_Item;

	return nullptr;
}

XPopupMenuItem* XPopupMenu::getMenuItem(_In_ const TString& path) const
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);

	if (getMenuInfo(MIIM_DATA, path, mii))
		return reinterpret_cast<XPopupMenuItem*>(mii.dwItemData);

	return nullptr;
}

XPopupMenuItem* XPopupMenu::getMenuItem(_In_ UINT mID) const noexcept
{
	for (auto xItem : m_vpMenuItem)
	{
		if (xItem && (mID == xItem->getCommandID()))
			return xItem;
	}
	return nullptr;
}


/// <summary>
/// Get an item group from a group id.
/// </summary>
/// <param name="nID"></param>
/// <returns>The item group requested or an empty group.</returns>

const DcxMenuItemGroup& XPopupMenu::getGroup(UINT nID) const noexcept
{
	const static DcxMenuItemGroup gEmpty;
	const auto& grps = getGroups();
	for (const auto& a : grps)
	{
		if (nID == a.m_ID)
			return a;
	}
	return gEmpty;
}

HMENU XPopupMenu::CommandIDToPath(_In_ UINT mID, _Out_ TString& tsPath, _In_opt_ HMENU hMenu) const
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_ID | MIIM_SUBMENU;

	if (!hMenu)
		hMenu = m_hMenu;

	const auto nCnt = GetMenuItemCount(hMenu);
	for (int i{}; i < nCnt; ++i)
	{
		GetMenuItemInfo(hMenu, i, TRUE, &mii);
		if (mii.wID == mID)
		{
			tsPath.addtok(++i);
			return hMenu;
		}
		if (mii.hSubMenu)
		{
			TString tsRes;
			if (auto hFound = CommandIDToPath(mID, tsRes, mii.hSubMenu); hFound)
			{
				tsPath.clear();
				tsPath.addtok(i + 1);
				tsPath.addtok(tsRes);
				return hFound;
			}
		}
	}
	return nullptr;
}

void XPopupMenu::setItemCheckToggle(UINT nPos, bool bEnable)
{
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;

	if (GetMenuItemInfo(this->getMenuHandle(), nPos, TRUE, &mii) == FALSE)
		throw Dcx::dcxException("Unable to get menu item info");

	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		p_Item->setCheckToggle(bEnable);
}

const bool XPopupMenu::isItemValid(const XPopupMenuItem* const pItem) const noexcept
{
	if (!pItem)
		return false;

	for (const XPopupMenuItem* const a : m_vpMenuItem)
	{
		if ((a) && (a == pItem))
			return true;
	}
	return false;
}

void XPopupMenu::toXml(VectorOfIcons& vIcons, TiXmlElement* const xml) const
{
	if (!xml)
		return;

	{
		switch (getStyle())
		{
		case XPopupMenu::MenuStyle::XPMS_OFFICE2003:
			xml->SetAttribute("style", "office2003");
			break;
		case XPopupMenu::MenuStyle::XPMS_OFFICE2003_REV:
			xml->SetAttribute("style", "office2003rev");
			break;
		case XPopupMenu::MenuStyle::XPMS_OFFICEXP:
			xml->SetAttribute("style", "officexp");
			break;
		case XPopupMenu::MenuStyle::XPMS_ICY:
			xml->SetAttribute("style", "icy");
			break;
		case XPopupMenu::MenuStyle::XPMS_ICY_REV:
			xml->SetAttribute("style", "icyrev");
			break;
		case XPopupMenu::MenuStyle::XPMS_GRADE:
			xml->SetAttribute("style", "grade");
			break;
		case XPopupMenu::MenuStyle::XPMS_GRADE_REV:
			xml->SetAttribute("style", "graderev");
			break;
		case XPopupMenu::MenuStyle::XPMS_CUSTOM:
			xml->SetAttribute("style", "custom");
			break;
		case XPopupMenu::MenuStyle::XPMS_VERTICAL:
			xml->SetAttribute("style", "vertical");
			break;
		case XPopupMenu::MenuStyle::XPMS_VERTICAL_REV:
			xml->SetAttribute("style", "verticalrev");
			break;
		case XPopupMenu::MenuStyle::XPMS_BUTTON:
			xml->SetAttribute("style", "button");
			break;
		case XPopupMenu::MenuStyle::XPMS_BUTTON_REV:
			xml->SetAttribute("style", "buttonrev");
			break;
		case XPopupMenu::MenuStyle::XPMS_CUSTOMBIG:
			xml->SetAttribute("style", "custombig");
			break;
		default:
		case XPopupMenu::MenuStyle::XPMS_NORMAL:
			xml->SetAttribute("style", "normal");
			break;
		}
	}

	if (auto tsMark(getMarkedText()); !tsMark.empty())
		xml->SetAttribute("mark", tsMark.c_str());

	if (!m_hBitmap.m_tsFilename.empty() && m_hBitmap.m_tsFilename != L"none")
		xml->LinkEndChild(m_hBitmap.toXml());

	xmlSaveImageList(vIcons, xml);

	xml->LinkEndChild(getColors().toXml());

	if (this->IsRoundedSelector())
		xml->SetAttribute("roundedselector", "1");
	if (this->IsRoundedWindow())
		xml->SetAttribute("roundedwindow", "1");
	if (this->IsToolTipsEnabled())
		xml->SetAttribute("tooltips", "1");

	if (auto alpha = gsl::narrow_cast<int>(this->IsAlpha()); alpha < 255)
		xml->SetAttribute("alpha", alpha);

	if (auto tsCallback(getCallback()); !tsCallback.empty())
		xml->SetAttribute("callback", tsCallback.c_str());

	{
		const TString tsStyles(getItemStyleString());
		xml->SetAttribute("itemstyles", tsStyles.c_str());
	}

	// TODO: save menu items...
	//for (const auto& item : this->m_vpMenuItem)
	//{
	//	xml->LinkEndChild(item->toXml(vIcons));
	//}
}

TiXmlElement* XPopupMenu::toXml(VectorOfIcons& vIcons) const
{
	auto xml = std::make_unique<TiXmlElement>("menu");
	toXml(vIcons, xml.get());
	return xml.release();
}

void XPopupMenu::fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis, const VectorOfIcons& vIcons)
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
	if (const auto tmp = gsl::narrow_cast<BYTE>(queryIntAttribute(xThis, "alpha")); tmp < 255)
		this->SetAlpha(std::byte{ tmp });

	if (const TString tsCallback(queryAttribute(xThis, "callback")); !tsCallback.empty())
		this->setCallback(tsCallback);

	if (auto xml = xThis->FirstChildElement("image"); xml)
		m_hBitmap.fromXml(xml);

	if (const TString tsStyle(queryAttribute(xThis, "style")); !tsStyle.empty())
		this->setStyle(this->parseStyle(tsStyle));

	if (const TString tsStyle(queryAttribute(xThis, "itemstyles")); !tsStyle.empty())
		this->setItemStyleString(tsStyle);

	if (auto xml = xThis->FirstChildElement("colours"); xml)
		m_MenuColors.fromXml(xml);

	if (auto himl = this->getImageList(); himl)
	{
		for (auto& a : vIcons)
		{
			if ((a.tsType != L"menu") || (a.tsID != L"0"))
				continue;

			TString tsSrc(queryEvalAttribute(a.xIcon, "src"));
			const TString tsIndex(queryAttribute(a.xIcon, "index", "0"));
			const TString tsFlags(queryAttribute(a.xIcon, "flags", "+"));
			Dcx::dcxLoadIconRange(himl, tsSrc, false, tsFlags, tsIndex);
		}
	}

	// now check for items...
	if (!Dcx::XPopups.LoadPopupItemsFromXML(this, this->getMenuHandle(), xThis))
		throw Dcx::dcxException(TEXT("Unable to load menu items: %"), this->getName());
}

void XPopupMenu::xmlSaveImageList(VectorOfIcons& vIcons, TiXmlElement* xml) const
{
	if (!m_hImageList || !xml)
		return;

	if (const auto cnt = ImageList_GetImageCount(m_hImageList); cnt > 0)
	{
		//if (!xml->Attribute("iconsize"))
		//{
		//	if (int cx{}, cy{}; ImageList_GetIconSize(m_hImageList, &cx, &cy))
		//	{
		//		xml->SetAttribute("iconsize", cx); // XPMI_ICONSIZE
		//	}
		//}

		xmlIcon xIcon;

		xIcon.tsType = L"menu";
		xIcon.tsID = L"0";
		xIcon.tsFlags = L"+B";

		for (int i{}; i < cnt; ++i)
		{
			if (auto hIcon = ImageList_GetIcon(m_hImageList, i, ILD_TRANSPARENT); hIcon)
			{
				Auto(DestroyIcon(hIcon));

				xIcon.tsSrc = IconToBase64(hIcon);

				vIcons.emplace_back(xIcon);
			}
		}
	}
}

bool XPopupMenu::DrawBorder() const
{
	if (getColor(MenuColours::XPMC_BORDER) == CLR_INVALID)
		return false;

	auto hMenuWnd = XPopupMenuManager::getBackWin();
	if (!IsWindow(hMenuWnd))
		return false;

	if (auto menuDc = ::GetWindowDC(hMenuWnd); menuDc)
	{
		Auto(ReleaseDC(hMenuWnd, menuDc));

		return DrawBorder(hMenuWnd, menuDc);
	}
	return false;
}

bool XPopupMenu::DrawBorder(_In_ HWND hWnd, _In_ HDC hdc) const noexcept
{
	const auto clr = getColor(MenuColours::XPMC_BORDER);

	if (clr == CLR_INVALID)
		return false;

	if (!IsWindow(hWnd))
		return false;

	{
		Dcx::dcxWindowRect rect(hWnd);
		const Dcx::dcxClientRect rcClient(hWnd, nullptr);
		const int borderThiness = rcClient.left - rect.left;
	
		::OffsetRect(&rect, -rect.left, -rect.top);
	
		::ExcludeClipRect(hdc, rect.left + borderThiness, rect.top + borderThiness, rect.right - borderThiness, rect.bottom - borderThiness);
		
		if (auto hPen = CreatePen(PS_INSIDEFRAME, 1, clr); hPen)
		{
			auto hOldPen = SelectPen(hdc, hPen);
			if (auto hBrush = CreateSolidBrush(getColor(MenuColours::XPMC_BACKGROUND)); hBrush)
			{
				auto hOldBrush = SelectBrush(hdc, hBrush);
				Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
				SelectBrush(hdc, hOldBrush);
				DeleteBrush(hBrush);
			}
			SelectPen(hdc, hOldPen);
			DeletePen(hPen);
		}
	}
	return true;
}

void XPopupMenu::xpop_a(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	// xpop -a - [MENU] [SWITCH] [PATH] [TAB] [+FLAGS] [ID] [ICON] ItemText (: Command) ([TAB] [TOOLTIP])
	// [PATH] cant be GID for this command.

	if (path[0] == L'=')
		throw DcxExceptions::dcxInvalidPath();

	const XSwitchFlags xflags(tsTabTwo.getfirsttok(1));		// tok 1 [+FLAGS]
	const auto mID = tsTabTwo.getnexttok().to_<UINT>();		// tok 2 [ID]
	const auto nIcon = tsTabTwo.getnexttok().to_int() - 1;	// tok 3 [ICON]

	TString tsItemText, tsTooltip;
	//TString itemcom;

	{
		const auto itemtext(tsTabTwo.getlasttoks());			// tok 4, -1 ItemText....

		if (tsTabTwo.numtok(TSTABCHAR) > 1)
		{
			tsItemText = itemtext.getfirsttok(1, TSTABCHAR).trim();
			tsTooltip = itemtext.getlasttoks().trim();
		}
		else
			tsItemText = itemtext;

		// Ook: this never seems to be used....
		//if (tsItemText.numtok(TEXT(':')) > 1)
		//{
		//	const TString tsTmp(tsItemText.getfirsttok(1, TEXT(':')).trim());
		//	itemcom = tsItemText.getnexttok(TEXT(':')).trim();
		//	tsItemText = tsTmp;
		//}
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
			mii.hSubMenu = AddSubMenu();

			if (!mii.hSubMenu)
				throw Dcx::dcxException(TEXT("Failed to create submenu."));
		}
		if (xflags[TEXT('c')])
			mii.fState |= MFS_CHECKED;
		if (xflags[TEXT('g')])
			mii.fState |= MFS_GRAYED;

		p_Item = std::make_unique<XPopupMenuItem>(this, tsItemText, tsTooltip, nIcon, (mii.hSubMenu != nullptr));

		if (xflags[TEXT('r')])
		{
			mii.fType |= MFT_RADIOCHECK;
			p_Item->setRadioCheck(true);
		}

		p_Item->setCheckToggle(xflags[TEXT('C')]);
	}

	p_Item->setCommandID(mID);

	mii.dwItemData = reinterpret_cast<ULONG_PTR>(p_Item.get());
	this->m_vpMenuItem.push_back(p_Item.release());
	InsertMenuItem(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
}

void XPopupMenu::xpop_c(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA;

	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
		throw Dcx::dcxException("Unable to get menu item info");

	DeleteSubMenu(mii.hSubMenu);

	mii.hSubMenu = AddSubMenu();

	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		p_Item->setSubMenu(TRUE);

	mii.fMask = MIIM_SUBMENU;

	SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
}

void XPopupMenu::xpop_d(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA;

	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
		throw Dcx::dcxException("Unable to get menu item info");

	DeleteSubMenu(mii.hSubMenu);

	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		p_Item->setSubMenu(FALSE);

	mii.hSubMenu = nullptr;
	mii.fMask = MIIM_SUBMENU;

	SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
}

void XPopupMenu::xpop_f(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_DATA;

	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
		throw Dcx::dcxException("Unable to get menu item info");

	DeleteSubMenu(mii.hSubMenu);

	if (const auto* const p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		this->deleteMenuItemData(p_Item, nullptr);

	DeleteMenu(hMenu, gsl::narrow_cast<UINT>(nPos), MF_BYPOSITION);
}

void XPopupMenu::xpop_i(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	const auto nIcon = tsTabTwo.to_int();

	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	const auto p_Item = getMenuItem(hMenu, nPos);
	if (!p_Item)
		throw DcxExceptions::dcxInvalidItem();

	p_Item->setItemIcon(nIcon);
}

void XPopupMenu::xpop_s(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	const XSwitchFlags xflags(tsTabTwo.getfirsttok(1));

	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	const auto p_Item = getMenuItem(hMenu, nPos);
	if (!p_Item)
		throw DcxExceptions::dcxInvalidItem();

	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STATE;

	// Flags may be empty, so no error if + is missing.
	if (xflags[TEXT('+')])
	{
		if (xflags[TEXT('c')])
			mii.fState |= MFS_CHECKED;
		if (xflags[TEXT('g')])
			mii.fState |= MFS_GRAYED;

		p_Item->setCheckToggle(xflags[TEXT('C')]);

		if (xflags[TEXT('r')])
		{
			const auto nFirst = tsTabTwo.getnexttok().to_<UINT>() - 1;
			const auto nLast = tsTabTwo.getnexttok().to_<UINT>() - 1;
			CheckMenuRadioItem(hMenu, nFirst, nLast, nPos, MF_BYPOSITION);
		}
		if (xflags[TEXT('R')])
			Dcx::XPopups.RedrawMenuIfOpen();
	}
	else
		p_Item->setCheckToggle(false);

	SetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii);
}

void XPopupMenu::xpop_t(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
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

void XPopupMenu::xpop_T(HMENU hMenu, int nPos, const TString& path, const TString& tsTabTwo)
{
	if (nPos < 0)
		throw DcxExceptions::dcxInvalidPath();

	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;

	if (GetMenuItemInfo(hMenu, gsl::narrow_cast<UINT>(nPos), TRUE, &mii) == FALSE)
		throw Dcx::dcxException("Unable to get menu item info");

	if (const auto p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		p_Item->setItemTooltip(tsTabTwo);
}

HMENU XPopupMenu::AddSubMenu()
{
	auto hSubMenu = CreatePopupMenu();
	if (hSubMenu)
	{
		if (!XPopupMenuManager::m_vpAllMenus.contains(hSubMenu))
			XPopupMenuManager::m_vpAllMenus[hSubMenu] = this;
	}
	return hSubMenu;
}

void XPopupMenu::DeleteSubMenu(HMENU hSubMenu) noexcept
{
	if (!hSubMenu)
		return;

	this->deleteAllItemData(hSubMenu);
	DestroyMenu(hSubMenu);

	XPopupMenuManager::m_vpAllMenus.erase(hSubMenu);
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

void XPMENUBAR::UAHDrawMenuBar(HWND mHwnd, UAHMENU* pUDM) noexcept
{
	if (!m_menuTheme)
		m_menuTheme = Dcx::UXModule.dcxOpenThemeData(mHwnd, L"Menu");

	RECT rc{};

	// get the menubar rect
	{
		MENUBARINFO mbi = { sizeof(mbi) };
		GetMenuBarInfo(mHwnd, OBJID_MENU, 0, &mbi);

		RECT rcWindow;
		GetWindowRect(mHwnd, &rcWindow);

		// the rcBar is offset by the window rect
		rc = mbi.rcBar;
		OffsetRect(&rc, -rcWindow.left, -rcWindow.top);
	}

	switch (m_Style)
	{
	case MainMenuStyle::XPMS_ICY:
	case MainMenuStyle::XPMS_OFFICE2003:
	case MainMenuStyle::XPMS_GRADE:
	{
		const auto clrStart = (m_Default.m_Colours.m_clrBack != CLR_INVALID) ? m_Default.m_Colours.m_clrBack : RGB(184, 199, 146);
		const auto clrEnd = (m_Default.m_Colours.m_clrHot != CLR_INVALID) ? m_Default.m_Colours.m_clrHot : RGB(240, 243, 231);
		XPopupMenuItem::DrawGradient(pUDM->hdc, &rc, clrStart, clrEnd, true);
	}
	break;
	case MainMenuStyle::XPMS_ICY_REV:
	case MainMenuStyle::XPMS_OFFICE2003_REV:
	case MainMenuStyle::XPMS_GRADE_REV:
	{
		const auto clrStart = (m_Default.m_Colours.m_clrHot != CLR_INVALID) ? m_Default.m_Colours.m_clrHot : RGB(240, 243, 231);
		const auto clrEnd = (m_Default.m_Colours.m_clrBack != CLR_INVALID) ? m_Default.m_Colours.m_clrBack : RGB(184, 199, 146);
		XPopupMenuItem::DrawGradient(pUDM->hdc, &rc, clrStart, clrEnd, true);
	}
	break;
	case MainMenuStyle::XPMS_None:
	case MainMenuStyle::XPMS_CUSTOMBIG:
		break;
	case MainMenuStyle::XPMS_OFFICEXP:
	case MainMenuStyle::XPMS_CUSTOM:
	case MainMenuStyle::XPMS_NORMAL:
	default:
	{
		if (m_Default.m_Colours.m_clrBack != CLR_INVALID)	// if menu colour set, use it
			Dcx::FillRectColour(pUDM->hdc, &rc, m_Default.m_Colours.m_clrBack);
		else if (m_menuTheme)	// otherwise try themed drawing
			Dcx::UXModule.dcxDrawThemeBackground(m_menuTheme, pUDM->hdc, MENU_BARBACKGROUND, (pUDM->dwFlags == 0xa00 ? MB_ACTIVE : MB_INACTIVE), &rc, nullptr);
		else
			Dcx::FillRectColour(pUDM->hdc, &rc, GetSysColor(COLOR_MENUBAR));	// if all else fails draw as standard menu colour.
	}
	break;
	}

	// if bitmap set draw it over the background colour.
	if (m_Default.m_hBkg.m_hBitmap)
		dcxDrawBitMap(pUDM->hdc, &rc, m_Default.m_hBkg.m_hBitmap, (m_Style != MainMenuStyle::XPMS_CUSTOM), false);
}

void XPMENUBAR::UAHDrawMenuBarItem(HWND mHwnd, UAHDRAWMENUITEM* pUDMI) noexcept
{
	XPMENUBARITEM mCols = this->m_Default;
	mCols.m_hBkg.m_hBitmap = nullptr;

	if (this->m_ItemSettings.contains(pUDMI->umi.iPosition))
		mCols = this->m_ItemSettings[pUDMI->umi.iPosition];

	COLORREF clrFill = mCols.m_Colours.m_clrBox;
	COLORREF clrText = mCols.m_Colours.m_clrText;
	COLORREF clrBorder = mCols.m_Colours.m_clrBox;

	if (this->m_bDrawBorder)
		clrBorder = mCols.m_Colours.m_clrBorder;

	// get the menu item string
	wchar_t menuString[256]{};
	MENUITEMINFO mii = { sizeof(mii), MIIM_STRING };
	{
		mii.dwTypeData = &menuString[0];
		mii.cch = gsl::narrow_cast<UINT>(std::size(menuString) - 1);

		GetMenuItemInfo(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
	}

	// get the item state for drawing

	DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

	//int iTextStateID = 0;
	//int iBackgroundStateID = 0;
	{
		//if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT))
		//{
		//	// normal display
		//	//iTextStateID = MPI_NORMAL;
		//	//iBackgroundStateID = MPI_NORMAL;
		//}
		if (pUDMI->dis.itemState & ODS_HOTLIGHT)
		{
			// hot tracking
			//iTextStateID = MPI_HOT;
			//iBackgroundStateID = MPI_HOT;

			//pbrBackground = &g_brItemBackgroundHot;
			clrFill = mCols.m_Colours.m_clrHot;
			clrText = mCols.m_Colours.m_clrHotText;
			if (this->m_bDrawBorder)
				clrBorder = mCols.m_Colours.m_clrHotBorder;
		}
		if (pUDMI->dis.itemState & ODS_SELECTED)
		{
			// clicked -- MENU_POPUPITEM has no state for this, though MENU_BARITEM does
			//iTextStateID = MPI_HOT;
			//iBackgroundStateID = MPI_HOT;

			//pbrBackground = &g_brItemBackgroundSelected;
			clrFill = mCols.m_Colours.m_clrSelection;
			clrText = mCols.m_Colours.m_clrSelectedText;
			if (this->m_bDrawBorder)
				clrBorder = mCols.m_Colours.m_clrSelectionBorder;
		}
		if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED))
		{
			// disabled / grey text
			//iTextStateID = MPI_DISABLED;
			//iBackgroundStateID = MPI_DISABLED;

			clrFill = mCols.m_Colours.m_clrDisabled;
			clrText = mCols.m_Colours.m_clrDisabledText;
			if (this->m_bDrawBorder)
				clrBorder = mCols.m_Colours.m_clrBorder;
		}
		if (pUDMI->dis.itemState & ODS_NOACCEL)
			dwFlags |= DT_HIDEPREFIX;
	}

	if (!this->m_menuTheme)
		this->m_menuTheme = Dcx::UXModule.dcxOpenThemeData(mHwnd, L"Menu");

	//if (this->m_bDrawBorder)
	//{
	//	//Dcx::FillRectColour(pUDMI->um.hdc, &pUDMI->dis.rcItem, this->m_Default.m_Colours.m_clrBack);
	//	dcxDrawRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, clrFill, clrBorder, this->m_bDrawRoundedBorder);
	//}
	//else
	//	Dcx::FillRectColour(pUDMI->um.hdc, &pUDMI->dis.rcItem, clrFill);

	if (mCols.m_hBkg.m_hBitmap)
		dcxDrawBitMap(pUDMI->um.hdc, &pUDMI->dis.rcItem, mCols.m_hBkg.m_hBitmap, true, false);
	else
		dcxDrawRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, clrFill, clrBorder, this->m_bDrawRoundedBorder);

	if (this->m_menuTheme)
	{
		const DTTOPTS opts = { sizeof(opts), (this->m_bDrawShadowText ? DTT_TEXTCOLOR | DTT_SHADOWCOLOR : DTT_TEXTCOLOR), clrText,0,RGB(0,0,0) };

		Dcx::UXModule.dcxDrawThemeTextEx(this->m_menuTheme, pUDMI->um.hdc, MENU_BARITEM, MBI_NORMAL, &menuString[0], mii.cch, dwFlags, &pUDMI->dis.rcItem, &opts);
	}
	else {
		if (this->m_bDrawShadowText)
			dcxDrawShadowText(pUDMI->um.hdc, &menuString[0], mii.cch, &pUDMI->dis.rcItem, dwFlags, clrText, RGB(0, 0, 0), 5, 5);
		else {
			const auto clrOld = SetTextColor(pUDMI->um.hdc, clrText);
			DrawTextW(pUDMI->um.hdc, &menuString[0], mii.cch, &pUDMI->dis.rcItem, dwFlags);
			SetTextColor(pUDMI->um.hdc, clrOld);
		}
	}
}

void XPMENUBAR::UAHDrawMenuNCBottomLine(HWND hWnd) const noexcept
{
	if (!hWnd)
		return;

	MENUBARINFO mbi = { sizeof(mbi) };
	if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi))
		return;

	RECT rcClient{};
	GetClientRect(hWnd, &rcClient);
	MapWindowRect(hWnd, nullptr, &rcClient);

	RECT rcWindow{};
	GetWindowRect(hWnd, &rcWindow);

	OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

	// the rcBar is offset by the window rect
	RECT rcAnnoyingLine = rcClient;
	rcAnnoyingLine.bottom = rcAnnoyingLine.top;
	rcAnnoyingLine.top--;


	if (HDC hdc = GetWindowDC(hWnd); hdc)
	{
		Dcx::FillRectColour(hdc, &rcAnnoyingLine, m_Default.m_Colours.m_clrBack);
		ReleaseDC(hWnd, hdc);
	}
}

// [+FLAGS] [ARGS]

void XPMENUBAR::Setup(HWND mHwnd, const XSwitchFlags& xflags, TString tsArgs)
{
	// control the custom menu bar settings
	if (!xflags[TEXT('+')])
		throw DcxExceptions::dcxInvalidFlag();

	// Enable flag is used by its self, cant be combined with other flags.
	// e = enable, [ARGS] = 1 or 0
	// r = enable/disable rounded borders, [ARGS] = 1 or 0
	// O = enable/disable drawing borders, [ARGS] = 1 or 0
	// f = load background image (bmp format only atm), [ARGS] = path/filename.bmp or [ARGS] = [ITEM INDEX] path/filename.bmp
	// s = enable/disable shadow text, [ARGS] = 1 or 0
	// v = visible/invisible menubar (works with custom or standard menubars)
	// S = menubar style, [ARGS] = stylename
	// 
	// i = item specific. combines with other flags +it = set item text colour.
	// R = redraw menubar. (can be combined with any flags, or used by its self)
	// 
	// [ARGS] = [ITEM INDEX] [TEXT] [SELECTED TEXT] [HOT TEXT] [BACKGROUND] [SELECTED BACKGROUND] [HOT BACKGROUND] [BORDER] [SELECTED BORDER] [HOT BORDER]
	// or when not item specific
	// [ARGS] = [TEXT] [SELECTED TEXT] [HOT TEXT] [BACKGROUND] [SELECTED BACKGROUND] [HOT BACKGROUND] [BORDER] [SELECTED BORDER] [HOT BORDER]
	//
	// t = text colour.
	// T = selected text colour.
	// H = hot text colour.
	// b = background colour
	// B = selected background colour.
	// h = hot background colour.
	// w = border colour.
	// W = selected border colour.
	// o = hot border colour.

	const auto numtok = tsArgs.numtok();

	if (xflags[TEXT('e')])
	{
		// enable/disable
		m_bEnable = (tsArgs == TEXT("1"));
	}
	else if (xflags[TEXT('r')])
	{
		// enable/disable
		m_bDrawRoundedBorder = (tsArgs == TEXT("1"));
	}
	else if (xflags[TEXT('O')])
	{
		// enable/disable
		m_bDrawBorder = (tsArgs == TEXT("1"));
	}
	else if (xflags[TEXT('f')] && !xflags[TEXT('i')])
	{
		// load bkg image.

		if (tsArgs.empty())
			m_Default.m_hBkg.reset();
		else {
			m_Default.m_hBkg.m_tsFilename = tsArgs;
			m_Default.m_hBkg.m_hBitmap = dcxLoadBitmap(m_Default.m_hBkg.m_hBitmap, tsArgs);
		}
	}
	else if (xflags[TEXT('s')])
	{
		// enable/disable
		m_bDrawShadowText = (tsArgs == TEXT("1"));
	}
	else if (xflags[TEXT('S')])
	{
		// style
		m_Style = XPopupMenu::parseStyle(tsArgs);
	}
	else if (xflags[TEXT('v')])
	{
		auto hTmp = GetMenu(mHwnd);

		// visable/invisable
		if (tsArgs == TEXT("1"))
		{
			if (m_hMenuBackup && m_hMenuBackup != hTmp && IsMenu(m_hMenuBackup))
			{
				SetMenu(mHwnd, m_hMenuBackup);
				m_hMenuBackup = nullptr;
			}
		}
		else {
			if (!m_hMenuBackup && m_hMenuBackup != hTmp)
			{
				SetMenu(mHwnd, nullptr);
				m_hMenuBackup = hTmp;
			}
		}
	}
	else {

		auto _SetColours = [](const XSwitchFlags& xflags, const TString& tsArgs, const XPMENUBARCOLORS& colDefaults) {
			XPMENUBARCOLORS cols{ colDefaults };

			if (xflags[TEXT('t')])
			{
				cols.m_clrText = tsArgs.gettok(1).to_<COLORREF>();
			}
			if (xflags[TEXT('T')])
			{
				cols.m_clrSelectedText = tsArgs.gettok(2).to_<COLORREF>();
			}
			if (xflags[TEXT('H')])
			{
				cols.m_clrHotText = tsArgs.gettok(3).to_<COLORREF>();
			}
			if (xflags[TEXT('b')])
			{
				cols.m_clrBack = tsArgs.gettok(4).to_<COLORREF>();
				cols.m_clrBox = cols.m_clrBack;
			}
			if (xflags[TEXT('B')])
			{
				cols.m_clrSelection = tsArgs.gettok(5).to_<COLORREF>();
			}
			if (xflags[TEXT('h')])
			{
				cols.m_clrHot = tsArgs.gettok(6).to_<COLORREF>();
			}
			if (xflags[TEXT('w')])
			{
				cols.m_clrBorder = tsArgs.gettok(7).to_<COLORREF>();
			}
			if (xflags[TEXT('W')])
			{
				cols.m_clrSelectionBorder = tsArgs.gettok(8).to_<COLORREF>();
			}
			if (xflags[TEXT('o')])
			{
				cols.m_clrHotBorder = tsArgs.gettok(9).to_<COLORREF>();
			}
			return cols;
		};

		if (xflags[TEXT('i')])
		{
			// item flags
			const auto mItem = tsArgs.getfirsttok(1).to_int() - 1;
			if (mItem < 0)
				throw DcxExceptions::dcxInvalidItem();

			XPMENUBARITEM xpItem;
			xpItem.m_Colours = m_Default.m_Colours;	// copy existing defaults first.
			if (m_ItemSettings.contains(mItem))	// see if custom item alrdy exists.
				xpItem = m_ItemSettings[mItem];	// if so copy it into new item.

			if (xflags[TEXT('f')])
			{
				// load bkg image.

				if (tsArgs.empty())
					xpItem.m_hBkg.reset();
				else {
					xpItem.m_hBkg.m_tsFilename = tsArgs;
					xpItem.m_hBkg.m_hBitmap = dcxLoadBitmap(xpItem.m_hBkg.m_hBitmap, tsArgs);
				}
			}
			else {
				if (numtok < 9)
					throw DcxExceptions::dcxInvalidArguments();

				xpItem.m_Colours = _SetColours(xflags, tsArgs.getlasttoks(), xpItem.m_Colours);
			}
			m_ItemSettings[mItem] = xpItem;
		}
		else {
			// general flags
			if (numtok < 9)
				throw DcxExceptions::dcxInvalidArguments();

			m_Default.m_Colours = _SetColours(xflags, tsArgs, m_Default.m_Colours);
		}
	}
	if (xflags[TEXT('R')])
		DrawMenuBar(mHwnd);
}
