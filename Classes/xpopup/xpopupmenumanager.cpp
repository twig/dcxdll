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

//#ifdef DEBUG
//WNDPROC XPopupMenuManager::g_OldmIRCMenusWindowProc = nullptr;
//#endif

/*!
 * \brief Constructor
 *
 * blah
 */

XPopupMenuManager::XPopupMenuManager()
: m_mIRCPopupMenu(nullptr)
, m_mIRCMenuBar(nullptr)
, m_bIsActiveMircMenubarPopup(false)
, m_bIsActiveMircPopup(false)
, m_bIsMenuBar(false)
, m_bIsSysMenu(false)
, m_hMenuCustom(nullptr)
, m_hMenuOwner(nullptr)
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
	WNDCLASSEX wc;
	ZeroMemory((void*)&wc , sizeof(WNDCLASSEX));

//#ifdef DEBUG
//	//wc.cbSize = sizeof(WNDCLASSEX);
//	//GetClassInfoEx(nullptr,TEXT("#32768"),&wc); // menu
//	HWND tmp_hwnd = CreateWindowEx(0,TEXT("#32768"),nullptr,WS_POPUP,0,0,1,1,nullptr,nullptr,GetModuleHandle(nullptr),nullptr);
//	if (tmp_hwnd != nullptr) {
//		g_OldmIRCMenusWindowProc = (WNDPROC)SetClassLongPtr(tmp_hwnd, GCLP_WNDPROC, (LONG_PTR)XPopupMenuManager::mIRCMenusWinProc);
//		DestroyWindow(tmp_hwnd);
//		DCX_DEBUG(Dcx::debug,TEXT("LoadDLL"), TEXT("Subclassed Menu Class"));
//	}
//#endif

	DCX_DEBUG(mIRCLinker::debug,TEXT("LoadDLL"), TEXT("Registering XPopup..."));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = XPopupMenu::XPopupWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle( nullptr );
	wc.hIcon         = nullptr;
	wc.hCursor       = nullptr;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = XPOPUPMENUCLASS;
	wc.hIconSm       = nullptr;
	RegisterClassEx(&wc);

	DCX_DEBUG(mIRCLinker::debug,TEXT("LoadDLL"), TEXT("Creating menu owner..."));
	m_hMenuOwner = CreateWindow(XPOPUPMENUCLASS, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, GetModuleHandle(nullptr), 0);

	m_mIRCPopupMenu = new XPopupMenu(TEXT("mirc"),(HMENU)nullptr);
	m_mIRCMenuBar = new XPopupMenu(TEXT("mircbar"),GetMenu(mIRCLinker::getHWND()));


	// XMenuBar stuff
	auto menu = GetMenu(mIRCLinker::getHWND());
	//int i = 0;
	const UINT buffSize = 30U;
	TString label(buffSize);
	MENUITEMINFO mii;

	ZeroMemory(&mii, sizeof(MENUITEMINFO));
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.cch = buffSize;
	mii.dwTypeData = label.to_chr();

	//while (GetMenuItemInfo(menu, i, TRUE, &mii)) {
	//	// We've found the tools menu, next one is the scriptable popup.
	//	if (label == TEXT("&Tools")) {
	//		HMENU scriptable = GetSubMenu(menu, i +1);;
	//
	//		// TODO: check if the next one is "&Window"
	//		g_mIRCScriptMenu = new XPopupMenu(TEXT("scriptpopup"), scriptable);
	//		break;
	//	}
	//
	//	// Reset buffer size
	//	mii.cch = buffSize;
	//	i++;
	//}

	for (UINT i = 0U; GetMenuItemInfo(menu, i, TRUE, &mii); i++)
	{
		// We've found the tools menu, next one is the scriptable popup.
		if (label == TEXT("&Tools")) {
			auto scriptable = GetSubMenu(menu, static_cast<int>(i + 1));

			// TODO: check if the next one is "&Window"
			g_mIRCScriptMenu = new XPopupMenu(TEXT("scriptpopup"), scriptable);
			break;
		}

		// Reset buffer size
		mii.cch = buffSize;
	}
}

void XPopupMenuManager::unload(void)
{
	/***** XPopup Stuff *****/
//#ifdef DEBUG
//	if (XPopupMenuManager::g_OldmIRCMenusWindowProc != nullptr) {
//		HWND tmp_hwnd = CreateWindowEx(0,TEXT("#32768"),nullptr,WS_POPUP,0,0,1,1,nullptr,nullptr,GetModuleHandle(nullptr),nullptr);
//		if (tmp_hwnd != nullptr) {
//			SetClassLongPtr(tmp_hwnd, GCLP_WNDPROC, (LONG_PTR)XPopupMenuManager::g_OldmIRCMenusWindowProc);
//			DestroyWindow(tmp_hwnd);
//			XPopupMenuManager::g_OldmIRCMenusWindowProc = nullptr;
//		}
//	}
//#endif

	//mIRCLinker::resetWindowProc();

	clearMenus();
	delete m_mIRCPopupMenu;

	m_mIRCMenuBar->cleanMenu(GetMenu(mIRCLinker::getHWND()));
	delete m_mIRCMenuBar;

	if (m_hMenuOwner != nullptr)
		DestroyWindow(m_hMenuOwner);

	UnregisterClass(XPOPUPMENUCLASS, GetModuleHandle(nullptr));
}

