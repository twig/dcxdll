
/*
	* Base Class to handle docking of windows to mIRC elements
	* & adjust the layout of these windows to make room for the docked windows.
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"

// statusbar stuff
HWND DcxDock::g_StatusBar = nullptr;
HIMAGELIST DcxDock::g_hImageList = nullptr;
INT DcxDock::g_iDynamicParts[SB_MAX_PARTSD] = { 0 };
INT DcxDock::g_iFixedParts[SB_MAX_PARTSD] = { 0 };
HFONT DcxDock::g_StatusFont = nullptr;
VectorOfDParts DcxDock::g_vParts;

// treebar stuff
bool DcxDock::g_bTakeOverTreebar = false;
COLORREF DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_MAX +1] = { CLR_INVALID };

DcxDock::DcxDock(HWND refHwnd, HWND dockHwnd, const DockTypes dockType)
: m_OldRefWndProc(nullptr)
, m_OldDockWndProc(nullptr)
, m_RefHwnd(refHwnd)
, m_hParent(dockHwnd)
, m_iType(dockType)
{
	this->m_VectorDocks.clear();
	if (IsWindow(this->m_RefHwnd)) {
		SetProp(this->m_RefHwnd,TEXT("DcxDock"),this);
		this->m_OldRefWndProc = SubclassWindow(this->m_RefHwnd, DcxDock::mIRCRefWinProc);
	}
	if (IsWindow(this->m_hParent)) {
		SetProp(this->m_hParent,TEXT("DcxDock"),this);
		this->m_OldDockWndProc = SubclassWindow(this->m_hParent, DcxDock::mIRCDockWinProc);
	}
	//if (dockType == DOCK_TYPE_TREE)
	//	AddStyles(this->m_RefHwnd,GWL_EXSTYLE,WS_EX_TRANSPARENT);
}

DcxDock::~DcxDock(void)
{

	this->UnDockAll();

	// reset to orig WndProc
	if (IsWindow(this->m_RefHwnd)) {
		RemoveProp(this->m_RefHwnd,TEXT("DcxDock"));
		if ((this->m_OldRefWndProc != nullptr) && ((WNDPROC)GetWindowLongPtr(this->m_RefHwnd, GWLP_WNDPROC) == DcxDock::mIRCRefWinProc))
			SubclassWindow(this->m_RefHwnd, this->m_OldRefWndProc);
	}
	if (IsWindow(this->m_hParent)) {
		RemoveProp(this->m_hParent,TEXT("DcxDock"));
		if ((this->m_OldDockWndProc != nullptr) && ((WNDPROC)GetWindowLongPtr(this->m_hParent, GWLP_WNDPROC) == DcxDock::mIRCDockWinProc))
			SubclassWindow(this->m_hParent, this->m_OldDockWndProc);
	}
	this->UpdateLayout();
}

bool DcxDock::DockWindow(HWND hwnd, const TString &flag)
{
	if (isDocked(hwnd))
		throw Dcx::dcxException(TEXT("Window (%) is already docked"), reinterpret_cast<DWORD>(hwnd));

	if (!IsWindow(this->m_hParent))
		throw Dcx::dcxException("Invalid Dock Host Window");

	//auto ud = new DCXULTRADOCK;
	//
	//ud->hwnd = hwnd;
	//ud->old_exstyles = GetWindowExStyle(hwnd);
	//ud->old_styles = GetWindowStyle(hwnd);
	//ud->flags = DOCKF_LEFT;
	//
	//if (flag.len() > 1) {
	//	switch (flag[1]) {
	//	case TEXT('r'):
	//		ud->flags = DOCKF_RIGHT;
	//		break;
	//
	//	case TEXT('t'):
	//		ud->flags = DOCKF_TOP;
	//		break;
	//
	//	case TEXT('b'):
	//		ud->flags = DOCKF_BOTTOM;
	//		break;
	//
	//	default:
	//		ud->flags = DOCKF_LEFT;
	//		break;
	//	}
	//}
	//
	//this->m_VectorDocks.push_back(ud);

	DWORD dFlags = DOCKF_LEFT;

	if (flag.len() > 1) {
		switch(flag[1]) {
			case TEXT('r'):
				dFlags = DOCKF_RIGHT;
				break;

			case TEXT('t'):
				dFlags = DOCKF_TOP;
				break;

			case TEXT('b'):
				dFlags = DOCKF_BOTTOM;
				break;

			default:
				dFlags = DOCKF_LEFT;
				break;
		}
	}
	m_VectorDocks.push_back(new DCXULTRADOCK{ hwnd, dFlags, GetWindowStyle(hwnd), GetWindowExStyle(hwnd),{ 0,0,0,0 } });

	RemStyles(hwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
	RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	//RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	AddStyles(hwnd,GWL_STYLE,WS_CHILDWINDOW);
	SetParent(hwnd, this->m_hParent);
	
	UpdateLayout();
	return true;
}

void DcxDock::UnDockWindow(const HWND hwnd)
{
	//for (const auto &ud : this->m_VectorDocks)
	//{
	//	if (ud != nullptr) {
	//		if (ud->hwnd == hwnd) {
	//			this->m_VectorDocks.erase(itStart);
	//			SetWindowLongPtr(ud->hwnd, GWL_STYLE, (LONG)ud->old_styles);
	//			SetWindowLongPtr(ud->hwnd, GWL_EXSTYLE, (LONG)ud->old_exstyles);
	//			RemStyles(ud->hwnd, GWL_STYLE, WS_CHILDWINDOW);
	//			SetParent(ud->hwnd, nullptr);
	//			SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER | SWP_FRAMECHANGED);
	//			delete ud;
	//			this->UpdateLayout();
	//			return;
	//		}
	//	}
	//}

	auto itEnd = m_VectorDocks.end();
	auto itGot = std::find_if(m_VectorDocks.begin(), itEnd, [hwnd](const LPDCXULTRADOCK ud) { if (ud != nullptr) { return (ud->hwnd == hwnd); } return false; });
	if (itGot != itEnd) {
		auto ud = *itGot;
		m_VectorDocks.erase(itGot);

		//SetWindowLongPtr(ud->hwnd, GWL_STYLE, static_cast<LONG>(ud->old_styles));
		//SetWindowLongPtr(ud->hwnd, GWL_EXSTYLE, static_cast<LONG>(ud->old_exstyles));
		//RemStyles(ud->hwnd, GWL_STYLE, WS_CHILDWINDOW);
		//SetParent(ud->hwnd, nullptr);
		//SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER | SWP_FRAMECHANGED);
		//delete ud;

		UnDockWindowPtr(ud);

		UpdateLayout();
	}

	//for (auto itStart = this->m_VectorDocks.begin(), itEnd = this->m_VectorDocks.end(); itStart != itEnd; ++itStart)
	//{
	//	if (*itStart != nullptr) {
	//		auto ud = *itStart;
	//		if (ud->hwnd == hwnd) {
	//			this->m_VectorDocks.erase(itStart);
	//			SetWindowLongPtr(ud->hwnd, GWL_STYLE, (LONG)ud->old_styles);
	//			SetWindowLongPtr(ud->hwnd, GWL_EXSTYLE, (LONG)ud->old_exstyles);
	//			RemStyles(ud->hwnd, GWL_STYLE, WS_CHILDWINDOW);
	//			SetParent(ud->hwnd, nullptr);
	//			SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER | SWP_FRAMECHANGED);
	//			delete ud;
	//			this->UpdateLayout();
	//			return;
	//		}
	//	}
	//}

	//auto itStart = this->m_VectorDocks.begin();
	//auto itEnd = this->m_VectorDocks.end();
	//
	//while (itStart != itEnd) {
	//	if (*itStart != nullptr) {
	//		auto ud = *itStart;
	//		if (ud->hwnd == hwnd) {
	//			this->m_VectorDocks.erase(itStart);
	//			SetWindowLongPtr(ud->hwnd,GWL_STYLE, (LONG)ud->old_styles);
	//			SetWindowLongPtr(ud->hwnd,GWL_EXSTYLE, (LONG)ud->old_exstyles);
	//			RemStyles(ud->hwnd,GWL_STYLE,WS_CHILDWINDOW);
	//			SetParent(ud->hwnd, nullptr);
	//			SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED);
	//			delete ud;
	//			this->UpdateLayout();
	//			return;
	//		}
	//	}
	//	++itStart;
	//}
}

void DcxDock::UnDockWindowPtr(gsl::owner<LPDCXULTRADOCK> ud)
{
	if (ud == nullptr)
		return;

	SetWindowLongPtr(ud->hwnd, GWL_STYLE, static_cast<LONG>(ud->old_styles));
	SetWindowLongPtr(ud->hwnd, GWL_EXSTYLE, static_cast<LONG>(ud->old_exstyles));
	RemStyles(ud->hwnd, GWL_STYLE, WS_CHILDWINDOW);
	SetParent(ud->hwnd, nullptr);
	SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
	delete ud;
}

void DcxDock::UnDockAll(void)
{
	// UnDock all windows.
	for (const auto &ud : m_VectorDocks)
	{
		//if (ud != nullptr)
		//{
		//	SetWindowLongPtr(ud->hwnd, GWL_STYLE, static_cast<LONG>(ud->old_styles));
		//	SetWindowLongPtr(ud->hwnd, GWL_EXSTYLE, static_cast<LONG>(ud->old_exstyles));
		//	RemStyles(ud->hwnd, GWL_STYLE, WS_CHILDWINDOW);
		//	SetParent(ud->hwnd, nullptr);
		//	SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
		//	delete ud;
		//}

		UnDockWindowPtr(ud);
	}
	m_VectorDocks.clear();
}

bool DcxDock::FindDock(const HWND hwnd)
{
	//for (const auto &x: this->m_VectorDocks) {
	//	if (x != nullptr) {
	//		if (x->hwnd == hwnd)
	//			return true;
	//	}
	//}
	//return false;

	auto itEnd = m_VectorDocks.end();
	return (std::find_if(m_VectorDocks.begin(), itEnd, [hwnd](const LPDCXULTRADOCK ud) { if (ud != nullptr) { return (ud->hwnd == hwnd); } return false; }) != itEnd);
}

LPDCXULTRADOCK DcxDock::GetDock(const HWND hwnd)
{
	//for (const auto &x: this->m_VectorDocks) {
	//	if (x != nullptr) {
	//		if (x->hwnd == hwnd)
	//			return x;
	//	}
	//}
	//return nullptr;

	auto itEnd = m_VectorDocks.end();
	auto itGot = std::find_if(m_VectorDocks.begin(), itEnd, [hwnd](const LPDCXULTRADOCK ud) { if (ud != nullptr) { return (ud->hwnd == hwnd); } return false; });
	if (itGot != itEnd)
		return *itGot;
	return nullptr;
}

bool DcxDock::isDocked(const HWND hwnd)
{
	return (FindDock(hwnd) || (GetProp(hwnd, TEXT("dcx_docked")) != nullptr));
}

void DcxDock::AdjustRect(WINDOWPOS *wp)
{
	if (dcx_testflag(wp->flags, SWP_NOSIZE) && dcx_testflag(wp->flags, SWP_NOMOVE)) // handle min/max case;
		return;

	RECT rcDocked = { 0 };
	int x = 0, y = 0, w = 0, h = 0; //nWin = this->m_VectorDocks.size();

	// count visible docked windows.

	//for (const auto &x: this->m_VectorDocks) {
	//	if ((x != nullptr) && IsWindowVisible(x->hwnd))
	//		nWin++;
	//}

	auto nWin = m_VectorDocks.size(); // for loop unneeded, max size will do
	if (nWin == 0) return;

	auto refh = wp->cy;
	auto refw = wp->cx;

	auto ytopoffset = wp->y;
	auto ybottomoffset = wp->y + refh;
	auto xleftoffset = wp->x;
	auto xrightoffset = wp->x + refw;

	auto hdwp = BeginDeferWindowPos(static_cast<int>(nWin));
	HDWP tmp = nullptr;

	// size docks
	for (const auto &ud: m_VectorDocks) {
		if (ud == nullptr)
			continue;

		if (!IsWindowVisible(ud->hwnd) || !GetWindowRect(ud->hwnd, &rcDocked))
			continue;

		OffsetRect(&rcDocked,-rcDocked.left,-rcDocked.top);
		switch (ud->flags)
		{
		case DOCKF_LEFT:
			{ // docked to left
				x = xleftoffset;
				y = ytopoffset;
				w = rcDocked.right;
				h = refh;
				xleftoffset += rcDocked.right;
				refw -= rcDocked.right;
			}
			break;
		case DOCKF_RIGHT:
			{ // docked to right
				x = xrightoffset - rcDocked.right;
				y = ytopoffset;
				w = rcDocked.right;
				h = refh;
				xrightoffset += rcDocked.right;
				refw -= rcDocked.right;
			}
			break;
		case DOCKF_TOP:
			{ // dock to top
				x = xleftoffset;
				y = ytopoffset;
				w = refw;
				h = rcDocked.bottom;
				ytopoffset += rcDocked.bottom;
				refh -= rcDocked.bottom;
			}
			break;
		default:
			{ // dock to bottom
				x = xleftoffset;
				y = ybottomoffset - rcDocked.bottom;
				w = refw;
				h = rcDocked.bottom;
				ybottomoffset -= rcDocked.bottom;
				refh -= rcDocked.bottom;
			}
			break;
		}
		tmp = DeferWindowPos(hdwp,ud->hwnd,nullptr,x,y,w,h,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
		if (tmp != nullptr)
			hdwp = tmp;
	}

	wp->x = xleftoffset;
	wp->y = ytopoffset;
	wp->cx = refw;
	wp->cy = refh;
	EndDeferWindowPos(hdwp);
}

LRESULT CALLBACK DcxDock::mIRCRefWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto pthis = static_cast<DcxDock *>(GetProp(mHwnd, TEXT("DcxDock")));
	if (pthis == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg) {
		case WM_WINDOWPOSCHANGING:
			{
				dcxlParam(LPWINDOWPOS,wp);

				if (wp == nullptr)
					break;

				if (((pthis->m_iType == DOCK_TYPE_MDI) || (pthis->m_iType == DOCK_TYPE_TREE)) && DcxDock::IsStatusbar()) {
					RECT rc;
					DcxDock::status_getRect(&rc);
					wp->cy -= (rc.bottom - rc.top);
				}
				pthis->AdjustRect(wp);
			}
			break;
		//case WM_PAINT:
		//	{
		//		if ( pthis->m_iType == DOCK_TYPE_TREE && DcxDock::g_bTakeOverTreebar) {
		//			PAINTSTRUCT ps;
		//			RECT rcClient, rcParent;
		//			HDC hdc, *hBuffer;
//
		//			// get treeviews rect
		//			GetClientRect(mHwnd,&rcClient);
		//			GetClientRect(mIRCLink.m_hTreebar, &rcParent);
//
		//			hdc = BeginPaint(mHwnd, &ps);
//
		//			//hBuffer = CreateHDCBuffer(hdc, &rcParent);
//
		//			////SendMessage(mIRCLink.m_hTreebar, WM_ERASEBKGND, (WPARAM)*hBuffer, nullptr);
		//			//SendMessage(mIRCLink.m_hTreebar, WM_PRINT, (WPARAM)*hBuffer, PRF_CLIENT|PRF_ERASEBKGND);
		//			////SendMessage(mIRCLink.m_hTreebar, WM_PRINTCLIENT, (WPARAM)*hBuffer, PRF_CLIENT);
//
		//			//CopyRect(&rcParent, &rcClient);
		//			//MapWindowRect(mHwnd, mIRCLink.m_hTreebar, &rcParent);
//
		//			//BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), *hBuffer, rcParent.left, rcParent.top, SRCCOPY);
//
		//			//DeleteHDCBuffer(hBuffer);
//
		//			//LRESULT lRes = CallWindowProc(pthis->m_OldRefWndProc, mHwnd, uMsg, (WPARAM)hdc, lParam);
//
		//			EndPaint(mHwnd, &ps);
		//			//return lRes;
		//			return 0L;
		//		}
		//	}
		//	break;
		case WM_ERASEBKGND:
			{
				if ( pthis->m_iType == DOCK_TYPE_TREE && DcxDock::g_bTakeOverTreebar)
					return FALSE;
					//return ((GetWindowLong(mHwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) ? TRUE : FALSE);
			}
			break;
		case TVM_SETITEM:
			{
				if ( pthis->m_iType != DOCK_TYPE_TREE || !DcxDock::g_bTakeOverTreebar)
					break;

				dcxlParam(LPTVITEMEX, pitem);

				if (pitem == nullptr)
					break;

				if (HIWORD(pitem->lParam) != 0)
				{
					TString buf;
					DcxDock::getTreebarItemType(buf, pitem->lParam);
					// <item type> <item pointer> <data1> <data2>
					//mIRCLinker::evalex(nullptr, 0, TEXT("$xtreebar_callback(setitem,%s,%ld,%d,%d)"), buf.to_chr(), pitem->hItem, LOWORD(pitem->lParam), HIWORD(pitem->lParam));
					// <item type> <wid> <status>
					mIRCLinker::evalex(nullptr, 0, TEXT("$xtreebar_callback(setitem,%s,%d,%s)"), buf.to_chr(), HIWORD(pitem->lParam), dcx_testflag(LOWORD(pitem->lParam), 256) ? TEXT("selected") : TEXT("deselected"));
				}
			}
			break;
		case TVM_INSERTITEM:
			{
				if (!DcxDock::g_bTakeOverTreebar)
					break;

				dcxlParam(LPTVINSERTSTRUCT, pTvis);

				if (pTvis == nullptr)
					break;

				if (dcx_testflag(pTvis->itemex.mask,TVIF_TEXT)) {
					TString buf;
					DcxDock::getTreebarItemType(buf, pTvis->itemex.lParam);
					mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_%d %s"), pTvis->itemex.lParam, pTvis->itemex.pszText);
					mIRCLinker::tsEvalex(buf, TEXT("$xtreebar_callback(geticons,%s,%%dcx_%d)"), buf.to_chr(), pTvis->itemex.lParam);
					auto i = buf.getfirsttok( 1 ).to_int() -1;
					if (i < 0)
						i = I_IMAGENONE; //0;
					pTvis->itemex.iImage = i;
					i = buf.getnexttok( ).to_int() -1;
					if (i < 0)
						i = I_IMAGENONE; //0;
					pTvis->itemex.iSelectedImage = i;
					pTvis->itemex.mask |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				}
			}
			break;

		//case WM_MOUSEMOVE:
		//{
		//	if (Dcx::SetCursorUx == nullptr)
		//		break;
//
		//	ShowCursor(FALSE);
		//	Auto(ShowCursor(TRUE));
//
		//	auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
		//	auto hCursor = Dcx::SystemToCustomCursor(GetCursor());
		//	if (hCursor != nullptr)
		//	{
		//		Dcx::SetCursorUx(hCursor);
		//		return 0;
		//	}
			//
		//	return lRes;
		//}

		//case WM_SETCURSOR:
		//{
		//	if ((Dcx::SetCursorUx == nullptr) || (pthis->m_iType != DOCK_TYPE_MDI))
		//		break;
//
		//	const auto iType = (UINT)LOWORD(lParam);
		//	auto hCursor = Dcx::AreaToCustomCursor(iType);
		//	if (hCursor != nullptr)
		//	{
		//		Dcx::SetCursorUx(hCursor);
		//		return TRUE;
		//	}
//
		//	ShowCursor(FALSE);
		//	Auto(ShowCursor(TRUE));
//
		//	auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
		//	hCursor = Dcx::SystemToCustomCursor(GetCursor());
		//	if (hCursor != nullptr)
		//	{
		//		Dcx::SetCursorUx(hCursor);
		//		return TRUE;
		//	}
		//	return lRes;
		//}

		//	// Ook: TODO: add custom cursors!
		//case WM_SETCURSOR:
		//{
		//	if ((HWND)wParam != mHwnd)
		//		break;
//
		//	const UINT iType = LOWORD(lParam);
		//	const auto hCursor = pthis->getCursor(iType);
//
		//	if (hCursor != nullptr)
		//	{
		//		if (GetCursor() != hCursor)
		//			SetCursor(hCursor);
		//	}
		//	break;
		//}

		//case WM_DESTROY:
		//{
		//	// do we need to swap back old windproc? window is being destroyed after all & its a static function...
		//	if (IsWindow(mHwnd))
		//		break;
//
		//	if ((pthis->m_OldRefWndProc != nullptr) && ((WNDPROC)GetWindowLongPtr(mHwnd, GWLP_WNDPROC) == DcxDock::mIRCRefWinProc))
		//		SubclassWindow(mHwnd, pthis->m_OldRefWndProc);
		//}
	}
	if (pthis->m_OldRefWndProc == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	return CallWindowProc(pthis->m_OldRefWndProc, mHwnd, uMsg, wParam, lParam);
}

//BOOL CALLBACK DcxDock::EnumTreebarWindows(HWND hwnd, LPARAM lParam)
//{
//	TCHAR title[256];
//	TCHAR *buf = (TCHAR *)lParam;
//	title[0] = 0;
//	GetWindowText(hwnd, title, 255);
//	if (lstrcmp(buf, title) == 0) {
//		mIRCDebug(TEXT("match: %ld : %s"), hwnd, title);
//		return FALSE;
//	}
//	return TRUE;
//}

LRESULT CALLBACK DcxDock::mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto pthis = static_cast<DcxDock *>(GetProp(mHwnd, TEXT("DcxDock")));
	if (pthis == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg) {
		case WM_SIZE:
			{
				if ((pthis->m_iType == DOCK_TYPE_MDI) && DcxDock::IsStatusbar()) { // parent of MDI type == main mIRC win.
					InvalidateRect(DcxDock::g_StatusBar, nullptr, FALSE); // needed to stop display corruption
					SendMessage(DcxDock::g_StatusBar,WM_SIZE, (WPARAM)0, (LPARAM)0);
					DcxDock::status_updateParts();
				}
			}
			break;

		case WM_WINDOWPOSCHANGING:
			{
				if ((lParam != NULL) && (pthis->m_iType != DOCK_TYPE_MDI) && DcxDock::IsStatusbar()) {
					dcxlParam(LPWINDOWPOS, wp);

					if ((dcx_testflag(wp->flags, SWP_NOSIZE)) && (dcx_testflag(wp->flags, SWP_NOMOVE)))
						break;

					const auto pos = DcxDock::getPos(wp->x, wp->y, wp->cx, wp->cy);
					if (pos == SwitchBarPos::SWB_TOP) // if at top then ignore it.
						break;

					RECT rc;
					DcxDock::status_getRect(&rc);
					if (pos == SwitchBarPos::SWB_BOTTOM) // if at bottom move it up.
						wp->y -= (rc.bottom - rc.top);
					else
						wp->cy -= (rc.bottom - rc.top);
				}
			}
			break;
		case WM_NOTIFY:
			{
				if (pthis->m_iType == DOCK_TYPE_TREE) {
					dcxlParam(LPNMHDR, hdr);

					if (hdr == nullptr || !DcxDock::g_bTakeOverTreebar)
						break;

					switch( hdr->code ) {
						case NM_CUSTOMDRAW:
							{
								dcxlParam(LPNMTVCUSTOMDRAW, lpntvcd);
								switch (lpntvcd->nmcd.dwDrawStage) {
									case CDDS_PREPAINT:
										return CDRF_NOTIFYITEMDRAW;
									case CDDS_ITEMPREPAINT:
										{
											if (dcx_testflag(lpntvcd->nmcd.uItemState, CDIS_HOT)) { // This makes sure the hot colour doesnt show as blue.
												if (DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_TEXT] != CLR_INVALID)
													lpntvcd->clrText = DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_TEXT];
												else
													lpntvcd->clrText = GetSysColor(COLOR_HOTLIGHT);

												if (DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_BKG] != CLR_INVALID) // only set a bkg colour if one is set in prefs.
													lpntvcd->clrTextBk = DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_BKG];
											}
											if (dcx_testflag(lpntvcd->nmcd.uItemState, CDIS_SELECTED)) { // This makes sure the selected colour doesnt show as grayed.
												if (DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED] != CLR_INVALID)
													lpntvcd->clrText = DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED];
												else
													lpntvcd->clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);

												if (DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED_BKG] != CLR_INVALID)
													lpntvcd->clrTextBk = DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_SELECTED_BKG];
												else
													lpntvcd->clrTextBk = GetSysColor(COLOR_HIGHLIGHT);
											}
											else {
												//HTREEITEM hItem = (HTREEITEM)lpntvcd->nmcd.dwItemSpec;
												//TVITEMEX tvi;
												//ZeroMemory(&tvi, sizeof(tvi));
												//tvi.mask = TVIF_CHILDREN|TVIF_STATE;
												//tvi.hItem = hItem;
												//tvi.stateMask = TVIS_EXPANDED;
												//
												//if (TreeView_GetItem(lpntvcd->nmcd.hdr.hwndFrom, &tvi))
												//{
												//	if (tvi.cChildren && !(tvi.state & TVIS_EXPANDED)) { // has children & not expanded
												//		DcxDock::getTreebarChildState(hItem, &tvi);
												//	}
												//}

												const int wid = HIWORD(lpntvcd->nmcd.lItemlParam);
												TString buf;
												mIRCLinker::tsEvalex(buf, TEXT("$window(@%d).sbcolor"), wid);
												if (!buf.empty()) {
													static const TString sbcolor(TEXT("s s message s event s highlight")); // 's' is used as a spacer.
													const auto clr = sbcolor.findtok(buf, 1);
													if ((clr == 0) || (clr >= Dcx::countof(DcxDock::g_clrTreebarColours))) // no match, do normal colours
														break;
													if (DcxDock::g_clrTreebarColours[clr-1] != CLR_INVALID) // text colour
														lpntvcd->clrText = DcxDock::g_clrTreebarColours[clr-1];
													if (DcxDock::g_clrTreebarColours[clr] != CLR_INVALID) // bkg colour
														lpntvcd->clrTextBk = DcxDock::g_clrTreebarColours[clr];
												}
											}
											return CDRF_NEWFONT;
										}
									default:
										return CDRF_DODEFAULT;
								}
							}
							break;
					case TVN_GETINFOTIP:
						{
							dcxlParam(LPNMTVGETINFOTIP, tcgit);

							if (tcgit != nullptr) {
								if (tcgit->cchTextMax < 1)
									break;

								TString buf((UINT)MIRC_BUFFER_SIZE_CCH);
								TVITEMEX item;
								ZeroMemory(&item, sizeof(item));

								item.hItem = tcgit->hItem;
								item.pszText = buf.to_chr();
								item.cchTextMax = MIRC_BUFFER_SIZE_CCH;
								item.mask = TVIF_TEXT;
								if (TreeView_GetItem(mIRCLinker::getTreeview(), &item)) {
									TString tsType;
									DcxDock::getTreebarItemType(tsType, item.lParam);
									mIRCLinker::execex(TEXT("/!set -nu1 %%dcx_%d %s"), item.lParam, item.pszText); // <- had wrong args causing instant crash when showing tooltips
									mIRCLinker::tsEvalex(buf, TEXT("$xtreebar_callback(gettooltip,%s,%%dcx_%d)"), tsType.to_chr(), item.lParam);

									if (!buf.empty())
										dcx_strcpyn(tcgit->pszText, buf.to_chr(), tcgit->cchTextMax);
								}
							}
							return 0L;
						}
						break;
					}
				}
				else if ((pthis->m_iType == DOCK_TYPE_MDI) && DcxDock::IsStatusbar()) {
					dcxlParam(LPNMHDR, hdr);

					if (hdr == nullptr)
						break;

					if (hdr->hwndFrom != DcxDock::g_StatusBar)
						break;

					const auto idPart = (reinterpret_cast<LPNMMOUSE>(hdr))->dwItemSpec + 1;

					switch( hdr->code ) {
						case NM_CLICK:
							{
								mIRCLinker::signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar sclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_DBLCLK:
							{
								mIRCLinker::signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar dclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_RCLICK:
							{
								mIRCLinker::signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar rclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_RDBLCLK:
							{
								mIRCLinker::signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar rdclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
					}
				}
			}
			break;
		case WM_DRAWITEM:
			{
				dcxlParam(LPDRAWITEMSTRUCT, lpDrawItem);

				if (lpDrawItem == nullptr)
					break;

				if (pthis->g_StatusBar != nullptr && pthis->g_StatusBar == lpDrawItem->hwndItem) {
					auto pPart = reinterpret_cast<LPSB_PARTINFOD>(lpDrawItem->itemData);
					if (pPart != nullptr) {
						auto rc = lpDrawItem->rcItem;

						if (pPart->m_BkgCol != nullptr)
							FillRect(lpDrawItem->hDC, &rc, pPart->m_BkgCol);

						if (pPart->m_iIcon > -1) {
							IMAGEINFO ii;
							if (ImageList_GetImageInfo(pthis->g_hImageList, pPart->m_iIcon, &ii))
							{
								if (ImageList_Draw(pthis->g_hImageList, pPart->m_iIcon, lpDrawItem->hDC, rc.left, rc.top + ((rc.bottom - rc.top) - (ii.rcImage.bottom - ii.rcImage.top)) / 2, ILD_TRANSPARENT))
									rc.left += (ii.rcImage.right - ii.rcImage.left) + 5;
							}
						}

						COLORREF oldTxtClr = CLR_INVALID;
						if (pPart->m_TxtCol != CLR_INVALID)
							oldTxtClr = SetTextColor(lpDrawItem->hDC, pPart->m_TxtCol);

						const auto oldbkg = SetBkMode(lpDrawItem->hDC, TRANSPARENT);

						if (!pPart->m_Text.empty())
							mIRC_DrawText(lpDrawItem->hDC, pPart->m_Text, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, false);
						else if (IsWindow(pPart->m_Child))
							SetWindowPos(pPart->m_Child, nullptr, rc.left, rc.top, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);

						SetBkMode( lpDrawItem->hDC, oldbkg );
						if (oldTxtClr != CLR_INVALID)
							SetTextColor(lpDrawItem->hDC, oldTxtClr);
						return TRUE;
					}
				}
			}
			break;

			// we use menu command instead now. here for refrence only
		//case WM_SHOWWINDOW: // not sent when SW_SHOWNORMAL is used.
		//	{
		//		static const TString tsTypes(TEXT("switchbar toolbar treebar mdi"));
		//		if ((BOOL)wParam)
		//			mIRCSignalDCX(dcxSignal.xdock, TEXT("%s enabled"), tsTypes.gettok( pthis->m_iType +1 ).to_chr());
		//		else
		//			mIRCSignalDCX(dcxSignal.xdock, TEXT("%s disabled"), tsTypes.gettok( pthis->m_iType +1 ).to_chr());
		//	}
		//	break;

		case WM_PARENTNOTIFY:
			{
				if (LOWORD(wParam) == WM_DESTROY)
					pthis->UnDockWindow((HWND)lParam);
			}
			break;

		//case WM_SETCURSOR:
		//{
		//	if (Dcx::SetCursorUx == nullptr)
		//		break;
//
		//	const auto iType = (UINT)LOWORD(lParam);
		//	auto hCursor = Dcx::AreaToCustomCursor(iType);
		//	if (hCursor != nullptr)
		//	{
		//		Dcx::SetCursorUx(hCursor);
		//		return TRUE;
		//	}
			//
		//	ShowCursor(FALSE);
		//	Auto(ShowCursor(TRUE));
			//
		//	auto lRes = mIRCLinker::callDefaultWindowProc(mHwnd, uMsg, wParam, lParam);
		//	hCursor = Dcx::SystemToCustomCursor(GetCursor());
		//	if (hCursor != nullptr)
		//	{
		//		Dcx::SetCursorUx(hCursor);
		//		return TRUE;
		//	}
		//	return lRes;
		//}

		//case WM_DESTROY:
		//{
		//	//delete pthis;
		//	//PostMessage(mHwnd, uMsg, 0, 0);
		//	//return 0L;
		//	if (IsWindow(mHwnd))
		//	{
		//		if ((WNDPROC)GetWindowLongPtr(mHwnd, GWLP_WNDPROC) == DcxDock::mIRCDockWinProc)
		//			SubclassWindow(mHwnd, pthis->m_OldDockWndProc);
		//	}
//
		//	LRESULT lRes = CallWindowProc(pthis->m_OldDockWndProc, mHwnd, uMsg, wParam, lParam);
//
		//	delete pthis;
//
		//	return lRes;
		//}
	}
	if (pthis->m_OldDockWndProc == nullptr)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	return CallWindowProc(pthis->m_OldDockWndProc, mHwnd, uMsg, wParam, lParam);
}

bool DcxDock::InitStatusbar(const TString &styles)
{
	if (IsWindow(g_StatusBar))
		return true;

	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;

	ZeroMemory(DcxDock::g_iDynamicParts, sizeof(DcxDock::g_iDynamicParts));
	ZeroMemory(DcxDock::g_iFixedParts, sizeof(DcxDock::g_iFixedParts));

	DcxDock::status_parseControlStyles(styles, &Styles, &ExStyles, &bNoTheme);

	g_StatusBar = CreateWindowExW(
		(DWORD)ExStyles,
		STATUSCLASSNAMEW,nullptr,
		(DWORD)Styles,
		0, 0, 0, 0, mIRCLinker::getHWND(), (HMENU)(mIRC_ID_OFFSET - 1), nullptr, nullptr);

	if (IsWindow(g_StatusBar)) {
		if ( bNoTheme )
			Dcx::UXModule.dcxSetWindowTheme( g_StatusBar , L" ", L" " );

		//SendMessage(g_StatusBar, SB_SETUNICODEFORMAT, TRUE, NULL);
		return true;
	}
	return false;
}

void DcxDock::UnInitStatusbar(void)
{
	if (IsWindow(g_StatusBar)) {
		DcxDock::status_cleanPartIcons();
		DestroyWindow(g_StatusBar);

		//auto itStart = DcxDock::g_vParts.begin();
		//auto itEnd = DcxDock::g_vParts.end();
		//
		//while (itStart != itEnd) {
		//	if (*itStart != nullptr) {
		//		if ((*itStart)->m_BkgCol != nullptr)
		//			DeleteBrush((*itStart)->m_BkgCol);
		//		delete *itStart;
		//	}
		//	++itStart;
		//}

		for (const auto &x : DcxDock::g_vParts)
		{
			if (x != nullptr) {
				if (x->m_BkgCol != nullptr)
					DeleteBrush(x->m_BkgCol);
				delete x;
			}
		}
		DcxDock::g_vParts.clear();
	}
	g_StatusBar = nullptr;

	if (g_hImageList != nullptr)
		ImageList_Destroy( g_hImageList );
	g_hImageList = nullptr;

	if (g_StatusFont != nullptr)
		DeleteFont(g_StatusFont);
	g_StatusFont = nullptr;
}

bool DcxDock::IsStatusbar(void)
{
	return (IsWindow(g_StatusBar) != FALSE);
}

void DcxDock::status_parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	*Styles = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	for (const auto &tsStyle: styles) {
		if (tsStyle == TEXT("grip"))
			*Styles |= SBARS_SIZEGRIP;
		else if (tsStyle == TEXT("tooltips"))
			*Styles |= SBARS_TOOLTIPS;
		else if (tsStyle == TEXT("nodivider"))
			*Styles |= CCS_NODIVIDER;
		else if (tsStyle == TEXT("notheme"))
			*bNoTheme = TRUE;
		else if (tsStyle == TEXT("disabled"))
			*Styles |= WS_DISABLED;
		else if (tsStyle == TEXT("transparent"))
			*ExStyles |= WS_EX_TRANSPARENT;
	}
}

void DcxDock::status_getRect(LPRECT rc) {
	GetWindowRect(g_StatusBar,rc);
}

void DcxDock::status_setBkColor( const COLORREF clrBk ) {
	SendMessage( g_StatusBar, SB_SETBKCOLOR, (WPARAM) 0, (LPARAM) clrBk );
}

void DcxDock::status_setParts( const UINT nParts, const LPINT aWidths ) {
	SendMessage( g_StatusBar, SB_SETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

UINT DcxDock::status_getParts( const UINT nParts, LPINT aWidths ) {
	return (UINT)SendMessage( g_StatusBar, SB_GETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

void DcxDock::status_setText(const int iPart, const int Style, const WCHAR *const lpstr) {
	SendMessage( g_StatusBar, SB_SETTEXTW, (WPARAM) iPart | Style, (LPARAM) lpstr );
}

UINT DcxDock::status_getTextLength( const int iPart ) {
	return (UINT)LOWORD(SendMessage( g_StatusBar, SB_GETTEXTLENGTHW, (WPARAM) iPart, NULL ));
}

UINT DcxDock::status_getPartFlags( const int iPart ) {
	return (UINT)HIWORD(SendMessage( g_StatusBar, SB_GETTEXTLENGTHW, (WPARAM) iPart, NULL ));
}

LRESULT DcxDock::status_getText( const int iPart, LPWSTR lpstr ) {
	return SendMessage( g_StatusBar, SB_GETTEXTW, (WPARAM) iPart, (LPARAM) lpstr );
}

void DcxDock::status_setTipText( const int iPart, const WCHAR *const lpstr ) {
	SendMessage( g_StatusBar, SB_SETTIPTEXTW, (WPARAM) iPart, (LPARAM) lpstr );
}

void DcxDock::status_getTipText( const int iPart, const int nSize, LPWSTR lpstr ) {
	SendMessage( g_StatusBar, SB_GETTIPTEXTW, (WPARAM) MAKEWPARAM (iPart, nSize), (LPARAM) lpstr );
}

void DcxDock::status_getRect( const int iPart, LPRECT lprc ) {
	SendMessage( g_StatusBar, SB_GETRECT, (WPARAM) iPart, (LPARAM) lprc );
}

void DcxDock::status_setIcon( const int iPart, const HICON hIcon ) {
	SendMessage( g_StatusBar, SB_SETICON, (WPARAM) iPart, (LPARAM) hIcon );
}

HICON DcxDock::status_getIcon( const int iPart ) {
	return (HICON)SendMessage( g_StatusBar, SB_GETICON, (WPARAM) iPart, (LPARAM) 0 );
}

LRESULT DcxDock::status_setPartInfo( const int iPart, const int Style, const LPSB_PARTINFOD pPart) {
  return SendMessage( g_StatusBar, SB_SETTEXT, (WPARAM) iPart | (Style | SBT_OWNERDRAW), (LPARAM) pPart );
}

void DcxDock::status_deletePartInfo(const int iPart)
{
	if (dcx_testflag(DcxDock::status_getPartFlags( iPart ),SBT_OWNERDRAW)) {
		auto pPart = reinterpret_cast<LPSB_PARTINFOD>(DcxDock::status_getText(iPart, nullptr));

		if (pPart == nullptr)
			return;

		//auto itStart = DcxDock::g_vParts.begin();
		//auto itEnd = DcxDock::g_vParts.end();
		//
		//while (itStart != itEnd) {
		//	if (*itStart != nullptr && *itStart == pPart) {
		//		DcxDock::status_setText( iPart, SBT_OWNERDRAW, nullptr);
		//		if (IsWindow(pPart->m_Child))
		//			DestroyWindow(pPart->m_Child);
		//		delete pPart;
		//		DcxDock::g_vParts.erase(itStart);
		//		return;
		//	}
		//	++itStart;
		//}

		//const auto itEnd = DcxDock::g_vParts.end();
		//const auto itGot = std::find(DcxDock::g_vParts.begin(), itEnd, pPart);
		//if (itGot != itEnd)
		//{
		//	DcxDock::status_setText(iPart, SBT_OWNERDRAW, nullptr);
		//	if (IsWindow(pPart->m_Child))
		//		DestroyWindow(pPart->m_Child);
		//	delete pPart;
		//	DcxDock::g_vParts.erase(itGot);
		//}

		if (Dcx::eraseIfFound(g_vParts, pPart))
		{
			DcxDock::status_setText(iPart, SBT_OWNERDRAW, nullptr);
			if (IsWindow(pPart->m_Child))
				DestroyWindow(pPart->m_Child);
			delete pPart;
		}
	}
}

HIMAGELIST &DcxDock::status_getImageList( ) noexcept {

	return g_hImageList;
}

void DcxDock::status_setImageList( HIMAGELIST himl ) {

	g_hImageList = himl;
}

HIMAGELIST DcxDock::status_createImageList( ) {

	return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

const UINT DcxDock::status_parseItemFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	if (!xflags[TEXT('+')])
		return iFlags;

	if (xflags[TEXT('n')])
		iFlags |= SBT_NOBORDERS;
	if (xflags[TEXT('p')])
		iFlags |= SBT_POPOUT;
	if (xflags[TEXT('f')])
		iFlags |= SBT_OWNERDRAW;

	return iFlags;
}

void DcxDock::status_cleanPartIcons( ) {

	for (auto n = 0; n < SB_MAX_PARTSD; n++ )
		DestroyIcon( DcxDock::status_getIcon( n ) );
}

LRESULT DcxDock::status_getBorders( LPINT aWidths ) {
  return SendMessage( g_StatusBar, SB_GETBORDERS, (WPARAM) 0, (LPARAM) aWidths );
}

void DcxDock::status_updateParts(void) {
	const auto nParts = DcxDock::status_getParts(0, nullptr);

	if (nParts == 0)
		return;

	RECT rcClient;
	if (!GetClientRect(DcxDock::g_StatusBar, &rcClient))
		return;

	auto pParts = std::make_unique<int[]>(nParts);
	//int *pParts = new int[nParts];
	//int borders[3];

	DcxDock::status_getParts(nParts, pParts.get());

	//DcxDock::status_getBorders(borders);

	const auto w = (rcClient.right - rcClient.left) / 100; // - (2 * borders[1]);

	for (auto i = decltype(nParts){0}; i < nParts; i++) {
		int pw = 0;
		if (DcxDock::g_iDynamicParts[i] != 0)
			pw = w * DcxDock::g_iDynamicParts[i];
		else
			pw = DcxDock::g_iFixedParts[i];
		if (i == 0)
			pParts[i] = pw;
		else {
			if (pw == -1)
				pParts[i] = -1;
			else
				pParts[i] = (pParts[i-1] + pw);
		}
	}

	DcxDock::status_setParts(nParts, pParts.get());
}

void DcxDock::status_setFont(HFONT f)
{
	if (f != nullptr) {
		SetWindowFont(g_StatusBar, f, TRUE);

		if (g_StatusFont != nullptr)
			DeleteFont(g_StatusFont);
		g_StatusFont = f;
	}
}

const SwitchBarPos DcxDock::getPos(const int x, const int y, const int w, const int h)
{
	RECT rc;
	if (!GetClientRect(mIRCLinker::getHWND(), &rc))
		return SwitchBarPos::SWB_NONE;

	if (x == rc.left && (y + h) == rc.bottom && (x + w) != rc.right)
		return SwitchBarPos::SWB_LEFT;
	if (x == rc.left && (y + h) != rc.bottom && (x + w) == rc.right)
		return SwitchBarPos::SWB_TOP;
	if (x == rc.left && (y + h) == rc.bottom && (x + w) == rc.right)
		return SwitchBarPos::SWB_BOTTOM;
	return SwitchBarPos::SWB_RIGHT;
}

void DcxDock::getTreebarItemType(TString &tsType, const LPARAM lParam)
{
	const UINT wid = HIWORD(lParam);
	switch (wid)
	{
	case 15000: // channel folder
		tsType = TEXT("channelfolder");
		break;
	case 15004:
		tsType = TEXT("transferfolder");
		break;
	case 15006: // window folder
		tsType = TEXT("windowfolder");
		break;
	case 15007: // notify folder
		tsType = TEXT("notifyfolder");
		break;
	case 0:
		tsType = TEXT("Unknown");
		break;
	default:
		{
			mIRCLinker::tsEvalex(tsType, TEXT("$window(@%d).type"), wid);
			if (tsType.empty())
				tsType = TEXT("notify");
		}
		break;
	}
}

//UINT DcxDock::getTreebarChildState(const HTREEITEM hParent, LPTVITEMEX pitem)
//{
//	ZeroMemory(pitem, sizeof(TVITEMEX));
//	for (HTREEITEM ptvitem = TreeView_GetChild(mIRCLinker::getTreeview(), hParent); ptvitem != nullptr; ptvitem = TreeView_GetNextSibling(mIRCLinker::getTreeview(), ptvitem)) {
//		pitem->hItem = ptvitem;
//		pitem->mask = TVIF_STATE|TVIF_CHILDREN;
//		pitem->stateMask = TVIS_EXPANDED;
//
//		if (TreeView_GetItem(mIRCLinker::getTreeview(), pitem)) {
//		}
//		TraverseChildren(ptvitem, buf, res, pitem);
//	}
//}
