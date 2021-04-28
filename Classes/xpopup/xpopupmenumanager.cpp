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
 * � ScriptsDB.org - 2006
 */
#include "defines.h"
#include "Classes/xpopup/xpopupmenumanager.h"
#include "Dcx.h"

#if DCX_CUSTOM_MENUS
std::vector<HWND> XPopupMenuManager::g_winlist;
WNDPROC XPopupMenuManager::g_OldmIRCMenusWindowProc = nullptr;
#endif

 /*!
  * \brief blah
  *
  * blah
  */

void XPopupMenuManager::load(void)
{
	/***** XPopup Stuff *****/
	WNDCLASSEX wc{};

#if DCX_CUSTOM_MENUS
	//wc.cbSize = sizeof(WNDCLASSEX);
	//GetClassInfoEx(nullptr,TEXT("#32768"),&wc); // menu
	//g_OldmIRCMenusWindowProc = wc.lpfnWndProc;
	//wc.lpfnWndProc = XPopupMenuManager::mIRCMenusWinProc;
	//RegisterClassEx(&wc);
	//DCX_DEBUG(mIRCLinker::debug,TEXT("LoadDLL"), TEXT("Subclassed Menu Class"));

	if (HWND tmp_hwnd = CreateWindowEx(0, TEXT("#32768"), nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr); tmp_hwnd)
	{
		g_OldmIRCMenusWindowProc = (WNDPROC)SetClassLongPtr(tmp_hwnd, GCLP_WNDPROC, (ULONG_PTR)XPopupMenuManager::mIRCMenusWinProc);
		DestroyWindow(tmp_hwnd);
		DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Subclassed Menu Class"));
	}
#endif

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Registering XPopup..."));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = XPopupMenu::XPopupWinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hbrBackground = GetStockBrush(COLOR_WINDOWFRAME);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = XPOPUPMENUCLASS;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Creating menu owner..."));
	m_hMenuOwner = CreateWindow(XPOPUPMENUCLASS, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);

	m_mIRCPopupMenu = std::make_unique<XPopupMenu>(TEXT("mirc"), (HMENU)nullptr);
	m_mIRCMenuBar = std::make_unique<XPopupMenu>(TEXT("mircbar"), GetMenu(mIRCLinker::getHWND()));

	// XMenuBar stuff
	auto menu = GetMenu(mIRCLinker::getHWND());

	constexpr UINT buffSize = 30U;
	TString label(buffSize);
	MENUITEMINFO mii{};

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.cch = buffSize;
	mii.dwTypeData = label.to_chr();

	for (auto i = 0U; GetMenuItemInfo(menu, i, TRUE, &mii); ++i)
	{
		// We've found the tools menu, next one is the scriptable popup.
		if (label == TEXT("&Tools"))
		{
			const auto scriptable = GetSubMenu(menu, gsl::narrow_cast<int>(i + 1));

			// TODO: check if the next one is "&Window"
			m_mIRCScriptMenu = std::make_unique<XPopupMenu>(TEXT("scriptpopup"), scriptable);
			break;
		}

		// Reset buffer size
		mii.cch = buffSize;
	}
}

