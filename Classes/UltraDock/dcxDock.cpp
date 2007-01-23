
/*
	* Base Class to handle docking of windows to mIRC elements
	* & adjust the layout of these windows to make room for the docked windows.
*/

#include "dcxDock.h"

HWND DcxDock::g_StatusBar = NULL;
HIMAGELIST DcxDock::g_hImageList = NULL;

DcxDock::DcxDock(HWND refHwnd, HWND dockHwnd, int dockType)
: m_OldRefWndProc(NULL)
, m_OldDockWndProc(NULL)
, m_RefHwnd(refHwnd)
, m_hParent(dockHwnd)
, m_iType(dockType)
{
	this->m_VectorDocks.clear();
	SetProp(this->m_RefHwnd,"DcxDock",this);
	this->m_OldRefWndProc = (WNDPROC)SetWindowLong(this->m_RefHwnd,GWL_WNDPROC,(LONG)DcxDock::mIRCRefWinProc);
	SetProp(this->m_hParent,"DcxDock",this);
	this->m_OldDockWndProc = (WNDPROC)SetWindowLong(this->m_hParent,GWL_WNDPROC,(LONG)DcxDock::mIRCDockWinProc);
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

	RemoveProp(this->m_RefHwnd,"DcxDock");
	RemoveProp(this->m_hParent,"DcxDock");

	// reset to orig WndProc
	if (this->m_OldRefWndProc != NULL)
		SetWindowLong(this->m_RefHwnd, GWL_WNDPROC, (LONG)this->m_OldRefWndProc);

	if (this->m_OldDockWndProc != NULL)
		SetWindowLong(this->m_hParent, GWL_WNDPROC, (LONG)this->m_OldDockWndProc);

	this->UpdateLayout();
}

bool DcxDock::DockWindow(HWND hwnd, TString &flag)
{
	if (isDocked(hwnd)) {
		mIRCDebug("D_ERROR Window (%d) is already docked", hwnd);
		return false;
	}
	LPDCXULTRADOCK ud = new DCXULTRADOCK;

	if (ud == NULL) {
		mIRCError("D_ERROR No Memory");
		return false;
	}
	ud->hwnd = hwnd;
	ud->old_exstyles = GetWindowLong(hwnd,GWL_EXSTYLE);
	ud->old_styles = GetWindowLong(hwnd,GWL_STYLE);
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
					pthis->AdjustRect((WINDOWPOS *) lParam);
					if (pthis->m_iType == DOCK_TYPE_MDI && DcxDock::IsStatusbar()) {
						WINDOWPOS * wp = (WINDOWPOS *) lParam;
						RECT rc;
						DcxDock::status_getRect(&rc);
						wp->cy -= (rc.bottom - rc.top);
					}
				}
			}
			break;
		//case WM_DESTROY:
		//{
		//	delete pthis;
		//	PostMessage(mHwnd, uMsg, 0, 0);
		//	return 0L;
		//	break;
		//}
	}
	return CallWindowProc(pthis->m_OldRefWndProc, mHwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK DcxDock::mIRCDockWinProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DcxDock *pthis = (DcxDock *)GetProp(mHwnd,"DcxDock");
	if (pthis == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	switch (uMsg) {
		case WM_SIZE:
			{
				if (pthis->m_iType == DOCK_TYPE_MDI && DcxDock::IsStatusbar()) { // parent of MDI type == main mIRC win.
					SendMessage(DcxDock::g_StatusBar,WM_SIZE, (WPARAM)0, (LPARAM)0);
					InvalidateRect(DcxDock::g_StatusBar, NULL, TRUE);
				}
			}
			break;
		case WM_WINDOWPOSCHANGING:
			{
				if (lParam != NULL && pthis->m_iType == DOCK_TYPE_TREE && DcxDock::IsStatusbar()) {
					WINDOWPOS * wp = (WINDOWPOS *) lParam;
					RECT rc;
					DcxDock::status_getRect(&rc);
					wp->cy -= (rc.bottom - rc.top);
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

bool DcxDock::InitStatusbar(void)
{
	if (IsWindow(g_StatusBar))
		return true;
	g_StatusBar = CreateWindowEx(0,STATUSCLASSNAME,NULL,
		WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP|SBARS_TOOLTIPS,
		0,0,0,0,mIRCLink.m_mIRCHWND,(HMENU)(mIRC_ID_OFFSET-1),NULL,NULL);
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