LRESULT XPopupMenuManager::OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	auto menu = reinterpret_cast<HMENU>(wParam);
	const auto isWinMenu = (HIWORD(lParam) != FALSE);
	auto currentMenubar = GetMenu(mIRCLinker::getHWND());
	const auto switchMenu = (g_mIRCScriptMenu != nullptr) &&                  // The mIRC scriptpopup menu has been wrapped,
		              (menu == g_mIRCScriptMenu->getMenuHandle()) && // The menu the same as the one just shown,
					  (currentMenubar != g_OriginalMenuBar) &&       // The menubar is our generated menubar,
					  (g_OriginalMenuBar != nullptr);                   // And ensure it has been generated.

	if (!isWinMenu) {
		if (switchMenu)
			SetMenu(mIRCLinker::getHWND(), g_OriginalMenuBar);

		// let mIRC populate the menus dynamically
		auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, WM_INITMENUPOPUP, wParam, lParam);

		if (switchMenu)
			SetMenu(mIRCLinker::getHWND(), currentMenubar);

		if (isMenuBarMenu(GetMenu(mHwnd), menu)) {
			m_bIsMenuBar = true;

			if (m_bIsActiveMircMenubarPopup)
			{
				auto hActive = (HWND)SendMessage(mIRCLinker::getMDIClient(), WM_MDIGETACTIVE, 0L, 0L);
				const bool isCustomMenu = Dcx::XPopups.isCustomMenu(menu);

				// Store the handle of the menu being displayed.
				if (isCustomMenu && (m_hMenuCustom == nullptr))
					m_hMenuCustom = menu;

				if (((!IsZoomed(hActive) || GetSystemMenu(hActive,FALSE) != menu)) && (!isCustomMenu) && (m_hMenuCustom == nullptr)) // This checks for custom submenus.
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
	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_INITMENUPOPUP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	auto menu = reinterpret_cast<HMENU>(wParam);

	// Unset the custom menu handle so we dont have to keep track of submenus anymore.
	if (menu == m_hMenuCustom)
		m_hMenuCustom = nullptr;

	if (m_bIsMenuBar && !m_bIsSysMenu && m_bIsActiveMircMenubarPopup)
		m_mIRCMenuBar->deleteAllItemData(menu);
	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_UNINITMENUPOPUP, wParam, lParam);
}
	
LRESULT XPopupMenuManager::OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (!m_bIsMenuBar && m_bIsActiveMircPopup)
		m_mIRCMenuBar->clearAllMenuItems();
	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_EXITMENULOOP, wParam, lParam);
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
		//			mIRCSignalDCX(dcxSignal.xdock, TEXT("menubar disabled"));
		//		else
		//			mIRCSignalDCX(dcxSignal.xdock, TEXT("menubar enabled"));
		//	}
		//	break;
		case 111: // toolbar
			{
				if (IsWindowVisible(mIRCLinker::getToolbar()))
					mIRCLinker::signalex(dcxSignal.xdock, TEXT("toolbar disabled"));
				else
					mIRCLinker::signalex(dcxSignal.xdock, TEXT("toolbar enabled"));
			}
			break;
		case 112: // switchbar
			{
				if (IsWindowVisible(mIRCLinker::getSwitchbar()))
					mIRCLinker::signalex(dcxSignal.xdock, TEXT("switchbar disabled"));
				else
					mIRCLinker::signalex(dcxSignal.xdock, TEXT("switchbar enabled"));
			}
			break;
		case 210: // treebar
			{
				if (IsWindowVisible(mIRCLinker::getTreebar()))
					mIRCLinker::signalex(dcxSignal.xdock, TEXT("treebar disabled"));
				else
					mIRCLinker::signalex(dcxSignal.xdock, TEXT("treebar enabled"));
			}
			break;
		default:
			break;
		}
	}
	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::parseCommand(const TString & input) {
	auto p_Menu = this->getMenuByName(input.getfirsttok(1), true);	// tok 1
	const XSwitchFlags flags(input.getnexttok());	// tok 2

	// Special mIRC Menu
	if (p_Menu == nullptr && !flags[TEXT('c')])
		throw Dcx::dcxException(TEXT("\"%\" doesn't exist : see /xpopup -c"), input.gettok(1));

	parseCommand(input, p_Menu);
}

