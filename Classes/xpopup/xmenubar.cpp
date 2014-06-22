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

/*
 *
 */
XMenuBar::XMenuBar() {
}

/*
 *
 */
XMenuBar::~XMenuBar() {
}

/*
 *
 */
void XMenuBar::parseXMenuBarCommand(const TString &input) {
	const XSwitchFlags flags(input.getfirsttok( 1 ));
	const UINT numtok = input.numtok();
	XPopupMenu *p_Menu;
	HMENU menuBar;
	const TString menuName(input.getnexttok( ));	// tok 2

	// Check if a callback alias has been marked
	if (!this->hasCallback() && !flags[TEXT('M')])
	{
		Dcx::error(TEXT("/xmenubar"), TEXT("No callback alias initialised. See /xmenubar -M"));
		return;
	}

	// Initialise callback.
	// xmenubar [-M] (ALIAS)
	if (flags[TEXT('M')]) {
		// Set alias.
		if (numtok > 1) {
			const TString alias(menuName);

			// Check if alias is valid.
			if (!mIRCLinker::isAlias(alias.to_chr()))
			{
				Dcx::error(TEXT("-M"), TEXT("Invalid callback alias specified"));
				return;
			}

			this->m_callback = alias;
		}
		// Reset alias and xmenubar.
		else {
			this->m_callback = TEXT("");
			this->resetMenuBar();
		}

		return;
	}

	menuBar = GetMenu(mIRCLinker::getHWND());

	// Add menu
	// xmenubar [-a] [MENU] [LABEL]
	if (flags[TEXT('a')]) {
		if (numtok < 3) {
			Dcx::error(TEXT("-a"), TEXT("Insufficient parameters"));
			return;
		}

		p_Menu = Dcx::XPopups.getMenuByName(menuName, TRUE);

		if (!validateMenu(p_Menu, TEXT("-a"), menuName))
			return;

		// Test if menu is already added.
		for (int i = 0; i < (int) m_vpXMenuBar.size(); i++) {
			if (m_vpXMenuBar[i] == p_Menu) {
				Dcx::error(TEXT("-a"), TEXT("Menu has already been added to XmenuBar."));
				return;
			}
		}

		p_Menu->attachToMenuBar(menuBar, input.gettok(3, -1));
	}
	// Removes menu
	// xmenubar [-d] [MENU]
	else if (flags[TEXT('d')]) {
		if (numtok < 2) {
			Dcx::error(TEXT("-d"), TEXT("Insufficient parameters"));
			return;
		}

		p_Menu = Dcx::XPopups.getMenuByName(menuName, TRUE);

		if (!validateMenu(p_Menu, TEXT("-d"), menuName))
			return;

		p_Menu->detachFromMenuBar(menuBar);
	}
	// Replace mIRC's menubar and generate our own
	// xmenubar [-g]
	// TODO: add support for styles if I can think of how.
	else if (flags[TEXT('g')]) {
		HMENU newMenu = CreateMenu();

		this->setMenuBar(menuBar, newMenu);

		// Redraws to include the system icons from MDI child.
		mIRCLinker::exec(TEXT("//window -a $qt($active)"));
	}
	// Change the label on the menu
	// xmenubar [-l] [MENU] [LABEL]
	else if (flags[TEXT('l')]) {
		if (numtok < 3) {
			Dcx::error(TEXT("-l"), TEXT("Insufficient parameters"));
			return;
		}

		p_Menu = Dcx::XPopups.getMenuByName(menuName, TRUE);

		if (!validateMenu(p_Menu, TEXT("-l"), menuName))
			return;

		const int offset = this->findMenuOffset(menuBar, p_Menu);

		if (offset < 0) {
			Dcx::errorex(TEXT("-l"), TEXT("\"%s\" menu not found in XMenuBar."), p_Menu->getName().to_chr());
			return;
		}

		ModifyMenu(menuBar, offset, MF_BYPOSITION, MF_STRING, input.gettok(3, -1).to_chr());
	}
	// Resets to original mIRC menubar
	// xmenubar [-r]
	else if (flags[TEXT('r')]) {
		this->resetMenuBar();
	}
	// Trigger command for a specific mIRC menu item.
	// xmenubar [-s] [ID]
	else if (flags[TEXT('s')]) {
		if (numtok < 2) {
			Dcx::error(TEXT("-s"), TEXT("Insufficient parameters"));
			return;
		}

		const int mID = menuName.to_int();

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
void XMenuBar::parseXMenuBarInfo(const TString &input, TCHAR *szReturnValue) const
{
	const TString prop(input.getfirsttok( 1 ));

	// Iterate through the names of menus added to XMenuBar.
	// N = 0 returns total number of menus
	// $xmenubar() [menu] [N]
	if (prop == TEXT("menu")) {
		const int i = input.getnexttok( ).to_int();	// tok 2

		if ((i < 0) || (i > (int) this->m_vpXMenuBar.size())) {
			Dcx::errorex(TEXT("$!xpopup().menubar"), TEXT("Invalid index: %d"), i);
			return;
		}

		// Return number of menus in menubar.
		if (i == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (int) this->m_vpXMenuBar.size());
		// Return name of specified menu.
		else
			dcx_strcpyn(szReturnValue, this->m_vpXMenuBar[i -1]->getName().to_chr(), MIRC_BUFFER_SIZE_CCH);

		return;
	}

	szReturnValue[0] = 0;
}

/*
 * Adds the menu to the current menubar.
 */
bool XMenuBar::addToMenuBar(HMENU menubar, XPopupMenu *p_Menu, const TString &label) {
	m_vpXMenuBar.push_back(p_Menu);
	return (AppendMenu(menubar, MF_POPUP, (UINT_PTR) p_Menu->getMenuHandle(), label.to_chr()) != 0);
}

/*
 *
 */
void XMenuBar::removeFromMenuBar(HMENU menubar, XPopupMenu *p_Menu) {
	if ((int) m_vpXMenuBar.size() == 0)
		return;

	// If no menubar is specified, get current menubar.
	if (menubar == NULL) {
		menubar = GetMenu(mIRCLinker::getHWND());

		if (!IsMenu(menubar))
			return;
	}

	// Remove the menu from the vector list.
	VectorOfXPopupMenu::iterator itStart = this->m_vpXMenuBar.begin();
	VectorOfXPopupMenu::iterator itEnd = this->m_vpXMenuBar.end();

	while (itStart != itEnd) {
		if (*itStart == p_Menu) {
			this->m_vpXMenuBar.erase(itStart);
			break;
		}

		++itStart;
	}

	const int offset = findMenuOffset(menubar, p_Menu);

	if (offset > 0)
		RemoveMenu(menubar, offset, MF_BYPOSITION);

	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 * Searches for the given menu in the menubar, and returns the zero-based index position.
 */
int XMenuBar::findMenuOffset(HMENU menubar, const XPopupMenu *p_Menu) const {
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
	//VectorOfXPopupMenu temp;
	//VectorOfXPopupMenu::iterator itStart = this->m_vpXMenuBar.begin();
	//VectorOfXPopupMenu::iterator itEnd = this->m_vpXMenuBar.end();

	//// Add menus to a temporary list to prevent errors in looping
	//while (itStart != itEnd) {
	//	temp.push_back(*itStart);
	//	++itStart;
	//}

	//itStart = temp.begin();
	//itEnd = temp.end();

	// Add menus to a temporary list to prevent errors in looping
	VectorOfXPopupMenu temp(this->m_vpXMenuBar);
#if DCX_USE_C11
	for (const auto &x: temp) {
		x->detachFromMenuBar(oldMenuBar);
	}
#else
	VectorOfXPopupMenu::iterator itStart = temp.begin();
	VectorOfXPopupMenu::iterator itEnd = temp.end();

	// Begin detaching ...
	while (itStart != itEnd) {
		(*itStart)->detachFromMenuBar(oldMenuBar);
		++itStart;
	}
#endif
	// Destroy the menu if it isnt the original mIRC menubar.
	if (g_OriginalMenuBar == NULL)
		g_OriginalMenuBar = oldMenuBar;
	else
		DestroyMenu(oldMenuBar);

	DrawMenuBar(mIRCLinker::getHWND());
}

/*
 *
 */
bool XMenuBar::validateMenu(const XPopupMenu *menu, const TString &flag, const TString &name) const {
	if (menu == NULL) {
		Dcx::errorex(flag.to_chr(), TEXT("Cannot find menu \"%s\"."), name.to_chr());
		return false;
	}
	// Prevent users from adding special menus.
	else if ((menu == Dcx::XPopups.getmIRCPopup()) || (menu == Dcx::XPopups.getmIRCMenuBar())) {
		Dcx::error(flag.to_chr(), TEXT("Cannot add \"mirc\" or \"mircbar\" menus."));
		return false;
	}

	return true;
}

/*
 *
 */
void XMenuBar::resetMenuBar() {
	if (g_OriginalMenuBar != NULL) {
		HMENU menubar = GetMenu(mIRCLinker::getHWND());

		this->setMenuBar(menubar, g_OriginalMenuBar);
		g_OriginalMenuBar = NULL;
	}
}

/*
 *
 */
bool XMenuBar::hasCallback() const {
	if (this->m_callback.len() == 0)
		return false;

	return true;
}

/*
 * Passes the clicked information back to the callback.
 * Returns TEXT('true') it should halt default processing.
 *
 * User should return TEXT('$true') from the callback to prevent default processing.
 */
bool XMenuBar::parseCallback(const UINT menuID) {
	TString result;

	mIRCLinker::tsEvalex(result, TEXT("$%s(%d)"), this->m_callback.to_chr(), menuID);

	if (result == TEXT("$true"))
		return true;

	return false;
}
