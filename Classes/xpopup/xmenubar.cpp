/*!
 * \file xmenubar.cpp
 * \brief blah
 *
 * blah
 *
 * \author William Nguyen ( twig at live dot it )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2007-2008
 */
#include "defines.h"
#include "Classes/xpopup/xmenubar.h"
#include "Classes/xpopup/xpopupmenumanager.h"
#include "Dcx.h"

void XMenuBar::parseXMenuBarCommand(const TString& input)
{
	const XSwitchFlags flags(input.getfirsttok(1));
	const auto numtok = input.numtok();
	const auto menuName(input.getnexttok());	// tok 2

	// Check if a callback alias has been marked
	if (!this->hasCallback() && !flags[TEXT('M')])
		throw Dcx::dcxException("No callback alias initialised. See /xmenubar -M");

	// Initialise callback.
	// xmenubar [-M] (ALIAS)
	if (flags[TEXT('M')])
	{
		// Set alias.
		if (numtok > 1)
		{
			// menuName == alias

			// Check if alias is valid.
			if (!mIRCLinker::isAlias(menuName))
				throw Dcx::dcxException("Invalid callback alias specified");

			this->m_callback = menuName;
		}
		// Reset alias and xmenubar.
		else {
			this->m_callback.clear();	// = TEXT("");
			this->resetMenuBar();
		}

		return;
	}

	// Add menu
	// xmenubar [-a] [MENU] [LABEL]
	if (auto menuBar = GetMenu(mIRCLinker::getHWND()); flags[TEXT('a')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto p_Menu = Dcx::XPopups.getMenuByName(menuName, true);

		validateMenu(p_Menu, menuName);

		// Test if menu is already added.
		{
			if (Dcx::find(m_vpXMenuBar, p_Menu))
				throw Dcx::dcxException("Menu has already been added to XmenuBar.");
		}
		p_Menu->attachToMenuBar(menuBar, input.getlasttoks());	// tok 3, -1
	}
	// Removes menu
	// xmenubar [-d] [MENU]
	else if (flags[TEXT('d')])
	{
		if (numtok < 2)
			throw DcxExceptions::dcxInvalidArguments();

		const auto p_Menu = Dcx::XPopups.getMenuByName(menuName, true);

		validateMenu(p_Menu, menuName);

		p_Menu->detachFromMenuBar(menuBar);
	}
	// Replace mIRC's menubar and generate our own
	// xmenubar [-g]
	// TODO: add support for styles if I can think of how.
	else if (flags[TEXT('g')])
	{
		auto newMenu = CreateMenu();

		this->setMenuBar(menuBar, newMenu);

		// Redraws to include the system icons from MDI child.
		mIRCLinker::exec(TEXT("//window -a $qt($active)"));
	}
	// Change the label on the menu
	// xmenubar [-l] [MENU] [LABEL]
	else if (flags[TEXT('l')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto* const p_Menu = Dcx::XPopups.getMenuByName(menuName, true);

		validateMenu(p_Menu, menuName);

		const auto offset = this->findMenuOffset(menuBar, p_Menu);

		if (offset < 0)
			throw Dcx::dcxException(TEXT("\"%\" menu not found in XMenuBar."), p_Menu->getName());

		ModifyMenu(menuBar, gsl::narrow_cast<UINT>(offset), MF_BYPOSITION, MF_STRING, input.getlasttoks().to_chr());	// tok 3, -1
	}
	// Resets to original mIRC menubar
	// xmenubar [-r]
	else if (flags[TEXT('r')])
	{
		this->resetMenuBar();
	}
	// Trigger command for a specific mIRC menu item.
	// xmenubar [-s] [ID]
	else if (flags[TEXT('s')])
	{
		if (numtok < 2)
			throw DcxExceptions::dcxInvalidArguments();

		Dcx::XPopups.TriggerMenuCommand(mIRCLinker::getHWND(), menuName.to_<UINT>());
		return;
	}
	// xmenubar [-p] [+FLAGS] [ARGS]
	else if (flags[TEXT('p')])
	{
		if (numtok < 2)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(menuName);
		m_Settings.Setup(mIRCLinker::getHWND(), xflags, input.getlasttoks());
	}
	// Force redraw so the updates are shown.
	DrawMenuBar(mIRCLinker::getHWND());
}

void XMenuBar::parseXMenuBarInfo(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const
{
	szReturnValue = parseXMenuBarInfo(input).to_chr();
}

TString XMenuBar::parseXMenuBarInfo(const TString& input) const
{
	TString tsRes;
	switch (const auto prop(input.getfirsttok(1)); std::hash<TString>()(prop))
	{
		// Iterate through the names of menus added to XMenuBar.
		// N = 0 returns total number of menus
		// $xmenubar([N]).menu
	case L"menu"_hash:
	{
		const auto iSize = m_vpXMenuBar.size();
		const auto i = input.getnexttok().to_<VectorOfXPopupMenu::size_type>();	// tok 2

		if (i > iSize)
			throw Dcx::dcxException(TEXT("Invalid index: %"), i);

		// Return number of menus in menubar.
		if (i == 0)
			tsRes += iSize;
		// Return name of specified menu.
		else
			tsRes = gsl::at(m_vpXMenuBar, i - 1)->getName();
	}
	break;
	// returns total number of menus added.
	// $xmenubar().count
	case L"count"_hash:
	{
		// Return number of menus in menubar.
		tsRes += m_vpXMenuBar.size();
	}
	break;
	// returns true/false if menu has been added.
	// $xmenubar([menu]).ismenu
	case L"ismenu"_hash:
	{
		const auto tsNameHash = std::hash<TString>()(input.getnexttok());
		for (const auto& a : m_vpXMenuBar)
		{
			if (a->getNameHash() == tsNameHash)
				return L"$true";
		}
		return L"$false";
	}
	break;
	// returns the callback alias if any.
	// $xmenubar().callback
	case L"callback"_hash:
	{
		tsRes = m_callback;
	}
	break;
	default:
		throw Dcx::dcxException(TEXT("Unknown prop \"%\""), prop);
		break;
	}
	return tsRes;
}

/*
 * Adds the menu to the current menubar.
 */
const bool XMenuBar::addToMenuBar(HMENU menubar, XPopupMenu* const p_Menu, const TString& label)
{
#if defined(XPOPUP_USE_UNIQUEPTR)
	m_vpXMenuBar.emplace_back(p_Menu);
#else
	m_vpXMenuBar.push_back(p_Menu);
#endif
	return (AppendMenu(menubar, MF_POPUP, (UINT_PTR)p_Menu->getMenuHandle(), label.to_chr()) != 0);
}

/*
 *
 */
void XMenuBar::removeFromMenuBar(HMENU menubar, const XPopupMenu* const p_Menu) noexcept
{
	if (m_vpXMenuBar.empty())
		return;

	// If no menubar is specified, get current menubar.
	if (!menubar)
	{
		menubar = GetMenu(mIRCLinker::getHWND());

		if (!IsMenu(menubar))
			return;
	}

	// Remove the menu from the vector list.
	Dcx::eraseIfFound(m_vpXMenuBar, p_Menu);

	if (const auto offset = findMenuOffset(menubar, p_Menu); offset > 0)
		RemoveMenu(menubar, gsl::narrow_cast<UINT>(offset), MF_BYPOSITION);

	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 * Searches for the given menu in the menubar, and returns the zero-based index position.
 */
const int XMenuBar::findMenuOffset(HMENU menubar, const XPopupMenu* const p_Menu) const noexcept
{
	MENUITEMINFO mii{};
	int offset = 0;					// Use 1 because 0 = the menubar itself when using GetMenuBarInfo()
	// Changed to 0 to allow pre-increment within while()
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU;

	// Whilst we can retrieve more menu items ...
	while (GetMenuItemInfo(menubar, gsl::narrow_cast<UINT>(++offset), TRUE, &mii))
	{
		// Continue if this isnt the menu we're after
		if (p_Menu->getMenuHandle() == mii.hSubMenu)
			return offset;
	}

	// Nothing found
	return -1;
}

/*
 * Sets the menubar into the mIRC window, and automatically destroys the old menubar
 * if it isnt the original mIRC menubar.
 */
void XMenuBar::setMenuBar(HMENU oldMenuBar, HMENU newMenuBar)
{
	if (!oldMenuBar || !newMenuBar)
		return;

	if (newMenuBar != g_OriginalMenuBar)
	{
		MENUINFO mi{ sizeof(MENUINFO),(MIM_BACKGROUND | MIM_HELPID | MIM_MAXHEIGHT | MIM_MENUDATA | MIM_STYLE),0,0,nullptr,0,0 };

		//ZeroMemory(&mi, sizeof(MENUINFO));
		//mi.cbSize = sizeof(MENUINFO);
		//mi.fMask = MIM_BACKGROUND | MIM_HELPID | MIM_MAXHEIGHT | MIM_MENUDATA | MIM_STYLE;

		GetMenuInfo(oldMenuBar, &mi);
		SetMenuInfo(newMenuBar, &mi);
	}

	SetMenu(mIRCLinker::getHWND(), newMenuBar);

	// Go through old menubar items and detach them

	// Add menus to a temporary list to prevent errors in looping
	{
		// scope used to make sure temp isnt used again.
		auto temp(this->m_vpXMenuBar);

		for (const auto& x : temp)
			x->detachFromMenuBar(oldMenuBar);
	}
	// Destroy the menu if it isnt the original mIRC menubar.
	if (!g_OriginalMenuBar)
		g_OriginalMenuBar = oldMenuBar;
	else
		DestroyMenu(oldMenuBar);

	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 *
 */
void XMenuBar::validateMenu(const XPopupMenu* const menu, const TString& name)
{
	if (!menu)
		throw Dcx::dcxException(TEXT("Cannot find menu \"%\"."), name);

	// Prevent users from adding special menus.
	if ((menu == Dcx::XPopups.getmIRCPopup()) || (menu == Dcx::XPopups.getmIRCMenuBar()) || (menu == Dcx::XPopups.getmIRCScriptMenu()))
		throw Dcx::dcxException("Cannot add \"mirc\" or \"mircbar\" or \"scriptpopup\" menus.");
}

/*
 *
 */
void XMenuBar::resetMenuBar()
{
	if (g_OriginalMenuBar)
	{
		auto menubar = GetMenu(mIRCLinker::getHWND());

		this->setMenuBar(menubar, g_OriginalMenuBar);
		g_OriginalMenuBar = nullptr;
	}
}

/*
 *
 */
const bool XMenuBar::hasCallback() const noexcept
{
	return (!this->m_callback.empty());
}

/*
 * Passes the clicked information back to the callback.
 * Returns '$true' it should halt default processing.
 *
 * User should return '$true' from the callback to prevent default processing.
 */
const bool XMenuBar::parseCallback(const UINT menuID)
{
	TString result;

	mIRCLinker::eval(result, TEXT("$%(%)"), m_callback, menuID);

	return (result == TEXT("$true"));
}
