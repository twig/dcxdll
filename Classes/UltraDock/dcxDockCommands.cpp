/*
	* Various docking commands that are not part of the old UltraDock
	*
	*	TODO: Add subclassing of channel/custom/toolbar/switchbar/treelist? SetClassLongPtr(new_wndproc) & FindWindowEx(classname)
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"

extern void UltraDock(const HWND mWnd,HWND temp, const TString &flag);
extern void UltraUnDock(const HWND hwnd);
extern bool FindUltraDock(const HWND hwnd);
extern LPDCXULTRADOCK GetUltraDock(const HWND hwnd);
extern void TreebarDock(HWND temp, const TString &flag);
extern void TreebarUnDock(const HWND hwnd);
extern bool FindTreebarDock(const HWND hwnd);
extern LPDCXULTRADOCK GetTreebarDock(const HWND hwnd);
void UnDock(const HWND hwnd);

/*! \brief DCXDOCK structure, used when adding docked windows.
 *
 * Added as "dcx_dock" to docked windows via AddProp()
 */

struct DCXDOCK {
	WNDPROC oldProc{};
	HWND win{};
	DWORD flags{};
	TString type;

	//DCXDOCK() noexcept
	//	: oldProc(nullptr)
	//	, win(nullptr)
	//	, flags(0U)
	//	, type()
	//{}
	DCXDOCK(const WNDPROC &proc, const HWND &hwnd, const DWORD &f, const TString &t)
		: oldProc(proc)
		, win(hwnd)
		, flags(f)
		, type(t)
	{}
};
using LPDCXDOCK = DCXDOCK *;

