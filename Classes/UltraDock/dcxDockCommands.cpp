/*
	* Various docking commands that are not part of the old UltraDock
*/

#include "../../defines.h"

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
			RemStyles(temp,GWL_STYLE,WS_POPUP);
			AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);

			// get window pos
			GetWindowRect(temp,&rc);

			// set parent and move it to top-left corner
			SetParent(temp,sWnd);
			MoveWindow(temp,0,0,rc.right-rc.left,rc.bottom-rc.top,1);

			// return OK
			lstrcpy(data,"+OK Window Docked");
		}
		else {
			lstrcpy(data,"-ERR Unable to find Window");
		}
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
				mWnd = (HWND)atol(d.gettok(2," ").to_chr());
				DockWindow(mWnd,data,(HWND)atol(d.gettok(3," ").to_chr()),"ListBox");
			}
			else lstrcpy(data,"-ERR Invalid Args");
		}
		break;
	case 'c':
		{
			if (d.numtok(" ") == 3) {
				mWnd = (HWND)atol(d.gettok(2," ").to_chr());
				DockWindow(mWnd,data,(HWND)atol(d.gettok(3," ").to_chr()),NULL);
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
