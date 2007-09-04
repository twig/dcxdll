
/*
	* Base Class to handle docking of windows to mIRC elements
	* & adjust the layout of these windows to make room for the docked windows.
*/

#include "dcxDock.h"

HWND DcxDock::g_StatusBar = NULL;
HIMAGELIST DcxDock::g_hImageList = NULL;
INT DcxDock::g_iDynamicParts[256] = { 0 };
INT DcxDock::g_iFixedParts[256] = { 0 };
bool DcxDock::g_bTakeOverTreebar = false;
COLORREF DcxDock::g_clrTreebarColours[8] = { 0 };

DcxDock::DcxDock(HWND refHwnd, HWND dockHwnd, int dockType)
: m_OldRefWndProc(NULL)
, m_OldDockWndProc(NULL)
, m_RefHwnd(refHwnd)
, m_hParent(dockHwnd)
, m_iType(dockType)
{
	this->m_VectorDocks.clear();
	if (IsWindow(this->m_RefHwnd)) {
		SetProp(this->m_RefHwnd,"DcxDock",this);
		this->m_OldRefWndProc = SubclassWindow(this->m_RefHwnd, DcxDock::mIRCRefWinProc);
	}
	if (IsWindow(this->m_hParent)) {
		SetProp(this->m_hParent,"DcxDock",this);
		this->m_OldDockWndProc = SubclassWindow(this->m_hParent, DcxDock::mIRCDockWinProc);
	}
	//if (dockType == DOCK_TYPE_TREE)
	//	AddStyles(this->m_RefHwnd,GWL_EXSTYLE,WS_EX_TRANSPARENT);
}

DcxDock::~DcxDock(void)
{
	// UnDock all windows.
	VectorOfDocks::iterator itStart = this->m_VectorDocks.begin();
	VectorOfDocks::iterator itEnd = this->m_VectorDocks.end();

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
	this->m_VectorDocks.clear();

	// reset to orig WndProc
	if (IsWindow(this->m_RefHwnd)) {
		RemoveProp(this->m_RefHwnd,"DcxDock");
		if (this->m_OldRefWndProc != NULL)
			SubclassWindow(this->m_RefHwnd, this->m_OldRefWndProc);
	}
	if (IsWindow(this->m_hParent)) {
		RemoveProp(this->m_hParent,"DcxDock");
		if (this->m_OldDockWndProc != NULL)
			SubclassWindow(this->m_hParent, this->m_OldDockWndProc);
	}
	this->UpdateLayout();
}

bool DcxDock::DockWindow(HWND hwnd, const TString &flag)
{
	if (isDocked(hwnd)) {
		mIRCDebug("D_ERROR Window (%d) is already docked", hwnd);
		return false;
	}
	if (!IsWindow(this->m_hParent)) {
		mIRCError("D_ERROR Invalid Dock Host Window");
		return false;
	}
	LPDCXULTRADOCK ud = new DCXULTRADOCK;

	if (ud == NULL) {
		mIRCError("D_ERROR No Memory");
		return false;
	}
	ud->hwnd = hwnd;
	ud->old_exstyles = GetWindowExStyle(hwnd);
	ud->old_styles = GetWindowStyle(hwnd);
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

	this->m_VectorDocks.push_back(ud);
	RemStyles(hwnd,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
	RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	//RemStyles(hwnd,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
	AddStyles(hwnd,GWL_STYLE,WS_CHILDWINDOW);
	SetParent(hwnd, this->m_hParent);
	this->UpdateLayout();
	return true;
}

void DcxDock::UnDockWindow(HWND hwnd)
{
	VectorOfDocks::iterator itStart = this->m_VectorDocks.begin();
	VectorOfDocks::iterator itEnd = this->m_VectorDocks.end();

	while (itStart != itEnd) {
		if (*itStart != NULL) {
			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
			if (ud->hwnd == hwnd) {
				this->m_VectorDocks.erase(itStart);
				SetWindowLong(ud->hwnd,GWL_STYLE, ud->old_styles);
				SetWindowLong(ud->hwnd,GWL_EXSTYLE, ud->old_exstyles);
			  RemStyles(ud->hwnd,GWL_STYLE,WS_CHILDWINDOW);
				SetParent(ud->hwnd, NULL);
				SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED);
				delete ud;
				this->UpdateLayout();
				return;
			}
		}
		itStart++;
	}
}

