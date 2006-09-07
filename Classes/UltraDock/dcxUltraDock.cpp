/*
	UltraDock v0.12 combined with dcx & improved :)
*/

#include "../../defines.h"

extern mIRCDLL mIRCLink;
BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam);

// UltraDock stuff
// mIRC components HWND
HWND treeb_hwnd, sb_hwnd, tb_hwnd, mdi_hwnd, lb_hwnd;
// switchbar position
int swb_pos;
// indicate if MDI is maxed out
BOOL MDIismax;

// UltraDock commands
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

  swb_pos = SwitchbarPos();
	//lb_hwnd = NULL;
  // Listbox opration creation
  lb_hwnd = CreateWindowEx(
    WS_EX_NOPARENTNOTIFY,
    "ListBox",
    NULL,
    WS_CHILD,
    100,
    100,
    200,
    200,
    mIRCLink.m_mIRCHWND,
    NULL,
    GetModuleHandle(NULL),
    NULL);
}
/*
	*	Eject ALL Docked dialogs.
*/
void CloseUltraDock(void)
{
  char text[256];
	TString d;
	size_t l;

  while ((l = ListBox_GetText(lb_hwnd, 0, text)) != LB_ERR) {
		if (l > 0) {
			d = text;
			HWND dhwnd = (HWND) atol(d.gettok(1," ").to_chr());
			if (IsWindow(dhwnd))
				EjectWindow(dhwnd);
		}
		ListBox_DeleteString(lb_hwnd, 0);
  }
	EnumChildWindows(mIRCLink.m_mIRCHWND,(WNDENUMPROC)EnumDocked,NULL);
	mIRC_size();
	DestroyWindow(lb_hwnd);
	lb_hwnd = NULL;
}
// #####################################################################################
// 0 == no swb, 1 == Left, 2 == Right, 3 == Top, 4 == Bottom
int SwitchbarPos(void)
{
  RECT rc, rsb, rtb, treeb;
  POINT sbp;

	GetClientRect(mIRCLink.m_mIRCHWND, &rc);
  GetClientRect(tb_hwnd, &rtb);
  GetWindowRect(sb_hwnd, &rsb);
  GetWindowRect(treeb_hwnd, &treeb);

  if (IsWindowVisible(sb_hwnd)) {

    sbp.x = rsb.left;
    sbp.y = rsb.top;
		ScreenToClient(mIRCLink.m_mIRCHWND, &sbp);
    int rsbw = rsb.right - rsb.left;
    int rsbh = rsb.bottom - rsb.top;

    if (IsWindowVisible(tb_hwnd)) {

      // toolbar height added
      rc.top += rtb.bottom - rtb.top;

      int rch = rc.bottom - rc.top;
      if (sbp.x == 0 && sbp.y == rtb.bottom - rtb.top) {
        if (rsbh > rch - 5 && rsbh < rch + 5) {
          return SWB_LEFT;
        }
        else {
          return SWB_TOP;
        }
      }
      else {
        if (rsbh > rch - 5 && rsbh < rch + 5) {
          return SWB_RIGHT;
        }
        else {
          return SWB_BOTTOM;
        }
      }
    }
    else {

      int rch = rc.bottom - rc.top;
      if (sbp.x == 0 && sbp.y == 0) {
        if (rsbh > rch - 5 && rsbh < rch + 5) {
          return SWB_LEFT;
        }
        else {
          return SWB_TOP;
        }
      }
      else {
        if (rsbh > rch - 5 && rsbh < rch + 5) {
          return SWB_RIGHT;
        }
        else {
          return SWB_BOTTOM;
        }
      }
    }
  }
  return SWB_NONE;
}

