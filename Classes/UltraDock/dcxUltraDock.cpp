/*
	UltraDock v0.12 combined with dcx & improved :)
*/

#include "../../defines.h"

extern mIRCDLL mIRCLink;

BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam);
LRESULT CALLBACK mIRCMDIProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// UltraDock stuff
// mIRC components HWND
HWND treeb_hwnd, sb_hwnd, tb_hwnd, mdi_hwnd;
VectorOfDocks v_docks; // list of all docked windows.
WNDPROC oldMDIProc; // old MDI window proc

void UpdatemIRC(void)
{ // force a window update.
	RECT rc;
	GetWindowRect(mIRCLink.m_mIRCHWND,&rc);
	SetWindowPos(mIRCLink.m_mIRCHWND,NULL,0,0,(rc.right-rc.left-10),(rc.bottom-rc.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOREDRAW);
	SetWindowPos(mIRCLink.m_mIRCHWND,NULL,0,0,(rc.right-rc.left),(rc.bottom-rc.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE);
}
/*
	* Setup Everything for UltraDock
*/
void InitUltraDock(void)
{
	/* UltraDock stuff */
  tb_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"mIRC_Toolbar",NULL);
  mdi_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"MDIClient",NULL);
  sb_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"mIRC_SwitchBar",NULL);
  treeb_hwnd = FindWindowEx(mIRCLink.m_mIRCHWND,NULL,"mIRC_TreeList",NULL);

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
			delete ud;
		}
		itStart++;
	}
	v_docks.clear();
	SetWindowLong(mdi_hwnd, GWL_WNDPROC, (LONG)oldMDIProc);
	UpdatemIRC();
}

bool FindUltraDock(HWND hwnd)
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
LPDCXULTRADOCK GetUltraDock(HWND hwnd)
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

void UltraDock(HWND mWnd, HWND temp, TString flag)
{
	if (FindUltraDock(temp)) {
		mIRCError("D_ERROR Window already docked");
		return;
	}

	LPDCXULTRADOCK ud = new DCXULTRADOCK;
	RECT rc;
	GetWindowRect(temp,&rc);

	ud->hwnd = temp;
	ud->flags = DOCKF_LEFT;

	if (flag.len() > 1) {
		mIRCDebug("docking to %s", flag.to_chr());

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

	ud->old_exstyles = GetWindowLong(temp,GWL_EXSTYLE);
	ud->old_styles = GetWindowLong(temp,GWL_STYLE);
	CopyRect(&ud->rc,&rc);
	v_docks.push_back(ud);
	RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
	RemStyles(temp,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	//RemStyles(temp,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);
	SetParent(temp, mIRCLink.m_mIRCHWND);
	UpdatemIRC();
}

void UltraUnDock(HWND hwnd)
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
				SetWindowPos(ud->hwnd, NULL, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED);
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
	if ((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE)) // handle min/max case;
		return;
	//if (wp->cy == 0 && wp->y == 0 && wp->cx == 0 && wp->x == 0) // handle min/max case;
	//	return;

	VectorOfDocks::iterator itStart = v_docks.begin();
	VectorOfDocks::iterator itEnd = v_docks.end();
	RECT rcDocked;
	int xleftoffset = 0, xrightoffset = 0, ytopoffset = 0, ybottomoffset = 0;
	int x,y,w,h,mdiw,mdih,nWin = 0;

	while (itStart != itEnd) {
		if ((*itStart != NULL) && (IsWindow(((LPDCXULTRADOCK)*itStart)->hwnd))) {
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
			if (IsWindow(ud->hwnd)) {
				GetWindowRect(ud->hwnd,&rcDocked);
				OffsetRect(&rcDocked,-rcDocked.left,-rcDocked.top);
				if (ud->flags == DOCKF_LEFT) { // docked to left
					x = xleftoffset;
					y = ytopoffset;
					w = rcDocked.right;
					h = mdih;
					xleftoffset += rcDocked.right;
					mdiw -= rcDocked.right;
				}
				else if (ud->flags == DOCKF_RIGHT) { // docked to left
					x = xrightoffset - rcDocked.right;
					y = ytopoffset;
					w = rcDocked.right;
					h = mdih;
					xrightoffset += rcDocked.right;
					mdiw -= rcDocked.right;
				}
				else if (ud->flags == DOCKF_TOP) { // dock to top
					x = xleftoffset;
					y = ytopoffset;
					w = mdiw;
					h = rcDocked.bottom;
					ytopoffset += rcDocked.bottom;
					mdih -= rcDocked.bottom;
				}
				else { // dock to bottom
					x = xleftoffset;
					y = ybottomoffset - rcDocked.bottom;
					w = mdiw;
					h = rcDocked.bottom;
					ybottomoffset -= rcDocked.bottom;
					mdih -= rcDocked.bottom;
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
int SwitchbarPos(void)
{
	RECT swb_rc, mdi_rc;

	if (IsWindowVisible(sb_hwnd)) {
		GetWindowRect(sb_hwnd,&swb_rc);
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
