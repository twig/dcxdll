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

#if DCX_CUSTOM_MENUS
#include <thread>

namespace
{
	WNDPROC g_OldmIRCMenusWindowProc = nullptr;
	HWND g_toolTipWin = nullptr;
	TOOLINFO g_toolItem{};
	std::mutex g_ListLock;

	class CallBackTimer
	{
	public:
		CallBackTimer() noexcept
			:_execute(false)
		{}

		~CallBackTimer()
		{
			if (_execute.load(std::memory_order_acquire))
			{
				stop();
			};
		}
		CallBackTimer(const CallBackTimer&) = delete;
		CallBackTimer(CallBackTimer&&) = delete;
		CallBackTimer& operator =(const CallBackTimer&) = delete;
		CallBackTimer& operator =(CallBackTimer&&) = delete;
		bool operator==(const CallBackTimer& other) const = default;

		void setstop() noexcept
		{
			_execute.store(false, std::memory_order_release);
		}

		void stop()
		{
			setstop();
			if (_thd.joinable())
				_thd.join();
		}

		void start(int interval, std::function<void(void)> func)
		{
			if (_execute.load(std::memory_order_acquire))
			{
				stop();
			};
			_execute.store(true, std::memory_order_release);
			_thd = std::thread([this, interval, func]()
				{
					while (_execute.load(std::memory_order_acquire))
					{
						func();
						std::this_thread::sleep_for(
							std::chrono::milliseconds(interval));
					}
				});
		}

		void start(int interval, std::function<void(HWND)> func, HWND arg)
		{
			if (_execute.load(std::memory_order_acquire))
			{
				stop();
			};
			_execute.store(true, std::memory_order_release);
			_thd = std::thread([this, interval, func, arg]()
				{
					while (_execute.load(std::memory_order_acquire))
					{
						func(arg);
						std::this_thread::sleep_for(
							std::chrono::milliseconds(interval));
					}
				});
		}

		//template <typename T>
		//void start(int interval, std::function<void(T)> func, T arg)
		//{
		//	if (_execute.load(std::memory_order_acquire))
		//	{
		//		stop();
		//	};
		//	_execute.store(true, std::memory_order_release);
		//	_thd = std::thread([this, interval, func, arg]()
		//		{
		//			while (_execute.load(std::memory_order_acquire))
		//			{
		//				func(arg);
		//				std::this_thread::sleep_for(
		//					std::chrono::milliseconds(interval));
		//			}
		//		});
		//}

		bool is_running() const noexcept
		{
			return (_execute.load(std::memory_order_acquire) &&
				_thd.joinable());
		}

	private:
		std::atomic<bool> _execute;
		std::thread _thd;
	};

	CallBackTimer dcxHoverTimer;

	// Get list of open menu windows.
	auto& getGlobalMenuWindowList() noexcept
	{
		static std::vector<HWND> winlist;

		return winlist;
	}

	// Get list of open menus.
	auto& getGlobalMenuList() noexcept
	{
		static std::vector<HMENU> menulist;

		return menulist;
	}

	HMENU getBackMenu()
	{
		std::scoped_lock lk(g_ListLock);
		if (const auto& m = getGlobalMenuList(); !m.empty())
			return m.back();
		return nullptr;
	}

	HWND getBackWin()
	{
		std::scoped_lock lk(g_ListLock);
		if (const auto& m = getGlobalMenuWindowList(); !m.empty())
			return m.back();
		return nullptr;
	}

	void AddBackMenu(HMENU hMenu)
	{
		std::scoped_lock lk(g_ListLock);
		getGlobalMenuList().push_back(hMenu);
	}

	void AddBackWin(HWND hwnd)
	{
		std::scoped_lock lk(g_ListLock);
		getGlobalMenuWindowList().push_back(hwnd);
	}

	void RemoveBackMenu()
	{
		std::scoped_lock lk(g_ListLock);
		if (auto& m = getGlobalMenuList(); !m.empty())
			m.pop_back();
	}

	void RemoveBackWin()
	{
		std::scoped_lock lk(g_ListLock);
		if (auto& m = getGlobalMenuWindowList(); !m.empty())
			m.pop_back();
	}
}
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

	const auto hInst = GetModuleHandle(nullptr);