void XPopupMenuManager::unload(void) noexcept
{
	/***** XPopup Stuff *****/

	//mIRCLinker::resetWindowProc();

	clearMenus();

	m_mIRCPopupMenu.reset(nullptr);

	m_mIRCMenuBar->cleanMenu(GetMenu(mIRCLinker::getHWND()));

	m_mIRCMenuBar.reset(nullptr);

	m_mIRCScriptMenu.reset(nullptr);

	if (m_hMenuOwner)
		DestroyWindow(m_hMenuOwner);

	UnregisterClass(XPOPUPMENUCLASS, GetModuleHandle(nullptr));

#if DCX_CUSTOM_MENUS
	if (XPopupMenuManager::g_OldmIRCMenusWindowProc != nullptr)
	{
		HWND tmp_hwnd = CreateWindowEx(0, TEXT("#32768"), nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
		if (tmp_hwnd != nullptr)
		{
			SetClassLongPtr(tmp_hwnd, GCLP_WNDPROC, (LONG_PTR)XPopupMenuManager::g_OldmIRCMenusWindowProc);
			DestroyWindow(tmp_hwnd);
			XPopupMenuManager::g_OldmIRCMenusWindowProc = nullptr;
		}
	}
#endif
}

LRESULT XPopupMenuManager::OnInitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (const auto isWinMenu = (Dcx::dcxHIWORD(lParam) != FALSE); !isWinMenu)
	{
		const auto menu = reinterpret_cast<HMENU>(wParam);
		const auto currentMenubar = GetMenu(mIRCLinker::getHWND());
		const auto switchMenu = (m_mIRCScriptMenu != nullptr) &&                  // The mIRC scriptpopup menu has been wrapped,
			(menu == m_mIRCScriptMenu->getMenuHandle()) && // The menu the same as the one just shown,
			(currentMenubar != g_OriginalMenuBar) &&       // The menubar is our generated menubar,
			(g_OriginalMenuBar != nullptr);                   // And ensure it has been generated.

		if (switchMenu)
			SetMenu(mIRCLinker::getHWND(), g_OriginalMenuBar);

		// let mIRC populate the menus dynamically
		const auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, WM_INITMENUPOPUP, wParam, lParam);

		if (switchMenu)
			SetMenu(mIRCLinker::getHWND(), currentMenubar);

		if (m_bIsMenuBar = isMenuBarMenu(GetMenu(mHwnd), menu); m_bIsMenuBar)
		{
			if (m_bIsActiveMircMenubarPopup)
			{
				const bool isCustomMenu = Dcx::XPopups.isCustomMenu(menu);

				// Store the handle of the menu being displayed.
				if (isCustomMenu && (m_hMenuCustom == nullptr))
					m_hMenuCustom = menu;

				if (const auto hActive = mIRCLinker::getActiveMDIWindow(); ((!IsZoomed(hActive) || GetSystemMenu(hActive, FALSE) != menu)) && (!isCustomMenu) && (m_hMenuCustom == nullptr)) // This checks for custom submenus.
					m_mIRCMenuBar->convertMenu(menu, TRUE);
			}
		}
		else {
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

LRESULT XPopupMenuManager::OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!m_bIsMenuBar && m_bIsActiveMircPopup)
		m_mIRCMenuBar->clearAllMenuItems();

	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_EXITMENULOOP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	// Check if the message came from the menubar
	if ((HIWORD(wParam) == 0) && (m_bIsMenuBar))
	{
		if (Dcx::XMenubar.hasCallback())
		{
			// If the user requested the message to be halted ...
			if (Dcx::XMenubar.parseCallback(LOWORD(wParam)))
			{
				// Stop parsing and prevent default action.
				return 0L;
			}
		}
		if (dcxSignal.xdock)
		{
			switch (LOWORD(wParam))
			{
			case 111: // toolbar
			{
				if (IsWindowVisible(mIRCLinker::getToolbar()))
					mIRCLinker::signal(TEXT("toolbar disabled"));
				else
					mIRCLinker::signal(TEXT("toolbar enabled"));
			}
			break;
			case 112: // switchbar
			{
				if (IsWindowVisible(mIRCLinker::getSwitchbar()))
					mIRCLinker::signal(TEXT("switchbar disabled"));
				else
					mIRCLinker::signal(TEXT("switchbar enabled"));
			}
			break;
			case 210: // treebar
			{
				if (IsWindowVisible(mIRCLinker::getTreebar()))
					mIRCLinker::signal(TEXT("treebar disabled"));
				else
					mIRCLinker::signal(TEXT("treebar enabled"));
			}
			break;
			default:
				break;
			}
		}
	}
	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::parseCommand(const TString& input)
{
	//const auto p_Menu = getMenuByName(input.getfirsttok(1), true);	// tok 1
	//const XSwitchFlags flags(input.getnexttok());	// tok 2
	//
	//// Special mIRC Menu
	//if (p_Menu == nullptr && !flags[TEXT('c')])
	//	throw Dcx::dcxException(TEXT("\"%\" doesn't exist : see /xpopup -c"), input.gettok(1));
	//
	//parseCommand(input, p_Menu);

	parseCommand(input, getMenuByName(input.getfirsttok(1), true));
}

void XPopupMenuManager::parseCommand(const TString& input, XPopupMenu* const p_Menu)
{
	const auto tsMenuName(input.getfirsttok(1));
	const XSwitchFlags flags(input.getnexttok());	// tok 2
	const auto numtok = input.numtok();

	// Special mIRC Menu
	if (p_Menu == nullptr && !flags[TEXT('c')])
		throw Dcx::dcxException(TEXT("\"%\" doesn't exist : see /xpopup -c"), tsMenuName);

	// xpopup -b - [MENU] [SWITCH] [FILENAME]
	if (flags[TEXT('b')])
	{
		HBITMAP hBitmap = nullptr;

		if (numtok > 2)
		{
			auto filename(input.getlasttoks().trim());	// tok 3, -1

			if (filename == TEXT("none"))
			{
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

		p_Menu->setBackBitmap(hBitmap);

	}
	// xpopup -c -> [MENU] [SWITCH] [STYLE]
	else if (flags[TEXT('c')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto uMenuHash = std::hash<TString>{}(tsMenuName);

		if (uMenuHash == TEXT("mirc"_hash) || uMenuHash == TEXT("mircbar"_hash))
			throw Dcx::dcxException(TEXT("Command not supported with mirc or mircbar menus"));

		if (p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" already exists"), tsMenuName);

		const auto style = XPopupMenu::parseStyle(input.getnexttok());	// tok 3
		this->m_vpXPMenu.push_back(new XPopupMenu(tsMenuName, style));
	}
	// xpopup -d -> [MENU] [SWITCH]
	else if (flags[TEXT('d')])
	{
		const auto uMenuHash = std::hash<TString>{}(tsMenuName);

		if (uMenuHash == TEXT("mirc"_hash) || uMenuHash == TEXT("mircbar"_hash))
			throw Dcx::dcxException(TEXT("Command not supported with mirc or mircbar menus"));

		this->deleteMenu(p_Menu);
	}
	// xpopup -i -> [MENU] -i [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('i')])
	{
		if (numtok < 5)
			throw DcxExceptions::dcxInvalidArguments();

		auto himl = p_Menu->getImageList();
		const auto tsFlags(input.getnexttok());			// tok 3
		const auto index = input.getnexttok().to_int();	// tok 4
		auto filename(input.getlasttoks());				// tok 5, -1

#if DCX_USE_WRAPPERS
		const Dcx::dcxIconResource icon(index, filename, false, tsFlags);

		ImageList_AddIcon(himl, icon.get());
#else
		const HICON icon = dcxLoadIcon(index, filename, false, tsFlags);
		if (icon == nullptr)
			throw Dcx::dcxException(TEXT("Unable to Load Icon: % in file: %"), index, filename);

		ImageList_AddIcon(himl, icon);
		DestroyIcon(icon);
#endif
	}
	// xpopup -j -> [MENU] [SWITCH]
	else if (flags[TEXT('j')])
	{
		p_Menu->destroyImageList();
	}
	// xpopup -l -> [MENU] [SWITCH] [N] [COLOR | default]
	else if (flags[TEXT('l')])
	{
		if (numtok < 4)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColor = gsl::narrow_cast<XPopupMenu::MenuColours>(input.getnexttok().to_<UINT>());	// tok 3
		const auto clr(input.getnexttok());				// tok 4

		if (clr == TEXT("default"))
			p_Menu->setDefaultColor(nColor);
		else
			p_Menu->setColor(nColor, clr.to_<COLORREF>());
	}
	// xpopup -m -> mirc -m
	else if (flags[TEXT('m')])
	{
		// do nothing in utf dll as this dll is mirc v7+ only.
	}
	// xpopup -M -> [MENU] [SWITCH] (TEXT)
	else if (flags[TEXT('M')])
	{
		p_Menu->setMarkedText(input.getlasttoks());	// tok 3, -1
	}
	// xpopup -p -> [MENU] [SWITCH] [COLORS]
	else if (flags[TEXT('p')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto colors(input.getlasttoks());	// tok 3, -1

		auto i = 1U;
		for (const auto& tmp : colors)
		{
			if (tmp == TEXT("default"))
				p_Menu->setDefaultColor(gsl::narrow_cast<XPopupMenu::MenuColours>(i));
			else
				p_Menu->setColor(gsl::narrow_cast<XPopupMenu::MenuColours>(i), tmp.to_<COLORREF>());
			++i;
		}
	}
	// xpopup -s -> [MENU] [SWITCH] [+FLAGS] [X] [Y] (OVER HWND)
	else if (flags[TEXT('s')])
	{
		if ((numtok < 5) || (!p_Menu))
			throw DcxExceptions::dcxInvalidArguments();

		const auto mflags = this->parseTrackFlags(input.getnexttok());	// tok 3
		auto x = input.getnexttok().to_int();								// tok 4
		auto y = input.getnexttok().to_int();								// tok 5

		/*
		Add offsetting for multiple monitor based on supplied hwnd this menu is to be associated with
		*/
		if (const auto hTrack = reinterpret_cast<HWND>(input.getnexttok().to_<ULONG_PTR>()); (hTrack && IsWindow(hTrack)))
		{
			// map window relative pos ($mouse.x/y) to screen pos for TrackPopupMenuEx()
			POINT pt{ x, y };
			MapWindowPoints(hTrack, nullptr, &pt, 1);
			x = pt.x;
			y = pt.y;
		}
		else {
			// Adjust relative location to take multi-monitor into account
			MONITORINFO mi{};
			const auto hMon = MonitorFromWindow(mIRCLinker::getHWND(), MONITOR_DEFAULTTONEAREST);

			mi.cbSize = sizeof(mi);
			GetMonitorInfo(hMon, &mi);

			x += mi.rcMonitor.left;
			y += mi.rcMonitor.top;
		}

		const auto ID = TrackPopupMenuEx(p_Menu->getMenuHandle(), TPM_RETURNCMD | mflags, x, y, m_hMenuOwner, nullptr);

		mIRCLinker::exec(TEXT("//.signal -n XPopup-% %"), p_Menu->getName(), ID);
	}
	// xpopup -t -> [MENU] [SWITCH] [STYLE]
	else if (flags[TEXT('t')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto style = XPopupMenu::parseStyle(input.getnexttok());	// tok 3

		p_Menu->setStyle(style);
	}
	// xpopup -x -> [MENU] [SWITCH] [+FLAGS]
	else if (flags[TEXT('x')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(input.getnexttok());	// tok 3

		if (!xflags[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		UINT iStyles = 0;
		if (xflags[TEXT('i')])
			iStyles |= XPS_ICON3D;
		if (xflags[TEXT('d')])
			iStyles |= XPS_DISABLEDSEL;
		if (xflags[TEXT('p')])
			iStyles |= XPS_ICON3DSHADOW;

		p_Menu->setItemStyle(iStyles);
	}
	// xpopup -R -> [MENU] [SWITCH] [+FLAGS] (FLAG OPTIONS)
	else if (flags[TEXT('R')])
	{
		if (numtok < 3)
			throw DcxExceptions::dcxInvalidArguments();

		const XSwitchFlags xflags(input.getnexttok());	// tok 3

		if (!xflags[TEXT('+')])
			throw DcxExceptions::dcxInvalidFlag();

		if (xflags[TEXT('r')]) // Set Rounded Selector on/off
			p_Menu->SetRounded((input.getnexttok().to_int() > 0));	// tok 4
		else if (xflags[TEXT('a')]) // Set Alpha value of menu. 0-255
		{
			const std::byte alpha{ (input.getnexttok().to_<UINT>() & 0xFF) };	// tok 4

			p_Menu->SetAlpha(alpha);
		}
	}
}

/*!
 * \brief blah
 *
 * blah [MENU] [PROP]
 */

TString XPopupMenuManager::parseIdentifier(const TString& input) const
{
	const auto numtok = input.numtok();
	const auto tsMenuName(input.getfirsttok(1));
	const auto prop(input.getnexttok());	// tok 2

	switch (const auto* const p_Menu = getMenuByName(tsMenuName, true); std::hash<TString>{}(prop))
	{
	case TEXT("ismenu"_hash):
	{
		return dcx_truefalse(p_Menu != nullptr);
	}
	break;
	case TEXT("menuname"_hash):
	{
		const auto i = tsMenuName.to_<size_t>();

		if (i > m_vpXPMenu.size())
			throw Dcx::dcxException(TEXT("Invalid index: %"), i);

		// Return number of menus.
		if (i == 0)
		{

			TString tsRes;
			tsRes += m_vpXPMenu.size();
			return tsRes;
		}
		// Return name of specified menu.
		else
			return gsl::at(m_vpXPMenu, i - 1)->getName();
	}
	break;
	case TEXT("menubar"_hash):
		break;
	case TEXT("style"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		switch (p_Menu->getStyle())
		{
		case XPopupMenu::MenuStyle::XPMS_OFFICE2003:
			return TEXT("office2003");
		case XPopupMenu::MenuStyle::XPMS_OFFICE2003_REV:
			return TEXT("office2003rev");
		case XPopupMenu::MenuStyle::XPMS_OFFICEXP:
			return TEXT("officeXP");
		case XPopupMenu::MenuStyle::XPMS_ICY:
			return TEXT("icy");
		case XPopupMenu::MenuStyle::XPMS_ICY_REV:
			return TEXT("icyrev");
		case XPopupMenu::MenuStyle::XPMS_GRADE:
			return TEXT("grade");
		case XPopupMenu::MenuStyle::XPMS_GRADE_REV:
			return TEXT("graderev");
		case XPopupMenu::MenuStyle::XPMS_VERTICAL:
			return TEXT("vertical");
		case XPopupMenu::MenuStyle::XPMS_VERTICAL_REV:
			return TEXT("verticalrev");
		case XPopupMenu::MenuStyle::XPMS_NORMAL:
			return TEXT("normal");
		case XPopupMenu::MenuStyle::XPMS_CUSTOM:
			return TEXT("custom");
		case XPopupMenu::MenuStyle::XPMS_BUTTON:
			return TEXT("button");
		case XPopupMenu::MenuStyle::XPMS_CUSTOMBIG:
			return TEXT("custombig");
		default:
			return TEXT("unknown");
		}
	}
	break;
	case TEXT("exstyle"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		TString szStyles(TEXT('+'));
		const auto iExStyles = p_Menu->getItemStyle();

		if (dcx_testflag(iExStyles, XPS_ICON3D))
			szStyles += TEXT('i');
		if (dcx_testflag(iExStyles, XPS_DISABLEDSEL))
			szStyles += TEXT('d');
		if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
			szStyles += TEXT('p');

		return szStyles;
	}
	break;
	case TEXT("colors"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		TString tsRes;
		_ts_sprintf(tsRes, TEXT("% % % % % % % % % % %"), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_BACKGROUND), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_ICONBOX),
			p_Menu->getColor(XPopupMenu::MenuColours::XPMC_CHECKBOX), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_CHECKBOX_DISABLED), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX_DISABLED), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_TEXT_DISABLED),
			p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX_BORDER), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SEPARATOR), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_TEXT), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTEDTEXT));
		return tsRes;
	}
	break;
	case TEXT("color"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		if (numtok != 3)
			throw DcxExceptions::dcxInvalidArguments();

		const auto nColor = gsl::narrow_cast<XPopupMenu::MenuColours>(input.getnexttok().to_<UINT>());	// tok 3
		if (nColor < XPopupMenu::MenuColours::XPMC_MIN || nColor > XPopupMenu::MenuColours::XPMC_MAX)
			throw Dcx::dcxException(TEXT("Invalid colour index used: %"), gsl::narrow_cast<UINT>(nColor));

		TString tsRes;
		tsRes += p_Menu->getColor(nColor);
		return tsRes;
	}
	break;
	case TEXT("isrounded"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		return dcx_truefalse(p_Menu->IsRounded());
	}
	break;
	case TEXT("alpha"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		TString tsRes;
		tsRes += gsl::narrow_cast<uint8_t>(p_Menu->IsAlpha());
		return tsRes;
	}
	break;
	case TEXT("marked"_hash):
	{
		if (p_Menu == nullptr)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		return p_Menu->getMarkedText();
	}
	break;
	case 0:
	default:
		throw Dcx::dcxException(TEXT("Unknown prop \"%\""), prop);
	}
	return {};
}

