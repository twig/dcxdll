/*
	* Various docking commands that are not part of the old UltraDock
	*
	*	TODO: Add subclassing of channel/custom/toolbar/switchbar/treelist? SetClassLongPtr(new_wndproc) & FindWindowEx(classname)
*/

#include "../../defines.h"

extern mIRCDLL mIRCLink;
// mIRC components HWND
extern HWND treeb_hwnd, sb_hwnd, tb_hwnd, mdi_hwnd, hTreeView;
extern HFONT pOrigTreeViewFont;
extern VectorOfDocks v_docks;
extern void UltraDock(const HWND mWnd,HWND temp,TString flag);
extern bool FindUltraDock(const HWND hwnd);
extern LPDCXULTRADOCK GetUltraDock(const HWND hwnd);

typedef struct tagDCXDOCK {
	WNDPROC oldProc;
	HWND win;
	TString type;
	DWORD flags;
} DCXDOCK, *LPDCXDOCK;

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
	if (GetProp(hwnd,"dcx_docked"))
		RemoveProp(hwnd,"dcx_docked");
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
					mIRCSignalDCX("size %s %d %d %d", dd->type.to_chr(), dd->win, LOWORD(lParam), HIWORD(lParam));
				else
					mIRCSignalDCX("size Custom %d %d %d", dd->win, LOWORD(lParam), HIWORD(lParam));

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