void XPopupMenuManager::parseCommand( const TString & input, XPopupMenu *const p_Menu ) {
	const auto tsMenuName(input.getfirsttok( 1 ));
	const XSwitchFlags flags(input.getnexttok( ));	// tok 2
	const auto numtok = input.numtok();

	// xpopup -b - [MENU] [SWITCH] [FILENAME]
	if (flags[TEXT('b')]) {
		HBITMAP hBitmap = nullptr;

		if (numtok > 2) {
			auto filename(input.getlasttoks().trim());	// tok 3, -1

			if (filename == TEXT("none")) {
				// ignore 'none' to maintain compatibility
			}
			else {
				if (!IsFile(filename))
					throw Dcx::dcxException(TEXT("Unable to Access File: %"), filename);
				
				hBitmap = dcxLoadBitmap(hBitmap, filename);

				if (hBitmap == nullptr)
					throw Dcx::dcxException(TEXT("Unable to Load Image: %"), filename);
			}
		}

		p_Menu->setBackBitmap( hBitmap );

	}
	// xpopup -c -> [MENU] [SWITCH] [STYLE]
	else if ((flags[TEXT('c')]) && (numtok > 2) && (tsMenuName != TEXT("mirc") || tsMenuName != TEXT("mircbar"))) {

		if (p_Menu != nullptr)
			throw Dcx::dcxException(TEXT("\"%\" already exists"), tsMenuName);

		auto style = XPopupMenu::parseStyle(input.getnexttok());	// tok 3
		this->m_vpXPMenu.push_back(new XPopupMenu(tsMenuName, style));
	}
	// xpopup -d -> [MENU] [SWITCH]
	else if ( flags[TEXT('d')] && ( tsMenuName != TEXT("mirc") || tsMenuName != TEXT("mircbar") ) ) {

		this->deleteMenu( p_Menu );
	}
	// xpopup -i -> [MENU] -i [FLAGS] [INDEX] [FILENAME]
	else if ( flags[TEXT('i')] && numtok > 4 ) {
		auto himl = p_Menu->getImageList();
		const auto tsFlags(input.getnexttok());			// tok 3
		const auto index = input.getnexttok().to_int();	// tok 4
		auto filename(input.getlasttoks());				// tok 5, -1

		//const HICON icon = dcxLoadIcon(index, filename, false, tsFlags);
		//if (icon == nullptr)
		//	throw Dcx::dcxException(TEXT("Unable to Load Icon: % in file: %"), index, filename);
		//
		//ImageList_AddIcon(himl, icon);
		//DestroyIcon(icon);

		Dcx::dcxIcon icon(index, filename, false, tsFlags);

		ImageList_AddIcon(himl, icon);
	}
	// xpopup -j -> [MENU] [SWITCH]
	else if (flags[TEXT('j')]) {
		p_Menu->destroyImageList();
	}
	// xpopup -l -> [MENU] [SWITCH] [N] [COLOR | default]
	else if ( flags[TEXT('l')] && numtok > 3 ) {

		const auto nColor = XPopupMenu::MenuColours(input.getnexttok( ).to_<UINT>( ));	// tok 3
		const auto clr(input.getnexttok());				// tok 4

		if (clr == TEXT("default"))
			p_Menu->setDefaultColor(nColor);
		else
			p_Menu->setColor(nColor, clr.to_<COLORREF>());
	}
	// xpopup -m -> mirc -m
	else if ( flags[TEXT('m')] && numtok == 2 && tsMenuName == TEXT("mirc")) {
		// do nothing in utf dll as this dll is mirc v7+ only.
		//if (!this->m_bPatched && mIRCLinker::isVersion(6,20)) {
		//	XPopupMenuManager::InterceptAPI(GetModuleHandle(nullptr), TEXT("User32.dll"), "TrackPopupMenu", (DWORD)XPopupMenuManager::XTrackPopupMenu, (DWORD)XPopupMenuManager::TrampolineTrackPopupMenu, 5);
		//	XPopupMenuManager::InterceptAPI(GetModuleHandle(nullptr), TEXT("User32.dll"), "TrackPopupMenuEx", (DWORD)XPopupMenuManager::XTrackPopupMenuEx, (DWORD)XPopupMenuManager::TrampolineTrackPopupMenuEx, 5);
		//	this->m_bPatched = true;
		//}
	}
	// xpopup -M -> [MENU] [SWITCH] (TEXT)
	else if (flags[TEXT('M')]) {
		p_Menu->setMarkedText(input.getlasttoks());	// tok 3, -1
	}
	// xpopup -p -> [MENU] [SWITCH] [COLORS]
	else if ( flags[TEXT('p')] && numtok > 2 ) {

		const auto colors(input.getlasttoks());	// tok 3, -1

		auto i = 1U;
		for (const auto &tmp : colors)
		{
			if (tmp == TEXT("default"))
				p_Menu->setDefaultColor(XPopupMenu::MenuColours(i));
			else
				p_Menu->setColor(XPopupMenu::MenuColours(i), tmp.to_<COLORREF>());
			++i;
		}
	}
	// xpopup -s -> [MENU] [SWITCH] [+FLAGS] [X] [Y] (OVER HWND)
	else if ( flags[TEXT('s')] && numtok > 4 ) {

		const auto mflags = this->parseTrackFlags(input.getnexttok());	// tok 3
		auto x = input.getnexttok().to_int();								// tok 4
		auto y = input.getnexttok().to_int();								// tok 5

		/*
		Add offsetting for multiple monitor based on supplied hwnd this menu is to be associated with
		*/
		auto hTrack = (HWND)input.getnexttok().to_num();	// tok 6

		if (hTrack != nullptr && IsWindow(hTrack)) {
			// map window relative pos ($mouse.x/y) to screen pos for TrackPopupMenuEx()
			POINT pt;
			pt.x = x;
			pt.y = y;
			MapWindowPoints(hTrack, nullptr, &pt, 1);
			x = pt.x;
			y = pt.y;
		}
		else {
			// Adjust relative location to take multi-monitor into account
			MONITORINFO mi;
			auto hMon = MonitorFromWindow(mIRCLinker::getHWND(), MONITOR_DEFAULTTONEAREST);

			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMon, &mi);

			x += mi.rcMonitor.left;
			y += mi.rcMonitor.top;
		}

		const auto ID = TrackPopupMenuEx(p_Menu->getMenuHandle(), TPM_RETURNCMD | mflags, x, y, m_hMenuOwner, nullptr);

		mIRCLinker::execex(TEXT("//.signal -n XPopup-%s %d"), p_Menu->getName( ).to_chr( ), ID );
	}
	// xpopup -t -> [MENU] [SWITCH] [STYLE]
	else if (flags[TEXT('t')] && numtok > 2) {
		auto style = XPopupMenu::parseStyle(input.getnexttok());	// tok 3

		p_Menu->setStyle(style);
	}
	// xpopup -x -> [MENU] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('x')] && numtok > 2 ) {

		const XSwitchFlags xflags(input.getnexttok( ));	// tok 3

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Missing '+' in front of flags");

		UINT iStyles = 0;
		if (xflags[TEXT('i')])
			iStyles |= XPS_ICON3D;
		if (xflags[TEXT('d')])
			iStyles |= XPS_DISABLEDSEL;
		if (xflags[TEXT('p')])
			iStyles |= XPS_ICON3DSHADOW;

		p_Menu->setItemStyle( iStyles );
	}
	// xpopup -R -> [MENU] [SWITCH] [+FLAGS] (FLAG OPTIONS)
	else if ( flags[TEXT('R')] && numtok > 2 ) {

		const XSwitchFlags xflags(input.getnexttok( ));	// tok 3

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Missing '+' in front of flags");

		if (xflags[TEXT('r')]) // Set Rounded Selector on/off
			p_Menu->SetRounded(((input.getnexttok( ).to_int() > 0) ? true : false));	// tok 4
		else if (xflags[TEXT('a')]) // Set Alpha value of menu. 0-255
		{
			const auto alpha = (BYTE)(input.getnexttok().to_int() & 0xFF);	// tok 4

			p_Menu->SetAlpha(alpha);
		}
	}
}