GSL_SUPPRESS(type.4)
BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam)
{
	if (auto dd = Dcx::dcxGetProp<LPDCXDOCK>(hwnd, TEXT("dcx_dock")); dd)
	{
		RemoveProp(hwnd,TEXT("dcx_dock"));
		SubclassWindow(hwnd, dd->oldProc);
		delete dd;
	}
	if (GetProp(hwnd,TEXT("dcx_docked")))
		UnDock(hwnd);
	return TRUE;
}
BOOL CALLBACK SizeDocked(HWND hwnd,LPARAM lParam)
{
	//const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<UINT>(GetProp(hwnd, TEXT("dcx_docked"))));
	const auto flags = Dcx::dcxGetProp<DockFlags>(hwnd, TEXT("dcx_docked"));
	const auto hParent = GetParent(hwnd);
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
			SetWindowPos(hwnd,nullptr,0,0,(rcParent.right - rcParent.left),(rcParent.bottom - rcParent.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
		else if (dcx_testflag(flags, DockFlags::DOCKF_AUTOH))
			SetWindowPos(hwnd,nullptr,0,0,(rcParent.right - rcParent.left),(rcThis.bottom - rcThis.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
		else if (dcx_testflag(flags, DockFlags::DOCKF_AUTOV))
			SetWindowPos(hwnd,nullptr,0,0,(rcThis.right - rcThis.left),(rcParent.bottom - rcParent.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
	}
	// disable scroll bars if requested, this needs to be done here as the listbox re-enables them.
	if (dcx_testflag(flags, DockFlags::DOCKF_NOSCROLLBARS))
		ShowScrollBar(hParent,SB_BOTH,FALSE);
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
		return DefWindowProc(mHwnd,uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_SIZE:
		{
			if (dcxSignal.xdock)
			{
				if (!dd->type.empty())
					mIRCLinker::signal(TEXT("size % % % %"), dd->type, reinterpret_cast<DWORD>(dd->win), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
				else
					mIRCLinker::signal(TEXT("size Custom % % %"), reinterpret_cast<DWORD>(dd->win), Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
			}
			EnumChildWindows(mHwnd,SizeDocked,NULL);
		}
		break;
	case WM_PARENTNOTIFY:
		{
			if (Dcx::dcxLOWORD(wParam) == WM_DESTROY)
				RemoveProp(reinterpret_cast<HWND>(lParam),TEXT("dcx_docked"));
		}
		break;
#ifdef DCX_DEBUG_OUTPUT
	case WM_CLOSE:
		{
			if (dcxSignal.xdock)
			{
				if (!dd->type.empty())
					mIRCLinker::signal(TEXT("close % %"), dd->type, reinterpret_cast<DWORD>(dd->win));
				else
					mIRCLinker::signal(TEXT("close Custom %"), reinterpret_cast<DWORD>(dd->win));
			}
		}
		break;
#endif
	case WM_DESTROY:
		{
			EnumChildWindows(mHwnd,EnumDocked,NULL);
			RemoveProp(mHwnd,TEXT("dcx_dock"));
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

void UnDock(const HWND hwnd)
{
	if (!GetProp(hwnd,TEXT("dcx_docked")))
		throw Dcx::dcxException("Window is not docked");

	// Remove Style for undocking purpose
	RemStyles(hwnd,GWL_STYLE,WS_BORDER);
	//WS_CHILDWINDOW |
	RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
	// Add Styles input by user
	AddStyles(hwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);	
	AddStyles(hwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
	RemStyles(hwnd,GWL_STYLE,WS_CHILDWINDOW);
	SetParent(hwnd, nullptr);
	SetWindowPos(hwnd,nullptr,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
	RemoveProp(hwnd,TEXT("dcx_docked"));
}

bool DockWindow(const HWND mWnd, const HWND temp, const TCHAR *find, const TString & flag)
{

	if ((FindUltraDock(temp)) || (GetProp(temp,TEXT("dcx_docked"))))
		throw Dcx::dcxException("Window is Already docked.");

	HWND sWnd{ nullptr };
	// get window HWND
	if (!find)
		sWnd = GetDlgItem(mWnd,32918);
	else
		sWnd = FindWindowEx(mWnd, nullptr, find, nullptr);

	if (!IsWindow(sWnd))
		throw Dcx::dcxException("Unable to Find Host Window.");

	const XSwitchFlags xflags(flag);

	if (!xflags['+'])
		throw Dcx::dcxException("Invalid Flags: Flags must start with a '+'");

	// change win style to child
	//RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
	RemStyles(temp,GWL_STYLE,WS_POPUP);
	AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);

	// get window pos
#if DCX_USE_WRAPPERS
	const Dcx::dcxWindowRect rc(temp);
#else
	RECT rc{};
	if (!GetWindowRect(temp, &rc))
		throw Dcx::dcxException("Unable to get window rect");
#endif

	// if prop not alrdy set then set it & subclass.
	if (!GetProp(sWnd,TEXT("dcx_dock")))
	{
		// subclass window.
		{
			//gsl::owner<LPDCXDOCK> dd = new DCXDOCK;
			//
			//if (SetProp(sWnd, TEXT("dcx_dock"), dd))
			//{
			//	dd->win = mWnd;
			//	dd->type = find;
			//	dd->oldProc = SubclassWindow(sWnd, mIRCDockWinProc);
			//}
			//else {
			//	delete dd;
			//	throw Dcx::dcxException("Unable to SetProp");
			//}

			auto dd = std::make_unique<DCXDOCK>( SubclassWindow(sWnd, mIRCDockWinProc), mWnd, 0U, find );

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

	SetProp(temp,TEXT("dcx_docked"),reinterpret_cast<HANDLE>(flags));
	//ShowScrollBar(sWnd,SB_BOTH,FALSE);
	AddStyles(sWnd,GWL_STYLE,WS_CLIPSIBLINGS|WS_CLIPCHILDREN); // this helps with rendering glitches.
	// set parent and move it to top-left corner
	SetParent(temp,sWnd);
	MoveWindow(temp,0,0,rc.right-rc.left,rc.bottom-rc.top,1);
	EnumChildWindows(sWnd,SizeDocked,NULL);
	return true;
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

		const auto dockHwnd = reinterpret_cast<HWND>(input.getnexttok().to_<DWORD>()); // tok 2

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
		//	UINT CursorType = input.getnexttok().to_<UINT>();
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
			mWnd = reinterpret_cast<HWND>(input.getnexttok().to_<DWORD>()); // tok 4

			if (!IsWindow(mWnd))
				throw DcxExceptions::dcxInvalidArguments();

			DockWindow(mWnd, dockHwnd, TEXT("ListBox"), flags);
		}
		//dock to custom/channel/query/status
		// [-c] [hwnd to dock] [+options] [hwnd to dock with]
		else if ((switches[1] == TEXT('c')) && (numtok > 3))
		{
			mWnd = reinterpret_cast<HWND>(input.getnexttok().to_<DWORD>()); // tok 4

			if (!IsWindow(mWnd))
				throw DcxExceptions::dcxInvalidArguments();

			DockWindow(mWnd, dockHwnd, nullptr, flags);
		}
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
		// [-r] [hwnd to dock] [+options] [W] [H]
		else if ((switches[1] == TEXT('r')) && (numtok > 4))
		{
			const auto w = input.getnexttok().to_int(); // tok 4
			const auto h = input.getnexttok().to_int(); // tok 5

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
	catch (const std::exception &e)
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
				const auto i = Dcx::numeric_cast<DWORD>(mIRCLinker::getSwitchbar());
				_ts_snprintf(refData, TEXT("%lu"), i); // don't use %p as this gives a hex result.
			}
			break;
			case TEXT("toolBarHwnd"_hash):
			{
				const auto i = Dcx::numeric_cast<DWORD>(mIRCLinker::getToolbar());
				_ts_snprintf(refData, TEXT("%lu"), i);
			}
			break;
			case TEXT("treeBarHwnd"_hash):
			{
				const auto i = Dcx::numeric_cast<DWORD>(mIRCLinker::getTreebar());
				_ts_snprintf(refData, TEXT("%lu"), i);
			}
			break;
			case 0: // error
			default:
				throw Dcx::dcxException(TEXT("Invalid prop (mIRC).%"), d.gettok(2));
			}
		}
		else {
			const auto hwnd = reinterpret_cast<HWND>(d.getfirsttok(1).to_<DWORD>());

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
				const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<UINT>(GetProp(hwnd, TEXT("dcx_docked"))));
				dcx_Con(flags == DockFlags::DOCKF_AUTOV, data);
			}
			break;
			case TEXT("isAutoH"_hash):
			{
				const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<UINT>(GetProp(hwnd, TEXT("dcx_docked"))));
				dcx_Con(flags == DockFlags::DOCKF_AUTOH, data);
			}
			break;
			case TEXT("isAutoS"_hash):
			{
				const auto flags = gsl::narrow_cast<DockFlags>(reinterpret_cast<UINT>(GetProp(hwnd, TEXT("dcx_docked"))));
				dcx_Con(flags == DockFlags::DOCKF_SIZE, data);
			}
			break;
			case TEXT("dockSide"_hash):
			{
				const auto *const ud = GetUltraDock(hwnd);

				if (!ud)
					throw Dcx::dcxException(TEXT("Window not docked to main mIRC window (%).%"), Dcx::numeric_cast<DWORD>(hwnd), d.gettok(2));

				const TCHAR *p = TEXT("unknown");
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
				throw Dcx::dcxException(TEXT("Invalid prop (%).%"), Dcx::numeric_cast<DWORD>(hwnd), d.gettok(2));
			}
		}
		return 3;
	}
	catch (const std::exception &e)
	{
		Dcx::error(TEXT("$!xdock"), TEXT("\"%\" error: %"), d, e.what());
	}
	catch (...) {
		// stop any left over exceptions...
		Dcx::error(TEXT("$!xdock"), TEXT("\"%\" error: Unknown Exception"), d);
	}
	return 0;
}

//#include "defines.h"
//
//extern mIRCDLL mIRCLink;
//// mIRC components HWND
//extern VectorOfDocks v_docks;
//extern void UltraDock(const HWND mWnd,HWND temp,TString &flag);
//extern bool FindUltraDock(const HWND hwnd);
//extern LPDCXULTRADOCK GetUltraDock(const HWND hwnd);
//
//typedef struct tagDCXDOCK {
//	WNDPROC oldProc;
//	HWND win;
//	TString type;
//	DWORD flags;
//} DCXDOCK, *LPDCXDOCK;
//
//typedef struct tagDCXDOCSIZE {
//	DWORD	width;
//	DWORD height;
//} DCXDOCKSIZE, *LPDCXDOCKSIZE;
//
//BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam)
//{
//	LPDCXDOCK dd = (LPDCXDOCK)GetProp(hwnd,TEXT("dcx_dock"));
//	if (dd != nullptr) {
//		RemoveProp(hwnd,TEXT("dcx_dock"));
//		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)dd->oldProc);
//		delete dd;
//	}
//	if (GetProp(hwnd,TEXT("dcx_docked")))
//		RemoveProp(hwnd,TEXT("dcx_docked"));
//	return TRUE;
//}
//BOOL CALLBACK SizeDocked(HWND hwnd,LPARAM lParam)
//{
//	DWORD flags = (DWORD)GetProp(hwnd,TEXT("dcx_docked"));
//	if (flags && flags != DOCKF_NORMAL) {
//		RECT rcParent, rcThis;
//		GetClientRect(GetParent(hwnd),&rcParent);
//		GetWindowRect(hwnd,&rcThis);
//		if (dcx_testflag(flags, DOCKF_SIZE)) 
//			SetWindowPos(hwnd,nullptr,0,0,(rcParent.right - rcParent.left),(rcParent.bottom - rcParent.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
//		else if (dcx_testflag(flags, DOCKF_AUTOH))
//			SetWindowPos(hwnd,nullptr,0,0,(rcParent.right - rcParent.left),(rcThis.bottom - rcThis.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
//		else if (dcx_testflag(flags, DOCKF_AUTOV))
//			SetWindowPos(hwnd,nullptr,0,0,(rcThis.right - rcThis.left),(rcParent.bottom - rcParent.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
//	}
//	return TRUE;
//}
//
//LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	LPDCXDOCK dd = (LPDCXDOCK)GetProp(mHwnd,TEXT("dcx_dock"));
//	if (dd == nullptr)
//		return DefWindowProc(mHwnd,uMsg, wParam, lParam);
//	switch (uMsg) {
//    case WM_SIZE:
//      {
//				if (dd->type.len())
//					mIRCSignalDCX(TEXT("size %s %d %d %d"), dd->type.to_chr(), dd->win, LOWORD(lParam), HIWORD(lParam));
//				else
//					mIRCSignalDCX(TEXT("size Custom %d %d %d"), dd->win, LOWORD(lParam), HIWORD(lParam));
//
//				EnumChildWindows(mHwnd,(WNDENUMPROC)SizeDocked,nullptr);
//      }
//      break;
//		case WM_PARENTNOTIFY:
//			{
//				if (LOWORD(wParam) == WM_DESTROY)
//					RemoveProp((HWND)lParam,TEXT("dcx_docked"));
//			}
//			break;
//		case WM_DESTROY:
//			{
//				RemoveProp(mHwnd,TEXT("dcx_dock"));
//				SetWindowLongPtr(mHwnd, GWLP_WNDPROC, (LONG_PTR)dd->oldProc);
//				delete dd;
//				PostMessage(mHwnd, uMsg, 0, 0);
//				return 0L;
//			}
//			break;
//	}
//	return CallWindowProc(dd->oldProc,mHwnd,uMsg,wParam,lParam);
//}
//
//void UnDock(const HWND hwnd)
//{
//	if (GetProp(hwnd,TEXT("dcx_docked")) == nullptr) {
//		DCXError(TEXT("/xdock -u"),TEXT("Window is not docked"));
//		return;
//	}
//  // Remove Style for undocking purpose
//  RemStyles(hwnd,GWL_STYLE,WS_BORDER);
//  //WS_CHILDWINDOW |
//  RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
//  // Add Styles input by user
//  AddStyles(hwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);	
//  AddStyles(hwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
//  RemStyles(hwnd,GWL_STYLE,WS_CHILDWINDOW);
//  SetParent(hwnd, nullptr);
//	SetWindowPos(hwnd,nullptr,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
//}
//
//bool DockWindow(const HWND mWnd, const HWND temp, const TCHAR *find, TString & flag)
//{
//	RECT rc;
//	HWND sWnd;
//
//	if ((FindUltraDock(temp)) || (GetProp(temp,TEXT("dcx_docked")) != nullptr)) {
//		DCXError(TEXT("/xdock"),TEXT("Window is Already docked."));
//		return false;
//	}
//
//	// get window HWND
//	if (find == nullptr)
//		sWnd = GetDlgItem(mWnd,32918);
//	else
//		sWnd = FindWindowEx(mWnd, nullptr, find, nullptr);
//
//	if (IsWindow(sWnd)) {
//		// change win style to child
//		//RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
//		RemStyles(temp,GWL_STYLE,WS_POPUP);
//		AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);
//
//		// get window pos
//		GetWindowRect(temp,&rc);
//
//		// if prop not alrdy set then set it & subclass.
//		if (GetProp(sWnd,TEXT("dcx_dock")) == nullptr)
//		{
//			// subclass window.
//			LPDCXDOCK dd = new DCXDOCK;
//
//			if (SetProp(sWnd, TEXT("dcx_dock"), dd)) {
//				dd->oldProc = (WNDPROC)SetWindowLongPtr(sWnd, GWLP_WNDPROC, (LONG_PTR) mIRCDockWinProc);
//				dd->win = mWnd;
//				dd->type = find;
//			}
//			else {
//				delete dd;
//				DCXError(TEXT("/xdock"),TEXT("Unable to SetProp"));
//				return false;
//			}
//		}
//
//		DWORD flags = DOCKF_NORMAL;
//
//		if (flag.len() > 1) {
//			switch(flag[1])
//			{
//				case TEXT('s'):
//					flags = DOCKF_SIZE;
//					break;
//
//				case TEXT('h'):
//					flags = DOCKF_AUTOH;
//					break;
//
//				case TEXT('v'):
//					flags = DOCKF_AUTOV;
//					break;
//
//				default:
//					flags = DOCKF_NORMAL;
//					break;
//			}
//		}
//
//		SetProp(temp,TEXT("dcx_docked"),(HANDLE) flags);
//		//ShowScrollBar(sWnd,SB_BOTH,FALSE);
//		AddStyles(sWnd,GWL_STYLE,WS_CLIPSIBLINGS|WS_CLIPCHILDREN); // this helps with rendering glitches.
//		// set parent and move it to top-left corner
//		SetParent(temp,sWnd);
//		MoveWindow(temp,0,0,rc.right-rc.left,rc.bottom-rc.top,1);
//		EnumChildWindows(sWnd,(WNDENUMPROC)SizeDocked,nullptr);
//		return true;
//	}
//	else
//		DCXError(TEXT("/xdock"),TEXT("Unable to find destination Window"));
//
//	return false;
//}
//
//// [SWITCH] [hwnd to dock] [+options] (destination hwnd)
//mIRC(xdock) {
//	TString input(data);
//	input.trim();
//	data[0] = 0;
//
//	int numtok = input.numtok(TEXT(" "));
//
//	if (numtok < 1) {
//		DCXError(TEXT("/xdock"),TEXT("Invalid Parameters"));
//		return 0;
//	}
//
//	TString switches(input.gettok(1, TEXT(" ")));
//
//	// update mirc
//	// /xdock -p
//	if (switches[1] == TEXT('p')) {
//		UpdatemIRC();
//		return 1;
//	}
//
//	// check if at least 2 parameters
//	if (numtok < 2) {
//		DCXError(TEXT("/xdock"),TEXT("Invalid Flag"));
//		return 0;
//	}
//
//	// show/hide switchbar
//	// [-S] [1|0]
//	if ((switches[1] == TEXT('S')) && (numtok == 2)) {
//		if ((input.gettok(2, TEXT(" ")).to_int() > 0) && (!IsWindowVisible(mIRCLink.m_hSwitchbar)))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);
//		else if ((input.gettok(2, TEXT(" ")).to_int() == 0) && (IsWindowVisible(mIRCLink.m_hSwitchbar)))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);
//
//		return 1;
//	}
//	// show/hide toolbar
//	// [-T] [1|0]
//	else if ((switches[1] == TEXT('T')) && (numtok == 2)) {
//		if ((input.gettok(2, TEXT(" ")).to_int() > 0) && (!IsWindowVisible(mIRCLink.m_hToolbar)))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
//		else if ((input.gettok(2, TEXT(" ")).to_int() == 0) && (IsWindowVisible(mIRCLink.m_hToolbar)))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
//
//		return 1;
//	}
//	// show/hide treebar
//	// [-R [1|0]
//	else if ((switches[1] == TEXT('R')) && (numtok == 2)) {
//		if ((input.gettok(2, TEXT(" ")).to_int() > 0) && (!IsWindowVisible(mIRCLink.m_hTreebar)))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(210,0), 0);
//		else if ((input.gettok(2, TEXT(" ")).to_int() == 0) && (IsWindowVisible(mIRCLink.m_hTreebar)))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(210,0), 0);
//
//		return 1;
//	}
//	// show/hide menubar
//	// [-M] [1|0]
//	else if ((switches[1] == TEXT('M')) && (numtok == 2)) {
//		if ((input.gettok(2, TEXT(" ")).to_int() > 0) && (!GetMenu(mIRCLinker::getHWND())))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);
//		else if ((input.gettok(2, TEXT(" ")).to_int() == 0) && (GetMenu(mIRCLinker::getHWND())))
//			SendMessage(mIRCLinker::getHWND(), WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);
//
//		return 1;
//	}
//	// enable/disable ghost drag for main mIRC window.
//	// [-G] [0-255]
//	else if ((switches[1] == TEXT('G')) && (numtok == 2)) {
//		int alpha = input.gettok(2,TEXT(" ")).to_int();
//		if ((alpha >= 0) && (alpha <= 255))
//			mIRCLink.m_bDoGhostDrag = alpha;
//		else {
//			DCXError(TEXT("xdock -G"),TEXT("Alpha Out Of Range"));
//			return 0;
//		}
//		return 1;
//	}
//	// set treebar font.
//	// [-F] [+flag] [args]
//	else if ((switches[1] == TEXT('F')) && (numtok > 2)) {
//		//treeb_hwnd
//		if (IsWindow(mIRCLinker::getTreeview())) {
//			TString flag(input.gettok(2));
//			switch(flag[1])
//			{
//			case TEXT('f'): // set Treebars Font: (+flags CHARSET SIZE FONTNAME)
//				{
//					if (numtok < 6) {
//						DCXError(TEXT("xdock -F"),TEXT("Invalid Font Args"));
//						return 0;
//					}
//					LOGFONT lf;
//
//					if (ParseCommandToLogfont(input.gettok(3, -1, TEXT(" ")), &lf)) {
//						HFONT hFont = CreateFontIndirect(&lf);
//						if (hFont != nullptr) {
//							HFONT f = (HFONT)SendMessage(mIRCLinker::getTreeview(),WM_GETFONT,0,0);
//							if (mIRCLinker::getTreeFont() == nullptr)
//								mIRCLink.m_hTreeFont = f;
//							SendMessage( mIRCLinker::getTreeview(), WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELPARAM(TRUE,0));
//							if (f != mIRCLinker::getTreeFont())
//								DeleteObject(f);
//						}
//					}
//				}
//				break;
//			case TEXT('c'): // set Treebars Colours: [+flags] [colour]
//				{
//					if (numtok < 4) {
//						DCXError(TEXT("xdock -F"),TEXT("Invalid Colour Args"));
//						return 0;
//					}
//					// TODO: undocumented. Non functional, mirc does owner draw, need to subclass
//					TString cflag(input.gettok(3));
//					COLORREF clr = (COLORREF)input.gettok(4).to_num();
//					switch(cflag[1])
//					{
//					case TEXT('t'): // text colour
//						{
//							TreeView_SetTextColor(mIRCLinker::getTreeview(),clr);
//						}
//						break;
//					case TEXT('b'): // bkg colour
//						{
//							TreeView_SetBkColor(mIRCLinker::getTreeview(),clr);
//						}
//						break;
//					case TEXT('l'): // line colour
//						{
//							TreeView_SetLineColor(mIRCLinker::getTreeview(),clr);
//						}
//						break;
//					default:
//						{
//							DCXError(TEXT("xdock -F"),TEXT("Invalid Colour flag"));
//							return 0;
//						}
//						break;
//					}
//				}
//				break;
//			case TEXT('s'): // set Treebars Styles: [styles]
//				{
//					if (numtok < 3) {
//						DCXError(TEXT("xdock -F"),TEXT("Invalid Style Args"));
//						return 0;
//					}
//					static const TString treebar_styles(TEXT("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll"));
//					int i = 3, numtok = input.numtok();
//					DWORD stylef = GetWindowLong(mIRCLinker::getTreeview(),GWL_STYLE);
//					while (i <= numtok) {
//						switch (treebar_styles.findtok(input.gettok(i).to_chr(),1))
//						{
//						case 1: // trackselect (off by default)
//							stylef |= TVS_TRACKSELECT;
//							break;
//						case 2: // notrackselect
//							stylef &= ~TVS_TRACKSELECT;
//							break;
//						case 3: // tooltips (on by default)
//							stylef &= ~TVS_NOTOOLTIPS;
//							break;
//						case 4: // notooltips
//							stylef |= TVS_NOTOOLTIPS;
//							break;
//						case 5: // infotips (on by default)
//							stylef |= TVS_INFOTIP;
//							break;
//						case 6: // noinfotips
//							stylef &= ~TVS_INFOTIP;
//							break;
//						case 7: // hasbuttons (on by default)
//							stylef |= TVS_HASBUTTONS;
//							break;
//						case 8: // nohasbuttons
//							stylef &= ~TVS_HASBUTTONS;
//							break;
//						case 9: // rootlines (on by default)
//							stylef |= TVS_LINESATROOT;
//							break;
//						case 10: // norootlines
//							stylef &= ~TVS_LINESATROOT;
//							break;
//						case 11: // singleexpand (off by default)
//							stylef |= TVS_SINGLEEXPAND;
//							break;
//						case 12: // nosingleexpand
//							stylef &= ~TVS_SINGLEEXPAND;
//							break;
//						case 13: // scroll (off by default)
//							stylef &= ~TVS_NOSCROLL;
//							break;
//						case 14: // noscroll (NB: this can lead to gfx glitches with scroll bars already shown)
//							stylef |= TVS_NOSCROLL;
//							break;
//						default: // unknown style ignore.
//							break;
//						}
//						i++;
//					}
//					SetWindowLong(mIRCLinker::getTreeview(),GWL_STYLE, stylef);
//					RedrawWindow(mIRCLinker::getTreeview(), nullptr, nullptr, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
//				}
//				break;
//			case TEXT('i'): // set image list [+flags] [index] [file]
//				{
//					if (mIRCLinker::getTreeImages() == nullptr) {
//						DCXError(TEXT("xdock -F"), TEXT("No Valid TreeView Image List"));
//						return 0;
//					}
//					if (input.gettok(3) == TEXT("clear")) {
//						HIMAGELIST o = TreeView_SetImageList(mIRCLinker::getTreeview(),nullptr,TVSIL_NORMAL);
//						if (o != nullptr && o != mIRCLinker::getTreeImages())
//							ImageList_Destroy(o);
//					}
//					else if (input.gettok(3) == TEXT("default")) {
//						HIMAGELIST o = TreeView_SetImageList(mIRCLinker::getTreeview(),mIRCLinker::getTreeImages(),TVSIL_NORMAL);
//						if (o != nullptr && o != mIRCLinker::getTreeImages())
//							ImageList_Destroy(o);
//					}
//					else {
//						HIMAGELIST himl = ImageList_Duplicate( TreeView_GetImageList( mIRCLinker::getTreeview(), TVSIL_NORMAL) );
//						if (himl != nullptr) {
//							int type = 0, index = input.gettok(4).to_int();
//							TString cflag(input.gettok(3));
//							cflag.trim();
//							TString filename(input.gettok(5,-1));
//							filename.trim();
//							// add replacement images.
//							switch (cflag[1])
//							{
//							case TEXT('s'): // status windows
//								type = 0;
//								break;
//							case TEXT('c'): // channel windows
//								type = 1;
//								break;
//							case TEXT('C'): // Custom windows
//								type = 2;
//								break;
//							case TEXT('S'): // DCC Send windows
//								type = 3;
//								break;
//							case TEXT('G'): // DCC Get windows
//								type = 4;
//								break;
//							default:
//								{
//									DCXError(TEXT("xdock -F"), TEXT("Invalid Image Flag"));
//									ImageList_Destroy(himl);
//									return 0;
//								}
//								break;
//							}
//							HICON hIcon = dcxLoadIcon(index,filename);
//							if (hIcon != nullptr) {
//								mIRCDebug(TEXT("count: %d"), ImageList_GetImageCount(himl));
//								mIRCDebug(TEXT("replace: %d"), ImageList_ReplaceIcon(himl,type,hIcon));
//								TVITEMEX item;
//								ZeroMemory(&item,sizeof(item));
//								item.hItem = TreeView_GetFirstVisible(mIRCLinker::getTreeview());
//								item.hItem = TreeView_GetNextVisible(mIRCLinker::getTreeview(), item.hItem);
//								item.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE; //I_IMAGECALLBACK == -1
//								mIRCDebug(TEXT("getitem: %d"), TreeView_GetItem(mIRCLinker::getTreeview(),&item));
//								mIRCDebug(TEXT("image: %d selected: %d"), item.iImage, item.iSelectedImage);
//								HIMAGELIST o = TreeView_SetImageList(mIRCLinker::getTreeview(), himl, TVSIL_NORMAL);
//								if (o != nullptr && o != mIRCLinker::getTreeImages())
//									ImageList_Destroy(o);
//								DestroyIcon(hIcon);
//							}
//							else {
//								ImageList_Destroy(himl);
//								return 0;
//							}
//						}
//					}
//				}
//				break;
//			default:
//				DCXError(TEXT("xdock -F"),TEXT("Invalid flag"));
//				return 0;
//			}
//		}
//		else
//			DCXError(TEXT("xdock -F"),TEXT("Unable to Find Treebar"));
//		return 1;
//	}
//
//	HWND dockHwnd = (HWND) input.gettok(2, TEXT(" ")).to_num();
//
//	if (!IsWindow(dockHwnd)) {
//		DCXError(TEXT("/xdock"),TEXT("Invalid Window to dock"));
//		return 0;
//	}
//
//	TString flags(input.gettok(3, TEXT(" ")));
//	if (flags[0] != TEXT('+')) {
//		DCXError(TEXT("/xdock"),TEXT("No Flags Found"));
//		return 0;
//	}
//
//	// dock to toolbar
//	// [-t] [hwnd to dock] [+options]
//	if ((switches[1] == TEXT('t')) && (numtok > 2)) {
//		DockWindow(mWnd, dockHwnd, TEXT("mIRC_Toolbar"), flags);
//	}
//	// dock to switchbar
//	// [-s] [hwnd to dock] [+options]
//	else if ((switches[1] == TEXT('s')) && (numtok > 2)) {
//		DockWindow(mWnd, dockHwnd, TEXT("mIRC_Switchbar"), flags);
//	}
//	// dock to nicklist/sidelistbox
//	// [-n] [hwnd to dock] [+options] [hwnd to dock with]
//	else if ((switches[1] == TEXT('n')) && (numtok > 3)) {
//		mWnd = (HWND) input.gettok(4,TEXT(" ")).to_num();
//
//		if (IsWindow(mWnd))
//			DockWindow(mWnd, dockHwnd, TEXT("ListBox"), flags);
//		else {
//			DCXError(TEXT("/xdock -n"),TEXT("Invalid window"));
//			return 0;
//		}
//	}
//	//dock to custom/channel/query/status
//	// [-c] [hwnd to dock] [+options] [hwnd to dock with]
//	else if ((switches[1] == TEXT('c')) && (numtok > 3)) {
//		mWnd = (HWND) input.gettok(4, TEXT(" ")).to_num();
//
//		if (IsWindow(mWnd))
//			DockWindow(mWnd, dockHwnd, nullptr, flags);
//		else {
//			DCXError(TEXT("/xdock -c"),TEXT("Invalid window"));
//			return 0;
//		}
//	}
//	// dock to treelist
//	// [-b] [hwnd to dock] [+options]
//	else if ((switches[1] == TEXT('b')) && (numtok > 2)) {
//		DockWindow(mWnd, dockHwnd, TEXT("mIRC_TreeList"), flags);
//	}
//	// dock to mIRC (UltraDock)
//	// [-m] [hwnd to dock] [+options]
//	else if ((switches[1] == TEXT('m')) && (numtok > 2)) {
//		UltraDock(mWnd, dockHwnd, flags);
//	}
//	// undock
//	// [-u] [hwnd to undock]
//	else if (switches[1] == TEXT('u')) {
//		if (FindUltraDock(dockHwnd))
//			UltraUnDock(dockHwnd);
//		else
//			UnDock(dockHwnd);
//	}
//	// resize docked window
//	// [-r] [hwnd to dock] [+options] [W] [H]
//	else if ((switches[1] == TEXT('r')) && (numtok > 4)) {
//		int w = input.gettok(4, TEXT(" ")).to_int();
//		int h = input.gettok(5, TEXT(" ")).to_int();
//
//		LPDCXULTRADOCK ud = GetUltraDock(dockHwnd);
//		DWORD dflags = 0;
//
//		if (ud != nullptr)
//			dflags = ud->flags;
//		else
//			dflags = (DWORD) GetProp(dockHwnd, TEXT("dcx_docked"));
//
//		if (dflags == nullptr) {
//			DCXError(TEXT("/xdock -r"),TEXT("Unable to find flags information."));
//			return 0;
//		}
//
//		RECT rc;
//		GetWindowRect(dockHwnd, &rc);
//		OffsetRect(&rc, -rc.left, -rc.top); // right & bottom now == width & height
//
//		switch(dflags)
//		{
//			case DOCKF_LEFT:
//			case DOCKF_RIGHT:
//			case DOCKF_AUTOV:
//				rc.right = w;
//				break;
//
//			case DOCKF_TOP:
//			case DOCKF_BOTTOM:
//			case DOCKF_AUTOH:
//				rc.bottom = h;
//				break;
//
//			case DOCKF_NORMAL:
//				rc.bottom = h;
//				rc.right = w;
//				break;
//
//			case DOCKF_SIZE:
//				DCXError(TEXT("/xdock -r"),TEXT("Can't resize an auto width & height dialog"));
//				return 0;
//
//			default:
//				DCXError(TEXT("/xdock -r"),TEXT("Unknown dock flag"));
//				return 0;
//		}
//
//		// x & y handled by mIRC update, only change width & height.
//		SetWindowPos(dockHwnd, nullptr, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOOWNERZORDER);
//		UpdatemIRC();
//		RedrawWindow( mIRCLinker::getHWND(), nullptr, nullptr, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
//	}
//	else {
//		DCXError(TEXT("/xdock"),TEXT("Invalid Flag"));
//		return 0;
//	}
//
//	return 1;
//}
//
//// hwnd|mIRC prop
//mIRC(_xdock)
//{
//	TString d(data);
//	d.trim();
//
//	data[0] = 0;
//
//	if (d.numtok(TEXT(" ")) < 2) {
//		TString error;
//		error.sprintf(TEXT("Invalid arguments (%s)"), d.gettok(1, TEXT(" ")).to_chr());
//		DCXError(TEXT("$ $+ xdock"),error.to_chr());
//
//		ret(TEXT("D_ERR: Invalid xdock arguments"));
//	}
//
//	if (d.gettok(1,TEXT(" ")) == TEXT("mIRC")) {
//		static const TString poslist(TEXT("switchBarPos toolBarPos treeBarPos switchBarSize toolBarSize treeBarSize isSwitchBar isToolBar isTreeBar isMenuBar text"));
//		int nType = poslist.findtok(d.gettok(2).to_chr(),1);
//		switch (nType)
//		{
//		case 1: // switchBarPos
//		case 2: // toolBarPos
//		case 3: // treeBarPos
//			{
//				switch (SwitchbarPos(nType -1))
//				{
//					case SWB_RIGHT:
//						lstrcpyn(data, TEXT("right"));
//						break;
//
//					case SWB_BOTTOM:
//						lstrcpyn(data, TEXT("bottom"));
//						break;
//
//					case SWB_TOP:
//						lstrcpyn(data, TEXT("top"));
//						break;
//
//					case SWB_LEFT:
//						lstrcpyn(data, TEXT("left"));
//						break;
//
//					case SWB_NONE:
//					default:
//						lstrcpyn(data, TEXT("none"));
//						break;
//				}
//			}
//			break;
//		case 4: // switchBarSize
//			{
//				RECT rc;
//				GetWindowRect(mIRCLink.m_hSwitchbar, &rc);
//				wnsprintf(data,TEXT("%d %d"), rc.right-rc.left, rc.bottom-rc.top);
//			}
//			break;
//		case 5: // toolBarSize
//			{
//				RECT rc;
//				GetWindowRect(mIRCLink.m_hToolbar, &rc);
//				wnsprintf(data,TEXT("%d %d"), rc.right-rc.left, rc.bottom-rc.top);
//			}
//			break;
//		case 6: // treeBarSize
//			{
//				RECT rc;
//				GetWindowRect(mIRCLink.m_hTreebar, &rc);
//				wnsprintf(data,TEXT("%d %d"), rc.right-rc.left, rc.bottom-rc.top);
//			}
//			break;
//		case 7: // isSwitchBar
//			{
//				if (IsWindowVisible(mIRCLink.m_hSwitchbar))
//					lstrcpyn(data,TEXT("$true"));
//				else
//					lstrcpyn(data,TEXT("$false"));
//			}
//			break;
//		case 8: // isToolBar
//			{
//				if (IsWindowVisible(mIRCLink.m_hToolbar))
//					lstrcpyn(data,TEXT("$true"));
//				else
//					lstrcpyn(data,TEXT("$false"));
//			}
//			break;
//		case 9: // isTreeBar
//			{
//				if (IsWindowVisible(mIRCLink.m_hTreebar))
//					lstrcpyn(data,TEXT("$true"));
//				else
//					lstrcpyn(data,TEXT("$false"));
//			}
//			break;
//		case 10: // isMenuBar
//			{
//				if (GetMenu(mIRCLinker::getHWND()))
//					lstrcpyn(data,TEXT("$true"));
//				else
//					lstrcpyn(data,TEXT("$false"));
//			}
//			break;
//		case 11: // text
//			{
//				if (GetWindowTextLength(mIRCLinker::getHWND()) > 0)
//					GetWindowText(mIRCLinker::getHWND(),data,MIRC_BUFFER_SIZE_CCH);
//			}
//			break;
//		case 0: // error
//		default:
//			{
//				TString error;
//				error.sprintf(TEXT("Invalid prop (mIRC).%s"), d.gettok(2, TEXT(" ")).to_chr());
//				DCXError(TEXT("$ $+ xdock"),error.to_chr());
//			}
//			break;
//		}
//	}
//	else {
//		HWND hwnd = (HWND)d.gettok(1,TEXT(" ")).to_num();
//
//		if (IsWindow(hwnd)) {
//			static const TString poslist(TEXT("isDocked hasDocked isAutoV isAutoH isAutoS dockSide text"));
//			int nType = poslist.findtok(d.gettok(2).to_chr(),1);
//			switch (nType)
//			{
//			case 1: // isDocked
//				{
//					if (GetProp(hwnd,TEXT("dcx_docked")) || FindUltraDock(hwnd))
//						lstrcpyn(data,TEXT("$true"));
//					else
//						lstrcpyn(data,TEXT("$false"));
//				}
//				break;
//			case 2: // hasDocked
//				{
//					if (GetProp(hwnd,TEXT("dcx_dock")))
//						lstrcpyn(data,TEXT("$true"));
//					else
//						lstrcpyn(data,TEXT("$false"));
//				}
//				break;
//			case 3: // isAutoV
//				{
//					DWORD flags = (DWORD)GetProp(hwnd,TEXT("dcx_docked"));
//					if (flags == DOCKF_AUTOV)
//						lstrcpyn(data,TEXT("$true"));
//					else
//						lstrcpyn(data,TEXT("$false"));
//				}
//				break;
//			case 4: // isAutoH
//				{
//					DWORD flags = (DWORD)GetProp(hwnd,TEXT("dcx_docked"));
//					if (flags == DOCKF_AUTOH)
//						lstrcpyn(data,TEXT("$true"));
//					else
//						lstrcpyn(data,TEXT("$false"));
//				}
//				break;
//			case 5: // isAutoS
//				{
//					DWORD flags = (DWORD)GetProp(hwnd,TEXT("dcx_docked"));
//					if (flags == DOCKF_SIZE)
//						lstrcpyn(data,TEXT("$true"));
//					else
//						lstrcpyn(data,TEXT("$false"));
//				}
//				break;
//			case 6: // dockSide
//				{
//					LPDCXULTRADOCK ud = GetUltraDock(hwnd);
//					if (ud != nullptr) {
//						switch(ud->flags)
//						{
//						case DOCKF_LEFT:
//							lstrcpyn(data,TEXT("left"));
//							break;
//						case DOCKF_RIGHT:
//							lstrcpyn(data,TEXT("right"));
//							break;
//						case DOCKF_TOP:
//							lstrcpyn(data,TEXT("top"));
//							break;
//						case DOCKF_BOTTOM:
//							lstrcpyn(data,TEXT("bottom"));
//							break;
//						default:
//							lstrcpyn(data,TEXT("unknown"));
//							break;
//						}
//					}
//					else {
//						TString error;
//						error.sprintf(TEXT("Window not docked to main mIRC window (%d).%s"), hwnd, d.gettok(2, TEXT(" ")).to_chr());
//						DCXError(TEXT("$ $+ xdock"),error.to_chr());
//					}
//				}
//				break;
//			case 7: // text
//				{
//					if (GetWindowTextLength(hwnd) > 0)
//						GetWindowText(hwnd,data,MIRC_BUFFER_SIZE_CCH);
//				}
//				break;
//			case 0: // error
//			default:
//				{
//					TString error;
//					error.sprintf(TEXT("Invalid prop (%d).%s"), hwnd, d.gettok(2, TEXT(" ")).to_chr());
//					DCXError(TEXT("$ $+ xdock"),error.to_chr());
//				}
//				break;
//			}
//		}
//		else {
//			TString error;
//			error.sprintf(TEXT("Invalid window (%s)"), d.gettok(1, TEXT(" ")).to_chr());
//			DCXError(TEXT("$ $+ xdock"),error.to_chr());
//		}
//	}
//	return 3;
//}