#if DCX_CUSTOM_MENUS
	if (HWND tmp_hwnd = CreateWindowEx(0, TEXT("#32768"), nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst, nullptr); tmp_hwnd)
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
	wc.hInstance = hInst;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	//wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hbrBackground = GetStockBrush(COLOR_WINDOWFRAME);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = XPOPUPMENUCLASS;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	DCX_DEBUG(mIRCLinker::debug, __FUNCTIONW__, TEXT("Creating menu owner..."));
	m_hMenuOwner = CreateWindow(XPOPUPMENUCLASS, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);

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

	clearMenus();

	m_mIRCPopupMenu.reset(nullptr);

	m_mIRCMenuBar->cleanMenu(GetMenu(mIRCLinker::getHWND()));

	m_mIRCMenuBar.reset(nullptr);

	m_mIRCScriptMenu.reset(nullptr);

	if (m_hMenuOwner)
		DestroyWindow(m_hMenuOwner);

	UnregisterClass(XPOPUPMENUCLASS, GetModuleHandle(nullptr));

#if DCX_CUSTOM_MENUS
	if (dcxHoverTimer.is_running())
	{
		try {
			dcxHoverTimer.stop();
		}
		catch (...) {};
	}

	if (g_toolTipWin && IsWindow(g_toolTipWin))
		DestroyWindow(g_toolTipWin);

	if (g_OldmIRCMenusWindowProc != nullptr)
	{
		if (HWND tmp_hwnd = CreateWindowEx(0, TEXT("#32768"), nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr); tmp_hwnd)
		{
			SetClassLongPtr(tmp_hwnd, GCLP_WNDPROC, (LONG_PTR)g_OldmIRCMenusWindowProc);
			DestroyWindow(tmp_hwnd);
			g_OldmIRCMenusWindowProc = nullptr;
		}
	}
#endif
}

#if DCX_CUSTOM_MENUS
HWND XPopupMenuManager::CreateTrackingToolTip(int toolID, HWND hDlg, WCHAR* pText) noexcept
{
	if (!hDlg || !pText)
		return nullptr;

	// Create a tooltip.
	HWND hwndTT = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASS, nullptr,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, nullptr, GetModuleHandle(nullptr), nullptr);

	if (!hwndTT)
		return nullptr;

	// Set up the tool information. In this case, the "tool" is the entire parent window.

	g_toolItem.cbSize = sizeof(TOOLINFO);
	g_toolItem.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
	g_toolItem.hwnd = hDlg;
	g_toolItem.hinst = GetModuleHandle(nullptr);
	g_toolItem.lpszText = pText;
	g_toolItem.uId = (UINT_PTR)hDlg;

	GetClientRect(hDlg, &g_toolItem.rect);

	// Associate the tooltip with the tool window.

	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&g_toolItem);

	return hwndTT;
}
#endif

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
				{
					m_mIRCMenuBar->convertMenu(menu, TRUE);
					Dcx::m_CurrentMenuAlpha = m_mIRCMenuBar->IsAlpha();
					Dcx::m_CurrentMenuRounded = m_mIRCMenuBar->IsRoundedWindow();
				}
			}
		}
		else {
			if (m_bIsActiveMircPopup)
			{
				m_mIRCPopupMenu->convertMenu(menu, FALSE);
				Dcx::m_CurrentMenuAlpha = m_mIRCPopupMenu->IsAlpha();
				Dcx::m_CurrentMenuRounded = m_mIRCPopupMenu->IsRoundedWindow();
			}
		}

		m_bIsSysMenu = false;
#if DCX_CUSTOM_MENUS
		getGlobalMenuList().push_back(menu);

		if (!g_toolTipWin)
		{
			static TCHAR szTest[] = L"tooltip...";
			g_toolTipWin = CreateTrackingToolTip(100, mHwnd, &szTest[0]);
		}