void mIRC_size(void)
{
  RECT rmdi, rsb, rtb, treeb;

  // mdi size starts at client size
	GetClientRect(mIRCLink.m_mIRCHWND, &rmdi);
  GetClientRect(tb_hwnd, &rtb);
  GetClientRect(sb_hwnd, &rsb);
  GetClientRect(treeb_hwnd, &treeb);

  // MDI is always there
  int nWin = 1;

  if (IsWindowVisible(tb_hwnd) && IsWindow(tb_hwnd))
    nWin++;
  if (IsWindowVisible(sb_hwnd) && IsWindow(sb_hwnd))
    nWin++;
  //if (IsWindowVisible(treeb_hwnd) && IsWindow(treeb_hwnd))
  //  nWin++;

  int nItem = ListBox_GetCount(lb_hwnd);
  if (nItem != LB_ERR) nWin += nItem;

  // DeferWindowPos Multiple Handle
  HDWP hdwp = BeginDeferWindowPos(nWin);
	HDWP tmp = NULL;

  // reajust top
  if (IsWindowVisible(tb_hwnd)) {
    //DeferWindowPos(hdwp, tb_hwnd, NULL, 0, 0, rmdi.right - rmdi.left, rtb.bottom - rtb.top, SWP_NOZORDER);
    tmp = DeferWindowPos(hdwp, tb_hwnd, NULL, 0, 0, rmdi.right - rmdi.left, rtb.bottom - rtb.top, SWP_NOZORDER);
		if (tmp != NULL) hdwp = tmp;
    //MoveWindow(tb_hwnd, 0, 0, rmdi.right - rmdi.left, rtb.bottom - rtb.top, false);
    rmdi.top += rtb.bottom - rtb.top;
  }

  // parse switchbar position on mdi square
  switch (swb_pos) {

    case SWB_LEFT:
      //DeferWindowPos(hdwp, sb_hwnd, NULL ,rmdi.left, rmdi.top, rsb.right - rsb.left, rmdi.bottom-rmdi.top, SWP_NOZORDER);
      tmp = DeferWindowPos(hdwp, sb_hwnd, NULL ,rmdi.left, rmdi.top, rsb.right - rsb.left, rmdi.bottom-rmdi.top, SWP_NOZORDER);
      //MoveWindow(sb_hwnd, rmdi.left, rmdi.top, rsb.right - rsb.left, rmdi.bottom-rmdi.top, false);
      rmdi.left += rsb.right - rsb.left;
      break;

    case SWB_RIGHT:
      //DeferWindowPos(hdwp, sb_hwnd, NULL, rmdi.right - (rsb.right-rsb.left), rmdi.top, rsb.right-rsb.left, rmdi.bottom-rmdi.top, SWP_NOZORDER);
      tmp = DeferWindowPos(hdwp, sb_hwnd, NULL, rmdi.right - (rsb.right-rsb.left), rmdi.top, rsb.right-rsb.left, rmdi.bottom-rmdi.top, SWP_NOZORDER);
      //MoveWindow(sb_hwnd, rmdi.right - (rsb.right - rsb.left), rmdi.top, rsb.right - rsb.left, rmdi.bottom-rmdi.top, false);
      rmdi.right -= rsb.right - rsb.left;
      break;

    case SWB_TOP:
      //DeferWindowPos(hdwp, sb_hwnd, NULL, rmdi.left, rmdi.top, rmdi.right - rmdi.left, rsb.bottom-rsb.top, SWP_NOZORDER);
      tmp = DeferWindowPos(hdwp, sb_hwnd, NULL, rmdi.left, rmdi.top, rmdi.right - rmdi.left, rsb.bottom-rsb.top, SWP_NOZORDER);
      //MoveWindow(sb_hwnd, rmdi.left, rmdi.top, rmdi.right - rmdi.left, rsb.bottom-rsb.top, false);
      rmdi.top += rsb.bottom - rsb.top;
      break;

    case SWB_BOTTOM:
      //DeferWindowPos(hdwp, sb_hwnd, NULL, rmdi.left, rmdi.bottom - (rsb.bottom-rsb.top), rmdi.right - rmdi.left, rsb.bottom-rsb.top, SWP_NOZORDER);
      tmp = DeferWindowPos(hdwp, sb_hwnd, NULL, rmdi.left, rmdi.bottom - (rsb.bottom-rsb.top), rmdi.right - rmdi.left, rsb.bottom-rsb.top, SWP_NOZORDER);
      //MoveWindow(sb_hwnd, rmdi.left, rmdi.bottom - (rsb.bottom-rsb.top), rmdi.right - rmdi.left, rsb.bottom-rsb.top, false);
      rmdi.bottom -= rsb.bottom - rsb.top;
      break;

    case SWB_NONE:
    default:
			tmp = NULL;
      break;
  }
	if (tmp != NULL) hdwp = tmp;

  HWND dhwnd;
  RECT drc;
  char text[256];
	TString d;

  int i = 0;
  while (i < nItem) {

    ListBox_GetText(lb_hwnd, i, text);

		d = text;
		dhwnd = (HWND) atol(d.gettok(1," ").to_chr());

    if (IsWindow(dhwnd)) {

      GetWindowRect(dhwnd, &drc);

			if (d.gettok(2," ") == "left") {
        //DeferWindowPos(hdwp, dhwnd, NULL, rmdi.left, rmdi.top, drc.right - drc.left, rmdi.bottom - rmdi.top, SWP_NOZORDER);
        tmp = DeferWindowPos(hdwp, dhwnd, NULL, rmdi.left, rmdi.top, drc.right - drc.left, rmdi.bottom - rmdi.top, SWP_NOZORDER);
				if (tmp != NULL) hdwp = tmp;
        //MoveWindow(dhwnd, rmdi.left, rmdi.top, drc.right - drc.left, rmdi.bottom - rmdi.top, false);
        rmdi.left += drc.right - drc.left;
      }
      else if (d.gettok(2," ") == "right") {
        //DeferWindowPos(hdwp, dhwnd, NULL, rmdi.right - (drc.right - drc.left), rmdi.top, drc.right - drc.left, rmdi.bottom - rmdi.top, SWP_NOZORDER);
        tmp = DeferWindowPos(hdwp, dhwnd, NULL, rmdi.right - (drc.right - drc.left), rmdi.top, drc.right - drc.left, rmdi.bottom - rmdi.top, SWP_NOZORDER);
				if (tmp != NULL) hdwp = tmp;
        //MoveWindow(dhwnd, rmdi.right - (drc.right - drc.left), rmdi.top, drc.right - drc.left, rmdi.bottom - rmdi.top, false);
        rmdi.right -= drc.right - drc.left;
      }
      else if (d.gettok(2," ") == "top") {
        //DeferWindowPos(hdwp, dhwnd, NULL, rmdi.left, rmdi.top, rmdi.right - rmdi.left, drc.bottom - drc.top, SWP_NOZORDER);
        tmp = DeferWindowPos(hdwp, dhwnd, NULL, rmdi.left, rmdi.top, rmdi.right - rmdi.left, drc.bottom - drc.top, SWP_NOZORDER);
				if (tmp != NULL) hdwp = tmp;
        //MoveWindow(dhwnd, rmdi.left, rmdi.top, rmdi.right - rmdi.left, drc.bottom - drc.top, false);
        rmdi.top += drc.bottom - drc.top;
      }
      else if (d.gettok(2," ") == "bottom") {
        //DeferWindowPos(hdwp, dhwnd, NULL, rmdi.left, rmdi.bottom -(drc.bottom - drc.top) , rmdi.right - rmdi.left, drc.bottom - drc.top, SWP_NOZORDER);
        tmp = DeferWindowPos(hdwp, dhwnd, NULL, rmdi.left, rmdi.bottom -(drc.bottom - drc.top) , rmdi.right - rmdi.left, drc.bottom - drc.top, SWP_NOZORDER);
				if (tmp != NULL) hdwp = tmp;
        //MoveWindow(dhwnd, rmdi.left, rmdi.bottom -(drc.bottom - drc.top) , rmdi.right - rmdi.left, drc.bottom - drc.top, false);
        rmdi.bottom -= drc.bottom - drc.top;
      }
    }
    i++;
  }

  //DeferWindowPos(hdwp, mdi_hwnd, NULL, rmdi.left, rmdi.top+1, rmdi.right - rmdi.left, rmdi.bottom - rmdi.top - 1, SWP_NOZORDER);
  tmp = DeferWindowPos(hdwp, mdi_hwnd, NULL, rmdi.left, rmdi.top+1, rmdi.right - rmdi.left, rmdi.bottom - rmdi.top - 1, SWP_NOZORDER);
	if (tmp != NULL) hdwp = tmp;
  //MoveWindow(mdi_hwnd, rmdi.left, rmdi.top+1, rmdi.right - rmdi.left, rmdi.bottom - rmdi.top - 1, true);

  EndDeferWindowPos(hdwp);
  //RedrawWindow(mIRC_hwnd, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE);
}