bool DcxDock::FindDock(const HWND hwnd)
{
	VectorOfDocks::iterator itStart = this->m_VectorDocks.begin();
	VectorOfDocks::iterator itEnd = this->m_VectorDocks.end();

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

LPDCXULTRADOCK DcxDock::GetDock(const HWND hwnd)
{
	VectorOfDocks::iterator itStart = this->m_VectorDocks.begin();
	VectorOfDocks::iterator itEnd = this->m_VectorDocks.end();

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

bool DcxDock::isDocked(const HWND hwnd)
{
	if (this->FindDock(hwnd) || (GetProp(hwnd,"dcx_docked") != NULL))
		return true;
	return false;
}

void DcxDock::AdjustRect(WINDOWPOS *wp)
{
	if ((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE)) // handle min/max case;
		return;

	VectorOfDocks::iterator itStart = this->m_VectorDocks.begin();
	VectorOfDocks::iterator itEnd = this->m_VectorDocks.end();
	RECT rcDocked;
	int xleftoffset = 0, xrightoffset = 0, ytopoffset = 0, ybottomoffset = 0;
	int x,y,w,h,refw,refh,nWin = 0; //nWin = this->m_VectorDocks.size();

	// count visible docked windows.
	while (itStart != itEnd) {
		if ((*itStart != NULL) && (IsWindowVisible(((LPDCXULTRADOCK)*itStart)->hwnd))) {
			nWin++; // count docked windows.
		}
		itStart++;
	}
	if (nWin == 0) return;
	itStart = this->m_VectorDocks.begin();
	refh = wp->cy;
	refw = wp->cx;

	ytopoffset = wp->y;
	ybottomoffset = wp->y + refh;
	xleftoffset = wp->x;
	xrightoffset = wp->x + refw;

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
				tmp = DeferWindowPos(hdwp,ud->hwnd,NULL,x,y,w,h,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
				if (tmp != NULL) hdwp = tmp;
			}
		}

		itStart++;
	}
	wp->x = xleftoffset;
	wp->y = ytopoffset;
	wp->cx = refw;
	wp->cy = refh;
	EndDeferWindowPos(hdwp);
}

LRESULT CALLBACK DcxDock::mIRCRefWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DcxDock *pthis = (DcxDock *)GetProp(mHwnd,"DcxDock");
	if (pthis == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg) {
		case WM_WINDOWPOSCHANGING:
			{
				if (lParam != NULL) {
					WINDOWPOS * wp = (WINDOWPOS *) lParam;
					if (((pthis->m_iType == DOCK_TYPE_MDI) || (pthis->m_iType == DOCK_TYPE_TREE)) && DcxDock::IsStatusbar()) {
						RECT rc;
						DcxDock::status_getRect(&rc);
						wp->cy -= (rc.bottom - rc.top);
					}
					pthis->AdjustRect(wp);
				}
			}
			break;
		//case WM_PAINT:
		//	{
		//		if (pthis->m_iType == DOCK_TYPE_TREE) {
		//			PAINTSTRUCT ps;
		//			RECT rcClient, rcParent;
		//			HDC hdc, *hBuffer;

		//			// get treeviews rect
		//			GetClientRect(mHwnd,&rcClient);
		//			GetClientRect(mIRCLink.m_hTreebar, &rcParent);

		//			hdc = BeginPaint(mHwnd, &ps);

		//			hBuffer = CreateHDCBuffer(hdc, &rcParent);

		//			SendMessage(mIRCLink.m_hTreebar, WM_PRINT, (WPARAM)*hBuffer, PRF_CLIENT);

		//			CopyRect(&rcParent, &rcClient);
		//			MapWindowRect(mHwnd, mIRCLink.m_hTreebar, &rcParent);

		//			BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), *hBuffer, rcParent.left, rcParent.top, SRCCOPY);

		//			DeleteHDCBuffer(hBuffer);

		//			LRESULT lRes = CallWindowProc(pthis->m_OldRefWndProc, mHwnd, uMsg, (WPARAM)hdc, lParam);

		//			EndPaint(mHwnd, &ps);
		//			return lRes;
		//		}
		//	}
		//	break;
		case WM_ERASEBKGND:
			{
				if ( pthis->m_iType == DOCK_TYPE_TREE && DcxDock::g_bTakeOverTreebar)
					return FALSE; //DefWindowProc(mHwnd, uMsg, wParam, lParam);
			}
			break;
#ifndef NDEBUG
		case TVM_SETITEM:
			{
				if ( pthis->m_iType != DOCK_TYPE_TREE || !DcxDock::g_bTakeOverTreebar)
					break;
				LPTVITEMEX pitem = (LPTVITEMEX)lParam;
				mIRCevalEX(NULL,0,"$xtreebar_callback(setitem,%ld,%ld)", pitem->hItem, pitem->lParam);
			}
			break;
#endif
		case TVM_INSERTITEM:
			{
				if (!DcxDock::g_bTakeOverTreebar)
					break;

				LPTVINSERTSTRUCT pTvis = (LPTVINSERTSTRUCT)lParam;
				if (pTvis->itemex.mask & TVIF_TEXT) {
					TString buf((UINT)64);
					int i = 0, wid = HIWORD(pTvis->itemex.lParam);
					mIRCevalEX(buf.to_chr(), 64, "$window(@%d).type", wid);
					if (buf.len() < 1)
						buf = "notify";
					mIRCevalEX(buf.to_chr(), 16, "$xtreebar_callback(geticons,%s,%800s)", buf.to_chr(), pTvis->itemex.pszText);
					i = buf.gettok( 1 ).to_int() -1;
					if (i < 0)
						i = 0;
					pTvis->itemex.iImage = i;
					i = buf.gettok( 2 ).to_int() -1;
					if (i < 0)
						i = 0;
					pTvis->itemex.iSelectedImage = i;
					pTvis->itemex.mask |= TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				}
			}
			break;
	}
	return CallWindowProc(pthis->m_OldRefWndProc, mHwnd, uMsg, wParam, lParam);
}
//BOOL CALLBACK DcxDock::EnumTreebarWindows(HWND hwnd, LPARAM lParam)
//{
//	TCHAR title[256];
//	char *buf = (char *)lParam;
//	title[0] = 0;
//	GetWindowText(hwnd, title, 255);
//	if (lstrcmp(buf, title) == 0) {
//		mIRCDebug("match: %ld : %s", hwnd, title);
//		return FALSE;
//	}
//	return TRUE;
//}