void UnDock(const HWND hwnd)
{
	if (GetProp(hwnd,"dcx_docked") == NULL) {
		DCXError("/xdock -u","Window is not docked");
		return;
	}
  // Remove Style for undocking purpose
  RemStyles(hwnd,GWL_STYLE,WS_BORDER);
  //WS_CHILDWINDOW |
  RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE);
  // Add Styles input by user
  AddStyles(hwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);	
  AddStyles(hwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
  RemStyles(hwnd,GWL_STYLE,WS_CHILDWINDOW);
  SetParent(hwnd, NULL);
	SetWindowPos(hwnd,NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
}

bool DockWindow(const HWND mWnd, const HWND temp, const char *find, TString & flag)
{
	RECT rc;
	HWND sWnd;

	if ((FindUltraDock(temp)) || (GetProp(temp,"dcx_docked") != NULL)) {
		DCXError("/xdock","Window is Already docked.");
		return false;
	}

	// get window HWND
	if (find == NULL)
		sWnd = GetDlgItem(mWnd,32918);
	else
		sWnd = FindWindowEx(mWnd, NULL, find, NULL);

	if (IsWindow(sWnd)) {
		// change win style to child
		//RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
		RemStyles(temp,GWL_STYLE,WS_POPUP);
		AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);

		// get window pos
		GetWindowRect(temp,&rc);

		// if prop not alrdy set then set it & subclass.
		if (GetProp(sWnd,"dcx_dock") == NULL)
		{
			// subclass window.
			LPDCXDOCK dd = new DCXDOCK;

			if (SetProp(sWnd, "dcx_dock", dd)) {
				dd->oldProc = (WNDPROC)SetWindowLong(sWnd, GWL_WNDPROC, (LONG) mIRCDockWinProc);
				dd->win = mWnd;
				dd->type = find;
			}
			else {
				delete dd;
				DCXError("/xdock","Unable to SetProp");
				return false;
			}
		}

		DWORD flags = DOCKF_NORMAL;

		if (flag.len() > 1) {
			switch(flag[1])
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

		SetProp(temp,"dcx_docked",(HANDLE) flags);
		//ShowScrollBar(sWnd,SB_BOTH,FALSE);
		AddStyles(sWnd,GWL_STYLE,WS_CLIPSIBLINGS|WS_CLIPCHILDREN); // this helps with rendering glitches.
		// set parent and move it to top-left corner
		SetParent(temp,sWnd);
		MoveWindow(temp,0,0,rc.right-rc.left,rc.bottom-rc.top,1);
		EnumChildWindows(sWnd,(WNDENUMPROC)SizeDocked,NULL);
		return true;
	}
	else
		DCXError("/xdock","Unable to find destination Window");

	return false;
}

// [SWITCH] [hwnd to dock] [+options] (destination hwnd)
mIRC(xdock) {
	TString input(data);
	input.trim();
	data[0] = 0;

	int numtok = input.numtok(" ");

	if (numtok < 1) {
		DCXError("/xdock","Invalid Parameters");
		return 0;
	}

	TString switches(input.gettok(1, " "));

	// update mirc
	// /xdock -p
	if (switches[1] == 'p') {
		UpdatemIRC();
		return 1;
	}

	// check if at least 2 parameters
	if (numtok < 2) {
		DCXError("/xdock","Invalid Flag");
		return 0;
	}

	// show/hide switchbar
	// [-S] [1|0]
	if ((switches[1] == 'S') && (numtok == 2)) {
		if ((input.gettok(2, " ").to_int() > 0) && (!IsWindowVisible(sb_hwnd)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);
		else if ((input.gettok(2, " ").to_int() == 0) && (IsWindowVisible(sb_hwnd)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(112,0), 0);

		return 1;
	}
	// show/hide toolbar
	// [-T] [1|0]
	else if ((switches[1] == 'T') && (numtok == 2)) {
		if ((input.gettok(2, " ").to_int() > 0) && (!IsWindowVisible(tb_hwnd)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);
		else if ((input.gettok(2, " ").to_int() == 0) && (IsWindowVisible(tb_hwnd)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(111,0), 0);

		return 1;
	}
	// show/hide treebar
	// [-R [1|0]
	else if ((switches[1] == 'R') && (numtok == 2)) {
		if ((input.gettok(2, " ").to_int() > 0) && (!IsWindowVisible(treeb_hwnd)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(210,0), 0);
		else if ((input.gettok(2, " ").to_int() == 0) && (IsWindowVisible(treeb_hwnd)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(210,0), 0);

		return 1;
	}
	// show/hide menubar
	// [-M] [1|0]
	else if ((switches[1] == 'M') && (numtok == 2)) {
		if ((input.gettok(2, " ").to_int() > 0) && (!GetMenu(mIRCLink.m_mIRCHWND)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);
		else if ((input.gettok(2, " ").to_int() == 0) && (GetMenu(mIRCLink.m_mIRCHWND)))
			SendMessage(mIRCLink.m_mIRCHWND, WM_COMMAND, (WPARAM) MAKEWPARAM(110,0), 0);

		return 1;
	}
	// enable/disable ghost drag for main mIRC window.
	// [-G] [0-255]
	else if ((switches[1] == 'G') && (numtok == 2)) {
		int alpha = input.gettok(2," ").to_int();
		if ((alpha >= 0) && (alpha <= 255))
			mIRCLink.m_bDoGhostDrag = alpha;
		else {
			DCXError("xdock -G","Alpha Out Of Range");
			return 0;
		}
		return 1;
	}
	// set treebar font.
	// [-F] [+flag] [args]
	else if ((switches[1] == 'F') && (numtok > 2)) {
		//treeb_hwnd
		if (IsWindow(hTreeView)) {
			TString flag(input.gettok(2));
			switch(flag[1])
			{
			case 'f': // set Treebars Font: (+flags CHARSET SIZE FONTNAME)
				{
					if (numtok < 6) {
						DCXError("xdock -F","Invalid Font Args");
						return 0;
					}
					LOGFONT lf;

					if (ParseCommandToLogfont(input.gettok(3, -1, " "), &lf)) {
						HFONT hFont = CreateFontIndirect(&lf);
						if (hFont != NULL) {
							HFONT f = (HFONT)SendMessage(hTreeView,WM_GETFONT,0,0);
							if (pOrigTreeViewFont == NULL)
								pOrigTreeViewFont = f;
							SendMessage( hTreeView, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELPARAM(TRUE,0));
							if (f != pOrigTreeViewFont)
								DeleteObject(f);
						}
					}
				}
				break;
			case 'c': // set Treebars Colours: [+flags] [colour]
				{
					if (numtok < 4) {
						DCXError("xdock -F","Invalid Colour Args");
						return 0;
					}
					// TODO: undocumented. Non functional, mirc does owner draw, need to subclass
					TString cflag(input.gettok(3));
					COLORREF clr = (COLORREF)input.gettok(4).to_num();
					switch(flag[1])
					{
					case 't': // text colour
						{
							TreeView_SetTextColor(hTreeView,clr);
						}
						break;
					case 'b': // bkg colour
						{
							TreeView_SetBkColor(hTreeView,clr);
						}
						break;
					case 'l': // line colour
						{
							TreeView_SetLineColor(hTreeView,clr);
						}
						break;
					default:
						{
							DCXError("xdock -F","Invalid Colour flag");
							return 0;
						}
						break;
					}
				}
				break;
			case 's': // set Treebars Styles: [styles]
				{
					if (numtok < 3) {
						DCXError("xdock -F","Invalid Style Args");
						return 0;
					}
					static const TString treebar_styles("trackselect notrackselect tooltips notooltips infotip noinfotip hasbuttons nohasbuttons rootlines norootlines singleexpand nosingleexpand scroll noscroll");
					int i = 1, numtok = input.numtok();
					DWORD stylef = GetWindowLong(hTreeView,GWL_STYLE);
					while (i <= numtok) {
						switch (treebar_styles.findtok(input.gettok(i).to_chr(),1))
						{
						case 1: // trackselect (off by default)
							stylef |= TVS_TRACKSELECT;
							break;
						case 2: // notrackselect
							stylef &= ~TVS_TRACKSELECT;
							break;
						case 3: // tooltips (on by default)
							stylef &= ~TVS_NOTOOLTIPS;
							break;
						case 4: // notooltips
							stylef |= TVS_NOTOOLTIPS;
							break;
						case 5: // infotips (on by default)
							stylef |= TVS_INFOTIP;
							break;
						case 6: // noinfotips
							stylef &= ~TVS_INFOTIP;
							break;
						case 7: // hasbuttons (on by default)
							stylef |= TVS_HASBUTTONS;
							break;
						case 8: // nohasbuttons
							stylef &= ~TVS_HASBUTTONS;
							break;
						case 9: // rootlines (on by default)
							stylef |= TVS_LINESATROOT;
							break;
						case 10: // norootlines
							stylef &= ~TVS_LINESATROOT;
							break;
						case 11: // singleexpand (off by default)
							stylef |= TVS_SINGLEEXPAND;
							break;
						case 12: // nosingleexpand
							stylef &= ~TVS_SINGLEEXPAND;
							break;
						case 13: // scroll (off by default)
							stylef &= ~TVS_NOSCROLL;
							break;
						case 14: // noscroll (NB: this can lead to gfx glitches with scroll bars already shown)
							stylef |= TVS_NOSCROLL;
							break;
						default: // unknown style ignore.
							break;
						}
						i++;
					}
					SetWindowLong(hTreeView,GWL_STYLE, stylef);
					RedrawWindow(hTreeView, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
				}
				break;
			default:
				DCXError("xdock -F","Invalid flag");
				return 0;
			}
		}
		else
			DCXError("xdock -F","Unable to Find Treebar");
		return 1;
	}

	HWND dockHwnd = (HWND) input.gettok(2, " ").to_num();

	if (!IsWindow(dockHwnd)) {
		DCXError("/xdock","Invalid Window to dock");
		return 0;
	}

	TString flags(input.gettok(3, " "));
	if (flags[0] != '+') {
		DCXError("/xdock","No Flags Found");
		return 0;
	}

	// dock to toolbar
	// [-t] [hwnd to dock] [+options]
	if ((switches[1] == 't') && (numtok > 2)) {
		DockWindow(mWnd, dockHwnd, "mIRC_Toolbar", flags);
	}
	// dock to switchbar
	// [-s] [hwnd to dock] [+options]
	else if ((switches[1] == 's') && (numtok > 2)) {
		DockWindow(mWnd, dockHwnd, "mIRC_Switchbar", flags);
	}
	// dock to nicklist/sidelistbox
	// [-n] [hwnd to dock] [+options] [hwnd to dock with]
	else if ((switches[1] == 'n') && (numtok > 3)) {
		mWnd = (HWND) input.gettok(4," ").to_num();

		if (IsWindow(mWnd))
			DockWindow(mWnd, dockHwnd, "ListBox", flags);
		else {
			DCXError("/xdock -n","Invalid window");
			return 0;
		}
	}
	//dock to custom/channel/query/status
	// [-c] [hwnd to dock] [+options] [hwnd to dock with]
	else if ((switches[1] == 'c') && (numtok > 3)) {
		mWnd = (HWND) input.gettok(4, " ").to_num();

		if (IsWindow(mWnd))
			DockWindow(mWnd, dockHwnd, NULL, flags);
		else {
			DCXError("/xdock -c","Invalid window");
			return 0;
		}
	}
	// dock to treelist
	// [-b] [hwnd to dock] [+options]
	else if ((switches[1] == 'b') && (numtok > 2)) {
		DockWindow(mWnd, dockHwnd, "mIRC_TreeList", flags);
	}
	// dock to mIRC (UltraDock)
	// [-m] [hwnd to dock] [+options]
	else if ((switches[1] == 'm') && (numtok > 2)) {
		UltraDock(mWnd, dockHwnd, flags);
	}
	// undock
	// [-u] [hwnd to undock]
	else if (switches[1] == 'u') {
		if (FindUltraDock(dockHwnd))
			UltraUnDock(dockHwnd);
		else
			UnDock(dockHwnd);
	}
	// resize docked window
	// [-r] [hwnd to dock] [+options] [W] [H]
	else if ((switches[1] == 'r') && (numtok > 4)) {
		int w = input.gettok(4, " ").to_int();
		int h = input.gettok(5, " ").to_int();

		LPDCXULTRADOCK ud = GetUltraDock(dockHwnd);
		DWORD dflags = 0;

		if (ud != NULL)
			dflags = ud->flags;
		else
			dflags = (DWORD) GetProp(dockHwnd, "dcx_docked");

		if (dflags == NULL) {
			DCXError("/xdock -r","Unable to find flags information.");
			return 0;
		}

		RECT rc;
		GetWindowRect(dockHwnd, &rc);
		OffsetRect(&rc, -rc.left, -rc.top); // right & bottom now == width & height

		switch(dflags)
		{
			case DOCKF_LEFT:
			case DOCKF_RIGHT:
			case DOCKF_AUTOV:
				rc.right = w;
				break;

			case DOCKF_TOP:
			case DOCKF_BOTTOM:
			case DOCKF_AUTOH:
				rc.bottom = h;
				break;

			case DOCKF_NORMAL:
				rc.bottom = h;
				rc.right = w;
				break;

			case DOCKF_SIZE:
				DCXError("/xdock -r","Can't resize an auto width & height dialog");
				return 0;

			default:
				DCXError("/xdock -r","Unknown dock flag");
				return 0;
		}

		// x & y handled by mIRC update, only change width & height.
		SetWindowPos(dockHwnd, NULL, 0, 0, rc.right, rc.bottom, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOOWNERZORDER);
		UpdatemIRC();
		RedrawWindow( mIRCLink.m_mIRCHWND, NULL, NULL, RDW_INTERNALPAINT|RDW_ALLCHILDREN|RDW_INVALIDATE|RDW_ERASE );
	}
	else {
		DCXError("/xdock","Invalid Flag");
		return 0;
	}

	return 1;
}

// hwnd|mIRC prop
mIRC(_xdock)
{
	TString d(data);
	d.trim();

	data[0] = 0;

	if (d.numtok(" ") < 2) {
		TString error;
		error.sprintf("Invalid arguments (%s)", d.gettok(1, " ").to_chr());
		DCXError("$ $+ xdock",error.to_chr());

		ret("D_ERR: Invalid xdock arguments");
	}

	if (d.gettok(1," ") == "mIRC") {
		static const TString poslist("switchBarPos toolBarPos treeBarPos switchBarSize toolBarSize treeBarSize isSwitchBar isToolBar isTreeBar isMenuBar text");
		int nType = poslist.findtok(d.gettok(2).to_chr(),1);
		switch (nType)
		{
		case 1: // switchBarPos
		case 2: // toolBarPos
		case 3: // treeBarPos
			{
				switch (SwitchbarPos(nType -1))
				{
					case SWB_RIGHT:
						lstrcpy(data, "right");
						break;

					case SWB_BOTTOM:
						lstrcpy(data, "bottom");
						break;

					case SWB_TOP:
						lstrcpy(data, "top");
						break;

					case SWB_LEFT:
						lstrcpy(data, "left");
						break;

					case SWB_NONE:
					default:
						lstrcpy(data, "none");
						break;
				}
			}
			break;
		case 4: // switchBarSize
			{
				RECT rc;
				GetWindowRect(sb_hwnd, &rc);
				wsprintf(data,"%d %d", rc.right-rc.left, rc.bottom-rc.top);
			}
			break;
		case 5: // toolBarSize
			{
				RECT rc;
				GetWindowRect(tb_hwnd, &rc);
				wsprintf(data,"%d %d", rc.right-rc.left, rc.bottom-rc.top);
			}
			break;
		case 6: // treeBarSize
			{
				RECT rc;
				GetWindowRect(treeb_hwnd, &rc);
				wsprintf(data,"%d %d", rc.right-rc.left, rc.bottom-rc.top);
			}
			break;
		case 7: // isSwitchBar
			{
				if (IsWindowVisible(sb_hwnd))
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			break;
		case 8: // isToolBar
			{
				if (IsWindowVisible(tb_hwnd))
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			break;
		case 9: // isTreeBar
			{
				if (IsWindowVisible(treeb_hwnd))
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			break;
		case 10: // isMenuBar
			{
				if (GetMenu(mIRCLink.m_mIRCHWND))
					lstrcpy(data,"$true");
				else
					lstrcpy(data,"$false");
			}
			break;
		case 11: // text
			{
				if (GetWindowTextLength(mIRCLink.m_mIRCHWND) > 0)
					GetWindowText(mIRCLink.m_mIRCHWND,data,900);
			}
			break;
		case 0: // error
		default:
			{
				TString error;
				error.sprintf("Invalid prop (mIRC).%s", d.gettok(2, " ").to_chr());
				DCXError("$ $+ xdock",error.to_chr());
			}
			break;
		}
	}
	else {
		HWND hwnd = (HWND)d.gettok(1," ").to_num();

		if (IsWindow(hwnd)) {
			static const TString poslist("isDocked hasDocked isAutoV isAutoH isAutoS dockSide text");
			int nType = poslist.findtok(d.gettok(2).to_chr(),1);
			switch (nType)
			{
			case 1: // isDocked
				{
					if (GetProp(hwnd,"dcx_docked") || FindUltraDock(hwnd))
						lstrcpy(data,"$true");
					else
						lstrcpy(data,"$false");
				}
				break;
			case 2: // hasDocked
				{
					if (GetProp(hwnd,"dcx_dock"))
						lstrcpy(data,"$true");
					else
						lstrcpy(data,"$false");
				}
				break;
			case 3: // isAutoV
				{
					DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
					if (flags == DOCKF_AUTOV)
						lstrcpy(data,"$true");
					else
						lstrcpy(data,"$false");
				}
				break;
			case 4: // isAutoH
				{
					DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
					if (flags == DOCKF_AUTOH)
						lstrcpy(data,"$true");
					else
						lstrcpy(data,"$false");
				}
				break;
			case 5: // isAutoS
				{
					DWORD flags = (DWORD)GetProp(hwnd,"dcx_docked");
					if (flags == DOCKF_SIZE)
						lstrcpy(data,"$true");
					else
						lstrcpy(data,"$false");
				}
				break;
			case 6: // dockSide
				{
					LPDCXULTRADOCK ud = GetUltraDock(hwnd);
					if (ud != NULL) {
						switch(ud->flags)
						{
						case DOCKF_LEFT:
							lstrcpy(data,"left");
							break;
						case DOCKF_RIGHT:
							lstrcpy(data,"right");
							break;
						case DOCKF_TOP:
							lstrcpy(data,"top");
							break;
						case DOCKF_BOTTOM:
							lstrcpy(data,"bottom");
							break;
						default:
							lstrcpy(data,"unknown");
							break;
						}
					}
					else {
						TString error;
						error.sprintf("Window not docked to main mIRC window (%d).%s", hwnd, d.gettok(2, " ").to_chr());
						DCXError("$ $+ xdock",error.to_chr());
					}
				}
				break;
			case 7: // text
				{
					if (GetWindowTextLength(hwnd) > 0)
						GetWindowText(hwnd,data,900);
				}
				break;
			case 0: // error
			default:
				{
					TString error;
					error.sprintf("Invalid prop (%d).%s", hwnd, d.gettok(2, " ").to_chr());
					DCXError("$ $+ xdock",error.to_chr());
				}
				break;
			}
		}
		else {
			TString error;
			error.sprintf("Invalid window (%s)", d.gettok(1, " ").to_chr());
			DCXError("$ $+ xdock",error.to_chr());
		}
	}
	return 3;
}
