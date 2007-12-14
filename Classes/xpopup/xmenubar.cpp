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
void XMenuBar::parseSwitchFlags(TString *switches, XSwitchFlags *flags) {
	// no -sign, missing params
	if ((*switches)[0] != '-')
		return;

	unsigned int i = 1, len = switches->len();

	while (i < len) {
		if ((*switches)[i] >= 'a' && (*switches)[i] <= 'z')
			flags->switch_flags[(int) ((*switches)[i] - 'a')] = 1;
		else if ((*switches)[i] >= 'A' && (*switches)[i] <= 'Z')
			flags->switch_cap_flags[(int) ((*switches)[i] - 'A')] = 1;

		i++;
	}
}

/*
 *
 */
void XMenuBar::parseXMenuBarCommand(const TString &input) {
	XSwitchFlags flags;
	int numtok = input.numtok();
	XPopupMenu *p_Menu;
	HMENU menuBar;
	TString menuName;

	ZeroMemory(&flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(&input.gettok(1), &flags);

	// Check if a callback alias has been marked
	if (!this->hasCallback() && !flags.switch_cap_flags[12])
	{
		DCXError("/xmenubar", "No callback alias initialised. See /xmenubar -M");
		return;
	}

	// Initialise callback.
	// xmenubar [-M] (ALIAS)
	if (flags.switch_cap_flags[12]) {
		// Set alias.
		if (numtok > 1) {
			TString alias;
			char res[10];
			TString result;

			// Check if alias is valid.
			alias.sprintf("$isalias(%s)", input.gettok(2));
			mIRCeval(alias.to_chr(), res, 10);
			result.sprintf("%s", res);
			
			if (result == "$false") {
				DCXError("-M", "Invalid callback alias specified");
				return;
			}

			this->m_callback = input.gettok(2);

			mIRCDebug("set alias = %s", m_callback.to_chr());
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
	if (flags.switch_flags[0]) {
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
	else if (flags.switch_flags[3]) {
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
	else if (flags.switch_flags[6]) {
		HMENU newMenu = CreateMenu();

		this->setMenuBar(menuBar, newMenu);

		// Redraws to include the system icons from MDI child.
		mIRCcom("//window -a \" $+ $active $+ \"");
	}
	// Change the label on the menu
	// xmenubar [-l] [MENU] [LABEL]
	else if (flags.switch_flags[11]) {
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
	else if (flags.switch_flags[17]) {
		this->resetMenuBar();
	}

	// Force redraw so the updates are shown.
	DrawMenuBar(mIRCLink.m_mIRCHWND);
}

/*
 *
 */
void XMenuBar::parseXMenuBarIdentifier(const TString &input, char *szReturnValue) {
	int numtok = input.numtok();
	TString prop(input.gettok(2));

	XPopupMenu *p_Menu = g_XPopupMenuManager.getMenuByName(input, TRUE);

	if (p_Menu == NULL) {
		TString error;
		error.sprintf("\"%s\" doesn't exist, see /xpopup -c", input.gettok(1).to_chr());
		DCXError("$!xmenubar()", error.to_chr());
		return;
	}
	else if ((p_Menu == g_mIRCPopupMenu) || (p_Menu == g_mIRCMenuBar))
	{
		DCXError("$!xmenubar()", "Invalid menu name : \"mirc\" or \"mircbar\" menus don't have access to this feature.");
		return;
	}

	//if (prop == "ismenu") {
	//	lstrcpy( szReturnValue, (p_Menu != NULL)?"$true":"$false" );
	//	return;
	//}
	if (prop == "menubar") {
		int i = input.gettok(1).to_int();

		if ((i < 0) || (i > (int) this->m_vpXMenuBar.size()))
		{
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
int XMenuBar::findMenuOffset(HMENU menubar, XPopupMenu *p_Menu) {
	MENUITEMINFO mii;
	int offset = 1;                    // Use 1 because 0 = the menubar itself when using GetMenuBarInfo()
	
	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU;

	// Whilst we can retrieve more menu items ...
	while (GetMenuItemInfo(menubar, offset, TRUE, &mii)) {
		// Continue if this isnt the menu we're after
		if (p_Menu->getMenuHandle() != mii.hSubMenu)
			offset++;
		else
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
bool XMenuBar::validateMenu(XPopupMenu *menu, TString flag, TString &name) {
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
bool XMenuBar::hasCallback() {
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
bool XMenuBar::parseCallback(UINT menuID) {
	TString alias;
	TString result;
	char res[10];

	mIRCevalEX(res, 10, "$%s(%d)", this->m_callback.to_chr(), menuID);

	result.sprintf("%s", mIRCLink.m_pData);
mIRCDebug("callback result = %s", result.to_chr());

	if (result == "$true")
		return true;

	return false;
}