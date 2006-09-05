/*
	* Various docking commands that are not part of the old UltraDock
	*
	*	TODO: Add subclassing of channel/custom/toolbar/switchbar/treelist? SetClassLongPtr(new_wndproc) & FindWindowEx(classname)
*/

#include "../../defines.h"

extern mIRCDLL mIRCLink;

typedef struct tagDCXDOCK {
	WNDPROC oldProc;
} DCXDOCK, *LPDCXDOCK;

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

LRESULT CALLBACK mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPDCXDOCK dd = (LPDCXDOCK)GetProp(mHwnd,"dcx_dock");
	if (dd == NULL)
		return DefWindowProc(mHwnd,uMsg, wParam, lParam);
	switch (uMsg) {
    case WM_SIZE:
      {
				wsprintf(mIRCLink.m_pData, "/.signal DCX size %d %d %d", mHwnd, LOWORD(lParam), HIWORD(lParam));
				SendMessage(mIRCLink.m_mIRCHWND, WM_USER +200, 0, mIRCLink.m_map_cnt);
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
void DockWindow(HWND mWnd,char *data,HWND temp,char *find)
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
				}
				else {
					delete dd;
					lstrcpy(data,"-ERR Unable to SetProp");
					return;
				}
			}
			// set parent and move it to top-left corner
			SetParent(temp,sWnd);
			MoveWindow(temp,0,0,rc.right-rc.left,rc.bottom-rc.top,1);

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
// +tsncb <hwnd to dock> [hwnd to dock too]
mIRC(xdock)
{
	TString d(data);
	d.trim();
	TString flag = d.gettok(1," ");
	if ((flag.len() != 2) || (flag[0] != '+')) {
		lstrcpy(data,"-ERR Invalid Flag");
		return 3;
	}
	switch (flag[1])
	{
	case 't':
		{
			DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"mIRC_Toolbar");
		}
		break;
	case 's':
		{
			DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"mIRC_Switchbar");
		}
		break;
	case 'n':
		{
			if (d.numtok(" ") == 3) {
				mWnd = (HWND)atol(d.gettok(3," ").to_chr());
				DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"ListBox");
			}
			else lstrcpy(data,"-ERR Invalid Args");
		}
		break;
	case 'c':
		{
			if (d.numtok(" ") == 3) {
				mWnd = (HWND)atol(d.gettok(3," ").to_chr());
				DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),NULL);
			}
			else lstrcpy(data,"-ERR Invalid Args");
		}
		break;
	case 'b':
		{
			DockWindow(mWnd,data,(HWND)atol(d.gettok(2," ").to_chr()),"mIRC_TreeList");
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
