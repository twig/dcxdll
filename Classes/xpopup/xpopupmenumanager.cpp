/*!
 * \file xpopupmenumanager.cpp
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
#include "Classes/xpopup/xpopupmenumanager.h"
#include "Dcx.h"



//extern HMENU g_OriginalMenuBar;
//extern XPopupMenu *g_mIRCScriptMenu;

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuManager::XPopupMenuManager()
: m_bPatched(false)
{
}

/*!
 * \brief Destructor
 *
 * blah
 */

XPopupMenuManager::~XPopupMenuManager() {
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::load(void)
{

	/***** XPopup Stuff *****/
	//GetClassInfoEx(NULL,"#32768",&wc); // menu
	//HWND tmp_hwnd = CreateWindowEx(0,"#32768",NULL,WS_POPUP,0,0,1,1,NULL,NULL,GetModuleHandle(NULL),NULL);
	//if (tmp_hwnd != NULL) {
	//	g_OldmIRCMenusWindowProc = (WNDPROC)SetClassLongPtr(tmp_hwnd,GCLP_WNDPROC,(LONG_PTR)mIRCMenusWinProc);
	//	DestroyWindow(tmp_hwnd);
	//}
	DCX_DEBUG(Dcx::debug,"LoadDLL", "Registering XPopup...");

	WNDCLASSEX wc;
	ZeroMemory((void*)&wc , sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = XPopupMenu::XPopupWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle( NULL );
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = XPOPUPMENUCLASS;
	wc.hIconSm       = NULL;
	RegisterClassEx(&wc);
	// TODO: delete or describe what that is for...
	//WNDCLASS wcpop;
	//ZeroMemory(&wcpop, sizeof(WNDCLASS));
	//wcpop.hInstance = GetModuleHandle(NULL);
	//wcpop.lpszClassName = XPOPUPMENUCLASS;
	//wcpop.lpfnWndProc = XPopupMenu::XPopupWinProc;
	//RegisterClass(&wcpop);

	DCX_DEBUG(Dcx::debug,"LoadDLL", "Creating menu owner...");
	m_hMenuOwner = CreateWindow(XPOPUPMENUCLASS, NULL, 0, 0, 0, 0, 0, (Dcx::XPPlusModule.isUseable() ? HWND_MESSAGE : 0), 0, GetModuleHandle(NULL), 0);

	m_mIRCPopupMenu = new XPopupMenu("mirc",(HMENU)NULL);
	m_mIRCMenuBar = new XPopupMenu("mircbar",GetMenu(Dcx::mIRC.getHWND()));


	// XMenuBar stuff
	HMENU menu = GetMenu(Dcx::mIRC.getHWND());
	int i = 0;
	const unsigned int buffSize = 30;
	TString label(buffSize);
	MENUITEMINFO mii;

	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.cch = buffSize;
	mii.dwTypeData = label.to_chr();

	while (GetMenuItemInfo(menu, i, TRUE, &mii)) {
		// We've found the tools menu, next one is the scriptable popup.
		if (label == "&Tools") {
			HMENU scriptable = GetSubMenu(menu, i +1);;

			// TODO: check if the next one is "&Window"
			g_mIRCScriptMenu = new XPopupMenu("scriptpopup", scriptable);
			break;
		}

		// Reset buffer size
		mii.cch = buffSize;
		i++;
	}
}

void XPopupMenuManager::unload(void)
{
	/***** XPopup Stuff *****/
	//HWND tmp_hwnd = CreateWindowEx(0,"#32768",NULL,WS_POPUP,0,0,1,1,NULL,NULL,GetModuleHandle(NULL),NULL);
	//if (tmp_hwnd != NULL) {
	//	SetClassLongPtr(tmp_hwnd,GCLP_WNDPROC,(LONG_PTR)g_OldmIRCMenusWindowProc);
	//	DestroyWindow(tmp_hwnd);
	//}
	Dcx::mIRC.resetWindowProc();

	clearMenus();
	delete m_mIRCPopupMenu;

	m_mIRCMenuBar->cleanMenu(GetMenu(Dcx::mIRC.getHWND()));
	delete m_mIRCMenuBar;

	if (m_hMenuOwner != NULL)
		DestroyWindow(m_hMenuOwner);

	UnregisterClass(XPOPUPMENUCLASS, GetModuleHandle(NULL));
}

LRESULT XPopupMenuManager::OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes;
	HMENU menu = (HMENU)wParam;
	bool isWinMenu = HIWORD(lParam) == TRUE ? true : false;
	HMENU currentMenubar = GetMenu(Dcx::mIRC.getHWND());
	bool switchMenu = (g_mIRCScriptMenu != NULL) &&                  // The mIRC scriptpopup menu has been wrapped,
		              (menu == g_mIRCScriptMenu->getMenuHandle()) && // The menu the same as the one just shown,
					  (currentMenubar != g_OriginalMenuBar) &&       // The menubar is our generated menubar,
					  (g_OriginalMenuBar != NULL);                   // And ensure it has been generated.

	if (!isWinMenu) {
		if (switchMenu)
			SetMenu(Dcx::mIRC.getHWND(), g_OriginalMenuBar);

		// let mIRC populate the menus dynamically
		lRes = Dcx::mIRC.callDefaultWindowProc(mHwnd, WM_INITMENUPOPUP, wParam, lParam);

		if (switchMenu)
			SetMenu(Dcx::mIRC.getHWND(), currentMenubar);

		if (isMenuBarMenu(GetMenu(mHwnd), menu)) {
			m_bIsMenuBar = true;

			if (m_bIsActiveMircMenubarPopup)
			{
				HWND hActive = (HWND)SendMessage(Dcx::mIRC.getMDIClient(), WM_MDIGETACTIVE, NULL, NULL);
				bool isCustomMenu = Dcx::XPopups.isCustomMenu(menu);

				// Store the handle of the menu being displayed.
				if (isCustomMenu && (m_hMenuCustom == NULL))
					m_hMenuCustom = menu;

				if (((!IsZoomed(hActive) || GetSystemMenu(hActive,FALSE) != menu)) && (!isCustomMenu) && (m_hMenuCustom == NULL)) // This checks for custom submenus.
				
				m_mIRCMenuBar->convertMenu(menu, TRUE);
			}
		}
		else {
			m_bIsMenuBar = false;

			if (m_bIsActiveMircPopup)
				m_mIRCPopupMenu->convertMenu(menu, FALSE);
		}

		m_bIsSysMenu = false;
		return lRes;
	}
	else
		m_bIsSysMenu = true;
	return Dcx::mIRC.callDefaultWindowProc(mHwnd, WM_INITMENUPOPUP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	HMENU menu = (HMENU) wParam;

	// Unset the custom menu handle so we dont have to keep track of submenus anymore.
	if (menu == m_hMenuCustom)
		m_hMenuCustom = NULL;

	if (m_bIsMenuBar && !m_bIsSysMenu && m_bIsActiveMircMenubarPopup)
		m_mIRCMenuBar->deleteAllItemData(menu);
	return Dcx::mIRC.callDefaultWindowProc(mHwnd, WM_UNINITMENUPOPUP, wParam, lParam);
}
	
LRESULT XPopupMenuManager::OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (!m_bIsMenuBar && m_bIsActiveMircPopup)
		m_mIRCMenuBar->clearAllMenuItems();
	return Dcx::mIRC.callDefaultWindowProc(mHwnd, WM_EXITMENULOOP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	// Check if the message came from the menubar
	if ((HIWORD(wParam) == 0) && (m_bIsMenuBar)) {
		if (Dcx::XMenubar.hasCallback()) {
			// If the user requested the message to be halted ...
			if (Dcx::XMenubar.parseCallback(LOWORD(wParam))) {
				// Stop parsing and prevent default action.
				return 0L;
			}
		}
		switch (LOWORD(wParam))
		{
		//case 110: // menubar, can't be re-enabled from menu obviously
		//	{
		//		if (IsWindowVisible(menubar))
		//			mIRCSignalDCX(dcxSignal.xdock, "menubar disabled");
		//		else
		//			mIRCSignalDCX(dcxSignal.xdock, "menubar enabled");
		//	}
		//	break;
		case 111: // toolbar
			{
				if (IsWindowVisible(Dcx::mIRC.getToolbar()))
					Dcx::mIRC.signalex(dcxSignal.xdock, "toolbar disabled");
				else
					Dcx::mIRC.signalex(dcxSignal.xdock, "toolbar enabled");
			}
			break;
		case 112: // switchbar
			{
				if (IsWindowVisible(Dcx::mIRC.getSwitchbar()))
					Dcx::mIRC.signalex(dcxSignal.xdock, "switchbar disabled");
				else
					Dcx::mIRC.signalex(dcxSignal.xdock, "switchbar enabled");
			}
			break;
		case 210: // treebar
			{
				if (IsWindowVisible(Dcx::mIRC.getTreebar()))
					Dcx::mIRC.signalex(dcxSignal.xdock, "treebar disabled");
				else
					Dcx::mIRC.signalex(dcxSignal.xdock, "treebar enabled");
			}
			break;
		default:
			break;
		}
	}
	return Dcx::mIRC.callDefaultWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::parseCommand(const TString & input) {
	XPopupMenu *p_Menu;
	XSwitchFlags flags(input.gettok(2));

	// Special mIRC Menu
	if ((p_Menu = this->getMenuByName(input.gettok(1), TRUE)) == NULL && !flags['c']) {
		Dcx::errorex("/xpopup", "\"%s\" doesn't exist : see /xpopup -c", input.gettok(1).to_chr());
		return;
	}

	parseCommand(input, p_Menu);
}

void XPopupMenuManager::parseCommand( const TString & input, XPopupMenu *p_Menu ) {
	XSwitchFlags flags(input.gettok(2));
	int numtok = input.numtok( );

	// xpopup -b - [MENU] [SWITCH] [FILENAME]
	if (flags['b']) {
		HBITMAP hBitmap = NULL;

		if (numtok > 2) {
			TString filename(input.gettok(3, -1 ).trim());

			if (filename == "none") {
				// ignore 'none' to maintain compatibility
			}
			else if (IsFile(filename)) {
				hBitmap = dcxLoadBitmap(hBitmap, filename);

				if (hBitmap == NULL)
					Dcx::errorex("/xpopup -b", "Unable to Load Image: %s", filename.to_chr());
			}
			else
				Dcx::errorex("/xpopup -b", "Unable to Access File: %s", filename.to_chr());
		}

		p_Menu->setBackBitmap( hBitmap );

	}
	// xpopup -c -> [MENU] [SWITCH] [STYLE]
	else if ((flags['c']) && (numtok > 2) && (input.gettok( 1 ) != "mirc" || input.gettok( 1 ) != "mircbar")) {

		if (p_Menu != NULL)
			Dcx::errorex("/xpopup -c", "\"%s\" already exists", input.gettok(1).to_chr());
		else {
			XPopupMenu::MenuStyle style = XPopupMenu::parseStyle(input.gettok(3));
			this->m_vpXPMenu.push_back(new XPopupMenu(input.gettok(1), style));
		}
	}
	// xpopup -d -> [MENU] [SWITCH]
	else if ( flags['d'] && ( input.gettok( 1 ) != "mirc" || input.gettok( 1 ) != "mircbar" ) ) {

		this->deleteMenu( p_Menu );
	}
	// xpopup -i -> [MENU] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if ( flags['i'] && numtok > 4 ) {
		HIMAGELIST himl = p_Menu->getImageList( );
		HICON icon;
		int index;

		index = input.gettok( 4 ).to_int( );
		TString filename(input.gettok( 5, -1 ));
		if (IsFile(filename)) {
			icon = dcxLoadIcon(index, filename, false, input.gettok( 3 ));
			if (icon != NULL) {
				ImageList_AddIcon( himl, icon );
				DestroyIcon( icon );
			}
			else
				Dcx::error("/xpopup -i","Unable to Load Icon");
		}
		else
			Dcx::error("/xpopup -i","Unable to Access File");
	}
	// xpopup -j -> [MENU] [SWITCH]
	else if (flags['j']) {
		p_Menu->destroyImageList();
	}
	// xpopup -l -> [MENU] [SWITCH] [N] [COLOR | default]
	else if ( flags['l'] && numtok > 3 ) {

		int nColor = input.gettok( 3 ).to_int( );
		TString clr(input.gettok( 4 ));

		if (clr == "default")
			p_Menu->setDefaultColor( nColor );
		else
			p_Menu->setColor( nColor, (COLORREF)clr.to_num( ) );
	}
	// xpopup -m -> mirc -m
	else if ( flags['m'] && numtok == 2 && input.gettok( 1 ) == "mirc") {
		if (!this->m_bPatched && Dcx::mIRC.isVersion(6,20)) {
			XPopupMenuManager::InterceptAPI(GetModuleHandle(NULL), "User32.dll", "TrackPopupMenu", (DWORD)XPopupMenuManager::XTrackPopupMenu, (DWORD)XPopupMenuManager::TrampolineTrackPopupMenu, 5);
			XPopupMenuManager::InterceptAPI(GetModuleHandle(NULL), "User32.dll", "TrackPopupMenuEx", (DWORD)XPopupMenuManager::XTrackPopupMenuEx, (DWORD)XPopupMenuManager::TrampolineTrackPopupMenuEx, 5);
			this->m_bPatched = true;
		}
	}
	// xpopup -M -> [MENU] [SWITCH] (TEXT)
	else if (flags['M']) {
		p_Menu->setMarkedText(input.gettok(3, -1));
	}
	// xpopup -p -> [MENU] [SWITCH] [COLORS]
	else if ( flags['p'] && numtok > 2 ) {

		TString colors(input.gettok( 3, -1 ));
		int i = 1, len = colors.numtok( );

		while ( i <= len ) {

			if (colors.gettok( i ) == "default")
				p_Menu->setDefaultColor( i );
			else
				p_Menu->setColor( i, (COLORREF)colors.gettok( i ).to_num( ) );
			++i;
		}
	}
	// xpopup -s -> [MENU] [SWITCH] [+FLAGS] [X] [Y] (OVER HWND)
	else if ( flags['s'] && numtok > 4 ) {

		UINT mflags = this->parseTrackFlags( input.gettok( 3 ) );
		int x = input.gettok( 4 ).to_int( );
		int y = input.gettok( 5 ).to_int( );

		/*
		Add offsetting for multiple monitor based on supplied hwnd this menu is to be associated with
		*/
		HWND hTrack = (HWND)input.gettok( 6 ).to_num();

		if (hTrack != NULL && IsWindow(hTrack)) {
			// map window relative pos ($mouse.x/y) to screen pos for TrackPopupMenuEx()
			POINT pt;
			pt.x = x;
			pt.y = y;
			MapWindowPoints(hTrack, NULL, &pt, 1);
			x = pt.x;
			y = pt.y;
		}
		else {
			// Adjust relative location to take multi-monitor into account
			HMONITOR hMon;
			MONITORINFO mi;
			hMon = MonitorFromWindow(Dcx::mIRC.getHWND(), MONITOR_DEFAULTTONEAREST);

			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMon, &mi);

			x += mi.rcMonitor.left;
			y += mi.rcMonitor.top;
		}

		UINT ID = TrackPopupMenuEx( p_Menu->getMenuHandle( ), TPM_RETURNCMD | mflags, x, y, m_hMenuOwner, NULL );

		Dcx::mIRC.execex("//.signal -n XPopup-%s %d", p_Menu->getName( ).to_chr( ), ID );
	}
	// xpopup -t -> [MENU] [SWITCH] [STYLE]
	else if (flags['t'] && numtok > 2) {
		XPopupMenu::MenuStyle style = XPopupMenu::parseStyle(input.gettok(3));

		p_Menu->setStyle(style);
	}
	// xpopup -x -> [MENU] [SWITCH] [+FLAGS]
	else if ( flags['x'] && numtok > 2 ) {

		TString flag(input.gettok( 3 ));

		if ( flag[0] == '+' ) {

			UINT iStyles = 0;
			int i = 1, len = (int)flag.len( );
			while ( i <= len ) {

				switch (flag[i]) {
					case 'i':
						iStyles |= XPS_ICON3D;
						break;
					case 'd':
						iStyles |= XPS_DISABLEDSEL;
						break;
					case 'p':
						iStyles |= XPS_ICON3DSHADOW;
						break;
					default:
						break;
				}

				++i;
			}

			p_Menu->setItemStyle( iStyles );
		}
	}
	// xpopup -R -> [MENU] [SWITCH] [+FLAGS] (FLAG OPTIONS)
	else if ( flags['R'] && numtok > 2 ) {

		TString flag(input.gettok( 3 ));

		if ( flag[0] == '+' ) {
			switch (flag[1]) {
				case 'r': // Set Rounded Selector on/off
					{
						p_Menu->SetRounded(((input.gettok( 4 ).to_int() > 0) ? true : false));
					}
					break;
				case 'a': // Set Alpha value of menu. 0-255
					{
						BYTE alpha = (BYTE)(input.gettok( 4 ).to_int() & 0xFF);

						if (alpha > 255)
							alpha = 255;

						p_Menu->SetAlpha(alpha);
					}
					break;
				default:
					break;
			}
		}
	}
}

/*!
 * \brief blah
 *
 * blah [MENU] [PROP]
 */

void XPopupMenuManager::parseIdentifier( const TString & input, char * szReturnValue ) {

	int numtok = input.numtok( );
	TString prop(input.gettok( 2 ));

	XPopupMenu * p_Menu = this->getMenuByName(input.gettok(1), TRUE);

	if ((p_Menu == NULL) && (prop != "ismenu") && (prop != "menuname") && (prop != "menubar")) {
		Dcx::errorex("$!xpopup()", "\"%s\" doesn't exist, see /xpopup -c", input.gettok(1).to_chr());
		return;
	}

	if (prop == "ismenu") {
		lstrcpy( szReturnValue, (p_Menu != NULL)?"$true":"$false" );
		return;
	}
	else if (prop == "menuname") {
		int i = input.gettok(1).to_int();

		if ((i < 0) || (i > (int) this->m_vpXPMenu.size()))
		{
			Dcx::errorex("$!xpopup().menuname", "Invalid index: %d", i);
			return;
		}

		// Return number of menus.
		if (i == 0)
			wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, "%d", (int) this->m_vpXPMenu.size());
		// Return name of specified menu.
		else
			lstrcpyn(szReturnValue, this->m_vpXPMenu[i -1]->getName().to_chr(), MIRC_BUFFER_SIZE_CCH);
			//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, "%s", this->m_vpXPMenu[i -1]->getName().to_chr());

		return;
	}
	else if ( prop == "style" ) {

		switch (p_Menu->getStyle( )) {
			case XPopupMenu::XPMS_OFFICE2003:
				lstrcpy( szReturnValue, "office2003" );
				break;
			case XPopupMenu::XPMS_OFFICE2003_REV:
				lstrcpy( szReturnValue, "office2003rev" );
				break;
			case XPopupMenu::XPMS_OFFICEXP:
				lstrcpy( szReturnValue, "officeXP" );
				break;
			case XPopupMenu::XPMS_ICY:
				lstrcpy( szReturnValue, "icy" );
				break;
			case XPopupMenu::XPMS_ICY_REV:
				lstrcpy( szReturnValue, "icyrev" );
				break;
			case XPopupMenu::XPMS_GRADE:
				lstrcpy( szReturnValue, "grade" );
				break;
			case XPopupMenu::XPMS_GRADE_REV:
				lstrcpy( szReturnValue, "graderev" );
				break;
			case XPopupMenu::XPMS_VERTICAL:
				lstrcpy( szReturnValue, "vertical" );
				break;
			case XPopupMenu::XPMS_VERTICAL_REV:
				lstrcpy( szReturnValue, "verticalrev" );
				break;
			case XPopupMenu::XPMS_NORMAL:
				lstrcpy( szReturnValue, "normal" );
				break;
			case XPopupMenu::XPMS_CUSTOM:
				lstrcpy( szReturnValue, "custom" );
				break;
			default:
				lstrcpy( szReturnValue, "unknown" );
				break;
		}
		return;
	}
	else if ( prop == "exstyle" ) {

		TString styles("+");
		UINT iExStyles = p_Menu->getItemStyle( );

		if ( iExStyles & XPS_ICON3D )
			styles += 'i';
		if ( iExStyles & XPS_DISABLEDSEL )
			styles += 'd';
		if ( iExStyles & XPS_ICON3DSHADOW )
			styles += 'p';

		lstrcpy( szReturnValue, styles.to_chr( ) );
		return;
	}
	else if ( prop == "colors" ) {

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", p_Menu->getColor( 1 ), p_Menu->getColor( 2 ),
			p_Menu->getColor( 3 ), p_Menu->getColor( 3 ), p_Menu->getColor( 5 ), p_Menu->getColor( 6 ),
			p_Menu->getColor( 7 ), p_Menu->getColor( 8 ), p_Menu->getColor( 9 ), p_Menu->getColor( 10 ), p_Menu->getColor( 11 ) );
		return;

	}
	else if ( prop == "color" && numtok > 2 ) {

		int nColor = input.gettok( 3 ).to_int( );
		if ( nColor > 0 && nColor < 11 ) {

			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%ld", p_Menu->getColor( nColor ) );
			return;
		}
	}
	else if ( prop == "isrounded") {
		lstrcpy( szReturnValue, (p_Menu->IsRounded() ? "$true" : "$false"));
		return;
	}
	else if ( prop == "alpha") {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, "%ld", p_Menu->IsAlpha());
		return;
	}
	else if (prop == "marked") {
		//wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, "%s", p_Menu->getMarkedText().to_chr());
		lstrcpyn(szReturnValue, p_Menu->getMarkedText().to_chr(), MIRC_BUFFER_SIZE_CCH);
		return;
	}
	szReturnValue[0] = 0;
}
	
