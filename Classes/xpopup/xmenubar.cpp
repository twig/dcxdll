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



//extern HMENU g_OriginalMenuBar;
//extern XPopupMenu *g_mIRCScriptMenu;
//
//
//
//
//XMenuBar::XMenuBar() {
//}
//
//
//
//
//XMenuBar::~XMenuBar() {
//}

/*
 *
 */
void XMenuBar::parseXMenuBarCommand(const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 1 ));
	const auto numtok = input.numtok();
	const auto menuName(input.getnexttok());	// tok 2

	// Check if a callback alias has been marked
	if (!this->hasCallback() && !flags[TEXT('M')])
		throw std::runtime_error("No callback alias initialised. See /xmenubar -M");

	// Initialise callback.
	// xmenubar [-M] (ALIAS)
	if (flags[TEXT('M')]) {
		// Set alias.
		if (numtok > 1) {
			// menuName == alias

			// Check if alias is valid.
			if (!mIRCLinker::isAlias(menuName.to_chr()))
				throw std::invalid_argument("Invalid callback alias specified");

			this->m_callback = menuName;
		}
		// Reset alias and xmenubar.
		else {
			this->m_callback.clear();	// = TEXT("");
			this->resetMenuBar();
		}

		return;
	}

	auto menuBar = GetMenu(mIRCLinker::getHWND());

	// Add menu
	// xmenubar [-a] [MENU] [LABEL]
	if (flags[TEXT('a')]) {
		if (numtok < 3)
			throw std::invalid_argument("Insufficient parameters");

		auto p_Menu = Dcx::XPopups.getMenuByName(menuName, true);

		validateMenu(p_Menu, menuName);

		// Test if menu is already added.
		{
			//for (const auto &m : m_vpXMenuBar)
			//{
			//	if (m == p_Menu)
			//		throw std::invalid_argument("Menu has already been added to XmenuBar.");
			//}

			//for (VectorOfXPopupMenu::size_type i = 0; i < m_vpXMenuBar.size(); i++) {
			//	if (m_vpXMenuBar[i] == p_Menu)
			//		throw std::invalid_argument("Menu has already been added to XmenuBar.");
			//}

			//const auto itEnd = m_vpXMenuBar.end();
			//const auto itGot = std::find(m_vpXMenuBar.begin(), itEnd, p_Menu);
			//if (itGot != itEnd)
			//	throw std::invalid_argument("Menu has already been added to XmenuBar.");

			if (Dcx::find(m_vpXMenuBar, p_Menu))
				throw std::invalid_argument("Menu has already been added to XmenuBar.");
		}
		p_Menu->attachToMenuBar(menuBar, input.getlasttoks());	// tok 3, -1
	}
	// Removes menu
	// xmenubar [-d] [MENU]
	else if (flags[TEXT('d')]) {
		if (numtok < 2)
			throw std::invalid_argument("Insufficient parameters");

		auto p_Menu = Dcx::XPopups.getMenuByName(menuName, true);

		validateMenu(p_Menu, menuName);

		p_Menu->detachFromMenuBar(menuBar);
	}
	// Replace mIRC's menubar and generate our own
	// xmenubar [-g]
	// TODO: add support for styles if I can think of how.
	else if (flags[TEXT('g')]) {
		auto newMenu = CreateMenu();

		this->setMenuBar(menuBar, newMenu);

		// Redraws to include the system icons from MDI child.
		mIRCLinker::exec(TEXT("//window -a $qt($active)"));
	}
	// Change the label on the menu
	// xmenubar [-l] [MENU] [LABEL]
	else if (flags[TEXT('l')]) {
		if (numtok < 3)
			throw std::invalid_argument("Insufficient parameters");

		auto p_Menu = Dcx::XPopups.getMenuByName(menuName, true);

		validateMenu(p_Menu, menuName);

		const auto offset = this->findMenuOffset(menuBar, p_Menu);

		if (offset < 0)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("\"%s\" menu not found in XMenuBar."), p_Menu->getName().to_chr()));

		ModifyMenu(menuBar, offset, MF_BYPOSITION, MF_STRING, input.getlasttoks().to_chr());	// tok 3, -1
	}
	// Resets to original mIRC menubar
	// xmenubar [-r]
	else if (flags[TEXT('r')]) {
		this->resetMenuBar();
	}
	// Trigger command for a specific mIRC menu item.
	// xmenubar [-s] [ID]
	else if (flags[TEXT('s')]) {
		if (numtok < 2)
			throw std::invalid_argument("Insufficient parameters");

		const auto mID = menuName.to_int();

		// MAKEWPARAM((# = Menu ID), (0 = Menu command));
		SendMessage(mIRCLinker::getHWND(), WM_COMMAND, MAKEWPARAM(mID, 0) , NULL);
		return;
	}

	// Force redraw so the updates are shown.
	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 *
 */
