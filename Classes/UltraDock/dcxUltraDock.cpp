/*
	UltraDock v0.12 combined with dcx & improved :)
*/

#include "../../defines.h"

extern mIRCDLL mIRCLink;

BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam);
LRESULT CALLBACK mIRCMDIProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// UltraDock stuff
// mIRC components HWND
HWND treeb_hwnd, sb_hwnd, tb_hwnd, mdi_hwnd, hTreeView;
VectorOfDocks v_docks; // list of all docked windows.
WNDPROC oldMDIProc; // old MDI window proc
HFONT pOrigTreeViewFont = NULL;

// force a window update.
void UpdatemIRC(void) {
	SendMessage(mIRCLink.m_mIRCHWND, WM_SIZE, NULL, NULL);
}
/*
	* Setup Everything for UltraDock
*/
void InitUltraDock(void)
{
	/* UltraDock stuff */
	DCX_DEBUG("InitUltraDock", "Finding mIRC_Toolbar...");
	tb_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"mIRC_Toolbar",NULL);

	DCX_DEBUG("InitUltraDock", "Finding MDIClient...");
	mdi_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"MDIClient",NULL);

	DCX_DEBUG("InitUltraDock", "Finding mIRC_SwitchBar...");
	sb_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"mIRC_SwitchBar",NULL);

	DCX_DEBUG("InitUltraDock", "Finding mIRC_TreeList...");
	treeb_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"mIRC_TreeList",NULL);

	hTreeView = GetWindow(treeb_hwnd,GW_CHILD);

	v_docks.clear();
	oldMDIProc = (WNDPROC)SetWindowLong(mdi_hwnd,GWL_WNDPROC, (LONG)mIRCMDIProc);
}
/*
	*	Eject ALL Docked dialogs.
*/
void CloseUltraDock(void)
{
	EnumChildWindows(mIRCLink.m_mIRCHWND,(WNDENUMPROC)EnumDocked,NULL);
	VectorOfDocks::iterator itStart = v_docks.begin();
	VectorOfDocks::iterator itEnd = v_docks.end();

	while (itStart != itEnd) {
		if (*itStart != NULL) {
			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
			SetWindowLong(ud->hwnd,GWL_STYLE, ud->old_styles);
			SetWindowLong(ud->hwnd,GWL_EXSTYLE, ud->old_exstyles);
		  RemStyles(ud->hwnd,GWL_STYLE,WS_CHILDWINDOW);
			SetParent(ud->hwnd, NULL);
			SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOACTIVATE);
			delete ud;
		}
		itStart++;
	}
	v_docks.clear();
	SetWindowLong(mdi_hwnd, GWL_WNDPROC, (LONG)oldMDIProc);
	UpdatemIRC();
}

bool FindUltraDock(const HWND hwnd)
{
	VectorOfDocks::iterator itStart = v_docks.begin();
	VectorOfDocks::iterator itEnd = v_docks.end();

	while (itStart != itEnd) {
		if (*itStart != NULL) {
			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
			if (ud->hwnd == hwnd)
				return true;
		}

		itStart++;
	}
	return false;
}
LPDCXULTRADOCK GetUltraDock(const HWND hwnd)
{
	VectorOfDocks::iterator itStart = v_docks.begin();
	VectorOfDocks::iterator itEnd = v_docks.end();

	while (itStart != itEnd) {
		if (*itStart != NULL) {
			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
			if (ud->hwnd == hwnd)
				return ud;
		}

		itStart++;
	}
	return NULL;
}