int XPopupMenuManager::parseMPopup(const TString & input)
{
	if (input.numtok( ) < 2) {
		Dcx::error("/mpopup","Invalid arguments");
		return 0;
	}

	if (input.gettok( 1 ) == "mirc") {
		if (input.gettok( 2 ) == "1")
			m_bIsActiveMircPopup = true;
		else
			m_bIsActiveMircPopup = false;
	}
	else if (input.gettok( 1 ) == "mircbar") {
		if (input.gettok( 2 ) == "1")
			m_bIsActiveMircMenubarPopup = true;
		else {
			m_bIsActiveMircMenubarPopup = false;
			m_mIRCMenuBar->cleanMenu(GetMenu(Dcx::mIRC.getHWND()));
		}
	}
	return 3;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::addMenu( XPopupMenu * p_Menu ) {

  if ( p_Menu != NULL )
    this->m_vpXPMenu.push_back( p_Menu );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::deleteMenu( XPopupMenu * p_Menu ) {

  VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin( );
  VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end( );

  while ( itStart != itEnd ) {

    if ( *itStart == p_Menu ) {

      delete *itStart;
      this->m_vpXPMenu.erase( itStart );
      return;
    }

    ++itStart;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::clearMenus( ) {

  VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin( );
  VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end( );

  while ( itStart != itEnd ) {

    if ( *itStart != NULL )
      delete *itStart;

    ++itStart;
  }
}

void XPopupMenuManager::setIsMenuBar(bool value)
{
	m_bIsMenuBar = value;
}

/*!
 * \brief blah
 *
 * blah
 */

XPopupMenu * XPopupMenuManager::getMenuByName(const TString &tsName, BOOL checkSpecial) { 
	if (checkSpecial) {
		if (tsName == "mircbar")
			return m_mIRCMenuBar;
		else if (tsName == "mirc")
			return Dcx::XPopups.getmIRCPopup();
		else if (tsName == "scriptpopup")
			return g_mIRCScriptMenu;
	}

	VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin();
	VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end();

	while (itStart != itEnd) {
		if (*itStart != NULL && (*itStart)->getName() == tsName)
			return *itStart;

		++itStart;
	}

	return NULL;
}

/*
 * Retrieves a menu by the handle.
 */
XPopupMenu* XPopupMenuManager::getMenuByHandle(const HMENU hMenu) {
	// Special cases
	if (hMenu == m_mIRCMenuBar->getMenuHandle())
		return m_mIRCMenuBar;
	else if (hMenu == Dcx::XPopups.getmIRCPopup()->getMenuHandle())
		return Dcx::XPopups.getmIRCPopup();
	else if (hMenu == g_mIRCScriptMenu->getMenuHandle())
		return g_mIRCScriptMenu;

	VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin();
	VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end();

	while (itStart != itEnd) {
		if (*itStart != NULL && (hMenu == (*itStart)->getMenuHandle()))
			return *itStart;

		++itStart;
	}

	return NULL;
}

XPopupMenu* XPopupMenuManager::getmIRCPopup(void)
{
	return m_mIRCPopupMenu;
}
XPopupMenu* XPopupMenuManager::getmIRCMenuBar(void)
{
	return m_mIRCMenuBar;
}

/*
 * Check if menu handle is a custom menu (don't include converted mIRC menus)
 */
bool XPopupMenuManager::isCustomMenu(const HMENU hMenu) {

	VectorOfXPopupMenu::iterator itStart = this->m_vpXPMenu.begin();
	VectorOfXPopupMenu::iterator itEnd = this->m_vpXPMenu.end();

	while (itStart != itEnd) {
		if (*itStart != NULL && (hMenu == (*itStart)->getMenuHandle()))
			return true;

		++itStart;
	}

	return false;
}

bool XPopupMenuManager::isMenuBarMenu(const HMENU hMenu, const HMENU hMatch) {
	HMENU hTemp;
	int i = 0, n = GetMenuItemCount(hMenu);

	while (i < n) {
		if ((hTemp = GetSubMenu(hMenu, i)) != NULL) {
			if (hTemp == hMatch)
				return true;

			if (isMenuBarMenu(hTemp, hMatch))
				return true;
		}

		++i;
	}

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

UINT XPopupMenuManager::parseTrackFlags( const TString & flags ) {

	UINT iFlags = 0;

	if ( flags[0] == '+' ) {

		int i = 1, len = (int)flags.len( );

		while ( i < len ) {

			switch (flags[i])
			{
			case 'b':
				iFlags |= TPM_BOTTOMALIGN;
				break;
			case 'c':
				iFlags |= TPM_CENTERALIGN;
				break;
			case 'l':
				iFlags |= TPM_LEFTALIGN;
				break;
			case 'm':
				iFlags |= TPM_LEFTBUTTON;
				break;
			case 'n':
				iFlags |= TPM_RIGHTBUTTON;
				break;
			case 'r':
				iFlags |= TPM_RIGHTALIGN;
				break;
			case 't':
				iFlags |= TPM_TOPALIGN;
				break;
			case 'v':
				iFlags |= TPM_VCENTERALIGN;
				break;
			}

			++i;
		}
	}

	return iFlags;
}
/*
	Following code taken from ODMenu class & modified for XPopup
	CODMenu class
	Code copyright: R.I.Allen for plug-in stuff
	Most owner drawn menu code copyright Brent Corcum - modified by RIA
*/
BOOL WINAPI XPopupMenuManager::XTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect)
{
	// Remove the No Notify flag. This fixes the popupmenus on mIRC 6.20
	uFlags &= ~TPM_NONOTIFY;
	return XPopupMenuManager::TrampolineTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}
BOOL WINAPI XPopupMenuManager::XTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
{
	// Remove the No Notify flag. This fixes the popupmenus on mIRC 6.20
	fuFlags &= ~TPM_NONOTIFY;
	return XPopupMenuManager::TrampolineTrackPopupMenuEx(hMenu, fuFlags, x, y, hwnd, lptpm);
}
BOOL XPopupMenuManager::InterceptAPI(HMODULE hLocalModule, const char* c_szDllName, const char* c_szApiName, DWORD dwReplaced, DWORD dwTrampoline, int offset)
{
	int i;
	DWORD dwOldProtect;
	DWORD dwAddressToIntercept = (DWORD)GetProcAddress(GetModuleHandle((char*)c_szDllName), (char*)c_szApiName);

	BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
	BYTE *pbReplaced = (BYTE *) dwReplaced;
	BYTE *pbTrampoline = (BYTE *) dwTrampoline;

	// Change the protection of the trampoline region
	// so that we can overwrite the first 5 + offset bytes.
	if (*pbTrampoline == 0xe9)
	{
		// target function starts with an ansolute jump
		// change tramoline to the target of the jump
		pbTrampoline++;
		int * pbOffset = (int*)pbTrampoline;
		pbTrampoline += *pbOffset + 4;
	}
	VirtualProtect((void *) pbTrampoline, 5+offset, PAGE_WRITECOPY, &dwOldProtect);
	for (i=0;i<offset;i++)
		*pbTrampoline++ = *pbTargetCode++;
	pbTargetCode = (BYTE *) dwAddressToIntercept;

	// Insert unconditional jump in the trampoline.
	*pbTrampoline++ = 0xE9;        // jump rel32
	*((signed int *)(pbTrampoline)) = (pbTargetCode+offset) - (pbTrampoline + 4);
	VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_EXECUTE, &dwOldProtect);

	// Overwrite the first 5 bytes of the target function
	VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_WRITECOPY, &dwOldProtect);

	// check to see whether we need to translate the pbReplaced pointer
	if (*pbReplaced == 0xe9)
	{
		pbReplaced++;
		int * pbOffset = (int*)pbReplaced;
		pbReplaced += *pbOffset + 4;
	}
	*pbTargetCode++ = 0xE9;        // jump rel32
	*((signed int *)(pbTargetCode)) = pbReplaced - (pbTargetCode +4);
	VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_EXECUTE, &dwOldProtect);

	// Flush the instruction cache to make sure 
	// the modified code is executed.
	FlushInstructionCache(GetCurrentProcess(), NULL, NULL);
	return TRUE;
}

BOOL WINAPI XPopupMenuManager::TrampolineTrackPopupMenu(
	HMENU hMenu,         // handle to shortcut menu
	UINT uFlags,         // options
	int x,               // horizontal position
	int y,               // vertical position
	int nReserved,       // reserved, must be zero
	HWND hWnd,           // handle to owner window
	CONST RECT *prcRect  // ignored
)
{
	// this procedure needs to be at least 10 bytes in length
	// it gets overwritten using self modifying code
	// it does not matter what the code is here
	double  a;
	double  b;

	a = 0.0;
	b = 1.0;
	a = a / b;
	return (a > 0);
}

BOOL WINAPI XPopupMenuManager::TrampolineTrackPopupMenuEx(
	HMENU hMenu,       // handle to shortcut menu
	UINT fuFlags,      // options
	int x,             // horizontal position
	int y,             // vertical position
	HWND hwnd,         // handle to window
	LPTPMPARAMS lptpm  // area not to overlap
)
{
	// this procedur eneeds to be at least 10 bytes in length
	// it gets overwritten using self modifying code
	// it does not matter what the code is here
	double  a;
	double  b;

	a = 0.0;
	b = 1.0;
	a = a / b;
	return (a > 0);
}

/*
 * Parses the menu information and returns a valid XPopupMenu.
 */
void XPopupMenuManager::LoadPopupsFromXML(const TiXmlElement *popups, const TiXmlElement *popup, const TString &popupName, const TString &popupDataset) {
	const TiXmlElement *globalStyles;
	const TiXmlElement *element;
	int totalIndexes;
	XPopupMenu::MenuStyle style;
	const char* attr;

	// Find the dataset with the name we want.
	for (element = popups->FirstChildElement("popup"); element != NULL; element = element->NextSiblingElement("popup")) {
		TString name(element->Attribute("name"));

		if (name == popupDataset) {
			popup = element;
			break;
		}
	}

	// Dataset not found.
	if (popup == NULL) {
		Dcx::errorex("/dcxml", "No Popup Dataset %s", popupDataset.to_chr());
		return;
	}

	XPopupMenu *menu = Dcx::XPopups.getMenuByName(popupName, FALSE);

	// Destroy a menu which already exists
	if (menu != NULL)
		Dcx::XPopups.deleteMenu(menu);

	// Find global styles branch
	globalStyles = popups->FirstChildElement("styles");

	// Move to "all" branch
	if (globalStyles != NULL)
		globalStyles = globalStyles->FirstChildElement("all");
	else
		globalStyles = NULL;

	// Create menu with style (from specific or global)
	attr = GetMenuAttributeFromXML("style", popup, globalStyles);
	style = XPopupMenu::parseStyle(attr);
	menu = new XPopupMenu(popupName, style);

	const TString colors("bgcolour iconcolour cbcolour discbcolour disselcolour distextcolour selcolour selbordercolour seperatorcolour textcolour seltextcolour");
	totalIndexes = colors.numtok();

	for (int i = 1; i <= totalIndexes; i++) {
		attr = GetMenuAttributeFromXML(colors.gettok(i).to_chr(), popup, globalStyles);

		if (attr != NULL) {
			TString tsBuff;
			Dcx::mIRC.tsEval(tsBuff, attr);
			menu->setColor(i, (COLORREF)tsBuff.to_int());
		}
	}

	// Set background image if CUSTOM style used
	if (style == XPopupMenu::XPMS_CUSTOM) {
		TString filename;

		Dcx::mIRC.tsEval(filename, popup->Attribute("background"));

		HBITMAP hBitmap = dcxLoadBitmap(NULL, filename);

		if (hBitmap != NULL)
			menu->setBackBitmap(hBitmap);
	}

	// Successfully created a menu.
	Dcx::XPopups.m_vpXPMenu.push_back(menu);

	// Parse icons
	element = popup->FirstChildElement("icons");

	if (element != NULL) {
		const char *tmp;
		TString command;
		TString flags;
		TString indexes;
		int nIcon;

		for (element = element->FirstChildElement("icon"); element != NULL; element = element->NextSiblingElement("icon")) {
			// Flags
			tmp = element->Attribute("flags");

			if (tmp == NULL)
				flags = "+";
			else
				flags = tmp;

			// Filename
			TString tsFilename;
			Dcx::mIRC.tsEval(tsFilename, element->Attribute("src"));

			tmp = element->Attribute("index");

			if (tmp == NULL)
				indexes = "0";
			else
				indexes = tmp;

			totalIndexes = indexes.numtok(",");

			for (int i = 1; i <= totalIndexes; i++) {
				nIcon = indexes.gettok(i, TSCOMMA).to_int();
				//xpudemo -i + 114 dcxstudio_gfx\shell.dll
				command.tsprintf("%s -i %s %d %s", popupName.to_chr(), flags.to_chr(), nIcon, tsFilename.to_chr());
				Dcx::XPopups.parseCommand(command, menu);
			}
		}
	}

	LoadPopupItemsFromXML(menu, menu->getMenuHandle(), popup);
}

/*
 * Function to append submenu items into a menu.
 * This method is recursive in order to parse submenus correctly.
 */
bool XPopupMenuManager::LoadPopupItemsFromXML(XPopupMenu *menu, HMENU hMenu, const TiXmlElement *items) {

	if ((menu == NULL) || (items == NULL) || (hMenu == NULL))
		return false;

	// Iterate through each child element.
	for (const TiXmlElement *element = items->FirstChildElement("item"); element != NULL; element = element->NextSiblingElement("item")) {
		MENUITEMINFO mii;
		int nPos = GetMenuItemCount(hMenu) +1;

		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);

		XPopupMenuItem *item = NULL;
		TString caption(element->Attribute("caption"));

		if (caption == "-") {
			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
			mii.fType = MFT_OWNERDRAW | MFT_SEPARATOR;

			item = new XPopupMenuItem(menu, TRUE);
		}
		else {
			int mID = TString(element->Attribute("id")).to_int();
			int mIcon = (element->Attribute("icon") != NULL ? TString(element->Attribute("icon")).to_int() -1 : -1);
			TString state(element->Attribute("state"));

			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
			mii.fType = MFT_OWNERDRAW;
			mii.wID = mID;

			// Checked
			if (state.find('c', 0))
				mii.fState |= MFS_CHECKED;
			
			// Gray
			if (state.find('g', 0))
				mii.fState |= MFS_GRAYED;

			// Submenu
			if (element->FirstChildElement("item") != NULL) {
				if (mii.hSubMenu != NULL)
					DestroyMenu(mii.hSubMenu);

				mii.fMask |= MIIM_SUBMENU;
				mii.hSubMenu = CreatePopupMenu();

				XPopupMenuManager::LoadPopupItemsFromXML(menu, mii.hSubMenu, element);
			}

			// TODO: command
			item = new XPopupMenuItem(menu, caption, mIcon, mii.hSubMenu != NULL ? TRUE : FALSE);
		}

		if (item != NULL) {
			menu->m_vpMenuItem.push_back(item);
			mii.dwItemData = (ULONG_PTR) item;
			InsertMenuItem(hMenu, nPos, TRUE, &mii);
		}
	}

	return true;
}

const char* XPopupMenuManager::GetMenuAttributeFromXML(const char *attrib, const TiXmlElement *popup, const TiXmlElement *global) {
	const char* tmp;
	
	tmp = popup->Attribute(attrib);

	// Specific menu attribute set, ignore global.
	if (tmp != NULL)
		return tmp;

	// Try to find global style.
	if (global == NULL)
		return NULL;

	return global->Attribute(attrib);
}
