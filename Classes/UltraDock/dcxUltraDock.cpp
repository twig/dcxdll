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
	//GetWindowRect(mdi_hwnd,&rc);
	//SetWindowPos(mdi_hwnd,NULL,0,0,(rc.right-rc.left-10),(rc.bottom-rc.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOREDRAW);
	//SetWindowPos(mdi_hwnd,NULL,0,0,(rc.right-rc.left),(rc.bottom-rc.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE);
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

void UltraDock(HWND mWnd,char *data,HWND temp,TString flag)
{
	if (IsWindow(temp)) {
		if (FindUltraDock(temp)) {
			lstrcpy(data,"D_ERROR Window already docked");
			return;
		}
		RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
		RemStyles(temp,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
		//RemStyles(temp,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
		AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);
		LPDCXULTRADOCK ud = new DCXULTRADOCK;
		ud->hwnd = temp;
		ud->flags = DOCKF_LEFT;
		if (flag.len() == 3) {
			switch(flag[2])
			{
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
		SetParent(temp, mIRCLink.m_mIRCHWND);
		UpdatemIRC();
	}
	else {
		// if HWND invalid, return an error msg
		lstrcpy(data,"-ERR Invalid window");
	}
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

	if (wp->cy == 0 && wp->y == 0 && wp->cx == 0 && wp->x == 0) // handle min/max case;
		return;

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
	//tmp = DeferWindowPos(hdwp,mdi_hwnd,NULL,xleftoffset,ytopoffset,mdiw,mdih,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
	//if (tmp != NULL) hdwp = tmp;
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

//void EjectWindow(HWND dhwnd)
//{
//  LPMYDCXWINDOW lpdcx = (LPMYDCXWINDOW) GetWindowLong(dhwnd, GWL_USERDATA);
//	if (lpdcx == NULL) return;
//
//  // Remove Style for undocking purpose
//  RemStyles(dhwnd,GWL_STYLE,WS_BORDER);
//  //WS_CHILDWINDOW |
//  RemStyles(dhwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
//  // Add Styles input by user
//  AddStyles(dhwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);	
//  AddStyles(dhwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
//  RemStyles(dhwnd,GWL_STYLE,WS_CHILDWINDOW);
//	//SetWindowLong(dhwnd,GWL_STYLE, lpdcx->old_styles);
//	//SetWindowLong(dhwnd,GWL_EXSTYLE, lpdcx->old_exstyles);
//  //SetParent(dhwnd, lpdcx->hParent);
//  SetParent(dhwnd, NULL);
//  SetWindowPos(dhwnd, NULL, lpdcx->rc.left, lpdcx->rc.top, lpdcx->rc.right - lpdcx->rc.left, lpdcx->rc.bottom - lpdcx->rc.top, SWP_NOZORDER);
//
//  delete lpdcx;
//  SetWindowLong(dhwnd, GWL_USERDATA, NULL);
//}
//
//int AlreadyWindow(HWND dhwnd) {
//
//  HWND dchwnd;
//  char text[256];
//	TString d;
//  int nItem = ListBox_GetCount(lb_hwnd);
//
//  int i = 0;
//  while (i < nItem) {
//
//    ListBox_GetText(lb_hwnd, i, text);
//
//		d = text;
//		dchwnd = (HWND) d.gettok(1," ").to_num();
//    if (dchwnd == dhwnd) return i;
//    i++;
//  }
//  return -1;
//}
//
//// Dock N HWND POS NAME
//mIRC(Dock) {
//	TString d(data);
//	d.trim();
//
//  if (d.numtok(" ") > 3) {
//		int pos = (int) d.gettok(1," ").to_num();
//		HWND dhwnd = (HWND) d.gettok(2," ").to_num();
//    if (IsWindow(dhwnd) && AlreadyWindow(dhwnd) == -1) {
//			if ((d.gettok(3," ") == "left") || (d.gettok(3," ") == "right") || (d.gettok(3," ") == "top") || (d.gettok(3," ") == "bottom")) {
//        AttachWindow(dhwnd);
//				ListBox_InsertString(lb_hwnd, pos, d.gettok(2,-1," ").to_chr());
//
//        mIRC_size();
//        ret("U_OK");
//      }
//    }
//  }
//  ret("U_ERROR");
//}
//// UnDock N
//mIRC(UnDock) {
//	TString d(data);
//	d.trim();
//
//	int pos = (int) atoi(d.gettok(1," ").to_chr());
//  int nItem = ListBox_GetCount(lb_hwnd);
//
//  // valid position
//  if (pos >= 0 && pos < nItem) {
//
//    char text[256];
//    if (ListBox_GetText(lb_hwnd, pos, text) != 0) {
//			d = text;
//			HWND dhwnd = (HWND) d.gettok(1," ").to_num();
//      if (IsWindow(dhwnd)) {
//        EjectWindow(dhwnd);
//        ListBox_DeleteString(lb_hwnd, pos);
//
//        mIRC_size();
//        ret("U_OK");
//      }
//    }
//  }
//  ret("U_ERROR");
//}
//// DockPos NAME
//mIRC(DockPos) {
//	TString d(data);
//	d.trim();
//
//	if (d.numtok(" ") > 0) {
//
//    char text[256];
//		TString tmp;
//    int nItem = ListBox_GetCount(lb_hwnd);
//
//    int i = 0;
//    while (i < nItem) {
//
//      ListBox_GetText(lb_hwnd, i, text);
//			tmp = text;
//
//			if (d.gettok(1," ") == tmp.gettok(3," ")) {
//        wsprintf(data,"%d",i);
//        return 3;
//      }
//      i++;
//    }
//  }
//  ret("-1");
//}
//
//// DockSide NAME
//mIRC(DockSide) {
//	TString d(data);
//	d.trim();
//
//  if (d.numtok(" ") > 0) {
//
//    char text[256];
//		TString tmp;
//    int nItem = ListBox_GetCount(lb_hwnd);
//
//    int i = 0;
//    while (i < nItem) {
//
//      ListBox_GetText(lb_hwnd, i, text);
//			tmp = text;
//
//			if (d.gettok(1," ") == tmp.gettok(3," ")) {
//				lstrcpy(data,tmp.gettok(2," ").to_chr());
//        return 3;
//      }
//      i++;
//    }
//  }
//  ret("-1");
//}
//
//mIRC(DockRefresh) {
//
//  mIRC_size();
//  ret("U_OK");
//}
//
//// ShowMenubar 1|0
//mIRC(ShowMenubar) {
//	TString d(data);
//	d.trim();
//
//  if (d.numtok(" ") > 0) {
//		int show = (int) atoi(d.gettok(1," ").to_chr());
//
//		if (show && !GetMenu(mIRCLink.m_mIRCHWND))
//			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);
//		else if(!show && GetMenu(mIRCLink.m_mIRCHWND))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);
//
//    ret("U_OK");
//  }
//  ret("U_ERROR");
//}
//
//// ShowSwitchbar 1|0
//mIRC(ShowSwitchbar) {
//	TString d(data);
//	d.trim();
//
//  if (d.numtok(" ") > 0) {
//		int show = (int) atoi(d.gettok(1," ").to_chr());
//
//    if (show && !IsWindowVisible(sb_hwnd))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);
//    else if(!show && IsWindowVisible(sb_hwnd))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);
//
//    ret("U_OK");
//  }
//  ret("U_ERROR");
//}
//
//// ShowToolbar 1|0
//mIRC(ShowToolbar) {
//	TString d(data);
//	d.trim();
//
//  if (d.numtok(" ") > 0) {
//		int show = (int) atoi(d.gettok(1," ").to_chr());
//
//    if (show && !IsWindowVisible(tb_hwnd))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
//    else if(!show && IsWindowVisible(tb_hwnd))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
//
//    ret("U_OK");
//  }
//  ret("U_ERROR");
//}
//
//// ShowTreebar 1|0
//// non functional!
//mIRC(ShowTreebar) {
//	TString d(data);
//	d.trim();
//
//  if (d.numtok(" ") > 0) {
//		int show = (int) d.gettok(1," ").to_num();
//
//    if (show && !IsWindowVisible(treeb_hwnd))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
//    else if(!show && IsWindowVisible(treeb_hwnd))
//      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
//
//    ret("U_OK");
//  }
//  ret("U_ERROR");
//}
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