/*!
 * \brief blah
 *
 * blah [MENU] [PROP]
 */

void XPopupMenuManager::parseIdentifier( const TString & input, TCHAR *const szReturnValue ) const
{
	const auto numtok = input.numtok();
	const auto tsMenuName(input.getfirsttok(1));
	const auto prop(input.getnexttok());	// tok 2

	const auto p_Menu = this->getMenuByName(tsMenuName, true);

	static const TString propList(TEXT("ismenu menuname menubar style exstyle colors color isrounded alpha marked"));
	const auto nType = propList.findtok(prop, 1);

	if ((p_Menu == nullptr) && (nType > 3))
		throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

	switch (nType) {
	case 1: // ismenu
		{
			dcx_Con(p_Menu != nullptr, szReturnValue);
		}
		break;
	case 2: // menuname
		{
			const auto i = tsMenuName.to_dword();

			if (i > this->m_vpXPMenu.size())
				throw Dcx::dcxException(TEXT("Invalid index: %"), i);

			// Return number of menus.
			if (i == 0)
				wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_vpXPMenu.size());
			// Return name of specified menu.
			else
				dcx_strcpyn(szReturnValue, this->m_vpXPMenu[i -1]->getName().to_chr(),MIRC_BUFFER_SIZE_CCH);
		}
		break;
	case 3: // menubar
		break;
	case 4: // style
		{
			switch (p_Menu->getStyle( )) {
				case XPopupMenu::XPMS_OFFICE2003:
					dcx_strcpyn( szReturnValue, TEXT("office2003"), MIRC_BUFFER_SIZE_CCH );
					break;
				case XPopupMenu::XPMS_OFFICE2003_REV:
					dcx_strcpyn(szReturnValue, TEXT("office2003rev"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_OFFICEXP:
					dcx_strcpyn(szReturnValue, TEXT("officeXP"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_ICY:
					dcx_strcpyn(szReturnValue, TEXT("icy"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_ICY_REV:
					dcx_strcpyn(szReturnValue, TEXT("icyrev"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_GRADE:
					dcx_strcpyn(szReturnValue, TEXT("grade"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_GRADE_REV:
					dcx_strcpyn(szReturnValue, TEXT("graderev"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_VERTICAL:
					dcx_strcpyn(szReturnValue, TEXT("vertical"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_VERTICAL_REV:
					dcx_strcpyn(szReturnValue, TEXT("verticalrev"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_NORMAL:
					dcx_strcpyn(szReturnValue, TEXT("normal"), MIRC_BUFFER_SIZE_CCH);
					break;
				case XPopupMenu::XPMS_CUSTOM:
					dcx_strcpyn(szReturnValue, TEXT("custom"), MIRC_BUFFER_SIZE_CCH);
					break;
				default:
					dcx_strcpyn(szReturnValue, TEXT("unknown"), MIRC_BUFFER_SIZE_CCH);
					break;
			}
		}
		break;
	case 5: // exstyle
		{
			TString styles(TEXT('+'));
			const auto iExStyles = p_Menu->getItemStyle();

			if (dcx_testflag(iExStyles, XPS_ICON3D))
				styles += TEXT('i');
			if (dcx_testflag(iExStyles, XPS_DISABLEDSEL))
				styles += TEXT('d');
			if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
				styles += TEXT('p');

			dcx_strcpyn( szReturnValue, styles.to_chr( ), MIRC_BUFFER_SIZE_CCH );
		}
		break;
	case 6: // colors
		{
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu"), p_Menu->getColor( XPopupMenu::MenuColours::XPMC_BACKGROUND ), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_ICONBOX),
				p_Menu->getColor(XPopupMenu::MenuColours::XPMC_CHECKBOX), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_CHECKBOX_DISABLED), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX_DISABLED), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_TEXT_DISABLED),
				p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX_BORDER), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SEPARATOR), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_TEXT), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTEDTEXT) );
		}
		break;
	case 7: // color
		{
			if (numtok > 2) {
				const auto nColor = XPopupMenu::MenuColours(input.getnexttok().to_<UINT>());	// tok 3
				if ( nColor > 0 && nColor < XPopupMenu::MenuColours::XPMC_MAX )
					wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), p_Menu->getColor( nColor ) );
			}
			else
				szReturnValue[0] = 0;

		}
		break;
	case 8: // isrounded
		{
			dcx_Con(p_Menu->IsRounded(), szReturnValue);
		}
		break;
	case 9: // alpha
		{
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), p_Menu->IsAlpha());
		}
		break;
	case 10: // marked
		{
			dcx_strcpyn(szReturnValue, p_Menu->getMarkedText().to_chr(), MIRC_BUFFER_SIZE_CCH);
		}
		break;
	case 0:
	default:
		throw Dcx::dcxException(TEXT("Unknown prop \"%\""), prop);
	}

	//if ((p_Menu == nullptr) && ((prop != TEXT("ismenu") && (prop != TEXT("menuname")) && (prop != TEXT("menubar")))) {
	//	Dcx::errorex(TEXT("$!xpopup()"), TEXT("\"%s\" doesn't exist, see /xpopup -c"), tsMenuName.to_chr());
	//	return;
	//}
	//
	//if (prop == TEXT("ismenu")) {
	//	lstrcpyn( szReturnValue, ((p_Menu != nullptr)?TEXT("$true"):TEXT("$false")), MIRC_BUFFER_SIZE_CCH );
	//	return;
	//}
	//else if (prop == TEXT("menuname")) {
	//	const int i = tsMenuName.to_int();
	//
	//	if ((i < 0) || (i > (int) this->m_vpXPMenu.size()))
	//	{
	//		Dcx::errorex(TEXT("$!xpopup().menuname"), TEXT("Invalid index: %d"), i);
	//		return;
	//	}
	//
	//	// Return number of menus.
	//	if (i == 0)
	//		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d"), (int) this->m_vpXPMenu.size());
	//	// Return name of specified menu.
	//	else
	//		lstrcpyn(szReturnValue, this->m_vpXPMenu[i -1]->getName().to_chr(),MIRC_BUFFER_SIZE_CCH);
	//
	//	return;
	//}
	//else if ( prop == TEXT("style") ) {
	//
	//	switch (p_Menu->getStyle( )) {
	//		case XPopupMenu::XPMS_OFFICE2003:
	//			lstrcpyn( szReturnValue, TEXT("office2003"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_OFFICE2003_REV:
	//			lstrcpyn( szReturnValue, TEXT("office2003rev"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_OFFICEXP:
	//			lstrcpyn( szReturnValue, TEXT("officeXP"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_ICY:
	//			lstrcpyn( szReturnValue, TEXT("icy"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_ICY_REV:
	//			lstrcpyn( szReturnValue, TEXT("icyrev"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_GRADE:
	//			lstrcpyn( szReturnValue, TEXT("grade"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_GRADE_REV:
	//			lstrcpyn( szReturnValue, TEXT("graderev"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_VERTICAL:
	//			lstrcpyn( szReturnValue, TEXT("vertical"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_VERTICAL_REV:
	//			lstrcpyn( szReturnValue, TEXT("verticalrev"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_NORMAL:
	//			lstrcpyn( szReturnValue, TEXT("normal"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		case XPopupMenu::XPMS_CUSTOM:
	//			lstrcpyn( szReturnValue, TEXT("custom"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//		default:
	//			lstrcpyn( szReturnValue, TEXT("unknown"), MIRC_BUFFER_SIZE_CCH );
	//			break;
	//	}
	//	return;
	//}
	//else if ( prop == TEXT("exstyle") ) {
	//
	//	TString styles(TEXT('+'));
	//	const UINT iExStyles = p_Menu->getItemStyle( );
	//
	//	if (dcx_testflag(iExStyles, XPS_ICON3D))
	//		styles += TEXT('i');
	//	if (dcx_testflag(iExStyles, XPS_DISABLEDSEL))
	//		styles += TEXT('d');
	//	if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
	//		styles += TEXT('p');
	//
	//	lstrcpyn( szReturnValue, styles.to_chr( ), MIRC_BUFFER_SIZE_CCH );
	//	return;
	//}
	//else if ( prop == TEXT("colors") ) {
	//
	//	wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld"), p_Menu->getColor( 1 ), p_Menu->getColor( 2 ),
	//		p_Menu->getColor( 3 ), p_Menu->getColor( 3 ), p_Menu->getColor( 5 ), p_Menu->getColor( 6 ),
	//		p_Menu->getColor( 7 ), p_Menu->getColor( 8 ), p_Menu->getColor( 9 ), p_Menu->getColor( 10 ), p_Menu->getColor( 11 ) );
	//	return;
	//
	//}
	//else if ( prop == TEXT("color") && numtok > 2 ) {
	//
	//	const int nColor = input.getnexttok( ).to_int( );	// tok 3
	//	if ( nColor > 0 && nColor < 11 ) {
	//
	//		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), p_Menu->getColor( nColor ) );
	//		return;
	//	}
	//}
	//else if ( prop == TEXT("isrounded")) {
	//	lstrcpyn( szReturnValue, ((p_Menu->IsRounded() ? TEXT("$true") : TEXT("$false"))), MIRC_BUFFER_SIZE_CCH);
	//	return;
	//}
	//else if ( prop == TEXT("alpha")) {
	//	wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%ld"), p_Menu->IsAlpha());
	//	return;
	//}
	//else if (prop == TEXT("marked")) {
	//	lstrcpyn(szReturnValue, p_Menu->getMarkedText().to_chr(), MIRC_BUFFER_SIZE_CCH);
	//	return;
	//}
	//szReturnValue[0] = 0;
}

const int XPopupMenuManager::parseMPopup(const TString & input)
{
	if (input.numtok( ) < 2)
		throw Dcx::dcxException("Invalid Arguments");

	const auto tsMenuName(input.getfirsttok(1));
	const auto iEnable = input.getnexttok().to_int();	// tok 2

	if (tsMenuName == TEXT("mirc"))
		m_bIsActiveMircPopup = (iEnable == 1);
	else if (tsMenuName == TEXT("mircbar")) {
		if (iEnable == 1)
			m_bIsActiveMircMenubarPopup = true;
		else {
			m_bIsActiveMircMenubarPopup = false;
			m_mIRCMenuBar->cleanMenu(GetMenu(mIRCLinker::getHWND()));
		}
	}
	return 3;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::addMenu( XPopupMenu *const p_Menu ) {

  if ( p_Menu != nullptr )
    this->m_vpXPMenu.push_back( p_Menu );
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::deleteMenu( const XPopupMenu *const p_Menu ) {

	if (this->m_vpXPMenu.empty())
		return;

	//auto itStart = this->m_vpXPMenu.begin();
	//auto itEnd = this->m_vpXPMenu.end();
	//
	//while (itStart != itEnd) {
	//
	//	if (*itStart == p_Menu) {
	//
	//		delete *itStart;
	//		this->m_vpXPMenu.erase(itStart);
	//		return;
	//	}
	//
	//	++itStart;
	//}

	const auto itEnd = this->m_vpXPMenu.end();
	const auto itGot = std::find(this->m_vpXPMenu.begin(), itEnd, p_Menu);
	if (itGot != itEnd)
	{
		delete *itGot;
		this->m_vpXPMenu.erase(itGot);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::clearMenus( ) {

	for (auto &a: this->m_vpXPMenu)
		delete a;

	this->m_vpXPMenu.clear();
}

void XPopupMenuManager::setIsMenuBar(const bool value)
{
	m_bIsMenuBar = value;
}

/*!
 * \brief blah
 *
 * blah
 */

XPopupMenu * XPopupMenuManager::getMenuByName(const TString &tsName, const bool checkSpecial) const { 
	if (checkSpecial) {
		if (tsName == TEXT("mircbar"))
			return m_mIRCMenuBar;
		else if (tsName == TEXT("mirc"))
			return Dcx::XPopups.getmIRCPopup();
		else if (tsName == TEXT("scriptpopup"))
			return g_mIRCScriptMenu;
	}

	for (const auto &x: this->m_vpXPMenu) {
		if (x != nullptr) {
			if (x->getName() == tsName)
				return x;
		}
	}
	return nullptr;
}

/*
 * Retrieves a menu by the handle.
 */
XPopupMenu* XPopupMenuManager::getMenuByHandle(const HMENU hMenu) const {
	// Special cases
	if (hMenu == m_mIRCMenuBar->getMenuHandle())
		return m_mIRCMenuBar;
	else if (hMenu == Dcx::XPopups.getmIRCPopup()->getMenuHandle())
		return Dcx::XPopups.getmIRCPopup();
	else if (hMenu == g_mIRCScriptMenu->getMenuHandle())
		return g_mIRCScriptMenu;

	for (const auto &x: this->m_vpXPMenu) {
		if (x != nullptr) {
			if (hMenu == x->getMenuHandle())
				return x;
		}
	}
	return nullptr;
}

XPopupMenu* XPopupMenuManager::getmIRCPopup(void) const
{
	return m_mIRCPopupMenu;
}

XPopupMenu* XPopupMenuManager::getmIRCMenuBar(void) const
{
	return m_mIRCMenuBar;
}

/*
 * Check if menu handle is a custom menu (don't include converted mIRC menus)
 */
const bool XPopupMenuManager::isCustomMenu(const HMENU hMenu) const {
	for (const auto &x: this->m_vpXPMenu) {
		if (x != nullptr) {
			if (hMenu == x->getMenuHandle())
				return true;
		}
	}
	return false;
}

const bool XPopupMenuManager::isMenuBarMenu(const HMENU hMenu, const HMENU hMatch) {
	const auto n = GetMenuItemCount(hMenu);

	for (auto i = decltype(n){0}; i < n; i++)
	{
		auto hTemp = GetSubMenu(hMenu, i);
		if (hTemp != nullptr) {
			if (hTemp == hMatch)
				return true;

			if (isMenuBarMenu(hTemp, hMatch))
				return true;
		}
	}

	return false;
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT XPopupMenuManager::parseTrackFlags( const TString & flags ) {

	UINT iFlags = 0;
	const XSwitchFlags xflags(flags);

	if ( !xflags[TEXT('+')] )
		return 0;

	if (xflags[TEXT('b')])
		iFlags |= TPM_BOTTOMALIGN;
	if (xflags[TEXT('c')])
		iFlags |= TPM_CENTERALIGN;
	if (xflags[TEXT('l')])
		iFlags |= TPM_LEFTALIGN;
	if (xflags[TEXT('m')])
		iFlags |= TPM_LEFTBUTTON;
	if (xflags[TEXT('n')])
		iFlags |= TPM_RIGHTBUTTON;
	if (xflags[TEXT('r')])
		iFlags |= TPM_RIGHTALIGN;
	if (xflags[TEXT('t')])
		iFlags |= TPM_TOPALIGN;
	if (xflags[TEXT('v')])
		iFlags |= TPM_VCENTERALIGN;

	return iFlags;
}
///*
//	Following code taken from ODMenu class & modified for XPopup
//	CODMenu class
//	Code copyright: R.I.Allen for plug-in stuff
//	Most owner drawn menu code copyright Brent Corcum - modified by RIA
//*/
//BOOL WINAPI XPopupMenuManager::XTrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT * prcRect)
//{
//	// Remove the No Notify flag. This fixes the popupmenus on mIRC 6.20
//	uFlags &= ~TPM_NONOTIFY;
//	return XPopupMenuManager::TrampolineTrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
//}
//BOOL WINAPI XPopupMenuManager::XTrackPopupMenuEx(HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm)
//{
//	// Remove the No Notify flag. This fixes the popupmenus on mIRC 6.20
//	fuFlags &= ~TPM_NONOTIFY;
//	return XPopupMenuManager::TrampolineTrackPopupMenuEx(hMenu, fuFlags, x, y, hwnd, lptpm);
//}
//BOOL XPopupMenuManager::InterceptAPI(HMODULE hLocalModule, const TCHAR* c_szDllName, const char* c_szApiName, DWORD dwReplaced, DWORD dwTrampoline, int offset)
//{
//	int i;
//	DWORD dwOldProtect;
//	DWORD dwAddressToIntercept = (DWORD)GetProcAddress(GetModuleHandle(c_szDllName), c_szApiName);
//
//	BYTE *pbTargetCode = (BYTE *) dwAddressToIntercept;
//	BYTE *pbReplaced = (BYTE *) dwReplaced;
//	BYTE *pbTrampoline = (BYTE *) dwTrampoline;
//
//	// Change the protection of the trampoline region
//	// so that we can overwrite the first 5 + offset bytes.
//	if (*pbTrampoline == 0xe9)
//	{
//		// target function starts with an ansolute jump
//		// change tramoline to the target of the jump
//		pbTrampoline++;
//		int * pbOffset = (int*)pbTrampoline;
//		pbTrampoline += *pbOffset + 4;
//	}
//	VirtualProtect((void *) pbTrampoline, 5+offset, PAGE_WRITECOPY, &dwOldProtect);
//	for (i=0;i<offset;i++)
//		*pbTrampoline++ = *pbTargetCode++;
//	pbTargetCode = (BYTE *) dwAddressToIntercept;
//
//	// Insert unconditional jump in the trampoline.
//	*pbTrampoline++ = 0xE9;        // jump rel32
//	*((signed int *)(pbTrampoline)) = (pbTargetCode+offset) - (pbTrampoline + 4);
//	VirtualProtect((void *) dwTrampoline, 5+offset, PAGE_EXECUTE, &dwOldProtect);
//
//	// Overwrite the first 5 bytes of the target function
//	VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_WRITECOPY, &dwOldProtect);
//
//	// check to see whether we need to translate the pbReplaced pointer
//	if (*pbReplaced == 0xe9)
//	{
//		pbReplaced++;
//		int * pbOffset = (int*)pbReplaced;
//		pbReplaced += *pbOffset + 4;
//	}
//	*pbTargetCode++ = 0xE9;        // jump rel32
//	*((signed int *)(pbTargetCode)) = pbReplaced - (pbTargetCode +4);
//	VirtualProtect((void *) dwAddressToIntercept, 5, PAGE_EXECUTE, &dwOldProtect);
//
//	// Flush the instruction cache to make sure 
//	// the modified code is executed.
//	FlushInstructionCache(GetCurrentProcess(), nullptr, nullptr);
//	return TRUE;
//}
//
//BOOL WINAPI XPopupMenuManager::TrampolineTrackPopupMenu(
//	HMENU hMenu,         // handle to shortcut menu
//	UINT uFlags,         // options
//	int x,               // horizontal position
//	int y,               // vertical position
//	int nReserved,       // reserved, must be zero
//	HWND hWnd,           // handle to owner window
//	CONST RECT *prcRect  // ignored
//)
//{
//	// this procedure needs to be at least 10 bytes in length
//	// it gets overwritten using self modifying code
//	// it does not matter what the code is here
//	double  a;
//	double  b;
//
//	a = 0.0;
//	b = 1.0;
//	a = a / b;
//	return (a > 0);
//}
//
//BOOL WINAPI XPopupMenuManager::TrampolineTrackPopupMenuEx(
//	HMENU hMenu,       // handle to shortcut menu
//	UINT fuFlags,      // options
//	int x,             // horizontal position
//	int y,             // vertical position
//	HWND hwnd,         // handle to window
//	LPTPMPARAMS lptpm  // area not to overlap
//)
//{
//	// this procedur eneeds to be at least 10 bytes in length
//	// it gets overwritten using self modifying code
//	// it does not matter what the code is here
//	double  a;
//	double  b;
//
//	a = 0.0;
//	b = 1.0;
//	a = a / b;
//	return (a > 0);
//}

/*
 * Parses the menu information and returns a valid XPopupMenu.
 */
void XPopupMenuManager::LoadPopupsFromXML(const TiXmlElement *const popups, const TiXmlElement *popup, const TString &popupName, const TString &popupDataset) {

	// Find the dataset with the name we want.
	for (auto element = popups->FirstChildElement("popup"); element != nullptr; element = element->NextSiblingElement("popup")) {
		const TString name(element->Attribute("name"));

		if (name == popupDataset) {
			popup = element;
			break;
		}
	}

	// Dataset not found.
	if (popup == nullptr)
		throw Dcx::dcxException(TEXT("No Popup Dataset %"), popupDataset);

	auto menu = Dcx::XPopups.getMenuByName(popupName, false);

	// Destroy a menu which already exists
	if (menu != nullptr)
		Dcx::XPopups.deleteMenu(menu);

	// Find global styles branch
	auto globalStyles = popups->FirstChildElement("styles");

	// Move to TEXT("all") branch
	if (globalStyles != nullptr)
		globalStyles = globalStyles->FirstChildElement("all");
	else
		globalStyles = nullptr;

	// Create menu with style (from specific or global)
	auto style = XPopupMenu::parseStyle(GetMenuAttributeFromXML("style", popup, globalStyles));
	menu = new XPopupMenu(popupName, style);

	const TString colors(TEXT("bgcolour iconcolour cbcolour discbcolour disselcolour distextcolour selcolour selbordercolour seperatorcolour textcolour seltextcolour"));

	UINT i = 1;
	for (const auto &tmp: colors)
	{
		const auto attr = GetMenuAttributeFromXML(tmp.c_str(), popup, globalStyles);	// tok i

		if (attr != nullptr) {
			TString tsBuff(attr);
			mIRCLinker::tsEval(tsBuff, tsBuff.to_chr());
			menu->setColor(XPopupMenu::MenuColours(i), tsBuff.to_<COLORREF>());
		}
		++i;
	}

	// Set background image if CUSTOM style used
	if (style == XPopupMenu::XPMS_CUSTOM) {
		const TString tsBkg(popup->Attribute("background"));

		if (!tsBkg.empty())
		{
			TString filename;

			mIRCLinker::tsEval(filename, tsBkg.to_chr());

			auto hBitmap = dcxLoadBitmap(nullptr, filename);

			if (hBitmap != nullptr)
				menu->setBackBitmap(hBitmap);
		}
	}

	// Successfully created a menu.
	Dcx::XPopups.m_vpXPMenu.push_back(menu);

	// Parse icons
	auto element = popup->FirstChildElement("icons");

	if (element != nullptr) {
		TString command;
		TString tsFilename;

		for (element = element->FirstChildElement("icon"); element != nullptr; element = element->NextSiblingElement("icon")) {
			// Flags
			const TString flags(queryAttribute(element, "flags", "+"));
			const TString tsSrc(element->Attribute("src"));
			const TString indexes(queryAttribute(element, "index", "0"));

			// Filename
			if (!tsSrc.empty())
			{
				mIRCLinker::tsEval(tsFilename, tsSrc.to_chr());

				if (!tsFilename.empty())
				{
					for (auto itStart = indexes.begin(TSCOMMA), itEnd = indexes.end(); itStart != itEnd; ++itStart)
					{
						// does (*itStart) NEED to be converted to an int?
						//command.tsprintf(TEXT("%s -i %s %d %s"), popupName.to_chr(), flags.to_chr(), (*itStart).to_int(), tsFilename.to_chr());
						command.tsprintf(TEXT("%s -i %s %s %s"), popupName.to_chr(), flags.to_chr(), (*itStart).to_chr(), tsFilename.to_chr());
						Dcx::XPopups.parseCommand(command, menu);
					}
					tsFilename.clear();
				}
			}
		}
	}

	if (!LoadPopupItemsFromXML(menu, menu->getMenuHandle(), popup))
		throw Dcx::dcxException(TEXT("Unable to load menu items: %"), popupName);
}

/*
 * Function to append submenu items into a menu.
 * This method is recursive in order to parse submenus correctly.
 */
const bool XPopupMenuManager::LoadPopupItemsFromXML(XPopupMenu *menu, HMENU hMenu, const TiXmlElement *const items) {

	if ((menu == nullptr) || (hMenu == nullptr) || (items == nullptr))
		return false;

	// Iterate through each child element.
	for (const auto *element = items->FirstChildElement("item"); element != nullptr; element = element->NextSiblingElement("item")) {
		MENUITEMINFO mii;
		const auto nPos = (UINT)GetMenuItemCount(hMenu) +1;

		ZeroMemory(&mii, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);

		//XPopupMenuItem *item = nullptr;
		std::unique_ptr<XPopupMenuItem> item;
		const TString caption(element->Attribute("caption"));

		if (caption == TEXT('-')) {
			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
			mii.fType = MFT_OWNERDRAW | MFT_SEPARATOR;

			//item = new XPopupMenuItem(menu, true);
			item = std::make_unique<XPopupMenuItem>(menu, true);
		}
		else {
			const auto mID = (UINT)queryIntAttribute(element, "id");
			const auto mIcon = queryIntAttribute(element, "icon") - 1;
			const XSwitchFlags xState(element->Attribute("state"));

			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE | MIIM_ID;
			mii.fType = MFT_OWNERDRAW;
			mii.wID = mID;

			// Checked
			if (xState[TEXT('c')])
				mii.fState |= MFS_CHECKED;
			
			// Gray
			if (xState[TEXT('g')])
				mii.fState |= MFS_GRAYED;

			// Submenu
			if (element->FirstChildElement("item") != nullptr) {
				if (mii.hSubMenu != nullptr)
					DestroyMenu(mii.hSubMenu);

				mii.fMask |= MIIM_SUBMENU;
				mii.hSubMenu = CreatePopupMenu();

				XPopupMenuManager::LoadPopupItemsFromXML(menu, mii.hSubMenu, element);
			}

			// TODO: command
			//item = new XPopupMenuItem(menu, caption, mIcon, (mii.hSubMenu != nullptr));
			item = std::make_unique<XPopupMenuItem>(menu, caption, mIcon, (mii.hSubMenu != nullptr));
		}

		// item never nullptr here
		mii.dwItemData = (ULONG_PTR)item.get();
		menu->m_vpMenuItem.push_back(item.release());
		InsertMenuItem(hMenu, nPos, TRUE, &mii);
	}

	return true;
}

const char* XPopupMenuManager::GetMenuAttributeFromXML(const char *const attrib, const TiXmlElement *const popup, const TiXmlElement *const global) {
	
	const auto tmp = popup->Attribute(attrib);

	// Specific menu attribute set, ignore global.
	if (tmp != nullptr)
		return tmp;

	// Try to find global style.
	if (global == nullptr)
		return nullptr;

	return global->Attribute(attrib);
}

//#ifdef DEBUG
//LRESULT CALLBACK XPopupMenuManager::mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//
//	// Incase execution somehow ends up here without this pointer being set.
//	if (XPopupMenuManager::g_OldmIRCMenusWindowProc == nullptr)
//		return DefWindowProc( mHwnd, uMsg, wParam, lParam);
//
//	switch (uMsg) {
//		case WM_NCCREATE:
//			{
//				CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
//				cs->dwExStyle |= WS_EX_LAYERED;
//				//return TRUE;
//				//return CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
//			}
//			break;
//		case WM_CREATE:
//			{
//				//CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
//				SetLayeredWindowAttributes(mHwnd, 0, (BYTE)0xCC, LWA_ALPHA); // 0xCC = 80% Opaque
//			}
//			break;
//		//case WM_ERASEBKGND:
//		//	{
//		//		if (GetProp(mHwnd, TEXT("dcx_ghosted")) == nullptr) {
//		//			SetProp(mHwnd, TEXT("dcx_ghosted"), (HANDLE)1);
//		//			LRESULT lRes = CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
//		//			AddStyles(mHwnd, GWL_EXSTYLE, WS_EX_LAYERED);
//		//			SetLayeredWindowAttributes(mHwnd, 0, (BYTE)0xCC, LWA_ALPHA); // 0xCC = 80% Opaque
//		//			RedrawWindow(mHwnd, nullptr, nullptr, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_UPDATENOW|RDW_INVALIDATE|RDW_ERASE|RDW_FRAME);
//		//			return lRes;
//		//		}
//		//	}
//		//	break;
//		//case WM_DESTROY:
//		//	{
//		//		if (GetProp(mHwnd, TEXT("dcx_ghosted")) != nullptr) {
//		//			RemoveProp(mHwnd, TEXT("dcx_ghosted"));
//		//		}
//		//	}
//		//	break;
//
//		//case WM_ERASEBKGND:
//		//	{
//		//		return TRUE;
//		//	}
//		//	break;
//	}
//
//	return CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
//}
//#endif
