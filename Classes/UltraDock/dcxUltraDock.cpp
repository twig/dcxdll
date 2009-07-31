/*
	* Handle docking of windows to mIRC's main window around the MDI Client window.
*/
#include "defines.h"
#include "Classes/UltraDock/dcxDock.h"
#include "Dcx.h"



BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam);

DcxDock *g_dockMDI = NULL;
DcxDock *g_dockTreebar = NULL;
DcxDock *g_dockSwitchbar = NULL; // needed to adjust size for statusbar.
DcxDock *g_dockToolbar = NULL; // needed to adjust size for statusbar.

// force a window update.
void UpdatemIRC(void) {
	SendMessage(Dcx::mIRC.getHWND(), WM_SIZE, NULL, NULL);
}

/*
	* Setup Everything for DcxDock
*/
void InitUltraDock(void)
{
	// set all colours as invalid
	for (int i = 0; i <= TREEBAR_COLOUR_MAX; i++)
		DcxDock::g_clrTreebarColours[i] = CLR_INVALID;

	g_dockMDI = new DcxDock(Dcx::mIRC.getMDIClient(), Dcx::mIRC.getHWND(), DOCK_TYPE_MDI);
	g_dockTreebar = new DcxDock(Dcx::mIRC.getTreeview(), Dcx::mIRC.getTreebar(), DOCK_TYPE_TREE);
	g_dockSwitchbar = new DcxDock(NULL, Dcx::mIRC.getSwitchbar(), DOCK_TYPE_SWITCH);
	g_dockToolbar = new DcxDock(NULL, Dcx::mIRC.getToolbar(), DOCK_TYPE_TOOL);
}
/*
	*	Eject ALL Docked dialogs.
*/
void CloseUltraDock(void)
{
	EnumChildWindows(Dcx::mIRC.getHWND(),(WNDENUMPROC)EnumDocked,NULL);

	DcxDock::UnInitStatusbar();

	delete g_dockToolbar;
	delete g_dockSwitchbar;
	delete g_dockTreebar;
	delete g_dockMDI;

	if (IsWindow(Dcx::mIRC.getTreeview()) && Dcx::mIRC.getTreeImages() != NULL) {
		HIMAGELIST o = TreeView_SetImageList(Dcx::mIRC.getTreeview(),Dcx::mIRC.getTreeImages(),TVSIL_NORMAL);
		if (o != NULL && o != Dcx::mIRC.getTreeImages())
			ImageList_Destroy(o);
	}

	UpdatemIRC();
}
bool FindUltraDock(const HWND hwnd)
{
	if (g_dockMDI == NULL)
		return false;
	return g_dockMDI->FindDock(hwnd);
}
bool FindTreebarDock(const HWND hwnd)
{
	if (g_dockTreebar == NULL)
		return false;
	return g_dockTreebar->FindDock(hwnd);
}
LPDCXULTRADOCK GetUltraDock(const HWND hwnd)
{
	if (g_dockMDI == NULL)
		return NULL;
	return g_dockMDI->GetDock(hwnd);
}

void UltraDock(const HWND mWnd, HWND temp, const TString &flag)
{
	// mWnd is unused.
	if (g_dockMDI == NULL)
		return;
	if ((FindUltraDock(temp)) || (FindTreebarDock(temp)) || (GetProp(temp,"dcx_docked") != NULL)) {
		Dcx::error("/xdock -m","Window already docked");
		return;
	}
	g_dockMDI->DockWindow(temp, flag);
}

void UltraUnDock(const HWND hwnd)
{
	if (g_dockMDI == NULL)
		return;
	g_dockMDI->UnDockWindow(hwnd);
}
LPDCXULTRADOCK GetTreebarDock(const HWND hwnd)
{
	if (g_dockTreebar == NULL)
		return NULL;
	return g_dockTreebar->GetDock(hwnd);
}

void TreebarDock(HWND temp, const TString &flag)
{
	// mWnd is unused.
	if (g_dockTreebar == NULL)
		return;
	if ((FindUltraDock(temp)) || (FindTreebarDock(temp)) || (GetProp(temp,"dcx_docked") != NULL)) {
		Dcx::error("/xdock -b","Window already docked");
		return;
	}
	g_dockTreebar->DockWindow(temp, flag);
	UpdatemIRC(); // seems needed to force the size update
}

void TreebarUnDock(const HWND hwnd)
{
	if (g_dockTreebar == NULL)
		return;
	g_dockTreebar->UnDockWindow(hwnd);
}

