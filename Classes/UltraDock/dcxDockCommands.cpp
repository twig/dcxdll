/*
	* Various docking commands that are not part of the old UltraDock
	*
	*	TODO: Add subclassing of channel/custom/toolbar/switchbar/treelist? SetClassLongPtr(new_wndproc) & FindWindowEx(classname)
*/

#include "../../defines.h"

extern mIRCDLL mIRCLink;
// mIRC components HWND
extern HWND treeb_hwnd, sb_hwnd, tb_hwnd, mdi_hwnd, lb_hwnd;

typedef struct tagDCXDOCK {
	WNDPROC oldProc;
	HWND win;
	TString type;
	DWORD flags;
} DCXDOCK, *LPDCXDOCK;

#define DOCKF_NORMAL	0x01	//!< No special flags.
#define DOCKF_AUTOH		0x02	//!< Auto Horizontal size.
#define DOCKF_AUTOV		0x04	//!< Auto Vertical size.
#define DOCKF_SIZE		0x08	//!< Auto Horizontal & Vertical size.

typedef struct tagDCXDOCSIZE {
	DWORD	width;
	DWORD height;
} DCXDOCKSIZE, *LPDCXDOCKSIZE;

BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam)
{
	LPDCXDOCK dd = (LPDCXDOCK)GetProp(hwnd,"dcx_dock");
	if (dd != NULL) {
		RemoveProp(hwnd,"dcx_dock");
		SetWindowLong(hwnd, GWL_WNDPROC, (LONG)dd->oldProc);
		delete dd;
	}
	return TRUE;
}
BOOL CALLBACK SizeDocked(HWND hwnd,LPARAM lParam)
{
	DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
	if (flags && flags != DOCKF_NORMAL) {
		RECT rcParent, rcThis;
		GetClientRect(GetParent(hwnd),&rcParent);
		GetWindowRect(hwnd,&rcThis);
		if (flags & DOCKF_SIZE) 
			SetWindowPos(hwnd,NULL,0,0,(rcParent.right - rcParent.left),(rcParent.bottom - rcParent.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
		else if (flags & DOCKF_AUTOH)
			SetWindowPos(hwnd,NULL,0,0,(rcParent.right - rcParent.left),(rcThis.bottom - rcThis.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
		else if (flags & DOCKF_AUTOV)
			SetWindowPos(hwnd,NULL,0,0,(rcThis.right - rcThis.left),(rcParent.bottom - rcParent.top),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOMOVE);
	}
	return TRUE;
}

LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPDCXDOCK dd = (LPDCXDOCK)GetProp(mHwnd,"dcx_dock");
	if (dd == NULL)
		return DefWindowProc(mHwnd,uMsg, wParam, lParam);
	switch (uMsg) {
    case WM_SIZE:
      {
				if (dd->type.len())
					wsprintf(mIRCLink.m_pData, "/.signal DCX size %s %d %d %d", dd->type.to_chr(), dd->win, LOWORD(lParam), HIWORD(lParam));
				else
					wsprintf(mIRCLink.m_pData, "/.signal DCX size Custom %d %d %d", dd->win, LOWORD(lParam), HIWORD(lParam));
				SendMessage(mIRCLink.m_mIRCHWND, WM_USER +200, 0, mIRCLink.m_map_cnt);
				EnumChildWindows(mHwnd,(WNDENUMPROC)SizeDocked,NULL);
      }
      break;
		case WM_PARENTNOTIFY:
			{
				if (LOWORD(wParam) == WM_DESTROY)
					RemoveProp((HWND)lParam,"dcx_docked");
			}
			break;
		case WM_DESTROY:
			{
				RemoveProp(mHwnd,"dcx_dock");
				SetWindowLong(mHwnd, GWL_WNDPROC, (LONG)dd->oldProc);
				delete dd;
				PostMessage(mHwnd, uMsg, 0, 0);
				return 0L;
			}
			break;
	}
	return CallWindowProc(dd->oldProc,mHwnd,uMsg,wParam,lParam);
}
void DockWindow(HWND mWnd,char *data,HWND temp,char *find, TString flag)
{
	RECT rc;
	HWND sWnd;

	// if HWND exists
	if (IsWindow(temp)) {
		// get window HWND
		if (find == NULL) sWnd = GetDlgItem(mWnd,32918);
		else sWnd = FindWindowEx(mWnd,NULL,find,NULL);
		if (IsWindow(sWnd)) {
			// change win style to child
			//RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
			RemStyles(temp,GWL_STYLE,WS_POPUP);
			AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);

			// get window pos
			GetWindowRect(temp,&rc);

			if (GetProp(sWnd,"dcx_dock") == NULL)
			{ // if prop not alrdy set then set it & subclass.
				// subclass window.
				LPDCXDOCK dd = new DCXDOCK;
				if (SetProp(sWnd,"dcx_dock",dd))
				{
					dd->oldProc = (WNDPROC)SetWindowLong(sWnd,GWL_WNDPROC, (LONG)mIRCDockWinProc);
					dd->win = mWnd;
					dd->type = find;
				}
				else {
					delete dd;
					lstrcpy(data,"-ERR Unable to SetProp");
					return;
				}
			}
			DWORD flags = DOCKF_NORMAL;
			if (flag.len() == 3) {
				switch(flag[2])
				{
				case 's':
					flags = DOCKF_SIZE;
					break;
				case 'h':
					flags = DOCKF_AUTOH;
					break;
				case 'v':
					flags = DOCKF_AUTOV;
					break;
				default:
					flags = DOCKF_NORMAL;
					break;
				}
			}
			SetProp(temp,"dcx_docked",(HANDLE)flags);
			// set parent and move it to top-left corner
			SetParent(temp,sWnd);
			MoveWindow(temp,0,0,rc.right-rc.left,rc.bottom-rc.top,1);
			EnumChildWindows(sWnd,(WNDENUMPROC)SizeDocked,NULL);

			// return OK
			lstrcpy(data,"+OK Window Docked");
		}
		else
			lstrcpy(data,"-ERR Unable to find Window");
	}
	else {
		// if HWND invalid, return an error msg
		lstrcpy(data,"-ERR Invalid window");
	}
}
// +tsncb[hvs] <hwnd to dock> [hwnd to dock too]
mIRC(xdock)
{
	TString d(data);
	d.trim();
	TString flag = d.gettok(1," ");
	if ((flag.len() < 2) || (flag.len() > 3) || (flag[0] != '+')) {
		lstrcpy(data,"-ERR Invalid Flag");
		return 3;
	}
	switch (flag[1])
	{
	case 't':
		{
			DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"mIRC_Toolbar",flag);
		}
		break;
	case 's':
		{
			DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"mIRC_Switchbar",flag);
		}
		break;
	case 'n':
		{
			if (d.numtok(" ") == 3) {
				mWnd = (HWND)atol(d.gettok(3," ").to_chr());
				DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"ListBox",flag);
			}
			else lstrcpy(data,"-ERR Invalid Args");
		}
		break;
	case 'c':
		{
			if (d.numtok(" ") == 3) {
				mWnd = (HWND)atol(d.gettok(3," ").to_chr());
				DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),NULL,flag);
			}
			else lstrcpy(data,"-ERR Invalid Args");
		}
		break;
	case 'b':
		{
			DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"mIRC_TreeList",flag);
		}
		break;
	default:
		{
			lstrcpy(data,"-ERR Invalid Flag");
		}
		break;
	}
	return 3;
}
// hwnd|mIRC prop
mIRC(_xdock)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok(" ") < 2) {
		TString error;
		error.sprintf("$ $+ xdock invalid arguments (%s)", d.gettok(1, " ").to_chr());
		mIRCError(error.to_chr());
		return 3;
	}
	if (d.gettok(1," ") == "mIRC") {
		if (d.gettok(2," ") == "SBPos") {
			wsprintf(data,"%d",SwitchbarPos());
		}
		else if (d.gettok(2," ") == "SBSize") {
			RECT rc;
			GetWindowRect(sb_hwnd, &rc);
			wsprintf(data,"%d %d", rc.right-rc.left, rc.bottom-rc.top);
		}
		else if (d.gettok(2," ") == "TBSize") {
			RECT rc;
			GetWindowRect(tb_hwnd, &rc);
			wsprintf(data,"%d %d", rc.right-rc.left, rc.bottom-rc.top);
		}
		else if (d.gettok(2," ") == "TLSize") {
			RECT rc;
			GetWindowRect(treeb_hwnd, &rc);
			wsprintf(data,"%d %d", rc.right-rc.left, rc.bottom-rc.top);
		}
		else if (d.gettok(2," ") == "IsSwitchbar") {
			if (IsWindowVisible(sb_hwnd))
				lstrcpy(data,"$true");
			else
				lstrcpy(data,"$false");
		}
		else if (d.gettok(2," ") == "IsToolbar") {
			if (IsWindowVisible(tb_hwnd))
				lstrcpy(data,"$true");
			else
				lstrcpy(data,"$false");
		}
		else if (d.gettok(2," ") == "IsTreebar") {
			if (IsWindowVisible(treeb_hwnd))
				lstrcpy(data,"$true");
			else
				lstrcpy(data,"$false");
		}
		else if (d.gettok(2," ") == "IsMenubar") {
			if (GetMenu(mIRCLink.m_mIRCHWND))
				lstrcpy(data,"$true");
			else
				lstrcpy(data,"$false");
		}
		else {
			TString error;
			error.sprintf("$ $+ xdock invalid prop (mIRC).%s", d.gettok(2, " ").to_chr());
			mIRCError(error.to_chr());
		}
	}
	else {
		HWND hwnd = (HWND)atol(d.gettok(1," ").to_chr());
		if (IsWindow(hwnd)) {
			if (d.gettok(2," ") == "IsDocked") {
				if (GetProp(hwnd,"dcx_docked"))
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			else if (d.gettok(2," ") == "HasDocked") {
				if (GetProp(hwnd,"dcx_dock"))
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			else if (d.gettok(2," ") == "IsAutoV") {
				DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
				if (flags & DOCKF_AUTOV)
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			else if (d.gettok(2," ") == "IsAutoH") {
				DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
				if (flags & DOCKF_AUTOH)
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			else if (d.gettok(2," ") == "IsAutoS") {
				DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
				if (flags & DOCKF_SIZE)
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
		}
		else {
			TString error;
			error.sprintf("$ $+ xdock invalid window (%s)", d.gettok(1, " ").to_chr());
			mIRCError(error.to_chr());
		}
	}
	return 3;
}
