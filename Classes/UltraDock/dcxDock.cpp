
/*
	* Base Class to handle docking of windows to mIRC elements
	* & adjust the layout of these windows to make room for the docked windows.
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"



// statusbar stuff
HWND DcxDock::g_StatusBar = NULL;
HIMAGELIST DcxDock::g_hImageList = NULL;
INT DcxDock::g_iDynamicParts[256] = { 0 };
INT DcxDock::g_iFixedParts[256] = { 0 };
HFONT DcxDock::g_StatusFont = NULL;
VectorOfParts DcxDock::g_vParts;

// treebar stuff
bool DcxDock::g_bTakeOverTreebar = false;
COLORREF DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_MAX +1] = { CLR_INVALID };

DcxDock::DcxDock(HWND refHwnd, HWND dockHwnd, const int dockType)
: m_OldRefWndProc(NULL)
, m_OldDockWndProc(NULL)
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
		if (this->m_OldRefWndProc != NULL)
			SubclassWindow(this->m_RefHwnd, this->m_OldRefWndProc);
	}
	if (IsWindow(this->m_hParent)) {
		RemoveProp(this->m_hParent,TEXT("DcxDock"));
		if (this->m_OldDockWndProc != NULL)
			SubclassWindow(this->m_hParent, this->m_OldDockWndProc);
	}
	this->UpdateLayout();
}

bool DcxDock::DockWindow(HWND hwnd, const TString &flag)
{
	if (isDocked(hwnd)) {
		Dcx::errorex(TEXT("xdock"), TEXT("Window (%d) is already docked"), hwnd);
		return false;
	}
	if (!IsWindow(this->m_hParent)) {
		Dcx::error(TEXT("xdock"), TEXT("Invalid Dock Host Window"));
		return false;
	}
	LPDCXULTRADOCK ud = new DCXULTRADOCK;

	if (ud == NULL) {
		Dcx::error(TEXT("xdock"), TEXT("No Memory"));
		return false;
	}
	ud->hwnd = hwnd;
	ud->old_exstyles = GetWindowExStyle(hwnd);
	ud->old_styles = GetWindowStyle(hwnd);
	ud->flags = DOCKF_LEFT;

	if (flag.len() > 1) {
		switch(flag[1]) {
			case TEXT('r'):
				ud->flags = DOCKF_RIGHT;
				break;

			case TEXT('t'):
				ud->flags = DOCKF_TOP;
				break;

			case TEXT('b'):
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

void DcxDock::UnDockAll(void)
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
	if (this->FindDock(hwnd) || (GetProp(hwnd,TEXT("dcx_docked")) != NULL))
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
	DcxDock *pthis = (DcxDock *)GetProp(mHwnd,TEXT("DcxDock"));
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
		//		if ( pthis->m_iType == DOCK_TYPE_TREE && DcxDock::g_bTakeOverTreebar) {
		//			PAINTSTRUCT ps;
		//			RECT rcClient, rcParent;
		//			HDC hdc, *hBuffer;

		//			// get treeviews rect
		//			GetClientRect(mHwnd,&rcClient);
		//			GetClientRect(mIRCLink.m_hTreebar, &rcParent);

		//			hdc = BeginPaint(mHwnd, &ps);

		//			//hBuffer = CreateHDCBuffer(hdc, &rcParent);

		//			////SendMessage(mIRCLink.m_hTreebar, WM_ERASEBKGND, (WPARAM)*hBuffer, NULL);
		//			//SendMessage(mIRCLink.m_hTreebar, WM_PRINT, (WPARAM)*hBuffer, PRF_CLIENT|PRF_ERASEBKGND);
		//			////SendMessage(mIRCLink.m_hTreebar, WM_PRINTCLIENT, (WPARAM)*hBuffer, PRF_CLIENT);

		//			//CopyRect(&rcParent, &rcClient);
		//			//MapWindowRect(mHwnd, mIRCLink.m_hTreebar, &rcParent);

		//			//BitBlt(hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top), *hBuffer, rcParent.left, rcParent.top, SRCCOPY);

		//			//DeleteHDCBuffer(hBuffer);

		//			//LRESULT lRes = CallWindowProc(pthis->m_OldRefWndProc, mHwnd, uMsg, (WPARAM)hdc, lParam);

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
#if DCX_DEBUG_OUTPUT
		case TVM_SETITEM:
			{
				if ( pthis->m_iType != DOCK_TYPE_TREE || !DcxDock::g_bTakeOverTreebar)
					break;
				LPTVITEMEX pitem = (LPTVITEMEX)lParam;
				TString buf;
				DcxDock::getTreebarItemType(buf, pitem->lParam);
				Dcx::mIRC.evalex(NULL,0,TEXT("$xtreebar_callback(setitem,%s,%ld,%ld)"), buf.to_chr(), pitem->hItem, pitem->lParam);
			}
			break;
#endif
		case TVM_INSERTITEM:
			{
				if (!DcxDock::g_bTakeOverTreebar)
					break;

				LPTVINSERTSTRUCT pTvis = (LPTVINSERTSTRUCT)lParam;
				if (pTvis->itemex.mask & TVIF_TEXT) {
					TString buf;
					DcxDock::getTreebarItemType(buf, pTvis->itemex.lParam);
					Dcx::mIRC.execex(TEXT("/!set -nu1 %%dcx_%d %s"), pTvis->itemex.lParam, pTvis->itemex.pszText );
					Dcx::mIRC.tsEvalex(buf, TEXT("$xtreebar_callback(geticons,%s,%%dcx_%d)"), buf.to_chr(), pTvis->itemex.lParam);
					int i = buf.gettok( 1 ).to_int() -1;
					if (i < 0)
						i = I_IMAGENONE; //0;
					pTvis->itemex.iImage = i;
					i = buf.gettok( 2 ).to_int() -1;
					if (i < 0)
						i = I_IMAGENONE; //0;
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
	DcxDock *pthis = (DcxDock *)GetProp(mHwnd,TEXT("DcxDock"));
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
					const int pos = DcxDock::getPos(wp->x, wp->y, wp->cx, wp->cy);
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
											if (lpntvcd->nmcd.uItemState & CDIS_HOT) { // This makes sure the hot colour doesnt show as blue.
												if (DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_TEXT] != CLR_INVALID)
													lpntvcd->clrText = DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_TEXT];
												else
													lpntvcd->clrText = GetSysColor(COLOR_HOTLIGHT);

												if (DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_BKG] != CLR_INVALID) // only set a bkg colour if one is set in prefs.
													lpntvcd->clrTextBk = DcxDock::g_clrTreebarColours[TREEBAR_COLOUR_HOT_BKG];
											}
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
												//HTREEITEM hItem = (HTREEITEM)lpntvcd->nmcd.dwItemSpec;
												//TVITEMEX tvi;
												//ZeroMemory(&tvi, sizeof(tvi));
												//tvi.mask = TVIF_CHILDREN|TVIF_STATE;
												//tvi.hItem = hItem;
												//tvi.stateMask = TVIS_EXPANDED;

												//if (TreeView_GetItem(lpntvcd->nmcd.hdr.hwndFrom, &tvi))
												//{
												//	if (tvi.cChildren && !(tvi.state & TVIS_EXPANDED)) { // has children & not expanded
												//		DcxDock::getTreebarChildState(hItem, &tvi);
												//	}
												//}
												const int wid = HIWORD(lpntvcd->nmcd.lItemlParam);
												TString buf;
												Dcx::mIRC.tsEvalex(buf, TEXT("$window(@%d).sbcolor"), wid);
												if (buf.len() > 0) {
													static const TString sbcolor(TEXT("s s message s event s highlight")); // 's' is used as a spacer.
													const int clr = sbcolor.findtok(buf.to_chr(), 1);
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

								TString tsType;
								TString buf((UINT)MIRC_BUFFER_SIZE_CCH);
								TVITEMEX item;
								ZeroMemory(&item, sizeof(item));

								item.hItem = tcgit->hItem;
								item.pszText = buf.to_chr();
								item.cchTextMax = MIRC_BUFFER_SIZE_CCH;
								item.mask = TVIF_TEXT;
								if (TreeView_GetItem(Dcx::mIRC.getTreeview(), &item)) {
									DcxDock::getTreebarItemType(tsType, item.lParam);
									Dcx::mIRC.execex(TEXT("/!set -nu1 %%dcx_%d %s"), item.lParam, item.pszText ); // <- had wrong args causing instant crash when showing tooltips
									Dcx::mIRC.tsEvalex(buf, TEXT("$xtreebar_callback(gettooltip,%s,%%dcx_%d)"), tsType.to_chr(), item.lParam);

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

					const int idPart = ((LPNMMOUSE)hdr)->dwItemSpec +1;

					switch( hdr->code ) {
						case NM_CLICK:
							{
								Dcx::mIRC.signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar sclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_DBLCLK:
							{
								Dcx::mIRC.signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar dclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_RCLICK:
							{
								Dcx::mIRC.signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar rclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
						case NM_RDBLCLK:
							{
								Dcx::mIRC.signalex(dcxSignal.xstatusbar, TEXT("DCXStatusbar rdclick %d %d"), hdr->hwndFrom, idPart);
								return TRUE;
							}
							break;
					}
				}
			}
			break;
		case WM_DRAWITEM:
			{
				LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
				if (pthis->g_StatusBar != NULL && pthis->g_StatusBar == lpDrawItem->hwndItem) {
					LPSB_PARTINFO pPart = (LPSB_PARTINFO)lpDrawItem->itemData;
					if (pPart != NULL) {
						RECT rc = lpDrawItem->rcItem;
						if (pPart->m_iIcon > -1) {
							IMAGEINFO ii;
							ImageList_GetImageInfo(pthis->g_hImageList, pPart->m_iIcon, &ii);
							ImageList_Draw(pthis->g_hImageList, pPart->m_iIcon, lpDrawItem->hDC, rc.left, rc.top + ((rc.bottom - rc.top) - (ii.rcImage.bottom - ii.rcImage.top)) / 2, ILD_TRANSPARENT);
							rc.left += (ii.rcImage.right - ii.rcImage.left) +5;
						}
						if (pPart->m_Text.len() > 0)
							mIRC_DrawText(lpDrawItem->hDC, pPart->m_Text, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE, false);
						else if (IsWindow(pPart->m_Child)) {
							SetWindowPos(pPart->m_Child, NULL, rc.left, rc.top,
								(rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
						}
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
		case WM_DESTROY:
		{
			delete pthis;
			PostMessage(mHwnd, uMsg, 0, 0);
			return 0L;
			//break;
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

	g_StatusBar = CreateWindowExW(
		ExStyles,
		STATUSCLASSNAMEW,NULL,
		Styles,
		0,0,0,0,Dcx::mIRC.getHWND(),(HMENU)(mIRC_ID_OFFSET-1),NULL,NULL);

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

		VectorOfParts::iterator itStart = DcxDock::g_vParts.begin();
		VectorOfParts::iterator itEnd = DcxDock::g_vParts.end();

		while (itStart != itEnd) {
			if (*itStart != NULL) {
				delete *itStart;
			}
			itStart++;
		}
	}
	g_StatusBar = NULL;

	if (g_hImageList != NULL)
		ImageList_Destroy( g_hImageList );
	g_hImageList = NULL;

	if (g_StatusFont != NULL)
		DeleteFont(g_StatusFont);
	g_StatusFont = NULL;
}
bool DcxDock::IsStatusbar(void)
{
	if (IsWindow(g_StatusBar))
		return true;
	return false;
}

void DcxDock::status_parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	const unsigned int numtok = styles.numtok( );
	*Styles = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	for (unsigned int i = 1; i <= numtok; i++ ) {

		if ( styles.gettok( i ) == TEXT("grip") )
			*Styles |= SBARS_SIZEGRIP;
		else if ( styles.gettok( i ) == TEXT("tooltips") )
			*Styles |= SBARS_TOOLTIPS;
		else if ( styles.gettok( i ) == TEXT("nodivider") )
			*Styles |= CCS_NODIVIDER;
		else if ( styles.gettok( i ) == TEXT("notheme") )
			*bNoTheme = TRUE;
		else if ( styles.gettok( i ) == TEXT("disabled") )
			*Styles |= WS_DISABLED;
		else if ( styles.gettok( i ) == TEXT("transparent") )
			*ExStyles |= WS_EX_TRANSPARENT;
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

void DcxDock::status_setText( const int iPart, const int Style, const LPWSTR lpstr ) {
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

void DcxDock::status_setTipText( const int iPart, const LPWSTR lpstr ) {
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

LRESULT DcxDock::status_setPartInfo( const int iPart, const int Style, const LPSB_PARTINFO pPart) {
  return SendMessage( g_StatusBar, SB_SETTEXT, (WPARAM) iPart | (Style | SBT_OWNERDRAW), (LPARAM) pPart );
}

void DcxDock::status_deletePartInfo(const int iPart)
{
	if (DcxDock::status_getPartFlags( iPart ) & SBT_OWNERDRAW) {
		LPSB_PARTINFO pPart = (LPSB_PARTINFO)DcxDock::status_getText(iPart, NULL);
		VectorOfParts::iterator itStart = DcxDock::g_vParts.begin();
		VectorOfParts::iterator itEnd = DcxDock::g_vParts.end();

		while (itStart != itEnd) {
			if (*itStart != NULL && ((LPSB_PARTINFO)*itStart) == pPart) {
				DcxDock::status_setText( iPart, SBT_OWNERDRAW, NULL);
				if (IsWindow(pPart->m_Child))
					DestroyWindow(pPart->m_Child);
				delete pPart;
				DcxDock::g_vParts.erase(itStart);
				return;
			}
			itStart++;
		}
	}
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

UINT DcxDock::status_parseItemFlags( const TString & flags ) {

	UINT len = flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != TEXT('+') ) 
		return iFlags;

	for (UINT i = 1; i < len; i++ )
	{
		switch(flags[i])
		{
		case TEXT('n'):
			iFlags |= SBT_NOBORDERS;
			break;
		case TEXT('p'):
			iFlags |= SBT_POPOUT;
			break;
		case TEXT('f'):
			iFlags |= SBT_OWNERDRAW;
			break;
		}
	}
	return iFlags;
}

void DcxDock::status_cleanPartIcons( ) {

	for (int n = 0; n < 256; n++ )
		DestroyIcon( (HICON) DcxDock::status_getIcon( n ) );
}

LRESULT DcxDock::status_getBorders( LPINT aWidths ) {
  return SendMessage( g_StatusBar, SB_GETBORDERS, (WPARAM) 0, (LPARAM) aWidths );
}

void DcxDock::status_updateParts(void) {
	const int nParts = DcxDock::status_getParts(0,NULL);

	if (nParts <= 0)
		return;

	RECT rcClient;
	int *pParts = new int[nParts];
	//int borders[3];

	GetClientRect(DcxDock::g_StatusBar, &rcClient);
	DcxDock::status_getParts(nParts, pParts);

	//DcxDock::status_getBorders(borders);

	const int w = (rcClient.right - rcClient.left) / 100; // - (2 * borders[1]);

	for (int i = 0; i < nParts; i++) {
		int pw;
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

void DcxDock::status_setFont(HFONT f)
{
	if (f != NULL) {
		SetWindowFont(g_StatusBar, f, TRUE);

		if (g_StatusFont != NULL)
			DeleteFont(g_StatusFont);
		g_StatusFont = f;
	}
}

int DcxDock::getPos(const int x, const int y, const int w, const int h)
{
	RECT rc;
	GetClientRect(Dcx::mIRC.getHWND(),&rc);
	if (x == rc.left && (y + h) == rc.bottom && (x + w) != rc.right)
		return SWB_LEFT;
	if (x == rc.left && (y + h) != rc.bottom && (x + w) == rc.right)
		return SWB_TOP;
	if (x == rc.left && (y + h) == rc.bottom && (x + w) == rc.right)
		return SWB_BOTTOM;
	return SWB_RIGHT;
}

void DcxDock::getTreebarItemType(TString &tsType, const LPARAM lParam)
{
	const int wid = HIWORD(lParam);
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
			Dcx::mIRC.tsEvalex(tsType, TEXT("$window(@%d).type"), wid);
			if (tsType.len() < 1)
				tsType = TEXT("notify");
		}
		break;
	}
}
//UINT DcxDock::getTreebarChildState(const HTREEITEM hParent, LPTVITEMEX pitem)
//{
//	ZeroMemory(pitem, sizeof(TVITEMEX));
//	for (HTREEITEM ptvitem = TreeView_GetChild(Dcx::mIRC.getTreeview(), hParent); ptvitem != NULL; ptvitem = TreeView_GetNextSibling(Dcx::mIRC.getTreeview(), ptvitem)) {
//		pitem->hItem = ptvitem;
//		pitem->mask = TVIF_STATE|TVIF_CHILDREN;
//		pitem->stateMask = TVIS_EXPANDED;
//
//		if (TreeView_GetItem(Dcx::mIRC.getTreeview(), pitem)) {
//		}
//		TraverseChildren(ptvitem, buf, res, pitem);
//	}
//}