LRESULT CALLBACK DcxDock::mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DcxDock *pthis = (DcxDock *)GetProp(mHwnd,"DcxDock");
	if (pthis == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg) {
		case WM_SIZE:
			{
				if ((pthis->m_iType == DOCK_TYPE_MDI) && DcxDock::IsStatusbar()) { // parent of MDI type == main mIRC win.
					InvalidateRect(DcxDock::g_StatusBar, NULL, FALSE); // needed to stop display corruption
					SendMessage(DcxDock::g_StatusBar,WM_SIZE, (WPARAM)0, (LPARAM)0);
					DcxDock::status_updateParts();
				}
			}
			break;

		case WM_WINDOWPOSCHANGING:
			{
				if ((lParam != NULL) && (pthis->m_iType != DOCK_TYPE_MDI) && DcxDock::IsStatusbar()) {
					WINDOWPOS * wp = (WINDOWPOS *) lParam;
					if ((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE))
						break;
					int pos = DcxDock::getPos(wp->x, wp->y, wp->cx, wp->cy);
					if (pos == 3) // if at top then ignore it.
						break;
					RECT rc;
					DcxDock::status_getRect(&rc);
					if (pos == 4) // if at bottom move it up.
						wp->y -= (rc.bottom - rc.top);
					else
						wp->cy -= (rc.bottom - rc.top);
				}
			}
			break;
		case WM_NOTIFY:
			{
				if (pthis->m_iType == DOCK_TYPE_TREE) {
					LPNMHDR hdr = (LPNMHDR) lParam;

					if (!hdr || !DcxDock::g_bTakeOverTreebar)
						break;

					switch( hdr->code ) {
						case NM_CUSTOMDRAW:
							{
								LPNMTVCUSTOMDRAW lpntvcd = (LPNMTVCUSTOMDRAW) lParam;
								switch (lpntvcd->nmcd.dwDrawStage) {
									case CDDS_PREPAINT:
										return CDRF_NOTIFYITEMDRAW;
									case CDDS_ITEMPREPAINT:
										{
											if (lpntvcd->nmcd.uItemState & CDIS_SELECTED) { // This makes sure the selected colour doesnt show as grayed.
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
												int wid = HIWORD(lpntvcd->nmcd.lItemlParam);
												TString buf((UINT)64);
												mIRCevalEX(buf.to_chr(), 64, "$window(@%d).sbcolor", wid);
												if (buf.len() > 0) {
													static const TString sbcolor("s s message s event s highlight"); // 's' is used as a spacer.
													int clr = sbcolor.findtok(buf.to_chr(), 1);
													if (clr == 0) // no match, do normal colours
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
							LPNMTVGETINFOTIP tcgit = (LPNMTVGETINFOTIP) lParam;
							if (tcgit != NULL) {
								if (tcgit->cchTextMax < 1)
									break;

								int wid = HIWORD(tcgit->lParam);
								TString tsType((UINT)64);
								TString buf((UINT)255);
								TVITEMEX item;
								ZeroMemory(&item, sizeof(item));

								item.hItem = tcgit->hItem;
								item.pszText = buf.to_chr();
								item.cchTextMax = 255;
								item.mask = TVIF_TEXT;
								if (TreeView_GetItem(mIRCLink.m_hTreeView, &item)) {
									mIRCevalEX(tsType.to_chr(), 64, "$window(@%d).type", wid);
									if (tsType.len() < 1)
										tsType = "notify";
									mIRCevalEX(buf.to_chr(), 255, "$xtreebar_callback(gettooltip,%s,%800s)", tsType.to_chr(), item.pszText);

									if (buf.len() > 0)
										lstrcpyn(tcgit->pszText, buf.to_chr(), tcgit->cchTextMax);
								}
							}
							return 0L;
						}
						break;
					}
				}
				else if ((pthis->m_iType == DOCK_TYPE_MDI) && DcxDock::IsStatusbar()) {
					LPNMHDR hdr = (LPNMHDR) lParam;

					if (!hdr)
						break;

					if (hdr->hwndFrom != DcxDock::g_StatusBar)
						break;

					int idPart = ((LPNMMOUSE)hdr)->dwItemSpec +1;

					switch( hdr->code ) {
						case NM_CLICK:
							{
								mIRCSignalDCX(dcxSignal.xstatusbar, "DCXStatusbar sclick %d %d", hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_DBLCLK:
							{
								mIRCSignalDCX(dcxSignal.xstatusbar, "DCXStatusbar dclick %d %d", hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_RCLICK:
							{
								mIRCSignalDCX(dcxSignal.xstatusbar, "DCXStatusbar rclick %d %d", hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_RDBLCLK:
							{
								mIRCSignalDCX(dcxSignal.xstatusbar, "DCXStatusbar rdclick %d %d", hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
					}
				}
			}
			break;
		case WM_PARENTNOTIFY:
			{
				if (LOWORD(wParam) == WM_DESTROY)
					pthis->UnDockWindow((HWND)lParam);
			}
			break;
		case WM_DESTROY:
		{
			delete pthis;
			PostMessage(mHwnd, uMsg, 0, 0);
			return 0L;
			break;
		}
	}
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

	g_StatusBar = CreateWindowEx(
		ExStyles,
		STATUSCLASSNAME,NULL,
		Styles,
		0,0,0,0,mIRCLink.m_mIRCHWND,(HMENU)(mIRC_ID_OFFSET-1),NULL,NULL);

	if ( bNoTheme )
		dcxSetWindowTheme( g_StatusBar , L" ", L" " );

	if (IsWindow(g_StatusBar))
		return true;
	return false;
}

void DcxDock::UnInitStatusbar(void)
{
	if (IsWindow(g_StatusBar)) {
		DcxDock::status_cleanPartIcons();
		DestroyWindow(g_StatusBar);
	}
	g_StatusBar = NULL;

	if (g_hImageList != NULL)
		ImageList_Destroy( g_hImageList );
	g_hImageList = NULL;
}
bool DcxDock::IsStatusbar(void)
{
	if (IsWindow(g_StatusBar))
		return true;
	return false;
}

void DcxDock::status_parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	unsigned int i = 1, numtok = styles.numtok( );
	*Styles = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	while ( i <= numtok ) {

		if ( styles.gettok( i ) == "grip" )
			*Styles |= SBARS_SIZEGRIP;
		else if ( styles.gettok( i ) == "tooltips" )
			*Styles |= SBARS_TOOLTIPS;
		else if ( styles.gettok( i ) == "nodivider" )
			*Styles |= CCS_NODIVIDER;
		else if ( styles.gettok( i ) == "notheme" )
			*bNoTheme = TRUE;
		else if ( styles.gettok( i ) == "disabled" )
			*Styles |= WS_DISABLED;
		else if ( styles.gettok( i ) == "transparent" )
			*ExStyles |= WS_EX_TRANSPARENT;
		i++;
	}
}

void DcxDock::status_getRect(LPRECT rc) {
	GetWindowRect(g_StatusBar,rc);
}

void DcxDock::status_setBkColor( const COLORREF clrBk ) {
	SendMessage( g_StatusBar, SB_SETBKCOLOR, (WPARAM) 0, (LPARAM) clrBk );
}

void DcxDock::status_setParts( const int nParts, const LPINT aWidths ) {
	SendMessage( g_StatusBar, SB_SETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

LRESULT DcxDock::status_getParts( const int nParts, LPINT aWidths ) {
	return SendMessage( g_StatusBar, SB_GETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

void DcxDock::status_setText( const int iPart, const int Style, const LPSTR lpstr ) {
	SendMessage( g_StatusBar, SB_SETTEXT, (WPARAM) iPart | Style, (LPARAM) lpstr );
}

UINT DcxDock::status_getTextLength( const int iPart ) {
	return (UINT)LOWORD(SendMessage( g_StatusBar, SB_GETTEXTLENGTH, (WPARAM) iPart, NULL ));
}

LRESULT DcxDock::status_getText( const int iPart, LPSTR lpstr ) {
	return SendMessage( g_StatusBar, SB_GETTEXT, (WPARAM) iPart, (LPARAM) lpstr );
}

void DcxDock::status_setTipText( const int iPart, const LPSTR lpstr ) {
	SendMessage( g_StatusBar, SB_SETTIPTEXT, (WPARAM) iPart, (LPARAM) lpstr );
}

void DcxDock::status_getTipText( const int iPart, const int nSize, LPSTR lpstr ) {
	SendMessage( g_StatusBar, SB_GETTIPTEXT, (WPARAM) MAKEWPARAM (iPart, nSize), (LPARAM) lpstr );
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

HIMAGELIST DcxDock::status_getImageList( ) {

	return g_hImageList;
}

void DcxDock::status_setImageList( HIMAGELIST himl ) {

	g_hImageList = himl;
}

HIMAGELIST DcxDock::status_createImageList( ) {

	return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

UINT DcxDock::status_parseItemFlags( TString & flags ) {

	INT i = 1, len = flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {
		switch(flags[i])
		{
		case 'n':
			iFlags |= SBT_NOBORDERS;
			break;
		case 'p':
			iFlags |= SBT_POPOUT;
			break;
		}

		++i;
	}
	return iFlags;
}

void DcxDock::status_cleanPartIcons( ) {

	int n = 0;
	while ( n < 256 ) {
		DestroyIcon( (HICON) DcxDock::status_getIcon( n ) );
		n++;
	}
}

LRESULT DcxDock::status_getBorders( LPINT aWidths ) {
  return SendMessage( g_StatusBar, SB_GETBORDERS, (WPARAM) 0, (LPARAM) aWidths );
}

void DcxDock::status_updateParts(void) {
	int nParts = DcxDock::status_getParts(0,NULL);

	if (nParts <= 0)
		return;

	RECT rcClient;
	int *pParts = new int[nParts];
	//int borders[3];
	int w, pw = 0;

	GetClientRect(DcxDock::g_StatusBar, &rcClient);
	DcxDock::status_getParts(nParts, pParts);

	//DcxDock::status_getBorders(borders);

	w = (rcClient.right - rcClient.left) / 100; // - (2 * borders[1]);

	for (int i = 0; i < nParts; i++) {
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

	DcxDock::status_setParts(nParts, pParts);
	delete [] pParts;
}

int DcxDock::getPos(int x, int y, int w, int h)
{
	RECT rc;
	GetClientRect(mIRCLink.m_mIRCHWND,&rc);
	if (x == rc.left && (y + h) == rc.bottom && (x + w) != rc.right)
		return SWB_LEFT;
	if (x == rc.left && (y + h) != rc.bottom && (x + w) == rc.right)
		return SWB_TOP;
	if (x == rc.left && (y + h) == rc.bottom && (x + w) == rc.right)
		return SWB_BOTTOM;
	return SWB_RIGHT;
}