void XMenuBar::parseXMenuBarInfo(const TString &input, TCHAR *const szReturnValue) const
{
	const auto prop(input.getfirsttok(1));

	// Iterate through the names of menus added to XMenuBar.
	// N = 0 returns total number of menus
	// $xmenubar() [menu] [N]
	if (prop == TEXT("menu")) {
		const auto iSize = m_vpXMenuBar.size();
#if TSTRING_TEMPLATES
		const auto i = input.getnexttok().to_<VectorOfXPopupMenu::size_type>();	// tok 2

		if (i > iSize)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index: %u"), i));
#else
		const VectorOfXPopupMenu::size_type i = input.getnexttok().to_int();	// tok 2
		if (i > iSize)
			throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Invalid index: %u"), i));
#endif

		// Return number of menus in menubar.
		if (i == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), iSize);
		// Return name of specified menu.
		else
			dcx_strcpyn(szReturnValue, this->m_vpXMenuBar[i -1]->getName().to_chr(), MIRC_BUFFER_SIZE_CCH);
	}
}

/*
 * Adds the menu to the current menubar.
 */
const bool XMenuBar::addToMenuBar(HMENU menubar, XPopupMenu *const p_Menu, const TString &label) {
	m_vpXMenuBar.push_back(p_Menu);
	return (AppendMenu(menubar, MF_POPUP, (UINT_PTR) p_Menu->getMenuHandle(), label.to_chr()) != 0);
}

/*
 *
 */
void XMenuBar::removeFromMenuBar(HMENU menubar, const XPopupMenu *const p_Menu) {
	if (m_vpXMenuBar.empty())
		return;

	// If no menubar is specified, get current menubar.
	if (menubar == nullptr) {
		menubar = GetMenu(mIRCLinker::getHWND());

		if (!IsMenu(menubar))
			return;
	}

	// Remove the menu from the vector list.
	{
		//auto itStart = this->m_vpXMenuBar.begin();
		//auto itEnd = this->m_vpXMenuBar.end();
		//
		//while (itStart != itEnd) {
		//	if (*itStart == p_Menu) {
		//		this->m_vpXMenuBar.erase(itStart);
		//		break;
		//	}
		//
		//	++itStart;
		//}

		const auto itEnd = m_vpXMenuBar.end();
		const auto itGot = std::find(m_vpXMenuBar.begin(), itEnd, p_Menu);
		if (itGot != itEnd)
			m_vpXMenuBar.erase(itGot);
	}

	const auto offset = findMenuOffset(menubar, p_Menu);

	if (offset > 0)
		RemoveMenu(menubar, offset, MF_BYPOSITION);

	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 * Searches for the given menu in the menubar, and returns the zero-based index position.
 */
const int XMenuBar::findMenuOffset(HMENU menubar, const XPopupMenu *const p_Menu) const {
	MENUITEMINFO mii;
	int offset = 0;					// Use 1 because 0 = the menubar itself when using GetMenuBarInfo()
									// Changed to 0 to allow pre-increment within while()
	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU;

	// Whilst we can retrieve more menu items ...
	while (GetMenuItemInfo(menubar, ++offset, TRUE, &mii)) {
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
void XMenuBar::setMenuBar(HMENU oldMenuBar, HMENU newMenuBar) {
	if (newMenuBar != g_OriginalMenuBar) {
		MENUINFO mi;

		ZeroMemory(&mi, sizeof(MENUINFO));
		mi.cbSize = sizeof(MENUINFO);
		mi.fMask = MIM_BACKGROUND | MIM_HELPID | MIM_MAXHEIGHT | MIM_MENUDATA | MIM_STYLE;

		GetMenuInfo(oldMenuBar, &mi);
		SetMenuInfo(newMenuBar, &mi);
	}

	SetMenu(mIRCLinker::getHWND(), newMenuBar);

	// Go through old menubar items and detach them

	// Add menus to a temporary list to prevent errors in looping
	{
		// scope used to make sure temp isnt used again.
		auto temp(this->m_vpXMenuBar);

		for (const auto &x : temp) {
			x->detachFromMenuBar(oldMenuBar);
		}
	}
	// Destroy the menu if it isnt the original mIRC menubar.
	if (g_OriginalMenuBar == nullptr)
		g_OriginalMenuBar = oldMenuBar;
	else
		DestroyMenu(oldMenuBar);

	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 *
 */
void XMenuBar::validateMenu(const XPopupMenu *const menu, const TString &name)
{
	if (menu == nullptr)
		throw std::invalid_argument(Dcx::dcxGetFormattedString(TEXT("Cannot find menu \"%s\"."), name.to_chr()));

	// Prevent users from adding special menus.
	if ((menu == Dcx::XPopups.getmIRCPopup()) || (menu == Dcx::XPopups.getmIRCMenuBar()))
		throw std::invalid_argument("Cannot add \"mirc\" or \"mircbar\" menus.");
}

/*
 *
 */
void XMenuBar::resetMenuBar() {
	if (g_OriginalMenuBar != nullptr) {
		auto menubar = GetMenu(mIRCLinker::getHWND());

		this->setMenuBar(menubar, g_OriginalMenuBar);
		g_OriginalMenuBar = nullptr;
	}
}

/*
 *
 */
const bool XMenuBar::hasCallback() const noexcept {
	return (!this->m_callback.empty());
}

/*
 * Passes the clicked information back to the callback.
 * Returns '$true' it should halt default processing.
 *
 * User should return '$true' from the callback to prevent default processing.
 */
const bool XMenuBar::parseCallback(const UINT menuID) {
	TString result;

	mIRCLinker::tsEvalex(result, TEXT("$%s(%d)"), this->m_callback.to_chr(), menuID);

	return (result == TEXT("$true"));
}
