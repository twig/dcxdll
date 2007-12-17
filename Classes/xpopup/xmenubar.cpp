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
 * © ScriptsDB.org - 2007
 */

#include "xmenubar.h"
#include "xpopupmenumanager.h"

extern XPopupMenu * g_mIRCPopupMenu;
extern XPopupMenu * g_mIRCMenuBar;
extern XPopupMenuManager g_XPopupMenuManager;

extern mIRCDLL mIRCLink;

extern HMENU g_OriginalMenuBar;
extern XPopupMenu *g_mIRCScriptMenu;

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
	XSwitchFlags flags(input.gettok(1));
	int numtok = input.numtok();
	XPopupMenu *p_Menu;
	HMENU menuBar;
	TString menuName;

	// Check if a callback alias has been marked
	if (!this->hasCallback() && !flags['M'])
	{
		DCXError("/xmenubar", "No callback alias initialised. See /xmenubar -M");
		return;
	}

	// Initialise callback.
	// xmenubar [-M] (ALIAS)
	if (flags['M']) {
		// Set alias.
		if (numtok > 1) {
			TString result((UINT) 100);
			TString alias(input.gettok(2));

			// Check if alias is valid.
			mIRCevalEX(result.to_chr(), 100, "$isalias(%s)", alias.to_chr());
			
			if (result == "$false") {
				DCXError("-M", "Invalid callback alias specified");
				return;
			}

			this->m_callback = alias;
		}
		// Reset alias and xmenubar.
		else {
			this->m_callback = "";
			this->resetMenuBar();
		}

		return;
	}

	menuBar = GetMenu(mIRCLink.m_mIRCHWND);

	// Add menu
	// xmenubar [-a] [MENU] [LABEL]
	if (flags['a']) {
		if (numtok < 3) {
			DCXError("-a", "Insufficient parameters");
			return;
		}

		menuName = input.gettok(2);
		p_Menu = g_XPopupMenuManager.getMenuByName(menuName, TRUE);

		if (!validateMenu(p_Menu, "-a", menuName))
			return;

		// Test if menu is already added.
		for (int i = 0; i < (int) m_vpXMenuBar.size(); i++) {
			if (m_vpXMenuBar[i] == p_Menu) {
				DCXError("-a", "Menu has already been added to XmenuBar.");
				return;
			}
		}

		p_Menu->attachToMenuBar(menuBar, input.gettok(3, -1));
	}
	// Removes menu
	// xmenubar [-d] [MENU]
	else if (flags['d']) {
		if (numtok < 2) {
			DCXError("-d", "Insufficient parameters");
			return;
		}

		menuName = input.gettok(2);
		p_Menu = g_XPopupMenuManager.getMenuByName(menuName, TRUE);

		if (!validateMenu(p_Menu, "-d", menuName))
			return;

		p_Menu->detachFromMenuBar(menuBar);
	}
	// Replace mIRC's menubar and generate our own
	// xmenubar [-g]
	// TODO: add support for styles if I can think of how.
	else if (flags['g']) {
		HMENU newMenu = CreateMenu();

		this->setMenuBar(menuBar, newMenu);

		// Redraws to include the system icons from MDI child.
		mIRCcom("//window -a \" $+ $active $+ \"");
	}
	// Change the label on the menu
	// xmenubar [-l] [MENU] [LABEL]
	else if (flags['l']) {
		if (numtok < 3) {
			DCXError("-l", "Insufficient parameters");
			return;
		}

		menuName = input.gettok(2);
		p_Menu = g_XPopupMenuManager.getMenuByName(menuName, TRUE);

		if (!validateMenu(p_Menu, "-l", menuName))
			return;

		int offset = this->findMenuOffset(menuBar, p_Menu);

		if (offset < 0) {
			TString error;
			error.sprintf("\"%s\" menu not found in XMenuBar.", p_Menu->getName().to_chr());
			DCXError("-l", error.to_chr());
			return;
		}

		ModifyMenu(menuBar, offset, MF_BYPOSITION, MF_STRING, input.gettok(3, -1).to_chr());
	}
	// Resets to original mIRC menubar
	// xmenubar [-r]
	else if (flags['r']) {
		this->resetMenuBar();
	}

	// Force redraw so the updates are shown.
	DrawMenuBar(mIRCLink.m_mIRCHWND);
}