// #####################################################################################
// 0 == no swb, 1 == Left, 2 == Right, 3 == Top, 4 == Bottom
int SwitchbarPos(const int type)
{
	RECT swb_rc, mdi_rc;
	HWND hwnd;

	switch (type)
	{
	case DOCK_TYPE_TOOL: // toolbar
		hwnd = Dcx::mIRC.getToolbar();
		break;
	case DOCK_TYPE_TREE: // treebar
		hwnd = Dcx::mIRC.getTreebar();
		break;
	default:
	case DOCK_TYPE_SWITCH: // switchbar
		hwnd = Dcx::mIRC.getSwitchbar();
		break;
	}
	if (IsWindowVisible(hwnd)) {
		GetWindowRect(hwnd,&swb_rc);
		GetWindowRect(Dcx::mIRC.getMDIClient(),&mdi_rc);
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

// Below is the old code, this is kept for refrence & will be removed when the new code is finished.
//#include "defines.h"
//
//extern mIRCDLL mIRCLink;
//
//BOOL CALLBACK EnumDocked(HWND hwnd,LPARAM lParam);
//LRESULT CALLBACK mIRCMDIProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//// UltraDock stuff
//VectorOfDocks v_docks; // list of all docked windows.
//WNDPROC oldMDIProc; // old MDI window proc
//
//// force a window update.
//void UpdatemIRC(void) {
//	SendMessage(Dcx::mIRC.getHWND(), WM_SIZE, NULL, NULL);
//}
///*
//	* Setup Everything for UltraDock
//*/
//void InitUltraDock(void)
//{
//	/* UltraDock stuff */
//	DCX_DEBUG("InitUltraDock", "Finding mIRC_Toolbar...");
//	mIRCLink.m_hToolbar = FindWindowEx(Dcx::mIRC.getHWND(),NULL,"mIRC_Toolbar",NULL);
//
//	DCX_DEBUG("InitUltraDock", "Finding MDIClient...");
//	mIRCLink.m_hMDI = FindWindowEx(Dcx::mIRC.getHWND(),NULL,"MDIClient",NULL);
//
//	DCX_DEBUG("InitUltraDock", "Finding mIRC_SwitchBar...");
//	mIRCLink.m_hSwitchbar = FindWindowEx(Dcx::mIRC.getHWND(),NULL,"mIRC_SwitchBar",NULL);
//
//	DCX_DEBUG("InitUltraDock", "Finding mIRC_TreeList...");
//	mIRCLink.m_hTreebar = FindWindowEx(Dcx::mIRC.getHWND(),NULL,"mIRC_TreeList",NULL);
//
//	if (IsWindow(mIRCLink.m_hTreebar)) {
//		Dcx::mIRC.getTreeview() = GetWindow(mIRCLink.m_hTreebar,GW_CHILD);
//		if (IsWindow(Dcx::mIRC.getTreeview()))
//			Dcx::mIRC.getTreeImages() = TreeView_GetImageList(Dcx::mIRC.getTreeview(),TVSIL_NORMAL);
//	}
//
//	v_docks.clear();
//	if (IsWindow(mIRCLink.m_hMDI))
//		oldMDIProc = (WNDPROC)SetWindowLongPtr(mIRCLink.m_hMDI,GWLP_WNDPROC, (LONG_PTR)mIRCMDIProc);
//
//}
///*
//	*	Eject ALL Docked dialogs.
//*/
//void CloseUltraDock(void)
//{
//	EnumChildWindows(Dcx::mIRC.getHWND(),(WNDENUMPROC)EnumDocked,NULL);
//	VectorOfDocks::iterator itStart = v_docks.begin();
//	VectorOfDocks::iterator itEnd = v_docks.end();
//
//	while (itStart != itEnd) {
//		if (*itStart != NULL) {
//			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
//			SetWindowLong(ud->hwnd,GWL_STYLE, ud->old_styles);
//			SetWindowLong(ud->hwnd,GWL_EXSTYLE, ud->old_exstyles);
//		  RemStyles(ud->hwnd,GWL_STYLE,WS_CHILDWINDOW);
//			SetParent(ud->hwnd, NULL);
//			SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED|SWP_NOACTIVATE);
//			delete ud;
//		}
//		itStart++;
//	}
//	v_docks.clear();
//
//	if (oldMDIProc != NULL)
//		SetWindowLongPtr(mIRCLink.m_hMDI, GWLP_WNDPROC, (LONG_PTR)oldMDIProc);
//
//	if (IsWindow(Dcx::mIRC.getTreeview()) && Dcx::mIRC.getTreeImages() != NULL) {
//		HIMAGELIST o = TreeView_SetImageList(Dcx::mIRC.getTreeview(),Dcx::mIRC.getTreeImages(),TVSIL_NORMAL);
//		if (o != NULL && o != Dcx::mIRC.getTreeImages())
//			ImageList_Destroy(o);
//	}
//
//	UpdatemIRC();
//}
//
//bool FindUltraDock(const HWND hwnd)
//{
//	VectorOfDocks::iterator itStart = v_docks.begin();
//	VectorOfDocks::iterator itEnd = v_docks.end();
//
//	while (itStart != itEnd) {
//		if (*itStart != NULL) {
//			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
//			if (ud->hwnd == hwnd)
//				return true;
//		}
//
//		itStart++;
//	}
//	return false;
//}
//LPDCXULTRADOCK GetUltraDock(const HWND hwnd)
//{
//	VectorOfDocks::iterator itStart = v_docks.begin();
//	VectorOfDocks::iterator itEnd = v_docks.end();
//
//	while (itStart != itEnd) {
//		if (*itStart != NULL) {
//			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
//			if (ud->hwnd == hwnd)
//				return ud;
//		}
//
//		itStart++;
//	}
//	return NULL;
//}
//
//void UltraDock(const HWND mWnd, HWND temp, TString &flag)
//{
//	if ((FindUltraDock(temp)) || (GetProp(temp,"dcx_docked") != NULL)) {
//		DCXError("/xdock -m","Window already docked");
//		return;
//	}
//
//	LPDCXULTRADOCK ud = new DCXULTRADOCK;
//
//	ud->hwnd = temp;
//	ud->old_exstyles = GetWindowLong(temp,GWL_EXSTYLE);
//	ud->old_styles = GetWindowLong(temp,GWL_STYLE);
//	ud->flags = DOCKF_LEFT;
//
//	if (flag.len() > 1) {
//		switch(flag[1]) {
//			case 'r':
//				ud->flags = DOCKF_RIGHT;
//				break;
//
//			case 't':
//				ud->flags = DOCKF_TOP;
//				break;
//
//			case 'b':
//				ud->flags = DOCKF_BOTTOM;
//				break;
//
//			default:
//				ud->flags = DOCKF_LEFT;
//				break;
//		}
//	}
//
//	v_docks.push_back(ud);
//	RemStyles(temp,GWL_STYLE,WS_CAPTION | DS_FIXEDSYS | DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_OVERLAPPED);
//	RemStyles(temp,GWL_EXSTYLE,WS_EX_CONTROLPARENT | WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
//	//RemStyles(temp,GWL_EXSTYLE,WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_STATICEDGE | WS_EX_NOPARENTNOTIFY);
//	AddStyles(temp,GWL_STYLE,WS_CHILDWINDOW);
//	SetParent(temp, Dcx::mIRC.getHWND());
//	UpdatemIRC();
//}
//
//void UltraUnDock(const HWND hwnd)
//{
//	VectorOfDocks::iterator itStart = v_docks.begin();
//	VectorOfDocks::iterator itEnd = v_docks.end();
//
//	while (itStart != itEnd) {
//		if (*itStart != NULL) {
//			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
//			if (ud->hwnd == hwnd) {
//				v_docks.erase(itStart);
//				SetWindowLong(ud->hwnd,GWL_STYLE, ud->old_styles);
//				SetWindowLong(ud->hwnd,GWL_EXSTYLE, ud->old_exstyles);
//			  RemStyles(ud->hwnd,GWL_STYLE,WS_CHILDWINDOW);
//				SetParent(ud->hwnd, NULL);
//				SetWindowPos(ud->hwnd, HWND_TOP, ud->rc.left, ud->rc.top, ud->rc.right - ud->rc.left, ud->rc.bottom - ud->rc.top, SWP_NOZORDER|SWP_FRAMECHANGED);
//				delete ud;
//				UpdatemIRC();
//				return;
//			}
//		}
//		itStart++;
//	}
//}
//void AdjustMDIRect(WINDOWPOS *wp)
//{
//	if ((wp->flags & SWP_NOSIZE) && (wp->flags & SWP_NOMOVE)) { // handle min/max case;
//		//HWND cwin = (HWND)SendMessage(mdi_hwnd,WM_MDIGETACTIVE,NULL,NULL);
//		//if (IsWindow(cwin)) {
//		//	SendMessage(mdi_hwnd,WM_MDIREFRESHMENU,0,0);
//		//	DrawMenuBar(cwin);
//		//}
//		return;
//	}
//
//	VectorOfDocks::iterator itStart = v_docks.begin();
//	VectorOfDocks::iterator itEnd = v_docks.end();
//	RECT rcDocked;
//	int xleftoffset = 0, xrightoffset = 0, ytopoffset = 0, ybottomoffset = 0;
//	int x,y,w,h,mdiw,mdih,nWin = 0; //nWin = v_docks.size();
//
//	// count visible docked windows.
//	while (itStart != itEnd) {
//		if ((*itStart != NULL) && (IsWindowVisible(((LPDCXULTRADOCK)*itStart)->hwnd))) {
//			nWin++; // count docked windows.
//		}
//		itStart++;
//	}
//	if (nWin == 0) return;
//	itStart = v_docks.begin();
//	mdih = wp->cy;
//	mdiw = wp->cx;
//
//	ytopoffset = wp->y;
//	ybottomoffset = wp->y + mdih;
//	xleftoffset = wp->x;
//	xrightoffset = wp->x + mdiw;
//
//  HDWP hdwp = BeginDeferWindowPos(nWin);
//	HDWP tmp;
//
//	// size docks
//	while (itStart != itEnd) {
//		if (*itStart != NULL) {
//			LPDCXULTRADOCK ud = (LPDCXULTRADOCK)*itStart;
//			if (IsWindowVisible(ud->hwnd)) {
//				GetWindowRect(ud->hwnd,&rcDocked);
//				OffsetRect(&rcDocked,-rcDocked.left,-rcDocked.top);
//				switch (ud->flags)
//				{
//				case DOCKF_LEFT:
//					{ // docked to left
//						x = xleftoffset;
//						y = ytopoffset;
//						w = rcDocked.right;
//						h = mdih;
//						xleftoffset += rcDocked.right;
//						mdiw -= rcDocked.right;
//					}
//					break;
//				case DOCKF_RIGHT:
//					{ // docked to right
//						x = xrightoffset - rcDocked.right;
//						y = ytopoffset;
//						w = rcDocked.right;
//						h = mdih;
//						xrightoffset += rcDocked.right;
//						mdiw -= rcDocked.right;
//					}
//					break;
//				case DOCKF_TOP:
//					{ // dock to top
//						x = xleftoffset;
//						y = ytopoffset;
//						w = mdiw;
//						h = rcDocked.bottom;
//						ytopoffset += rcDocked.bottom;
//						mdih -= rcDocked.bottom;
//					}
//					break;
//				default:
//					{ // dock to bottom
//						x = xleftoffset;
//						y = ybottomoffset - rcDocked.bottom;
//						w = mdiw;
//						h = rcDocked.bottom;
//						ybottomoffset -= rcDocked.bottom;
//						mdih -= rcDocked.bottom;
//					}
//					break;
//				}
//				tmp = DeferWindowPos(hdwp,ud->hwnd,NULL,x,y,w,h,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
//				if (tmp != NULL) hdwp = tmp;
//			}
//		}
//
//		itStart++;
//	}
//	wp->x = xleftoffset;
//	wp->y = ytopoffset;
//	wp->cx = mdiw;
//	wp->cy = mdih;
//	EndDeferWindowPos(hdwp);
//}
//
//// #####################################################################################
//// 0 == no swb, 1 == Left, 2 == Right, 3 == Top, 4 == Bottom
//int SwitchbarPos(const int type)
//{
//	RECT swb_rc, mdi_rc;
//	HWND hwnd;
//
//	switch (type)
//	{
//	case 1: // toolbar
//		hwnd = mIRCLink.m_hToolbar;
//		break;
//	case 2: // treebar
//		hwnd = mIRCLink.m_hTreebar;
//		break;
//	default:
//	case 0: // switchbar
//		hwnd = mIRCLink.m_hSwitchbar;
//		break;
//	}
//	if (IsWindowVisible(hwnd)) {
//		GetWindowRect(hwnd,&swb_rc);
//		GetWindowRect(mIRCLink.m_hMDI,&mdi_rc);
//		if (swb_rc.left >= mdi_rc.right)
//			return SWB_RIGHT;
//		if (swb_rc.top >= mdi_rc.bottom)
//			return SWB_BOTTOM;
//		if (swb_rc.bottom <= mdi_rc.top)
//			return SWB_TOP;
//		return SWB_LEFT;
//	}
//	return SWB_NONE;
//}
//
//LRESULT CALLBACK mIRCMDIProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	switch (uMsg) {
//		case WM_WINDOWPOSCHANGING:
//			{
//				WINDOWPOS *wp = (WINDOWPOS *) lParam;
//				if (wp != NULL)
//					AdjustMDIRect(wp);
//			}
//			break;
//		case WM_DESTROY:
//		{
//			SetWindowLongPtr(Dcx::mIRC.getHWND(), GWLP_WNDPROC, (LONG_PTR)oldMDIProc);
//			PostMessage(mHwnd, uMsg, 0, 0);
//			return 0L;
//			break;
//		}
//	}
//	return CallWindowProc(oldMDIProc, mHwnd, uMsg, wParam, lParam);
//}
//
