/*
	* Various docking commands that are not part of the old UltraDock
	*
	*	TODO: Add subclassing of channel/custom/toolbar/switchbar/treelist? SetClassLongPtr(new_wndproc) & FindWindowEx(classname)
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"

/*! \brief DCXDOCK structure, used when adding docked windows.
 *
 * Added as "dcx_dock" to docked windows via AddProp()
 */

namespace
{
	struct DCXDOCK
	{
		WNDPROC oldProc{};
		HWND win{};
		DWORD flags{};
		TString type;

		DCXDOCK(const WNDPROC& proc, const HWND& hwnd, const DWORD& f, const TString& t)
			: oldProc(proc)
			, win(hwnd)
			, flags(f)
			, type(t)
		{}
	};
	using LPDCXDOCK = DCXDOCK*;

	BOOL CALLBACK SizeDocked(HWND hwnd, LPARAM lParam) noexcept
	{
		if (!hwnd)
			return FALSE;

		const auto flags = Dcx::dcxGetProp<DockFlags>(hwnd, TEXT("dcx_docked"));
		const auto hParent = GetParent(hwnd);
		if (!hParent)
			return FALSE;

		if (flags != DockFlags::DOCKF_NONE && flags != DockFlags::DOCKF_NORMAL)
		{
			RECT rcParent{}, rcThis{};
			if (!GetClientRect(hParent, &rcParent) || !GetWindowRect(hwnd, &rcThis))
				return FALSE;

			if (dcx_testflag(flags, DockFlags::DOCKF_SHOWSCROLLBARS))
			{
				// mIRC's channel/query/status window's scrollbar isnt a system scrollbar so these functions fail.
				//SCROLLBARINFO sbi;
				//// vertical scroller
				//ZeroMemory(&sbi,sizeof(SCROLLBARINFO));
				//sbi.cbSize = sizeof(SCROLLBARINFO);
				//GetScrollBarInfo(hScroll, OBJID_CLIENT, &sbi);
				//if (!(sbi.rgstate[0] & (STATE_SYSTEM_INVISIBLE|STATE_SYSTEM_OFFSCREEN|STATE_SYSTEM_UNAVAILABLE)))
				//	rcParent.right -= (sbi.rcScrollBar.right - sbi.rcScrollBar.left);

				//// Horizontal scroller
				//ZeroMemory(&sbi,sizeof(SCROLLBARINFO));
				//sbi.cbSize = sizeof(SCROLLBARINFO);
				//GetScrollBarInfo(hParent, OBJID_HSCROLL, &sbi);
				//if (!(sbi.rgstate[0] & (STATE_SYSTEM_INVISIBLE|STATE_SYSTEM_OFFSCREEN|STATE_SYSTEM_UNAVAILABLE)))
				//	rcParent.bottom -= (sbi.rcScrollBar.bottom - sbi.rcScrollBar.top);

				if (const auto hScroll = FindWindowEx(hParent, nullptr, TEXT("ScrollBar"), nullptr); IsWindow(hScroll) /*&& IsWindowVisible(hScroll)*/)
				{
					RECT rcScroll{};

					//if (!GetWindowRect(hScroll, &rcScroll))
					//	return FALSE;
					//
					//MapWindowRect(nullptr,hParent, &rcScroll);

					if (!GetWindowRectParent(hScroll, &rcScroll))
						return FALSE;

					rcParent.right -= (rcScroll.right - rcScroll.left);
				}
			}
			if (dcx_testflag(flags, DockFlags::DOCKF_SIZE))
				SetWindowPos(hwnd, nullptr, 0, 0, (rcParent.right - rcParent.left), (rcParent.bottom - rcParent.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOMOVE);
			else if (dcx_testflag(flags, DockFlags::DOCKF_AUTOH))
				SetWindowPos(hwnd, nullptr, 0, 0, (rcParent.right - rcParent.left), (rcThis.bottom - rcThis.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOMOVE);
			else if (dcx_testflag(flags, DockFlags::DOCKF_AUTOV))
				SetWindowPos(hwnd, nullptr, 0, 0, (rcThis.right - rcThis.left), (rcParent.bottom - rcParent.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOMOVE);
#if DCX_USE_TESTCODE
			else if (dcx_testflag(flags, DockFlags::DOCKF_UNDERSTATIC))
			{
				auto refWnd = FindWindowExW(hParent, nullptr, TEXT("Static"), nullptr);
				if (!refWnd)
					return FALSE;

				RECT rcRef{};
				if (!GetWindowRect(refWnd, &rcRef))
					return FALSE;
				MapWindowRect(nullptr, hwnd, &rcRef);

				SetWindowPos(refWnd, nullptr, 0, 0, (rcRef.right - rcRef.left), (rcParent.bottom - rcParent.top) - (rcThis.bottom - rcThis.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOMOVE);

				SetWindowPos(hwnd, nullptr, 0, (rcRef.bottom - rcRef.top) - (rcThis.bottom - rcThis.top), (rcRef.right - rcRef.left), (rcThis.bottom - rcThis.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
			}
			else if (dcx_testflag(flags, DockFlags::DOCKF_ABOVESTATIC))
			{
				auto refWnd = FindWindowExW(hParent, nullptr, TEXT("Static"), nullptr);
				if (!refWnd)
					return FALSE;

				RECT rcRef{};
				if (!GetWindowRect(refWnd, &rcRef))
					return FALSE;
				MapWindowRect(nullptr, hwnd, &rcRef);

				SetWindowPos(refWnd, nullptr, 0, (rcThis.bottom - rcThis.top), (rcRef.right - rcRef.left), (rcParent.bottom - rcParent.top) - (rcThis.bottom - rcThis.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

				SetWindowPos(hwnd, nullptr, 0, 0, (rcRef.right - rcRef.left), (rcThis.bottom - rcThis.top), SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
			}
#endif
		}
		// disable scroll bars if requested, this needs to be done here as the listbox re-enables them.
		if (dcx_testflag(flags, DockFlags::DOCKF_NOSCROLLBARS))
			ShowScrollBar(hParent, SB_BOTH, FALSE);
		return TRUE;
	}

	LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		const auto dd = Dcx::dcxGetProp<LPDCXDOCK>(mHwnd, TEXT("dcx_dock"));
#ifdef DCX_DEBUG_OUTPUT
		if (dcxSignal.xdock)
			mIRCLinker::signal(TEXT("dock debug %"), uMsg);
#endif
		if (!dd)
			return DefWindowProc(mHwnd, uMsg, wParam, lParam);

		switch (uMsg)
		{
		case WM_SIZE:
		{
#if DCX_USE_TESTCODE
			//auto lRes = CallWindowProc(dd->oldProc, mHwnd, uMsg, wParam, lParam);

			if (dcxSignal.xdock)
			{
				if (!dd->type.empty())
					mIRCLinker::signal(TEXT("size % % % %"), dd->type, from_hwnd(dd->win), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
				else
					mIRCLinker::signal(TEXT("size Custom % % %"), from_hwnd(dd->win), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
			}
			EnumChildWindows(mHwnd, SizeDocked, 0);
			//return lRes;
			//return DefWindowProc(mHwnd, uMsg, wParam, lParam);
#else
			if (dcxSignal.xdock)
			{
				if (!dd->type.empty())
					mIRCLinker::signal(TEXT("size % % % %"), dd->type, from_hwnd(dd->win), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
				else
					mIRCLinker::signal(TEXT("size Custom % % %"), from_hwnd(dd->win), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
			}
			EnumChildWindows(mHwnd, SizeDocked, 0);
#endif
		}
		break;
		case WM_PARENTNOTIFY:
		{
			if (Dcx::dcxLOWORD(wParam) == WM_DESTROY)
				RemoveProp(to_hwnd(lParam), TEXT("dcx_docked"));
		}
		break;
#ifdef DCX_DEBUG_OUTPUT
		case WM_CLOSE:
		{
			if (dcxSignal.xdock)
			{
				if (!dd->type.empty())
					mIRCLinker::signal(TEXT("close % %"), dd->type, from_hwnd(dd->win));
				else
					mIRCLinker::signal(TEXT("close Custom %"), from_hwnd(dd->win));
			}
		}
		break;
#endif
		case WM_DESTROY:
		{
			EnumChildWindows(mHwnd, EnumDocked, 0);
			RemoveProp(mHwnd, TEXT("dcx_dock"));
			// check windproc hasnt been changed
			if (Dcx::dcxGetWindowProc(mHwnd) == mIRCDockWinProc)
				SubclassWindow(mHwnd, dd->oldProc);
			delete dd;
			PostMessage(mHwnd, uMsg, 0, 0);
			return 0L;
		}
		break;
		default:
			break;
		}
		if (!dd->oldProc)
			return DefWindowProc(mHwnd, uMsg, wParam, lParam);

		return CallWindowProc(dd->oldProc, mHwnd, uMsg, wParam, lParam);
	}

	bool DockWindow(const HWND mWnd, const HWND temp, const TCHAR* find, const TString& flag)
	{

		if ((FindUltraDock(temp)) || (GetProp(temp, TEXT("dcx_docked"))))
			throw Dcx::dcxException("Window is Already docked.");

		HWND sWnd{ nullptr };
		// get window HWND
		if (!find)
			sWnd = GetDlgItem(mWnd, 32918);
		else
			sWnd = FindWindowEx(mWnd, nullptr, find, nullptr);

		if (!IsWindow(sWnd))
			throw Dcx::dcxException("Unable to Find Host Window.");

		const XSwitchFlags xflags(flag);

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flags: Flags must start with a '+'");

		// change win style to child
		//RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
		RemStyles(temp, GWL_STYLE, WS_POPUP);
		AddStyles(temp, GWL_STYLE, WS_CHILDWINDOW);

		// get window pos
#if DCX_USE_WRAPPERS
		const Dcx::dcxWindowRect rc(temp);
#else
		RECT rc{};
		if (!GetWindowRect(temp, &rc))
			throw Dcx::dcxException("Unable to get window rect");
#endif

		// if prop not alrdy set then set it & subclass.
		if (!GetProp(sWnd, TEXT("dcx_dock")))
		{
			// subclass window.
			{
				auto dd = std::make_unique<DCXDOCK>(SubclassWindow(sWnd, mIRCDockWinProc), mWnd, 0U, find);

				if (!SetProp(sWnd, TEXT("dcx_dock"), dd.release()))
					throw Dcx::dcxException("Unable to SetProp");

			}
		}
		DockFlags flags = DockFlags::DOCKF_NORMAL;
		if (xflags[TEXT('s')])
			flags = DockFlags::DOCKF_SIZE;				// Auto size Horizontal & Vertical

		if (xflags[TEXT('h')])
			flags = DockFlags::DOCKF_AUTOH;			// Auto Size Horizontal

		if (xflags[TEXT('v')])
			flags = DockFlags::DOCKF_AUTOV;			// Auto Size Vertical

		if (xflags[TEXT('b')])
			flags |= DockFlags::DOCKF_NOSCROLLBARS;	// disable scrollbars
		if (xflags[TEXT('B')])
			flags |= DockFlags::DOCKF_SHOWSCROLLBARS;	// make scrollbars visible & don't overlap them.

		SetProp(temp, TEXT("dcx_docked"), reinterpret_cast<HANDLE>(flags));
		//ShowScrollBar(sWnd,SB_BOTH,FALSE);
		AddStyles(sWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN); // this helps with rendering glitches.
		// set parent and move it to top-left corner
		SetParent(temp, sWnd);
		MoveWindow(temp, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 1);
		EnumChildWindows(sWnd, SizeDocked, 0);
		return true;
	}

#if DCX_USE_TESTCODE
	bool DockChanTypeWindow(const HWND mWnd, const HWND temp, const TCHAR* find, const TString& flag)
	{

		if ((FindUltraDock(temp)) || (GetProp(temp, TEXT("dcx_docked"))))
			throw Dcx::dcxException("Window is Already docked.");

		HWND sWnd{ mWnd };
		// get window HWND
		if (find)
			sWnd = FindWindowEx(mWnd, nullptr, find, nullptr);

		if (!IsWindow(sWnd))
			throw Dcx::dcxException("Unable to Find Host Window.");

		const XSwitchFlags xflags(flag);

		if (!xflags[TEXT('+')])
			throw Dcx::dcxException("Invalid Flags: Flags must start with a '+'");

		// change win style to child
		//RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
		RemStyles(temp, GWL_STYLE, WS_POPUP);
		AddStyles(temp, GWL_STYLE, WS_CHILDWINDOW);

		// get window pos
#if DCX_USE_WRAPPERS
		const Dcx::dcxWindowRect rc(temp);
#else
		RECT rc{};
		if (!GetWindowRect(temp, &rc))
			throw Dcx::dcxException("Unable to get window rect");
#endif

		// if prop not alrdy set then set it & subclass.
		if (!GetProp(sWnd, TEXT("dcx_dock")))
		{
			// subclass window.
			{
				auto dd = std::make_unique<DCXDOCK>(SubclassWindow(sWnd, mIRCDockWinProc), mWnd, 0U, find);

				if (!SetProp(sWnd, TEXT("dcx_dock"), dd.release()))
					throw Dcx::dcxException("Unable to SetProp");
			}
		}
		DockFlags flags = DockFlags::DOCKF_NORMAL;
		if (xflags[TEXT('s')])
			flags = DockFlags::DOCKF_SIZE;				// Auto size Horizontal & Vertical

		if (xflags[TEXT('h')])
			flags = DockFlags::DOCKF_AUTOH;			// Auto Size Horizontal

		if (xflags[TEXT('v')])
			flags = DockFlags::DOCKF_AUTOV;			// Auto Size Vertical

		if (xflags[TEXT('b')])
			flags |= DockFlags::DOCKF_NOSCROLLBARS;	// disable scrollbars
		if (xflags[TEXT('B')])
			flags |= DockFlags::DOCKF_SHOWSCROLLBARS;	// make scrollbars visible & don't overlap them.

		if (xflags[TEXT('U')])
			flags |= DockFlags::DOCKF_UNDERSTATIC;
		else if (xflags[TEXT('A')])
			flags |= DockFlags::DOCKF_ABOVESTATIC;

		SetProp(temp, TEXT("dcx_docked"), reinterpret_cast<HANDLE>(flags));
		//ShowScrollBar(sWnd,SB_BOTH,FALSE);
		AddStyles(sWnd, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN); // this helps with rendering glitches.
		// set parent and move it to top-left corner
		SetParent(temp, sWnd);
		MoveWindow(temp, 0, 0, rc.right - rc.left, rc.bottom - rc.top, 1);
		EnumChildWindows(sWnd, SizeDocked, 0);
		return true;
	}
#endif

	void UnDock(const HWND hwnd)
	{
		if (!GetProp(hwnd, TEXT("dcx_docked")))
			throw Dcx::dcxException("Window is not docked");

		// Remove Style for undocking purpose
		RemStyles(hwnd, GWL_STYLE, WS_BORDER);
		//WS_CHILDWINDOW |
		RemStyles(hwnd, GWL_EXSTYLE, WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
		// Add Styles input by user
		AddStyles(hwnd, GWL_STYLE, WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
		AddStyles(hwnd, GWL_EXSTYLE, WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
		RemStyles(hwnd, GWL_STYLE, WS_CHILDWINDOW);
		SetParent(hwnd, nullptr);
		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
		RemoveProp(hwnd, TEXT("dcx_docked"));
	}
}

GSL_SUPPRESS(type.4)
BOOL CALLBACK EnumDocked(HWND hwnd, LPARAM lParam)
{
	if (auto dd = Dcx::dcxGetProp<LPDCXDOCK>(hwnd, TEXT("dcx_dock")); dd)
	{
		RemoveProp(hwnd, TEXT("dcx_dock"));
		SubclassWindow(hwnd, dd->oldProc);
		delete dd;
	}
	if (GetProp(hwnd, TEXT("dcx_docked")))
		UnDock(hwnd);
	return TRUE;
}

/*! \brief xdock [SWITCH] [hwnd to dock] [+options] (destination hwnd)
 *
 * xdock -p
 * xdock -C [hwnd] [cursor type] [filename.cur]
 * xdock -M [1|0]
 * xdock -R [1|0]
 * xdock -S [1|0]
 * xdock -T [1|0]
 *
 * xdock -b [hwnd to dock] [+options]
 * xdock -c [hwnd to dock] [+options] [destination hwnd]
 * xdock -u [hwnd to undock]
 * xdock -n [hwnd to dock] [+options] [destination hwnd]
 * xdock -m [hwnd to dock] [+options]
 * xdock -r [hwnd to dock] [+options] [W] [H]
 */
mIRC(xdock)
{
	TString input(data);

	data[0] = 0;

	try {
		input.trim();

		auto mIRCWnd = mIRCLinker::getHWND();

#ifdef DCX_DEBUG_OUTPUT
		if (mIRCLinker::getMainVersion() >= 7)
		{
			DCX_DEBUG(mIRCLinker::debug, TEXT("xdock"), TEXT("mIRC V7+ detected..."));
		}
#endif

		const auto numtok = input.numtok();

		if (numtok < 1)
			throw DcxExceptions::dcxInvalidArguments();

		const auto switches(input.getfirsttok(1));

		// update mirc
		// /xdock -p
		if (switches[1] == TEXT('p'))
		{
			UpdatemIRC();
			return 1;
		}

		// check if at least 2 parameters
		if (numtok < 2)
			throw DcxExceptions::dcxInvalidFlag();

		const auto dockHwnd = to_hwnd(input.getnexttokas<size_t>()); // tok 2

		// show/hide switchbar
		// [-S] [1|0]
		if ((switches[1] == TEXT('S')) && (numtok == 2))
		{
			//if ((dockHwnd) && !IsWindowVisible(mIRCLinker::getSwitchbar()))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(112, 0), 0);
			//else if ((!dockHwnd) && IsWindowVisible(mIRCLinker::getSwitchbar()))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(112, 0), 0);

			if (dockHwnd)
			{
				if (!IsWindowVisible(mIRCLinker::getSwitchbar()))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(112, 0), 0);
			}
			else {
				if (IsWindowVisible(mIRCLinker::getSwitchbar()))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(112, 0), 0);
			}

			return 1;
		}
		// show/hide toolbar
		// [-T] [1|0]
		else if ((switches[1] == TEXT('T')) && (numtok == 2))
		{
			//if ((dockHwnd > 0) && (!IsWindowVisible(mIRCLinker::getToolbar())))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(111, 0), 0);
			//else if ((dockHwnd == 0) && (IsWindowVisible(mIRCLinker::getToolbar())))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(111, 0), 0);

			if (dockHwnd)
			{
				if (!IsWindowVisible(mIRCLinker::getToolbar()))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(111, 0), 0);
			}
			else {
				if (IsWindowVisible(mIRCLinker::getToolbar()))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(111, 0), 0);
			}

			return 1;
		}
		// show/hide treebar
		// [-R] [1|0]
		else if ((switches[1] == TEXT('R')) && (numtok == 2))
		{
			//if ((dockHwnd > 0) && (!IsWindowVisible(mIRCLinker::getTreebar())))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(210, 0), 0);
			//else if ((dockHwnd == 0) && (IsWindowVisible(mIRCLinker::getTreebar())))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(210, 0), 0);

			if (dockHwnd)
			{
				if (!IsWindowVisible(mIRCLinker::getTreebar()))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(210, 0), 0);
			}
			else {
				if (IsWindowVisible(mIRCLinker::getTreebar()))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(210, 0), 0);
			}

			return 1;
		}
		// show/hide menubar
		// [-M] [1|0]
		else if ((switches[1] == TEXT('M')) && (numtok == 2))
		{
			//if ((dockHwnd > 0) && (!GetMenu(mIRCWnd)))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(110, 0), 0);
			//else if ((dockHwnd == 0) && (GetMenu(mIRCWnd)))
			//	SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(110, 0), 0);

			if (dockHwnd)
			{
				if (!GetMenu(mIRCWnd))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(110, 0), 0);
			}
			else {
				if (GetMenu(mIRCWnd))
					SendMessage(mIRCWnd, WM_COMMAND, MAKEWPARAM(110, 0), 0);
			}

			return 1;
		}
		//// change cursor
		//// [-C] [cursor type] [filename.cur]
		//else if ((switches[1] == TEXT('C')) && (numtok >= 2)) {
		//	auto ud = GetUltraDock(dockHwnd);
		//	UINT CursorType = input.getnexttokas<UINT>();
		//	TString tsFilename(input.getlasttoks());
		//
		//	return 1;
		//}

		if (!IsWindow(dockHwnd))
			throw Dcx::dcxException("Invalid Window to dock");

		const auto flags(input.getnexttok()); // tok 3

		if ((numtok > 2) && (flags[0] != TEXT('+')))
			throw DcxExceptions::dcxInvalidFlag();

		// dock to toolbar
		// [-t] [hwnd to dock] [+options]
		if ((switches[1] == TEXT('t')) && (numtok > 2))
		{
			DockWindow(mWnd, dockHwnd, TEXT("mIRC_Toolbar"), flags);
		}
		// dock to switchbar
		// [-s] [hwnd to dock] [+options]
		else if ((switches[1] == TEXT('s')) && (numtok > 2))
		{
			DockWindow(mWnd, dockHwnd, TEXT("mIRC_Switchbar"), flags);
		}
		// dock to nicklist/sidelistbox
		// [-n] [hwnd to dock] [+options] [hwnd to dock with]
		else if ((switches[1] == TEXT('n')) && (numtok > 3))
		{
			mWnd = to_hwnd(input.getnexttokas<size_t>()); // tok 4

			if (!IsWindow(mWnd))
				throw DcxExceptions::dcxInvalidArguments();

			DockWindow(mWnd, dockHwnd, TEXT("ListBox"), flags);
		}
		//dock to custom/channel/query/status
		// [-c] [hwnd to dock] [+options] [hwnd to dock with]
		else if ((switches[1] == TEXT('c')) && (numtok > 3))
		{
			mWnd = to_hwnd(input.getnexttokas<size_t>()); // tok 4

			if (!IsWindow(mWnd))
				throw DcxExceptions::dcxInvalidArguments();

			DockWindow(mWnd, dockHwnd, nullptr, flags);
		}
#if DCX_USE_TESTCODE
		//dock to custom/channel/query/status
		// [-e] [hwnd to dock] [+options] [hwnd to dock with]
		else if (switches[1] == TEXT('e'))
		{
			if (numtok < 4)
				throw DcxExceptions::dcxInvalidArguments();

			mWnd = to_hwnd(input.getnexttokas<size_t>()); // tok 4

			if (!IsWindow(mWnd))
				throw DcxExceptions::dcxInvalidArguments();

			DockChanTypeWindow(mWnd, dockHwnd, nullptr, flags);
		}
#endif
		// dock to treelist
		// [-b] [hwnd to dock] [+options]
		else if ((switches[1] == TEXT('b')) && (numtok > 2))
		{
			if (flags.find(TEXT('o'), 0)) // o for Overlay, old style docking with no layout adjustment
				DockWindow(mWnd, dockHwnd, TEXT("mIRC_TreeBar"), flags);	// v2 dll is mirc v7+
			else
				TreebarDock(dockHwnd, flags);
		}
		// dock to mIRC (UltraDock)
		// [-m] [hwnd to dock] [+options]
		else if ((switches[1] == TEXT('m')) && (numtok > 2))
		{
			UltraDock(mWnd, dockHwnd, flags);
		}
		// undock
		// [-u] [hwnd to undock]
		else if (switches[1] == TEXT('u'))
		{
			if (FindUltraDock(dockHwnd))
				UltraUnDock(dockHwnd);
			else if (FindTreebarDock(dockHwnd))
				TreebarUnDock(dockHwnd);
			else
				UnDock(dockHwnd);
		}
		// resize docked window
		// [-r] [hwnd to resize] [+options] [W] [H]
		else if ((switches[1] == TEXT('r')) && (numtok > 4))
		{
			const auto w = input.getnexttokas<int>(); // tok 4
			const auto h = input.getnexttokas<int>(); // tok 5

			auto ud = GetUltraDock(dockHwnd);
			DockFlags dflags{ DockFlags::DOCKF_NONE };

			if (!ud) // not an UltraDock, try TreebarDock
				ud = GetTreebarDock(dockHwnd);

			if (ud)
				dflags = ud->flags;
			else // not Ultradock or TreebarDock, try normal dock
				dflags = Dcx::dcxGetProp<DockFlags>(dockHwnd, TEXT("dcx_docked"));

			if (dflags == DockFlags::DOCKF_NONE) // not any dock.
				throw DcxExceptions::dcxInvalidFlag();

#if DCX_USE_WRAPPERS
			Dcx::dcxWindowRect rc(dockHwnd);
#else
			RECT rc;
			if (!GetWindowRect(dockHwnd, &rc))
				throw Dcx::dcxException("Unable to get window rect");
#endif

			OffsetRect(&rc, -rc.left, -rc.top); // right & bottom now == width & height
			dflags &= ~(DockFlags::DOCKF_NOSCROLLBARS | DockFlags::DOCKF_SHOWSCROLLBARS);	// ignore scrollbar flags

			switch (dflags)
			{
			case DockFlags::DOCKF_LEFT:
			case DockFlags::DOCKF_RIGHT:
			case DockFlags::DOCKF_AUTOV:
				rc.right = w;
				break;

			case DockFlags::DOCKF_TOP:
			case DockFlags::DOCKF_BOTTOM:
			case DockFlags::DOCKF_AUTOH:
				rc.bottom = h;
				break;

			case DockFlags::DOCKF_NORMAL:
				rc.bottom = h;
				rc.right = w;
				break;

			case DockFlags::DOCKF_SIZE:
				throw Dcx::dcxException("Can't resize an auto width & height dialog");

			default:
				throw DcxExceptions::dcxInvalidFlag();
			}

			// x & y handled by mIRC update, only change width & height.
			SetWindowPos(dockHwnd, nullptr, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOOWNERZORDER);
			UpdatemIRC();
			RedrawWindow(mIRCLinker::getHWND(), nullptr, nullptr, RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE);
		}
		else
			throw Dcx::dcxException("Unknown command or invalid syntax");

		return 1;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("/xdock"), TEXT("\"%\" error: %"), input, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("/xdock"), TEXT("\"%\" error: Unknown Exception"), input);
	}
	return 0;
}

/*! \brief $xdock(hwnd|mIRC).prop
 *
 * $xdock(mIRC).switchBarPos
 * $xdock(mIRC).toolBarPos
 * $xdock(mIRC).treeBarPos
 * $xdock(mIRC).switchBarSize
 * $xdock(mIRC).toolBarSize
 * $xdock(mIRC).treeBarSize
 * $xdock(mIRC).isSwitchBar
 * $xdock(mIRC).isToolBar
 * $xdock(mIRC).isTreeBar
 * $xdock(mIRC).isMenuBar
 * $xdock(mIRC).switchBarHwnd
 * $xdock(mIRC).toolBarHwnd
 * $xdock(mIRC).treeBarHwnd
 * $xdock(mIRC).text
 *
 * $xdock(hwnd).isDocked
 * $xdock(hwnd).hasDocked
 * $xdock(hwnd).isAutoV
 * $xdock(hwnd).isAutoH
 * $xdock(hwnd).isAutoS
 * $xdock(hwnd).dockSide
 * $xdock(hwnd).text
 *
 */
mIRC(_xdock)
{
	TString d(data);

	data[0] = 0;

	const mIRCResultString refData(data);

	try {
		d.trim();

		if (d.numtok() < 2)
			throw Dcx::dcxException(TEXT("Invalid arguments (%)"), d.gettok(1));

		if (d.getfirsttok(1) == TEXT("mIRC"))
		{
			const auto DockTypeToSwitchbarPos = [refData](const DockTypes dType) noexcept {
				switch (SwitchbarPos(dType))
				{
				case SwitchBarPos::SWB_RIGHT:
					refData = TEXT("right");
					break;

				case SwitchBarPos::SWB_BOTTOM:
					refData = TEXT("bottom");
					break;

				case SwitchBarPos::SWB_TOP:
					refData = TEXT("top");
					break;

				case SwitchBarPos::SWB_LEFT:
					refData = TEXT("left");
					break;

				case SwitchBarPos::SWB_NONE:
				default:
					refData = TEXT("none");
					break;
				}
			};

			switch (std::hash<TString>{}(d.getnexttok()))
			{
			case TEXT("switchBarPos"_hash):
				DockTypeToSwitchbarPos(DockTypes::DOCK_TYPE_SWITCH);
				break;
			case TEXT("toolBarPos"_hash):
				DockTypeToSwitchbarPos(DockTypes::DOCK_TYPE_TOOL);
				break;
			case TEXT("treeBarPos"_hash):
				DockTypeToSwitchbarPos(DockTypes::DOCK_TYPE_TREE);
				break;
			case TEXT("switchBarSize"_hash):
			{
#if DCX_USE_WRAPPERS
				const Dcx::dcxWindowRect rc(mIRCLinker::getSwitchbar());
#else
				RECT rc;
				if (!GetWindowRect(mIRCLinker::getSwitchbar(), &rc))
					throw Dcx::dcxException("Unable to get window rect");
#endif
				_ts_snprintf(refData, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			}
			break;
			case TEXT("toolBarSize"_hash):
			{
#if DCX_USE_WRAPPERS
				const Dcx::dcxWindowRect rc(mIRCLinker::getToolbar());
#else
				RECT rc;
				if (!GetWindowRect(mIRCLinker::getToolbar(), &rc))
					throw Dcx::dcxException("Unable to get window rect");
#endif
				_ts_snprintf(refData, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			}
			break;
			case TEXT("treeBarSize"_hash):
			{
#if DCX_USE_WRAPPERS
				const Dcx::dcxWindowRect rc(mIRCLinker::getTreebar());
#else
				RECT rc;
				if (!GetWindowRect(mIRCLinker::getTreebar(), &rc))
					throw Dcx::dcxException("Unable to get window rect");
#endif
				_ts_snprintf(refData, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
			}
			break;
			case TEXT("isSwitchBar"_hash):
			{
				dcx_Con((IsWindowVisible(mIRCLinker::getSwitchbar()) != FALSE), data);
			}
			break;
			case TEXT("isToolBar"_hash):
			{
				dcx_Con((IsWindowVisible(mIRCLinker::getToolbar()) != FALSE), data);
			}
			break;
			case TEXT("isTreeBar"_hash):
			{
				dcx_Con((IsWindowVisible(mIRCLinker::getTreebar()) != FALSE), data);
			}
			break;
			case TEXT("isMenuBar"_hash):
			{
				dcx_Con((GetMenu(mIRCLinker::getHWND()) != nullptr), data);
			}
			break;
			case TEXT("text"_hash):
			{
				if (GetWindowTextLength(mIRCLinker::getHWND()) > 0)
					GetWindowText(mIRCLinker::getHWND(), data, MIRC_BUFFER_SIZE_CCH);
			}
			break;
			case TEXT("switchBarHwnd"_hash):
			{
				const auto i = from_hwnd(mIRCLinker::getSwitchbar());
				_ts_snprintf(refData, TEXT("%zu"), i); // don't use %p as this gives a hex result.
			}
			break;
			case TEXT("toolBarHwnd"_hash):
			{
				const auto i = from_hwnd(mIRCLinker::getToolbar());
				_ts_snprintf(refData, TEXT("%zu"), i);
			}
			break;
			case TEXT("treeBarHwnd"_hash):
			{
				const auto i = from_hwnd(mIRCLinker::getTreebar());
				_ts_snprintf(refData, TEXT("%zu"), i);
			}
			break;
			case 0: // error
			default:
				throw Dcx::dcxException(TEXT("Invalid prop (mIRC).%"), d.gettok(2));
			}
		}
		else {
			const auto hwnd = to_hwnd(d.getfirsttok(1).to_<size_t>());

			if (!IsWindow(hwnd))
				throw Dcx::dcxException(TEXT("Invalid window (%)"), d.gettok(1));

			switch (std::hash<TString>{}(d.getnexttok()))
			{
			case TEXT("isDocked"_hash):
			{
				dcx_Con(GetProp(hwnd, TEXT("dcx_docked")) || FindUltraDock(hwnd) || FindTreebarDock(hwnd), data);
			}
			break;
			case TEXT("hasDocked"_hash):
			{
				dcx_Con((GetProp(hwnd, TEXT("dcx_dock")) != nullptr), data);
			}
			break;
			case TEXT("isAutoV"_hash):
			{
				const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<size_t>(GetProp(hwnd, TEXT("dcx_docked"))));
				dcx_Con(flags == DockFlags::DOCKF_AUTOV, data);
			}
			break;
			case TEXT("isAutoH"_hash):
			{
				const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<size_t>(GetProp(hwnd, TEXT("dcx_docked"))));
				dcx_Con(flags == DockFlags::DOCKF_AUTOH, data);
			}
			break;
			case TEXT("isAutoS"_hash):
			{
				const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<size_t>(GetProp(hwnd, TEXT("dcx_docked"))));
				dcx_Con(flags == DockFlags::DOCKF_SIZE, data);
			}
			break;
			case TEXT("dockSide"_hash):
			{
				const auto* const ud = GetUltraDock(hwnd);

				if (!ud)
					throw Dcx::dcxException(TEXT("Window not docked to main mIRC window (%).%"), from_hwnd(hwnd), d.gettok(2));

				const TCHAR* p = TEXT("unknown");
				switch (ud->flags)
				{
				case DockFlags::DOCKF_LEFT:
					p = TEXT("left");
					break;
				case DockFlags::DOCKF_RIGHT:
					p = TEXT("right");
					break;
				case DockFlags::DOCKF_TOP:
					p = TEXT("top");
					break;
				case DockFlags::DOCKF_BOTTOM:
					p = TEXT("bottom");
					break;
				}
				refData = p;
			}
			break;
			case TEXT("text"_hash):
			{
				if (GetWindowTextLength(hwnd) > 0)
					GetWindowText(hwnd, data, mIRCLinker::c_mIRC_Buffer_Size_cch);
			}
			break;
			case 0: // error
			default:
				throw Dcx::dcxException(TEXT("Invalid prop (%).%"), from_hwnd(hwnd), d.gettok(2));
			}
		}
		return 3;
	}
	catch (const std::exception& e)
	{
		Dcx::error(TEXT("$!xdock"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!xdock"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}