#endif
		return lRes;
	}
	else
		m_bIsSysMenu = true;

	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_INITMENUPOPUP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnUninitMenuPopup(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	auto menu = reinterpret_cast<HMENU>(wParam);

#if DCX_CUSTOM_MENUS
	if (!m_bIsSysMenu)	// we dont care about system menus
	{
		if (auto vMenu = getGlobalMenuList(); !vMenu.empty())	// check vector is not empty before poping.
			vMenu.pop_back();
	}
#endif

	// Unset the custom menu handle so we dont have to keep track of submenus anymore.
	if (menu == m_hMenuCustom)
		m_hMenuCustom = nullptr;

	if (m_bIsMenuBar && !m_bIsSysMenu && m_bIsActiveMircMenubarPopup)
		m_mIRCMenuBar->deleteAllItemData(menu);

	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_UNINITMENUPOPUP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnExitMenuLoop(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
#if DCX_CUSTOM_MENUS
	if (g_toolTipWin)
	{
		DestroyWindow(g_toolTipWin);
		g_toolTipWin = nullptr;
	}
	getGlobalMenuList().clear();
#endif

	if (!m_bIsMenuBar && m_bIsActiveMircPopup)
		m_mIRCMenuBar->clearAllMenuItems();

	return mIRCLinker::callDefaultWindowProc(mHwnd, WM_EXITMENULOOP, wParam, lParam);
}

LRESULT XPopupMenuManager::OnCommand(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	// Check if the message came from the menubar
	if ((Dcx::dcxHIWORD(wParam) == 0) && (m_bIsMenuBar))
	{
		if (Dcx::XMenubar.hasCallback())
		{
			// If the user requested the message to be halted ...
			if (Dcx::XMenubar.parseCallback(Dcx::dcxLOWORD(wParam)))
			{
				// Stop parsing and prevent default action.
				return 0L;
			}
		}
		if (dcxSignal.xdock)
		{
			switch (Dcx::dcxLOWORD(wParam))
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
	parseCommand(input, getMenuByName(input.getfirsttok(1), true));
}

void XPopupMenuManager::parseCommand(const TString& input, XPopupMenu* const p_Menu)
{
	const auto tsMenuName(input.getfirsttok(1));
	const XSwitchFlags flags(input.getnexttok());	// tok 2
	const auto numtok = input.numtok();

	// Special mIRC Menu
	if (!p_Menu && !flags[TEXT('c')])
		throw Dcx::dcxException(TEXT("\"%\" doesn't exist : see /xpopup -c"), tsMenuName);

	// xpopup -b - [MENU] [SWITCH] [FILENAME]
	if (flags[TEXT('b')])
	{
		HBITMAP hBitmap = nullptr;
		TString tsFilename;

		if (numtok > 2)
		{
			tsFilename = input.getlasttoks().trim();	// tok 3, -1

			if (tsFilename == TEXT("none"))
			{
				// ignore 'none' to maintain compatibility
			}
			else {
				if (!IsFile(tsFilename))
					throw Dcx::dcxException(TEXT("Unable to Access File: %"), tsFilename);

				hBitmap = dcxLoadBitmap(hBitmap, tsFilename);

				if (hBitmap == nullptr)
					throw Dcx::dcxException(TEXT("Unable to Load Image: %"), tsFilename);
			}
		}

		p_Menu->setBackBitmap(hBitmap, tsFilename);
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

		// Ook: maybe change to something like this instead? need to check how this affects alloc/free
		//std::vector<XPopupMenu> testv;
		//testv.emplace_back(tsMenuName, style);
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
		if (!himl)
			throw DcxExceptions::dcxUnableToCreateImageList();

		const auto tsFlags(input.getnexttok());	// tok 3
		const auto tsIndex(input.getnexttok());	// tok 4
		auto filename(input.getlasttoks());		// tok 5, -1

		Dcx::dcxLoadIconRange(himl, filename, false, tsFlags, tsIndex);
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

		p_Menu->setItemStyleString(input.getnexttok());
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
			p_Menu->SetRoundedSelector((input.getnexttok().to_int() > 0));	// tok 4
		else if (xflags[TEXT('a')]) // Set Alpha value of menu. 0-255
		{
			const std::byte alpha{ (input.getnexttok().to_<UINT>() & 0xFF) };	// tok 4

			p_Menu->SetAlpha(alpha);
		}
		else if (xflags[TEXT('t')]) // enable/disable tooltips for menu
		{
			p_Menu->setTooltipsState((input.getnexttok().to_int() ? true : false));
		}
		else if (xflags[TEXT('R')]) // Set Rounded menu window on/off
			p_Menu->SetRoundedWindow((input.getnexttok().to_int() > 0));	// tok 4
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
	case TEXT("istooltips"_hash):
	{
		if (!p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		return dcx_truefalse(p_Menu->IsToolTipsEnabled());
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
		if (!p_Menu)
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
		//if (!p_Menu)
		//	throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);
		//
		//TString szStyles(TEXT('+'));
		//const auto iExStyles = p_Menu->getItemStyle();
		//
		//if (dcx_testflag(iExStyles, XPS_ICON3D))
		//	szStyles += TEXT('i');
		//if (dcx_testflag(iExStyles, XPS_DISABLEDSEL))
		//	szStyles += TEXT('d');
		//if (dcx_testflag(iExStyles, XPS_ICON3DSHADOW))
		//	szStyles += TEXT('p');
		//
		//return szStyles;

		if (!p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		return p_Menu->getItemStyleString();
	}
	break;
	case TEXT("colours"_hash):
	case TEXT("colors"_hash):
	{
		if (!p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		TString tsRes;
		_ts_sprintf(tsRes, TEXT("% % % % % % % % % % %"), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_BACKGROUND), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_ICONBOX),
			p_Menu->getColor(XPopupMenu::MenuColours::XPMC_CHECKBOX), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_CHECKBOX_DISABLED), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX_DISABLED),
			p_Menu->getColor(XPopupMenu::MenuColours::XPMC_TEXT_DISABLED), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTIONBOX_BORDER),
			p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SEPARATOR), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_TEXT), p_Menu->getColor(XPopupMenu::MenuColours::XPMC_SELECTEDTEXT));
		return tsRes;
	}
	break;
	case TEXT("colour"_hash):
	case TEXT("color"_hash):
	{
		if (!p_Menu)
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
		if (!p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		return dcx_truefalse(p_Menu->IsRoundedSelector());
	}
	break;
	case TEXT("isroundedmenu"_hash):
	{
		if (!p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		return dcx_truefalse(p_Menu->IsRoundedWindow());
	}
	break;
	case TEXT("alpha"_hash):
	{
		if (!p_Menu)
			throw Dcx::dcxException(TEXT("\"%\" doesn't exist, see /xpopup -c"), tsMenuName);

		TString tsRes;
		tsRes += gsl::narrow_cast<uint8_t>(p_Menu->IsAlpha());
		return tsRes;
	}
	break;
	case TEXT("marked"_hash):
	{
		if (!p_Menu)
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

XPopupMenuItem* XPopupMenuManager::getMenuItemByID(const HMENU hMenu, const int id) const noexcept
{
	if ((!hMenu) || (id == -1))
		return nullptr;

	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;
	if (GetMenuItemInfoW(hMenu, id, TRUE, &mii))
	{
		if (auto* p_Item = reinterpret_cast<XPopupMenuItem*>(mii.dwItemData); p_Item)
		{
			if (Dcx::XPopups.isItemValid(p_Item))
				return p_Item;
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
	if (!hMenu || !hMatch)
		return false;

	const auto n = GetMenuItemCount(hMenu);

	for (auto i = decltype(n){0}; i < n; ++i)
	{
		if (const auto hTemp = GetSubMenu(hMenu, i); hTemp)
		{
			if (hTemp == hMatch)
				return true;

			if (isMenuBarMenu(hTemp, hMatch))
				return true;
		}
	}

	return false;
}

const bool XPopupMenuManager::isItemValid(const XPopupMenuItem* const pItem) const noexcept
{
	if (!pItem)
		return false;
	if (m_mIRCMenuBar && m_mIRCMenuBar->isItemValid(pItem))
		return true;
	if (m_mIRCPopupMenu && m_mIRCPopupMenu->isItemValid(pItem))
		return true;
	if (m_mIRCScriptMenu && m_mIRCScriptMenu->isItemValid(pItem))
		return true;
	for (const auto a : m_vpXPMenu)
	{
		if (a && a->isItemValid(pItem))
			return true;
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
	if (!popups)
		throw DcxExceptions::dcxInvalidArguments();

	if (!popup)
	{
		// Find the dataset with the name we want.
		for (auto element = popups->FirstChildElement("popup"); element; element = element->NextSiblingElement("popup"))
		{
			if (const TString name(element->Attribute("name")); name == popupDataset)
			{
				popup = element;
				break;
			}
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

	if (queryIntAttribute(popup, "version") > 0)
	{
		VectorOfIcons vIcons;
		// Parse icons
		if (const auto* xIcons = popup->FirstChildElement("icons"); xIcons)
		{
			for (auto xIcon = xIcons->FirstChildElement("icon"); xIcon; xIcon = xIcon->NextSiblingElement("icon"))
			{
				xmlIcon xi;
				xi.xIcon = xIcon;
				xi.tsID = L"0";
				xi.tsType = L"menu";

				vIcons.push_back(xi);
			}
		}
		menu->fromXml(popups, popup, vIcons);
		return;
	}

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
		if (auto tsAttr(GetMenuAttributeFromXML(tmp.value, popup, globalStyles)); !tsAttr.empty())
		{
			mIRCLinker::eval(tsAttr, tsAttr);
			if (!tsAttr.empty())
				menu->setColor(tmp.code, tsAttr.to_<COLORREF>());
		}
	}

	// Set background image if CUSTOM style used
	if ((style == XPopupMenu::MenuStyle::XPMS_CUSTOM) || (style == XPopupMenu::MenuStyle::XPMS_CUSTOMBIG))
	{
		if (TString tsBkg(queryEvalAttribute(popup, "background")); !tsBkg.empty())
		{
			if (const auto hBitmap = dcxLoadBitmap(nullptr, tsBkg); hBitmap)
				menu->setBackBitmap(hBitmap, tsBkg);
		}
	}

	// Successfully created a menu.
	Dcx::XPopups.addMenu(menu);

	// Parse icons
	if (const auto* xIcons = popup->FirstChildElement("icons"); xIcons)
	{
		for (auto xIcon = xIcons->FirstChildElement("icon"); xIcon; xIcon = xIcon->NextSiblingElement("icon"))
		{
			// Flags
			const TString flags(queryAttribute(xIcon, "flags", "+"));
			const TString indexes(queryAttribute(xIcon, "index", "0"));

			// Filename
			if (const TString tsSrc(queryEvalAttribute(xIcon, "src")); !tsSrc.empty())
			{
				TString command;
				_ts_sprintf(command, TEXT("% -i % % %"), popupName, flags, indexes, tsSrc);
				Dcx::XPopups.parseCommand(command, menu);
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

	// Iterate through each child Element.
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
	if (!attrib || !popup)
		return {};

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
void XPopupMenuManager::dcxCheckMenuHover() noexcept
{
	//static POINT savedpt{};
	//
	//// make a copy of list to avoid it being modified elsewhere.
	//std::vector<HWND> v = getGlobalMenuWindowList();
	//
	//if (v.empty())
	//	return;
	//
	//if (Dcx::dcxCursorPos pt; pt)
	//{
	//	if (HWND win = v.back(); win)
	//	{
	//		if (Dcx::dcxWindowRect rc(win); rc)
	//		{
	//			if (PtInRect(&rc, pt))
	//			{
	//				// over window...
	//
	//				if ((savedpt.x == pt.x) && (savedpt.y == pt.y))
	//				{
	//					// hover!
	//					PostMessage(win, WM_NCMOUSEHOVER, HTMENU, Dcx::dcxMAKELPARAM(pt.x,pt.y));
	//				}
	//			}
	//		}
	//	}
	//	savedpt = pt;
	//}

	// make a copy of list to avoid it being modified elsewhere.
	if (std::vector<HWND> v(getGlobalMenuWindowList()); !v.empty())
		dcxCheckMenuHover2(v.back());
}

void XPopupMenuManager::dcxCheckMenuHover2(HWND win) noexcept
{
	static POINT savedpt{};

	if (!win)
		return;

	if (Dcx::dcxCursorPos pt; pt)
	{
		if (Dcx::dcxWindowRect rc(win); rc)
		{
			if (PtInRect(&rc, pt))
			{
				// over window...

				if ((savedpt.x == pt.x) && (savedpt.y == pt.y))
				{
					// hover!
					PostMessage(win, WM_NCMOUSEHOVER, HTMENU, Dcx::dcxMAKELPARAM(pt.x, pt.y));
				}
			}
		}
		savedpt = pt;
	}
}

LRESULT CALLBACK XPopupMenuManager::mIRCMenusWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Incase execution somehow ends up here without this pointer being set.
	if (!g_OldmIRCMenusWindowProc)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	const WindowMessages mm = gsl::narrow_cast<WindowMessages>(uMsg);

	switch (mm)
	{
	case WindowMessages::eWM_NCCREATE:
	{
		if (Dcx::m_CurrentMenuAlpha == std::byte{ 255 })
			break;

		dcxlParam(LPCREATESTRUCT, cs);

		cs->dwExStyle |= WS_EX_LAYERED | WS_EX_COMPOSITED;
	}
	break;

	case WindowMessages::eWM_CREATE:
	{
		if (Dcx::m_CurrentMenuAlpha != std::byte{ 255 })
		{
			dcxlParam(LPCREATESTRUCT, cs);
			cs->dwExStyle |= WS_EX_LAYERED | WS_EX_COMPOSITED;

			// check for previous menu...
			if (!getGlobalMenuWindowList().empty())
			{
				// change previous window.
				if (auto parent = getBackWin(); parent)
				{
					// make sure previous menu is layered.
					if (const auto dwStyle = dcxGetWindowExStyle(parent); !dcx_testflag(dwStyle, WS_EX_LAYERED))
						dcxSetWindowExStyle(parent, dwStyle | WS_EX_LAYERED);

					// set alpha for previous menu.
					//SetLayeredWindowAttributes(parent, 0, Dcx::setting_CustomMenusAlpha, LWA_ALPHA); // 0xCC = 80% Opaque, 0xC0
				}
			}
		}

		// add this window to list.
		AddBackWin(mHwnd);
	}
	break;

	case WindowMessages::eWM_DESTROY:
	{
		if (getGlobalMenuWindowList().empty())
			break;

		// remove ourselfs.
		RemoveBackWin();

		if (!getGlobalMenuWindowList().empty())
		{
			if (Dcx::m_CurrentMenuAlpha != std::byte{ 255 })
			{
				// get previous menu window.
				if (auto parent = getBackWin(); parent)
				{
					if (const auto dwStyle = dcxGetWindowExStyle(parent); !dcx_testflag(dwStyle, WS_EX_LAYERED))
						break;

					SetLayeredWindowAttributes(parent, 0, 0xFFU, LWA_ALPHA); // 0xCC = 80% Opaque
				}
			}
		}
	}
	break;

	// code for rounded windows etc..
	case WindowMessages::eWM_SIZE:
	{
		// message is sent AFTER the window is resized.
		// this is client area size, we need window rect

		if (!Dcx::m_CurrentMenuRounded)
			break;
		if (Dcx::dcxWindowRect rc(mHwnd); rc)
		{
			const auto width = rc.Width();
			const auto height = rc.Height();
			constexpr int radius = 10;
			if (auto m_Region = CreateRoundRectRgn(0, 0, width, height, radius, radius); m_Region)
				SetWindowRgn(mHwnd, m_Region, TRUE);
		}
	}
	break;

	case WindowMessages::eMN_SELECTITEM:
	{
		if (!dcxHoverTimer.is_running())
		{
			const Dcx::dcxCursorPos pt;
			const Dcx::dcxWindowRect rc(mHwnd);

			// hide any current tooltip
			//if (g_toolTipWin && IsWindow(g_toolTipWin))
			//	SendMessage(g_toolTipWin, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&g_toolItem);

			// hide any current tooltip
			if (g_toolTipWin && IsWindow(g_toolTipWin))
				Dcx::dcxToolTip_TrackActivate(g_toolTipWin, FALSE, &g_toolItem);

			// start thread to check for hover...
			if (!getGlobalMenuList().empty() && !getGlobalMenuWindowList().empty() && PtInRect(&rc, pt))
				dcxHoverTimer.start(600, XPopupMenuManager::dcxCheckMenuHover);

			//if (!getGlobalMenuList().empty() && !getGlobalMenuWindowList().empty() && PtInRect(&rc, pt))
			//	dcxHoverTimer.start(600, XPopupMenuManager::dcxCheckMenuHover2, mHwnd);
		}

		// if cursor NOT over menu, make all menus solid.
		if (wParam == UINT_MAX)
		{
			if (Dcx::m_CurrentMenuAlpha == std::byte{ 255 })
				break;

			// iterate through all saved hwnds & make all solid.
			for (const auto& win : getGlobalMenuWindowList())
			{
				if (const auto dwStyle = dcxGetWindowExStyle(win); dcx_testflag(dwStyle, WS_EX_LAYERED))
				{
					BYTE current_alpha{ 0xFFU };
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

	case WindowMessages::eMN_FINDMENUWINDOWFROMPOINT:
	{
		if (Dcx::m_CurrentMenuAlpha == std::byte{ 255 })
			break;

		const auto lRes = CallWindowProc(g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
		auto menu_hwnd = reinterpret_cast<HWND>(lRes);

		bool bAfter = false;

		for (const auto& win : getGlobalMenuWindowList())
		{
			if ((win == menu_hwnd) || (!menu_hwnd))
				bAfter = true;

			if (const auto dwStyle = dcxGetWindowExStyle(win); dcx_testflag(dwStyle, WS_EX_LAYERED))
			{
				BYTE current_alpha{ 255 };
				DWORD dFlags{ LWA_ALPHA };
				if (GetLayeredWindowAttributes(win, nullptr, &current_alpha, &dFlags))
				{
					const BYTE alpha = (bAfter ? 0xFFU : std::to_integer<BYTE>(Dcx::m_CurrentMenuAlpha));
					if (current_alpha != alpha)
						SetLayeredWindowAttributes(win, 0, alpha, LWA_ALPHA);
				}
			}
		}

		return lRes;
	}
	break;

	case WindowMessages::eWM_NCMOUSEHOVER:
	{
		if (dcxHoverTimer.is_running())
			dcxHoverTimer.stop();

		if (!getGlobalMenuList().empty() && g_toolTipWin && IsWindow(g_toolTipWin))
		{
			const POINT pt{ GET_X_LPARAM(lParam) , GET_Y_LPARAM(lParam) };
			if (auto hMenu = getBackMenu(); hMenu)
			{
				if (const auto id = MenuItemFromPoint(nullptr, hMenu, pt); id >= 0)
				{
					if (auto p_Item = Dcx::XPopups.getMenuItemByID(hMenu, id); p_Item)
					{
						if (p_Item->IsTooltipsEnabled())
						{
							//g_toolItem.lpszText = const_cast<TCHAR*>(p_Item->getItemTooltipText().to_chr());
							//SendMessage(g_toolTipWin, TTM_SETTOOLINFO, 0, (LPARAM)&g_toolItem);
							//if (!p_Item->getItemTooltipText().empty())
							//{
							//	SendMessage(g_toolTipWin, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem);
							//	SendMessage(g_toolTipWin, TTM_TRACKPOSITION, 0, Dcx::dcxMAKELPARAM(GET_X_LPARAM(lParam) + 10, GET_Y_LPARAM(lParam) - 20));
							//}

							g_toolItem.lpszText = const_cast<TCHAR*>(p_Item->getItemTooltipText().to_chr());
							Dcx::dcxToolTip_SetToolInfo(g_toolTipWin, &g_toolItem);
							if (!p_Item->getItemTooltipText().empty())
							{
								Dcx::dcxToolTip_TrackActivate(g_toolTipWin, TRUE, &g_toolItem);
								Dcx::dcxToolTip_TrackPosition(g_toolTipWin, pt.x + 10, pt.y - 20);
							}
						}
					}
				}
			}
		}
		return 0;
	}
	break;

	default:
		break;
	}

	return CallWindowProc(g_OldmIRCMenusWindowProc, mHwnd, uMsg, wParam, lParam);
}
#endif