void AttachWindow(HWND dhwnd)
{
  RECT rcClient;
  // Window Struct to memorise Dialog size
  LPMYDCXWINDOW lpdcx = new MYDCXWINDOW;
  GetWindowRect(dhwnd,&lpdcx->rc);
  GetClientRect(dhwnd,&rcClient);
  SetWindowLong(dhwnd, GWL_USERDATA, (LONG) lpdcx);

  // Remove Style for docking purpose
	lpdcx->old_styles = GetWindowLong(dhwnd, GWL_STYLE);
	lpdcx->old_exstyles = GetWindowLong(dhwnd, GWL_EXSTYLE);
	//SetWindowLong(dhwnd,GWL_STYLE, (lpdcx->old_styles & ~(WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED)) | WS_CHILDWINDOW);
  RemStyles(dhwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
	//SetWindowLong(dhwnd,GWL_EXSTYLE, (lpdcx->old_exstyles & ~(WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY)));
  RemStyles(dhwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
  //RemStyles(dhwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
  AddStyles(dhwnd,GWL_STYLE,WS_CHILDWINDOW);

  // save parent window
	SetParent(dhwnd, mIRCLink.m_mIRCHWND);
  SetWindowPos(dhwnd, NULL, 0, 0, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, SWP_NOZORDER | SWP_NOMOVE);
}

void EjectWindow(HWND dhwnd)
{
  LPMYDCXWINDOW lpdcx = (LPMYDCXWINDOW) GetWindowLong(dhwnd, GWL_USERDATA);
	if (lpdcx == NULL) return;

  // Remove Style for undocking purpose
  RemStyles(dhwnd,GWL_STYLE,WS_BORDER);
  //WS_CHILDWINDOW |
  RemStyles(dhwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
  // Add Styles input by user
  AddStyles(dhwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);	
  AddStyles(dhwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
  RemStyles(dhwnd,GWL_STYLE,WS_CHILDWINDOW);
	//SetWindowLong(dhwnd,GWL_STYLE, lpdcx->old_styles);
	//SetWindowLong(dhwnd,GWL_EXSTYLE, lpdcx->old_exstyles);
  //SetParent(dhwnd, lpdcx->hParent);
  SetParent(dhwnd, NULL);
  SetWindowPos(dhwnd, NULL, lpdcx->rc.left, lpdcx->rc.top, lpdcx->rc.right - lpdcx->rc.left, lpdcx->rc.bottom - lpdcx->rc.top, SWP_NOZORDER);

  delete lpdcx;
  SetWindowLong(dhwnd, GWL_USERDATA, NULL);
}

int AlreadyWindow(HWND dhwnd) {

  HWND dchwnd;
  char text[256];
	TString d;
  int nItem = ListBox_GetCount(lb_hwnd);

  int i = 0;
  while (i < nItem) {

    ListBox_GetText(lb_hwnd, i, text);

		d = text;
		dchwnd = (HWND) atol(d.gettok(1," ").to_chr());
    if (dchwnd == dhwnd) return i;
    i++;
  }
  return -1;
}

// Dock N HWND POS NAME
mIRC(Dock) {
	TString d(data);
	d.trim();

  if (d.numtok(" ") > 3) {
		int pos = (int) atoi(d.gettok(1," ").to_chr());
		HWND dhwnd = (HWND) atol(d.gettok(2," ").to_chr());
    if (IsWindow(dhwnd) && AlreadyWindow(dhwnd) == -1) {
			if ((d.gettok(3," ") == "left") || (d.gettok(3," ") == "right") || (d.gettok(3," ") == "top") || (d.gettok(3," ") == "bottom")) {
        AttachWindow(dhwnd);
				ListBox_InsertString(lb_hwnd, pos, d.gettok(2,-1," ").to_chr());

        mIRC_size();
        ret("U_OK");
      }
    }
  }
  ret("U_ERROR");
}
// UnDock N
mIRC(UnDock) {
	TString d(data);
	d.trim();

	int pos = (int) atoi(d.gettok(1," ").to_chr());
  int nItem = ListBox_GetCount(lb_hwnd);

  // valid position
  if (pos >= 0 && pos < nItem) {

    char text[256];
    if (ListBox_GetText(lb_hwnd, pos, text) != 0) {
			d = text;
			HWND dhwnd = (HWND) atol(d.gettok(1," ").to_chr());
      if (IsWindow(dhwnd)) {
        EjectWindow(dhwnd);
        ListBox_DeleteString(lb_hwnd, pos);

        mIRC_size();
        ret("U_OK");
      }
    }
  }
  ret("U_ERROR");
}
// DockPos NAME
mIRC(DockPos) {
	TString d(data);
	d.trim();

	if (d.numtok(" ") > 0) {

    char text[256];
		TString tmp;
    int nItem = ListBox_GetCount(lb_hwnd);

    int i = 0;
    while (i < nItem) {

      ListBox_GetText(lb_hwnd, i, text);
			tmp = text;

			if (d.gettok(1," ") == tmp.gettok(3," ")) {
        wsprintf(data,"%d",i);
        return 3;
      }
      i++;
    }
  }
  ret("-1");
}

// DockSide NAME
mIRC(DockSide) {
	TString d(data);
	d.trim();

  if (d.numtok(" ") > 0) {

    char text[256];
		TString tmp;
    int nItem = ListBox_GetCount(lb_hwnd);

    int i = 0;
    while (i < nItem) {

      ListBox_GetText(lb_hwnd, i, text);
			tmp = text;

			if (d.gettok(1," ") == tmp.gettok(3," ")) {
				lstrcpy(data,tmp.gettok(2," ").to_chr());
        return 3;
      }
      i++;
    }
  }
  ret("-1");
}

mIRC(DockRefresh) {

  mIRC_size();
  ret("U_OK");
}

// ShowMenubar 1|0
mIRC(ShowMenubar) {
	TString d(data);
	d.trim();

  if (d.numtok(" ") > 0) {
		int show = (int) atoi(d.gettok(1," ").to_chr());

		if (show && !GetMenu(mIRCLink.m_mIRCHWND))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);
		else if(!show && GetMenu(mIRCLink.m_mIRCHWND))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);

    ret("U_OK");
  }
  ret("U_ERROR");
}

// ShowSwitchbar 1|0
mIRC(ShowSwitchbar) {
	TString d(data);
	d.trim();

  if (d.numtok(" ") > 0) {
		int show = (int) atoi(d.gettok(1," ").to_chr());

    if (show && !IsWindowVisible(sb_hwnd))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);
    else if(!show && IsWindowVisible(sb_hwnd))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);

    ret("U_OK");
  }
  ret("U_ERROR");
}

// ShowToolbar 1|0
mIRC(ShowToolbar) {
	TString d(data);
	d.trim();

  if (d.numtok(" ") > 0) {
		int show = (int) atoi(d.gettok(1," ").to_chr());

    if (show && !IsWindowVisible(tb_hwnd))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
    else if(!show && IsWindowVisible(tb_hwnd))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);

    ret("U_OK");
  }
  ret("U_ERROR");
}

// ShowTreebar 1|0
// non functional!
mIRC(ShowTreebar) {
	TString d(data);
	d.trim();

  if (d.numtok(" ") > 0) {
		int show = (int) atoi(d.gettok(1," ").to_chr());

    if (show && !IsWindowVisible(treeb_hwnd))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
    else if(!show && IsWindowVisible(treeb_hwnd))
      SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);

    ret("U_OK");
  }
  ret("U_ERROR");
}