const int XPopupMenuManager::parseMPopup(const TString& input)
{
	if (input.numtok() < 2)
		throw DcxExceptions::dcxInvalidArguments();

	const auto uMenuHash = std::hash<TString>{}(input.getfirsttok(1));
	const auto iEnable = input.getnexttok().to_int();	// tok 2

	if (uMenuHash == TEXT("mirc"_hash))
		m_bIsActiveMircPopup = (iEnable == 1);
	else if (uMenuHash == TEXT("mircbar"_hash))
	{
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

void XPopupMenuManager::addMenu(XPopupMenu* const p_Menu)
{
	if (p_Menu)
		this->m_vpXPMenu.push_back(p_Menu);
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::deleteMenu(const XPopupMenu* const p_Menu) noexcept
{
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

	//const auto itEnd = this->m_vpXPMenu.end();
	//const auto itGot = std::find(this->m_vpXPMenu.begin(), itEnd, p_Menu);
	//if (itGot != itEnd)
	//{
	//	delete *itGot;
	//	this->m_vpXPMenu.erase(itGot);
	//}

	if (Dcx::eraseIfFound(m_vpXPMenu, p_Menu))
		delete p_Menu;
}

/*!
 * \brief blah
 *
 * blah
 */

void XPopupMenuManager::clearMenus() noexcept
{
	for (const auto& a : this->m_vpXPMenu)
		delete a;

	this->m_vpXPMenu.clear();
}

/*!
 * \brief blah
 *
 * blah
 */

XPopupMenu* XPopupMenuManager::getMenuByHash(const std::size_t uHash, const bool bCheckSpecial) const noexcept
{
	if (bCheckSpecial)
	{
		if (uHash == TEXT("mircbar"_hash))
			return m_mIRCMenuBar.get();
		else if (uHash == TEXT("mirc"_hash))
			return Dcx::XPopups.getmIRCPopup();
		else if (uHash == TEXT("scriptpopup"_hash))
			return m_mIRCScriptMenu.get();
	}

	for (const auto& x : m_vpXPMenu)
	{
		if (x != nullptr)
		{
			if (x->getNameHash() == uHash)
				return x;
		}
	}
	return nullptr;
}

XPopupMenu* XPopupMenuManager::getMenuByName(const TString& tsName, const bool bCheckSpecial) const noexcept
{
	return getMenuByHash(std::hash<TString>{}(tsName), bCheckSpecial);
}

/*
 * Retrieves a menu by the handle.
 */
XPopupMenu* XPopupMenuManager::getMenuByHandle(const HMENU hMenu) const noexcept
{
	// Special cases
	if (hMenu == m_mIRCMenuBar->getMenuHandle())
		return m_mIRCMenuBar.get();
	else if (hMenu == Dcx::XPopups.getmIRCPopup()->getMenuHandle())
		return Dcx::XPopups.getmIRCPopup();
	else if (hMenu == m_mIRCScriptMenu->getMenuHandle())
		return m_mIRCScriptMenu.get();

	for (const auto& x : this->m_vpXPMenu)
	{
		if (x != nullptr)
		{
			if (hMenu == x->getMenuHandle())
				return x;
		}
	}
	return nullptr;
}

/*
 * Check if menu handle is a custom menu (don't include converted mIRC menus)
 */
const bool XPopupMenuManager::isCustomMenu(const HMENU hMenu) const noexcept
{
	for (const auto& x : this->m_vpXPMenu)
	{
		if (x != nullptr)
		{
			if (hMenu == x->getMenuHandle())
				return true;
		}
	}
	return false;
}

const bool XPopupMenuManager::isMenuBarMenu(const HMENU hMenu, const HMENU hMatch)
{
	const auto n = GetMenuItemCount(hMenu);

	for (auto i = decltype(n){0}; i < n; ++i)
	{
		if (const auto hTemp = GetSubMenu(hMenu, i); hTemp != nullptr)
		{
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

const UINT XPopupMenuManager::parseTrackFlags(const TString& flags) noexcept
{
	UINT iFlags = 0;
	const XSwitchFlags xflags(flags);

	if (!xflags[TEXT('+')])
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

/*
 * Parses the menu information and returns a valid XPopupMenu.
 */
void XPopupMenuManager::LoadPopupsFromXML(const TiXmlElement* const popups, const TiXmlElement* popup, const TString& popupName, const TString& popupDataset)
{
	// Find the dataset with the name we want.
	for (auto element = popups->FirstChildElement("popup"); element; element = element->NextSiblingElement("popup"))
	{
		const TString name(element->Attribute("name"));

		if (name == popupDataset)
		{
			popup = element;
			break;
		}
	}

	// Dataset not found.
	if (!popup)
		throw Dcx::dcxException(TEXT("No Popup Dataset %"), popupDataset);

	// Destroy a menu which already exists
	if (const auto* const menu = Dcx::XPopups.getMenuByName(popupName, false); menu)
		Dcx::XPopups.deleteMenu(menu);

	// Find global styles branch
	auto globalStyles = popups->FirstChildElement("styles");

	// Move to "all" branch
	if (globalStyles)
		globalStyles = globalStyles->FirstChildElement("all");
	else
		globalStyles = nullptr;

	// Create menu with style (from specific or global)
	const auto style = XPopupMenu::parseStyle(GetMenuAttributeFromXML("style", popup, globalStyles));
	// Ook: this needs looked at, should be protected....
	const auto menu = new XPopupMenu(popupName, style);

	//const static TString colors(TEXT("bgcolour iconcolour cbcolour discbcolour disselcolour distextcolour selcolour selbordercolour seperatorcolour textcolour seltextcolour"));
	//
	//UINT i = 1;
	//for (const auto& tmp : colors)
	//{
	//	if (auto tsAttr = GetMenuAttributeFromXML(tmp.c_str(), popup, globalStyles); !tsAttr.empty())
	//	{
	//		mIRCLinker::eval(tsAttr, tsAttr);
	//		menu->setColor(gsl::narrow_cast<XPopupMenu::MenuColours>(i), tsAttr.to_<COLORREF>());
	//	}
	//	++i;
	//}

	//const static char* colors[] = {
	//	"bgcolour"
	//	"iconcolour"
	//	"cbcolour"
	//	"discbcolour"
	//	"disselcolour"
	//	"distextcolour"
	//	"selcolour"
	//	"selbordercolour"
	//	"seperatorcolour"
	//	"textcolour"
	//	"seltextcolour"
	//};
	//
	//UINT i = 1;
	//for (const auto& tmp : colors)
	//{
	//	if (auto tsAttr = GetMenuAttributeFromXML(tmp, popup, globalStyles); !tsAttr.empty())
	//	{
	//		mIRCLinker::eval(tsAttr, tsAttr);
	//		menu->setColor(gsl::narrow_cast<XPopupMenu::MenuColours>(i), tsAttr.to_<COLORREF>());
	//	}
	//	++i;
	//}

	const static Dcx::CodeValue<const char*, XPopupMenu::MenuColours> colors[] = {
		{ XPopupMenu::MenuColours::XPMC_BACKGROUND, "bgcolour" },
		{ XPopupMenu::MenuColours::XPMC_ICONBOX, "iconcolour"},
		{ XPopupMenu::MenuColours::XPMC_CHECKBOX, "cbcolour"},
		{ XPopupMenu::MenuColours::XPMC_CHECKBOX_DISABLED, "discbcolour"},
		{ XPopupMenu::MenuColours::XPMC_SELECTIONBOX_DISABLED, "disselcolour"},
		{ XPopupMenu::MenuColours::XPMC_TEXT_DISABLED, "distextcolour"},
		{ XPopupMenu::MenuColours::XPMC_SELECTIONBOX, "selcolour"},
		{ XPopupMenu::MenuColours::XPMC_SELECTIONBOX_BORDER, "selbordercolour"},
		{ XPopupMenu::MenuColours::XPMC_SEPARATOR, "seperatorcolour"},
		{ XPopupMenu::MenuColours::XPMC_TEXT, "textcolour"},
		{ XPopupMenu::MenuColours::XPMC_SELECTEDTEXT, "seltextcolour"}
	};

	for (const auto& tmp : colors)
	{
		if (auto tsAttr = GetMenuAttributeFromXML(tmp.value, popup, globalStyles); !tsAttr.empty())
		{
			mIRCLinker::eval(tsAttr, tsAttr);
			menu->setColor(tmp.code, tsAttr.to_<COLORREF>());
		}
	}

	// Set background image if CUSTOM style used
	if (style == XPopupMenu::MenuStyle::XPMS_CUSTOM)
	{
		const TString tsBkg(popup->Attribute("background"));

		if (!tsBkg.empty())
		{
			TString filename;

			mIRCLinker::eval(filename, tsBkg);

			if (const auto hBitmap = dcxLoadBitmap(nullptr, filename); hBitmap)
				menu->setBackBitmap(hBitmap);
		}
	}

	// Successfully created a menu.
	Dcx::XPopups.addMenu(menu);

	// Parse icons
	if (const auto* element = popup->FirstChildElement("icons"); element)
	{
		for (element = element->FirstChildElement("icon"); element; element = element->NextSiblingElement("icon"))
		{
			// Flags
			const TString flags(queryAttribute(element, "flags", "+"));
			const TString tsSrc(element->Attribute("src"));
			const TString indexes(queryAttribute(element, "index", "0"));

			// Filename
			if (!tsSrc.empty())
			{
				TString tsFilename;
				mIRCLinker::eval(tsFilename, tsSrc);

				if (!tsFilename.empty())
				{
					TString command;
					const auto itEnd = indexes.end();
					for (auto itStart = indexes.begin(TSCOMMACHAR); itStart != itEnd; ++itStart)
					{
						// does (*itStart) NEED to be converted to an int?
						//command.tsprintf(TEXT("%s -i %s %d %s"), popupName.to_chr(), flags.to_chr(), (*itStart).to_int(), tsFilename.to_chr());
						//command.tsprintf(TEXT("%s -i %s %s %s"), popupName.to_chr(), flags.to_chr(), (*itStart).to_chr(), tsFilename.to_chr());
						_ts_sprintf(command, TEXT("% -i % % %"), popupName, flags, (*itStart), tsFilename);
						Dcx::XPopups.parseCommand(command, menu);
					}
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
const bool XPopupMenuManager::LoadPopupItemsFromXML(XPopupMenu* menu, HMENU hMenu, const TiXmlElement* const items)
{
	if ((!menu) || (!hMenu) || (!items))
		return false;

	// Iterate through each child m_pElement.
	for (const auto* element = items->FirstChildElement("item"); element; element = element->NextSiblingElement("item"))
	{
		MENUITEMINFO mii{};
		const auto nPos = gsl::narrow_cast<UINT>(GetMenuItemCount(hMenu)) + 1U;

		mii.cbSize = sizeof(MENUITEMINFO);

		std::unique_ptr<XPopupMenuItem> item;
		const TString caption(element->Attribute("caption"));

		if (caption == TEXT('-'))
		{
			mii.fMask = MIIM_DATA | MIIM_FTYPE | MIIM_STATE;
			mii.fType = MFT_OWNERDRAW | MFT_SEPARATOR;

			item = std::make_unique<XPopupMenuItem>(menu, true);
		}
		else {
			const auto mID = gsl::narrow_cast<UINT>(queryIntAttribute(element, "id"));
			const auto mIcon = queryIntAttribute(element, "icon") - 1;
			const XSwitchFlags xState(TString(element->Attribute("state")));

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
			if (element->FirstChildElement("item"))
			{
				if (mii.hSubMenu)
					DestroyMenu(mii.hSubMenu);

				mii.fMask |= MIIM_SUBMENU;
				mii.hSubMenu = CreatePopupMenu();

				XPopupMenuManager::LoadPopupItemsFromXML(menu, mii.hSubMenu, element);
			}

			// TODO: command
			item = std::make_unique<XPopupMenuItem>(menu, caption, mIcon, (mii.hSubMenu != nullptr));
		}

		// item never nullptr here
		mii.dwItemData = reinterpret_cast<ULONG_PTR>(item.get());
		menu->m_vpMenuItem.push_back(item.release());
		InsertMenuItem(hMenu, nPos, TRUE, &mii);
	}

	return true;
}

const TString XPopupMenuManager::GetMenuAttributeFromXML(const char* const attrib, const TiXmlElement* const popup, const TiXmlElement* const global)
{
	const TString tmp(popup->Attribute(attrib));

	// Specific menu attribute set, ignore global.
	if (!tmp.empty())
		return tmp;

	// Try to find global style.
	if (!global)
		return tmp;

	return TString(global->Attribute(attrib));
}

#if DCX_CUSTOM_MENUS
LRESULT CALLBACK XPopupMenuManager::mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Incase execution somehow ends up here without this pointer being set.
	if (!XPopupMenuManager::g_OldmIRCMenusWindowProc)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	// only if custom menus enabled.
	if (Dcx::setting_CustomMenusAlpha == 255)
		return CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);

	//Dcx::XPopups.
	const MenuMessages mm = static_cast<MenuMessages>(uMsg);

	switch (mm)
	{
	case MenuMessages::WMN_NCCREATE:
	{
		dcxlParam(LPCREATESTRUCT, cs);
		cs->dwExStyle |= WS_EX_LAYERED | WS_EX_COMPOSITED;
	}
	break;

	case MenuMessages::WMN_CREATE:
	{
		dcxlParam(LPCREATESTRUCT, cs);
		cs->dwExStyle |= WS_EX_LAYERED | WS_EX_COMPOSITED;

		// check for previous menu...
		if (!g_winlist.empty())
		{
			// change previous window.
			auto parent = g_winlist.back();

			// make sure previous menu is layered.
			if (const auto dwStyle = dcxGetWindowExStyle(parent); !dcx_testflag(dwStyle, WS_EX_LAYERED))
				dcxSetWindowExStyle(parent, dwStyle | WS_EX_LAYERED);

			// set alpha for previous menu.
			//SetLayeredWindowAttributes(parent, 0, Dcx::setting_CustomMenusAlpha, LWA_ALPHA); // 0xCC = 80% Opaque, 0xC0
		}
		// add this window to list.
		g_winlist.push_back(mHwnd);

	}
	break;

	//case WM_PAINT:
	//{
	//	//if (!Dcx::dcxGetProp<BOOL>(mHwnd, TEXT("dcx_test_menu")))
	//	//	break;
	//
	//	const auto dwStyle = dcxGetWindowExStyle(mHwnd);
	//
	//	//BOOL bEnabled = FALSE;
	//	//Dcx::VistaModule.dcxDwmIsCompositionEnabled(&bEnabled);
	//	//if (bEnabled)
	//	//{
	//	//	DWM_BLURBEHIND blur{DWM_BB_ENABLE, TRUE, nullptr, FALSE};
	//	//	Dcx::VistaModule.dcxDwmEnableBlurBehindWindow(mHwnd, &blur);
	//	//}
	//
	//	//// playing around with menu transparency
	//	//const BYTE alpha = 0x7F;
	//
	//	// If alpha == 255 then menu is fully opaque so no need to change to layered.
	//	//if (alpha < 255) {
	//	//	HWND hMenuWnd = mHwnd;
	//
	//	//	if (IsWindow(hMenuWnd)) {
	//	//		DWORD dwStyle = GetWindowExStyle(hMenuWnd);
	//
	//	//		if (!dcx_testflag(dwStyle, WS_EX_LAYERED))
	//	//		{
	//	//			SetWindowLong(hMenuWnd, GWL_EXSTYLE, dwStyle | WS_EX_LAYERED);
	//	//			SetLayeredWindowAttributes(hMenuWnd, 0, (BYTE)alpha, LWA_ALPHA); // 0xCC = 80% Opaque
	//	//		}
	//	//	}
	//	//}
	//}
	//break;

	//case WM_ERASEBKGND:
	//{
	//	const auto dwStyle = dcxGetWindowExStyle(mHwnd);
	//	const auto isLayered = dcx_testflag(dwStyle, WS_EX_LAYERED);
	//
	//	if (!isLayered)
	//	{
	//		dcxSetWindowExStyle(mHwnd, dwStyle | WS_EX_LAYERED);
	//		SetLayeredWindowAttributes(mHwnd, 0, (BYTE)0xCC, LWA_ALPHA); // 0xCC = 80% Opaque
	//	}
	//	else
	//	{
	//		//dcxSetWindowExStyle(mHwnd, dwStyle | WS_EX_LAYERED);
	//		SetLayeredWindowAttributes(mHwnd, 0, (BYTE)0xFF, LWA_ALPHA); // 0xCC = 80% Opaque
	//	}
	//
	//	//HDC hdc = (HDC)wParam;
	//	//BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, 0xC0, AC_SRC_OVER };
	//	//UpdateLayeredWindow(mHwnd, hdc, nullptr, nullptr, src, nullptr, 0, &stBlend, 0);
	//
	//	//BOOL bEnabled = FALSE;
	//	//Dcx::VistaModule.dcxDwmIsCompositionEnabled(&bEnabled);
	//	//if (bEnabled)
	//	//{
	//	//	DWM_BLURBEHIND blur{ DWM_BB_ENABLE, TRUE, nullptr, FALSE };
	//	//	Dcx::VistaModule.dcxDwmEnableBlurBehindWindow(mHwnd, &blur);
	//	//}
	//
	//	//if (GetProp(mHwnd, TEXT("dcx_ghosted")) == nullptr)
	//	//{
	//	//	SetProp(mHwnd, TEXT("dcx_ghosted"), (HANDLE)1);
	//	//	LRESULT lRes = CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
	//	//	AddStyles(mHwnd, GWL_EXSTYLE, WS_EX_LAYERED);
	//	//	SetLayeredWindowAttributes(mHwnd, 0, (BYTE)0xCC, LWA_ALPHA); // 0xCC = 80% Opaque
	//	//	RedrawWindow(mHwnd, nullptr, nullptr, RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_INVALIDATE | RDW_ERASE | RDW_FRAME);
	//	//	return lRes;
	//	//}
	//}
	//break;

	case MenuMessages::WMN_DESTROY:
	{
		if (g_winlist.empty())
			break;

		// remove ourselfs.
		g_winlist.pop_back();

		if (!g_winlist.empty())
		{
			// get previous menu window.
			auto parent = g_winlist.back();

			if (const auto dwStyle = dcxGetWindowExStyle(parent); !dcx_testflag(dwStyle, WS_EX_LAYERED))
				break;

			SetLayeredWindowAttributes(parent, 0, 0xFFU, LWA_ALPHA); // 0xCC = 80% Opaque
		}
	}
	break;

	// code for rounded windows etc..
	case MenuMessages::WMN_SIZE:
	{
		if (!Dcx::setting_CustomMenusRounded)
			break;

		// message is sent AFTER the window is resized.
		// this is client area size, we need window rect
		//const UINT width = Dcx::dcxLOWORD(lParam);
		//const UINT height = Dcx::dcxHIWORD(lParam);

		if (RECT rc{}; GetWindowRect(mHwnd, &rc))
		{

			const UINT width = rc.right - rc.left;
			const UINT height = rc.bottom - rc.top;

			constexpr int radius = 10;

			if (auto m_Region = CreateRoundRectRgn(0, 0, width, height, radius, radius); m_Region)
			{
				SetWindowRgn(mHwnd, m_Region, TRUE);

				//if (auto hdc = GetWindowDC(mHwnd); hdc)
				//{
				//	FrameRgn(hdc, m_Region, GetStockBrush(BLACK_BRUSH), 1, 1);
				//	ReleaseDC(mHwnd, hdc);
				//}
			}
		}
	}
	break;

	//case MenuMessages::WMN_ERASEBKGND:
	//{
	//	if (!Dcx::setting_CustomMenusRounded)
	//		break;
	//
	//	const auto lRes = CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
	//	if (auto m_Region = CreateRectRgn(0, 0, 0, 0); m_Region)
	//	{
	//		const auto err = GetWindowRgn(mHwnd, m_Region);
	//
	//		//OffsetRgn(m_Region, -1, -1);
	//		//if ((err != NULLREGION) && (err != ERROR))
	//		//	FrameRgn((HDC)wParam, m_Region, GetStockBrush(BLACK_BRUSH), 2, 2);
	//
	//		RECT rc{};
	//		GetRgnBox(m_Region, &rc);
	//		rc.left += 2;
	//		rc.right -= 2;
	//		rc.top += 2;
	//		rc.bottom -= 2;
	//
	//		DeleteRgn(m_Region);
	//
	//		//DrawEdge((HDC)wParam, &rc, 0, 0);
	//		//FrameRect((HDC)wParam, &rc, GetStockBrush(BLACK_BRUSH));
	//
	//		if (auto tmp = CreateRoundRectRgn(rc.left, rc.top, rc.right, rc.bottom, 10, 10); tmp)
	//		{
	//			FrameRgn((HDC)wParam, m_Region, GetStockBrush(BLACK_BRUSH), 2, 2);
	//			DeleteRgn(tmp);
	//		}
	//	}
	//	return lRes;
	//}
	//break;

//#if DCX_DEBUG_OUTPUT
//	//case WM_MOUSEMOVE:
//	//{  // never called.
//	//	TString msg;
//	//	msg.tsprintf(TEXT("called: %d"), mHwnd);
//	//	mIRCLinker::debug(TEXT("WM_MOUSEMOVE"), msg);
//	//}
//	//break;
//	//case WM_MOUSELEAVE:
//	//{  // never called.
//	//	TString msg;
//	//	msg.tsprintf(TEXT("called: %d"), mHwnd);
//	//	mIRCLinker::debug(TEXT("WM_MOUSELEAVE"), msg);
//	//}
//	//break;
//#endif

	case MenuMessages::MN_SELECTITEM:
	{
		if (wParam == UINT_MAX)
		{
			for (auto itGet = g_winlist.begin(); itGet != g_winlist.end(); ++itGet)
			{
				auto win = *itGet;
				if (const auto dwStyle = dcxGetWindowExStyle(win); dcx_testflag(dwStyle, WS_EX_LAYERED))
				{
					BYTE current_alpha{ 255 };
					DWORD dFlags{ LWA_ALPHA };
					if (GetLayeredWindowAttributes(win, nullptr, &current_alpha, &dFlags))
					{
						if (current_alpha != 0xFFU)
							SetLayeredWindowAttributes(win, 0, 0xFFU, LWA_ALPHA); // make window solid
					}
				}
			}
		}
	}
	break;

	case MenuMessages::MN_FINDMENUWINDOWFROMPOINT:
	{
		const auto lRes = CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
		auto menu_hwnd = reinterpret_cast<HWND>(lRes);

		bool bAfter = false;
		for (auto itGet = g_winlist.begin(); itGet != g_winlist.end(); ++itGet)
		{
			auto win = *itGet;

			if ((win == menu_hwnd) || (!menu_hwnd))
				bAfter = true;

			if (const auto dwStyle = dcxGetWindowExStyle(win); dcx_testflag(dwStyle, WS_EX_LAYERED))
			{
				BYTE current_alpha{ 255 };
				DWORD dFlags{ LWA_ALPHA };
				if (GetLayeredWindowAttributes(win, nullptr, &current_alpha, &dFlags))
				{
					const BYTE alpha = (bAfter ? 0xFFU : Dcx::setting_CustomMenusAlpha);
					if (current_alpha != alpha)
						SetLayeredWindowAttributes(win, 0, alpha, LWA_ALPHA); // make window solid
				}

				//SetLayeredWindowAttributes(win, 0, (bAfter ? 0xFFU : Dcx::setting_CustomMenusAlpha), LWA_ALPHA);
			}
		}

		return lRes;
	}
	break;

	default:
		break;
	}

	return CallWindowProc(XPopupMenuManager::g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
}
#endif