/*
 *
 */
void XMenuBar::parseXMenuBarInfo(const TString &input, char *szReturnValue) {
	int numtok = input.numtok();
	TString prop(input.gettok(1));

	// Iterate through the names of menus added to XMenuBar.
	// N = 0 returns total number of menus
	// $xmenubar() [menu] [N]
	if (prop == "menu") {
		int i = input.gettok(2).to_int();

		if ((i < 0) || (i > (int) this->m_vpXMenuBar.size())) {
			TString error;
			error.sprintf("Invalid index: %d", i);
			DCXError("$!xpopup().menubar", error.to_chr());
			return;
		}

		// Return number of menus in menubar.
		if (i == 0)
			wsprintf(szReturnValue, "%d", (int) this->m_vpXMenuBar.size());
		// Return name of specified menu.
		else
			wsprintf(szReturnValue, "%s", this->m_vpXMenuBar[i -1]->getName());

		return;
	}

	szReturnValue[0] = 0;
}

/*
 * Adds the menu to the current menubar.
 */
bool XMenuBar::addToMenuBar(HMENU menubar, XPopupMenu *p_Menu, TString label) {
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
		menubar = GetMenu(mIRCLink.m_mIRCHWND);

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

	int offset = findMenuOffset(menubar, p_Menu);

	if (offset > 0)
		RemoveMenu(menubar, offset, MF_BYPOSITION);

	DrawMenuBar(mIRCLink.m_mIRCHWND);
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

	SetMenu(mIRCLink.m_mIRCHWND, newMenuBar);

	// Go through old menubar items and detach them
	VectorOfXPopupMenu temp;
	VectorOfXPopupMenu::iterator itStart = this->m_vpXMenuBar.begin();
	VectorOfXPopupMenu::iterator itEnd = this->m_vpXMenuBar.end();

	// Add menus to a temporary list to prevent errors in looping
	while (itStart != itEnd) {
		temp.push_back(*itStart);
		++itStart;
	}

	itStart = temp.begin();
	itEnd = temp.end();

	// Begin detaching ...
	while (itStart != itEnd) {
		(*itStart)->detachFromMenuBar(oldMenuBar);
		++itStart;
	}

	// Destroy the menu if it isnt the original mIRC menubar.
	if (g_OriginalMenuBar == NULL)
		g_OriginalMenuBar = oldMenuBar;
	else
		DestroyMenu(oldMenuBar);

	DrawMenuBar(mIRCLink.m_mIRCHWND);
}

/*
 *
 */
bool XMenuBar::validateMenu(const XPopupMenu *menu, const TString &flag, const TString &name) const {
	if (menu == NULL) {
		TString error;
		error.sprintf("Cannot find menu \"%s\".", name.to_chr());
		DCXError(flag.to_chr(), error.to_chr());
		return false;
	}
	// Prevent users from adding special menus.
	else if ((menu == g_mIRCPopupMenu) || (menu == g_mIRCMenuBar)) {
		DCXError(flag.to_chr(), "Cannot add \"mirc\" or \"mircbar\" menus.");
		return false;
	}

	return true;
}

/*
 *
 */
void XMenuBar::resetMenuBar() {
	if (g_OriginalMenuBar != NULL) {
		HMENU menubar = GetMenu(mIRCLink.m_mIRCHWND);

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
 * Returns 'true' it should halt default processing.
 *
 * User should return '$true' from the callback to prevent default processing.
 */
bool XMenuBar::parseCallback(const UINT menuID) {
	TString result((UINT)10);

	mIRCevalEX(result.to_chr(), 10, "$%s(%d)", this->m_callback.to_chr(), menuID);

	if (result == "$true")
		return true;

	return false;
}