void UltraDock(const HWND mWnd, HWND temp, TString flag)
{
	if ((FindUltraDock(temp)) || (GetProp(temp,"dcx_docked") != NULL)) {
		DCXError("/xdock -m","Window already docked");
		return;
	}

	LPDCXULTRADOCK ud = new DCXULTRADOCK;

	ud->hwnd = temp;
	ud->old_exstyles = GetWindowLong(temp,GWL_EXSTYLE);
	ud->old_styles = GetWindowLong(temp,GWL_STYLE);
	ud->flags = DOCKF_LEFT;

	if (flag.len() > 1) {
		switch(flag[1]) {
			case 'r':
				ud->flags = DOCKF_RIGHT;
				break;

			case 't':
				ud->flags = DOCKF_TOP;
				break;

			case 'b':
				ud->flags = DOCKF_BOTTOM;
				break;

			default:
				ud->flags = DOCKF_LEFT;
				break;
		}
	}

	v_docks.push_back(ud);
	RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
	RemStyles(temp,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	//RemStyles(temp,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);
	SetParent(temp, mIRCLink.m_mIRCHWND);
	UpdatemIRC();
}

void UltraUnDock(const HWND hwnd)
{
	VectorOfDocks::iterator itStart = v_docks.begin();
	VectorOfDocks::iterator itEnd = v_docks.end();

	while (itStart != itEnd) {
		if (*itStart != NULL) {
			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
			if (ud->hwnd == hwnd) {
				v_docks.erase(itStart);
				SetWindowLong(ud->hwnd,GWL_STYLE, ud->old_styles);
				SetWindowLong(ud->hwnd,GWL_EXSTYLE, ud->old_exstyles);
			  RemStyles(ud->hwnd,GWL_STYLE,WS_CHILDWINDOW);
				SetParent(ud->hwnd, NULL);
				SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED);
				delete ud;
				UpdatemIRC();
				return;
			}
		}
		itStart++;
	}
}
void AdjustMDIRect(WINDOWPOS *wp)
{
	if ((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE)) { // handle min/max case;
		//HWND cwin = (HWND)SendMessage(mdi_hwnd,WM_MDIGETACTIVE,NULL,NULL);
		//if (IsWindow(cwin)) {
		//	SendMessage(mdi_hwnd,WM_MDIREFRESHMENU,0,0);
		//	DrawMenuBar(cwin);
		//}
		return;
	}

	VectorOfDocks::iterator itStart = v_docks.begin();
	VectorOfDocks::iterator itEnd = v_docks.end();
	RECT rcDocked;
	int xleftoffset = 0, xrightoffset = 0, ytopoffset = 0, ybottomoffset = 0;
	int x,y,w,h,mdiw,mdih,nWin = 0; //nWin = v_docks.size();

	// testing dont delete.
	while (itStart != itEnd) {
		if ((*itStart != NULL) && (IsWindowVisible(((LPDCXULTRADOCK)*itStart)->hwnd))) {
			nWin++; // count docked windows.
		}
		itStart++;
	}
	if (nWin == 0) return;
	itStart = v_docks.begin();
	mdih = wp->cy;
	mdiw = wp->cx;

	ytopoffset = wp->y;
	ybottomoffset = wp->y + mdih;
	xleftoffset = wp->x;
	xrightoffset = wp->x + mdiw;

  HDWP hdwp = BeginDeferWindowPos(nWin);
	HDWP tmp;

	// size docks
	while (itStart != itEnd) {
		if (*itStart != NULL) {
			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
			if (IsWindowVisible(ud->hwnd)) {
				GetWindowRect(ud->hwnd,&rcDocked);
				OffsetRect(&rcDocked,-rcDocked.left,-rcDocked.top);
				switch (ud->flags)
				{
				case DOCKF_LEFT:
					{ // docked to left
						x = xleftoffset;
						y = ytopoffset;
						w = rcDocked.right;
						h = mdih;
						xleftoffset += rcDocked.right;
						mdiw -= rcDocked.right;
					}
					break;
				case DOCKF_RIGHT:
					{ // docked to right
						x = xrightoffset - rcDocked.right;
						y = ytopoffset;
						w = rcDocked.right;
						h = mdih;
						xrightoffset += rcDocked.right;
						mdiw -= rcDocked.right;
					}
					break;
				case DOCKF_TOP:
					{ // dock to top
						x = xleftoffset;
						y = ytopoffset;
						w = mdiw;
						h = rcDocked.bottom;
						ytopoffset += rcDocked.bottom;
						mdih -= rcDocked.bottom;
					}
					break;
				default:
					{ // dock to bottom
						x = xleftoffset;
						y = ybottomoffset - rcDocked.bottom;
						w = mdiw;
						h = rcDocked.bottom;
						ybottomoffset -= rcDocked.bottom;
						mdih -= rcDocked.bottom;
					}
					break;
				}
				tmp = DeferWindowPos(hdwp,ud->hwnd,NULL,x,y,w,h,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
				if (tmp != NULL) hdwp = tmp;
			}
		}

		itStart++;
	}
	wp->x = xleftoffset;
	wp->y = ytopoffset;
	wp->cx = mdiw;
	wp->cy = mdih;
	EndDeferWindowPos(hdwp);
}

// #####################################################################################
// 0 == no swb, 1 == Left, 2 == Right, 3 == Top, 4 == Bottom
int SwitchbarPos(const int type)
{
	RECT swb_rc, mdi_rc;
	HWND hwnd;

	switch (type)
	{
	case 1: // toolbar
		hwnd = tb_hwnd;
		break;
	case 2: // treebar
		hwnd = treeb_hwnd;
		break;
	default:
	case 0: // switchbar
		hwnd = sb_hwnd;
		break;
	}
	if (IsWindowVisible(hwnd)) {
		GetWindowRect(hwnd,&swb_rc);
		GetWindowRect(mdi_hwnd,&mdi_rc);
		if (swb_rc.left >= mdi_rc.right)
			return SWB_RIGHT;
		if (swb_rc.top >= mdi_rc.bottom)
			return SWB_BOTTOM;
		if (swb_rc.bottom <= mdi_rc.top)
			return SWB_TOP;
		return SWB_LEFT;
	}
	return SWB_NONE;
}

LRESULT CALLBACK mIRCMDIProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_WINDOWPOSCHANGING:
			{
				WINDOWPOS *wp = (WINDOWPOS *) lParam;
				if (wp != NULL)
					AdjustMDIRect(wp);
			}
			break;
		case WM_DESTROY:
		{
			SetWindowLong(mIRCLink.m_mIRCHWND, GWL_WNDPROC, (LONG)oldMDIProc);
			PostMessage(mHwnd, uMsg, 0, 0);
			return 0L;
			break;
		}
	}
	return CallWindowProc(oldMDIProc, mHwnd, uMsg, wParam, lParam);
}
