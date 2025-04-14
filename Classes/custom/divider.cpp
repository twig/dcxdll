/*!
 * \file divider.cpp
 * \brief Custom Win32 API Divider Control
 *
 * blah
 *
 * \author David Legault ( clickhere at scriptsdb dot org )
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */
#include "defines.h"
#include "divider.h"

LRESULT CALLBACK DividerWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		break;
	case WM_CREATE:
	{
		const auto lpdvdata = new DVCONTROLDATA;

		SetProp(mHwnd, TEXT("dvc_data"), lpdvdata);

		return 0L;
	}
	break;

	case WM_SIZE:
	{
		Divider_SizeWindowContents(mHwnd, Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
		return 0L;
	}
	break;

	case WM_LBUTTONDOWN:
	{
		Divider_OnLButtonDown(mHwnd, uMsg, wParam, lParam);
		return 0L;
	}
	break;

	case WM_LBUTTONUP:
	{
		Divider_OnLButtonUp(mHwnd, uMsg, wParam, lParam);
		return 0L;
	}
	break;

	case WM_MOUSEMOVE:
	{
		Divider_OnMouseMove(mHwnd, uMsg, wParam, lParam);
		return 0;
	}
	break;

	case WM_MOUSELEAVE:
	{
		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			lpdvdata->m_bMouseOver = false;
			if (lpdvdata->clrBar.clrBar != CLR_INVALID)
			{
				// this allows drawing a custom coloured positioning bar.
				if (RECT rc{}; GetClientRect(mHwnd, &rc))
				{
					if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
						DrawSolidBar(mHwnd, true, lpdvdata->m_iBarPos, rc.bottom, lpdvdata->m_iLineWidth, lpdvdata->clrBar.clrBar);
					else
						DrawSolidBar(mHwnd, false, lpdvdata->m_iBarPos, rc.right, lpdvdata->m_iLineWidth, lpdvdata->clrBar.clrBar);
				}
				return 0L;
			}
		}
	}
	break;

	case WM_SETCURSOR:
	{
		if (Divider_SetCursor(mHwnd, wParam, lParam))
			return TRUE;
	}
	break;

	// wParam == PANE ID, lParam == pointer to a pane item
	case DV_SETPANE:
	{
		if ((wParam != DVF_PANELEFT) && (wParam != DVF_PANERIGHT))
			return FALSE;

		const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));

		if (!lpdvdata)
			return FALSE;

		dcxlParam(LPDVPANEINFO, lpdvpi);

		if (!lpdvpi)
			return FALSE;

		//// Left/Top Pane
		//if (wParam == DVF_PANELEFT) {
		//
		//	// Invalid structure size
		//	if (lpdvpi->cbSize != lpdvdata->m_Panes[0].cbSize)
		//		return FALSE;
		//
		//	//MessageBox( mHwnd, TEXT("Received DV_SETPANE"), TEXT("DV_SETPANE"), MB_OK );
		//
		//	if (dcx_testflag(lpdvpi->fMask, DVPIM_CHILD)) {
		//		lpdvdata->m_Panes[0].hChild = lpdvpi->hChild;
		//	}
		//	if (lpdvpi->fMask & DVPIM_IDEAL) {
		//		lpdvdata->m_Panes[0].cxIdeal = lpdvpi->cxIdeal;
		//	}
		//	if (lpdvpi->fMask & DVPIM_MIN) {
		//		lpdvdata->m_Panes[0].cxMin = lpdvpi->cxMin;
		//	}
		//	if (lpdvpi->fMask & DVPIM_STYLE) {
		//		lpdvdata->m_Panes[0].fStyle = lpdvpi->fStyle;
		//	}
		//}
		//// Right/Bottom Pane
		//else if (wParam == DVF_PANERIGHT) {
		//
		//	// Invalid structure size
		//	if (lpdvpi->cbSize != lpdvdata->m_Panes[1].cbSize)
		//		return FALSE;
		//
		//	if (lpdvpi->fMask & DVPIM_CHILD) {
		//		lpdvdata->m_Panes[1].hChild = lpdvpi->hChild;
		//	}
		//	if (lpdvpi->fMask & DVPIM_IDEAL) {
		//		lpdvdata->m_Panes[1].cxIdeal = lpdvpi->cxIdeal;
		//	}
		//	if (lpdvpi->fMask & DVPIM_MIN) {
		//		lpdvdata->m_Panes[1].cxMin = lpdvpi->cxMin;
		//	}
		//	if (lpdvpi->fMask & DVPIM_STYLE) {
		//		lpdvdata->m_Panes[1].fStyle = lpdvpi->fStyle;
		//	}
		//}

		// Left/Top Pane by default.
		auto lpdvinfo = &lpdvdata->m_LeftTopPane;
		// Right/Bottom Pane
		if (wParam == DVF_PANERIGHT)
			lpdvinfo = &lpdvdata->m_RightBottomPane;

		// Invalid structure size
		if (lpdvpi->cbSize != lpdvinfo->cbSize)
			return FALSE;

		if (dcx_testflag(lpdvpi->fMask, DVPIM_CHILD))
			lpdvinfo->hChild = lpdvpi->hChild;

		if (dcx_testflag(lpdvpi->fMask, DVPIM_IDEAL))
			lpdvinfo->cxIdeal = lpdvpi->cxIdeal;

		if (dcx_testflag(lpdvpi->fMask, DVPIM_MIN))
			lpdvinfo->cxMin = lpdvpi->cxMin;

		if (dcx_testflag(lpdvpi->fMask, DVPIM_STYLE))
			lpdvinfo->fStyle = lpdvpi->fStyle;

		if (RECT rc{}; GetClientRect(mHwnd, &rc))
			Divider_SizeWindowContents(mHwnd, rc.right - rc.left, rc.bottom - rc.top);

		return TRUE;
	}
	break;

	case DV_GETPANE:
	{
		if (!wParam || !lParam)
			return FALSE;

		switch (auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); wParam)
		{
		default:
			break;
		case DVF_PANELEFT:
			*(reinterpret_cast<LPDVPANEINFO>(lParam)) = lpdvdata->m_LeftTopPane;
			break;
		case DVF_PANERIGHT:
			*(reinterpret_cast<LPDVPANEINFO>(lParam)) = lpdvdata->m_RightBottomPane;
			break;
		}

	}
	break;

	// wParam == nullptr, lParam == POSITION VALUE
	case DV_SETDIVPOS:
	{
		const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));
		if (!lpdvdata)
			return FALSE;

		RECT rc{};
		if (!GetClientRect(mHwnd, &rc))
			return FALSE;

		const UINT iPos = gsl::narrow_cast<UINT>(lParam);
		const UINT width = (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT) ? gsl::narrow_cast<UINT>(rc.right - rc.left) : gsl::narrow_cast<UINT>(rc.bottom - rc.top));

		if ((iPos >= lpdvdata->m_LeftTopPane.cxMin) && (iPos <= (width - lpdvdata->m_RightBottomPane.cxMin)))
		{
			lpdvdata->m_iBarPos = iPos;
			Divider_SizeWindowContents(mHwnd, rc.right - rc.left, rc.bottom - rc.top);
			return TRUE;
		}
		return FALSE;
	}
	break;

	// wParam == (OUT) BOOL isVertical? (not used atm), lParam == (OUT) integer bar_position
	case DV_GETDIVPOS:
	{
		if (!lParam)
			return FALSE;

		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			*(reinterpret_cast<LPINT>(lParam)) = (lpdvdata->m_bDragging ? lpdvdata->m_iOldPos : gsl::narrow_cast<int>(lpdvdata->m_iBarPos));
			if (wParam)
				*(reinterpret_cast<BOOL*>(wParam)) = (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT) ? TRUE : FALSE);
		}
		return TRUE;
	}

	// wParam == (IN) DVNM_DRAG_START or DVNM_DRAG_END, lParam == (IN) LPPOINT bar_position x & y
	//case DV_CHANGEPOS:
	//{
	//	// does nothing here... this is handled by the OurMessage() routine.
	//	return 0L;
	//}
	//break;

	// wParam == clr unselected, lParam == clr selected. (old way)
	// wParam == 0, lParam == DVBARCOLORS*
	case DV_SETBARCOLOR:
	{
		//if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		//{
		//	if (auto cTmp = gsl::narrow_cast<COLORREF>(wParam); cTmp != CLR_INVALID)
		//		lpdvdata->clrBar.clrBar = cTmp;
		//	if (auto cTmp = gsl::narrow_cast<COLORREF>(lParam); cTmp != CLR_INVALID)
		//		lpdvdata->clrBar.clrSelBarFg = cTmp;
		//	return TRUE;
		//}
		//return FALSE;

		if (!lParam)
			return FALSE;

		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			const DVBARCOLORS* dbc = reinterpret_cast<DVBARCOLORS*>(lParam);

			if (dbc->clrBar != CLR_INVALID)
				lpdvdata->clrBar.clrBar = dbc->clrBar;
			if (dbc->clrSelBarFg != CLR_INVALID)
				lpdvdata->clrBar.clrSelBarFg = dbc->clrSelBarFg;
			if (dbc->clrSelBarBg != CLR_INVALID)
				lpdvdata->clrBar.clrSelBarBg = dbc->clrSelBarBg;
			if (dbc->clrBarHover != CLR_INVALID)
				lpdvdata->clrBar.clrBarHover = dbc->clrBarHover;
			return TRUE;
		}
		return FALSE;
	}
	break;

	// wParam == (OUT) clr, lParam == (OUT) clr selected. (old way)
	// wParam == 0, lParam == DVBARCOLORS*
	case DV_GETBARCOLOR:
	{
		//if (!wParam || !lParam)
		//	return FALSE;
		//
		//if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		//{
		//	*(reinterpret_cast<COLORREF*>(wParam)) = lpdvdata->clrBar.clrBar;
		//	*(reinterpret_cast<COLORREF*>(lParam)) = lpdvdata->clrBar.clrSelBarFg;
		//	return TRUE;
		//}
		//return FALSE;

		if (!lParam)
			return FALSE;

		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			*reinterpret_cast<DVBARCOLORS*>(lParam) = lpdvdata->clrBar;
			return TRUE;
		}
		return FALSE;
	}
	break;

	// wParam == 0, lParam == width in pixels
	case DV_SETBARWIDTH:
	{
		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			lpdvdata->m_iLineWidth = gsl::narrow_cast<UINT>(lParam);
			return TRUE;
		}
		return FALSE;
	}
	break;

	// wParam == 0, lParam == (OUT) width in pixels
	case DV_GETBARWIDTH:
	{
		if (!lParam)
			return FALSE;

		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			*(reinterpret_cast<UINT*>(lParam)) = lpdvdata->m_iLineWidth;
			return TRUE;
		}
		return FALSE;
	}
	break;

	// wParam == TRUE/FALSE, lParam == (OUT) previous lock state. (optional, BOOL* or nullptr)
	case DV_LOCKBAR:
	{
		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			if (lParam)
				*(reinterpret_cast<BOOL*>(lParam)) = gsl::narrow_cast<BOOL>(lpdvdata->m_bLocked);
			lpdvdata->m_bLocked = gsl::narrow_cast<bool>(wParam);
			return TRUE;
		}
		return FALSE;
	}
	break;

	// wParam == 0, lParam == 0
	// returns bitmask of states.
	case DV_GETBARSTATE:
	{
		LRESULT lRes{};
		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			if (lpdvdata->m_bDragging)
				lRes |= DVBS_DRAGGING;
			if (lpdvdata->m_bLocked)
				lRes |= DVBS_LOCKED;
		}
		return lRes;
	}
	break;

	case WM_ERASEBKGND:
	{
		// wParam == HDC
		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			if (lpdvdata->clrBar.clrBar != CLR_INVALID)
			{
				// this allows drawing a custom coloured positioning bar.
				if (RECT rc{}; GetClientRect(mHwnd, &rc))
				{
					const auto clr = (lpdvdata->m_bMouseOver ? lpdvdata->clrBar.clrBarHover : lpdvdata->clrBar.clrBar);
					Dcx::FillRectColour((HDC)wParam, &rc, clr);
				}
				return TRUE;
			}
		}
	}
	break;

	case WM_DESTROY:
	{
		if (const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data")); lpdvdata)
		{
			// Clear Panes and delete child windows
			// Send Notifications to Parent Window About deletion of child windows

			NMDIVIDER nmdv(mHwnd, DVN_DELETEDPANE, DVNM_LPARAM | DVNM_PANEID, DVF_PANELEFT, 0, lpdvdata->m_LeftTopPane.lParam);

			SendMessage(GetParent(mHwnd), WM_NOTIFY, nmdv.hdr.idFrom, reinterpret_cast<LPARAM>(&nmdv));

			nmdv.iPaneId = DVF_PANERIGHT;
			nmdv.lParam = lpdvdata->m_RightBottomPane.lParam;
			SendMessage(GetParent(mHwnd), WM_NOTIFY, nmdv.hdr.idFrom, reinterpret_cast<LPARAM>(&nmdv));

			if (IsWindow(lpdvdata->m_LeftTopPane.hChild))
				DestroyWindow(lpdvdata->m_LeftTopPane.hChild);

			if (IsWindow(lpdvdata->m_RightBottomPane.hChild))
				DestroyWindow(lpdvdata->m_RightBottomPane.hChild);

			delete lpdvdata;
		}

		RemoveProp(mHwnd, TEXT("dvc_data"));

		return 0L;
	}
	break;
	}
	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

void Divider_SizeWindowContents(HWND mHwnd, const int nWidth, const int nHeight) noexcept
{
	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));
	if (!lpdvdata)
		return;

	if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
	{
		if (lpdvdata->m_LeftTopPane.hChild)
			MoveWindow(lpdvdata->m_LeftTopPane.hChild, 0, 0, gsl::narrow_cast<int>(lpdvdata->m_iBarPos), nHeight, TRUE);
		if (lpdvdata->m_RightBottomPane.hChild)
			MoveWindow(lpdvdata->m_RightBottomPane.hChild, gsl::narrow_cast<int>(lpdvdata->m_iBarPos + lpdvdata->m_iLineWidth), 0,
				gsl::narrow_cast<int>(nWidth - lpdvdata->m_iBarPos - lpdvdata->m_iLineWidth), nHeight, TRUE);
	}
	else {
		if (lpdvdata->m_LeftTopPane.hChild)
			MoveWindow(lpdvdata->m_LeftTopPane.hChild, 0, 0, nWidth, gsl::narrow_cast<int>(lpdvdata->m_iBarPos), TRUE);
		if (lpdvdata->m_RightBottomPane.hChild)
			MoveWindow(lpdvdata->m_RightBottomPane.hChild, 0, gsl::narrow_cast<int>(lpdvdata->m_iBarPos + lpdvdata->m_iLineWidth),
				nWidth, gsl::narrow_cast<int>(nHeight - lpdvdata->m_iBarPos - lpdvdata->m_iLineWidth), TRUE);
	}
}

namespace
{
	HBRUSH CreateXorBrush() noexcept
	{
		//static HBRUSH hbr{};
		//
		//if (!hbr)
		//{
		//	constexpr static const WORD _dotPatternBmp[8] =
		//	{
		//		0x00aa, 0x0055, 0x00aa, 0x0055,
		//		0x00aa, 0x0055, 0x00aa, 0x0055
		//	};
		//
		//	if (const auto hbm = CreateBitmap(8, 8, 1, 1, &_dotPatternBmp[0]); hbm)
		//	{
		//		Auto(DeleteObject(hbm));
		//
		//		hbr = CreatePatternBrush(hbm);
		//	}
		//}
		//return hbr;

		static Dcx::dcxBrush hbr;

		if (!hbr)
		{
			constexpr static const WORD _dotPatternBmp[8] =
			{
				0x00aa, 0x0055, 0x00aa, 0x0055,
				0x00aa, 0x0055, 0x00aa, 0x0055
			};

			if (const auto hbm = CreateBitmap(8, 8, 1, 1, &_dotPatternBmp[0]); hbm)
			{
				Auto(DeleteObject(hbm));

				hbr = CreatePatternBrush(hbm);
			}
		}
		return hbr;
	}
}

GSL_SUPPRESS(type.4)
void DrawXorBar(HDC hdc, const int x1, const int y1, const int width, const int height, COLORREF clrFg, COLORREF clrBg) noexcept
{
	if (!hdc)
		return;

	//// Ook: Possibly pre-allocate these...
	//constexpr static const WORD _dotPatternBmp[8] =
	//{
	//	0x00aa, 0x0055, 0x00aa, 0x0055,
	//	0x00aa, 0x0055, 0x00aa, 0x0055
	//};
	//
	//if (const auto hbm = CreateBitmap(8, 8, 1, 1, &_dotPatternBmp[0]); hbm)
	//{
	//	Auto(DeleteObject(hbm));
	//
	//	if (const auto hbr = CreatePatternBrush(hbm); hbr)
	//	{
	//		Auto(DeleteObject(hbr));
	//
	//		SetBrushOrgEx(hdc, x1, y1, nullptr);
	//		const auto hbrushOld = Dcx::dcxSelectObject<HBRUSH>(hdc, hbr);
	//		Auto(Dcx::dcxSelectObject<HBRUSH>(hdc, hbrushOld));
	//
	//		// sets colours of bar
	//		if (clrFg != CLR_INVALID)
	//			SetTextColor(hdc, clrFg);
	//		if (clrBg != CLR_INVALID)
	//			SetBkColor(hdc, clrBg);
	//
	//		PatBlt(hdc, x1, y1, width, height, PATINVERT);
	//	}
	//}

	if (const auto hbr = CreateXorBrush(); hbr)
	{
		SetBrushOrgEx(hdc, x1, y1, nullptr);
		const auto hbrushOld = Dcx::dcxSelectObject<HBRUSH>(hdc, hbr);
		Auto(Dcx::dcxSelectObject<HBRUSH>(hdc, hbrushOld));

		// sets colours of bar
		if (clrFg != CLR_INVALID)
			SetTextColor(hdc, clrFg);
		if (clrBg != CLR_INVALID)
			SetBkColor(hdc, clrBg);

		PatBlt(hdc, x1, y1, width, height, PATINVERT);
	}
}

void DrawXorBar(HWND mHwnd, const int x1, const int y1, const int width, const int height, COLORREF clrFg, COLORREF clrBg) noexcept
{
	if (!mHwnd)
		return;

	if (const auto hdc = GetWindowDC(mHwnd); hdc)
	{
		Auto(ReleaseDC(mHwnd, hdc));

		DrawXorBar(hdc, x1, y1, width, height, clrFg, clrBg);
	}
}

void DrawSolidBar(HDC hdc, bool bVert, UINT uPos, int iLength, int iLineWidth, COLORREF clrFg) noexcept
{
	if (!hdc)
		return;

	RECT rcBar{};
	if (bVert)
	{
		rcBar.left = uPos;
		rcBar.top = 0;
		rcBar.right = rcBar.left + iLineWidth;
		rcBar.bottom = iLength;
	}
	else {
		rcBar.left = 0;
		rcBar.top = uPos;
		rcBar.right = iLength;
		rcBar.bottom = rcBar.top + iLineWidth;
	}
	Dcx::FillRectColour(hdc, &rcBar, clrFg);
}

void DrawSolidBar(HWND mHwnd, bool bVert, UINT uPos, int iLength, int iLineWidth, COLORREF clrFg) noexcept
{
	if (!mHwnd)
		return;

	if (const auto hdc = GetWindowDC(mHwnd); hdc)
	{
		Auto(ReleaseDC(mHwnd, hdc));

		DrawSolidBar(hdc, bVert, uPos, iLength, iLineWidth, clrFg);
	}
}

void Divider_GetChildControl(HWND mHwnd, const UINT pane, const LPDVPANEINFO result) noexcept
{
	SendMessage(mHwnd, DV_GETPANE, pane, reinterpret_cast<LPARAM>(result));
}

void Divider_CalcBarPos(HWND mHwnd, POINT* pt, RECT* rect) noexcept
{
	if (!mHwnd || !pt || !rect)
		return;

	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));

	if (!GetWindowRect(mHwnd, rect))
		return;

	MapWindowPoints(mHwnd, nullptr, pt, 1);

	pt->x -= rect->left;
	pt->y -= rect->top;

	OffsetRect(rect, -rect->left, -rect->top);

	if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
	{
		if (pt->x < gsl::narrow_cast<int>(lpdvdata->m_LeftTopPane.cxMin))
			pt->x = gsl::narrow_cast<int>(lpdvdata->m_LeftTopPane.cxMin);
		else if (pt->x > rect->right - gsl::narrow_cast<int>(lpdvdata->m_RightBottomPane.cxMin))
			pt->x = rect->right - gsl::narrow_cast<int>(lpdvdata->m_RightBottomPane.cxMin);

		if (pt->x < 3)
			pt->x = 2;
		else if (pt->x > rect->right - 4)
			pt->x = rect->right - 4;

	}
	else {
		if (pt->y < gsl::narrow_cast<int>(lpdvdata->m_LeftTopPane.cxMin))
			pt->y = gsl::narrow_cast<int>(lpdvdata->m_LeftTopPane.cxMin);
		else if (pt->y > rect->bottom - gsl::narrow_cast<int>(lpdvdata->m_RightBottomPane.cxMin))
			pt->y = rect->bottom - gsl::narrow_cast<int>(lpdvdata->m_RightBottomPane.cxMin);

		if (pt->y < 3)
			pt->y = 2;
		else if (pt->y > rect->bottom - 4)
			pt->y = rect->bottom - 4;
	}
}

LRESULT Divider_OnLButtonDown(HWND mHwnd, const UINT iMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!mHwnd)
		return 0L;

	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));
	if (!lpdvdata)
		return 0L;

	if (lpdvdata->m_bLocked)
		return 0L;

	// horizontal position of cursor 
	POINT pt{ Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam) };

	// if TRUE is returned then dont drag.
	if (gsl::narrow_cast<BOOL>(SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_START), reinterpret_cast<LPARAM>(&pt))) != FALSE)
		return 0L;

	RECT rect{};

	Divider_CalcBarPos(mHwnd, &pt, &rect);

	lpdvdata->m_bDragging = true;

	SetCapture(mHwnd);

	if (const auto hdc = GetWindowDC(mHwnd); hdc)
	{
		Auto(ReleaseDC(mHwnd, hdc));

		if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
		{
			DrawXorBar(hdc, pt.x - 2, 1, 4, rect.bottom - 2, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
			lpdvdata->m_iOldPos = pt.x;
		}
		else {
			DrawXorBar(hdc, 1, pt.y - 2, rect.right - 2, 4, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
			lpdvdata->m_iOldPos = pt.y;
		}
	}
	//SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_START), reinterpret_cast<LPARAM>(&pt));

	return 0L;
}

LRESULT Divider_OnLButtonUp(HWND mHwnd, const UINT iMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));

	if (!lpdvdata || !lpdvdata->m_bDragging || !mHwnd)
		return 0L;

	// horizontal position of cursor 
	POINT pt{ Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam) };

	RECT rect{};

	// handle cursor being released outside window.
	GetClientRect(mHwnd, &rect);
	if (!PtInRect(&rect, pt))
	{
		// drawing over bar removed it.
		Divider_RemoveBar(mHwnd, &rect);

		lpdvdata->m_bDragging = false;
		ReleaseCapture();
		SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_END), reinterpret_cast<LPARAM>(&pt));
		return 0L;
	}

	Divider_CalcBarPos(mHwnd, &pt, &rect);

	Divider_RemoveBar(mHwnd, &rect);

	lpdvdata->m_bDragging = false;

	// if TRUE is returned then dont drag.
	if (gsl::narrow_cast<BOOL>(SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_END), reinterpret_cast<LPARAM>(&pt))) != FALSE)
	{
		ReleaseCapture();
		return 0L;
	}

	//convert the divider position back to screen coords.
	if (GetWindowRect(mHwnd, &rect))
	{
		pt.x += rect.left;
		pt.y += rect.top;

		//now convert into CLIENT coordinates
		MapWindowPoints(nullptr, mHwnd, &pt, 1);
		if (GetClientRect(mHwnd, &rect))
		{
			if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
				lpdvdata->m_iBarPos = gsl::narrow_cast<UINT>(pt.x);
			else
				lpdvdata->m_iBarPos = gsl::narrow_cast<UINT>(pt.y);

			//position the child controls
			Divider_SizeWindowContents(mHwnd, rect.right, rect.bottom);
		}
	}
	ReleaseCapture();
	//SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_END), reinterpret_cast<LPARAM>(&pt));
	return 0L;
}

LRESULT Divider_OnMouseMove(HWND mHwnd, const UINT iMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));

	if (!lpdvdata)
		return 0L;

	if (!lpdvdata->m_bDragging)
	{
		if ((lpdvdata->clrBar.clrBarHover != CLR_INVALID) && !lpdvdata->m_bMouseOver)
		{
			RECT rect{};

			// horizontal position of cursor
			const POINT pt{ Dcx::dcxLOWORD(lParam),Dcx::dcxHIWORD(lParam) };

			GetClientRect(mHwnd, &rect);
			if (!PtInRect(&rect, pt))
				return 0L;

			if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
				DrawSolidBar(mHwnd, true, lpdvdata->m_iBarPos, rect.bottom, lpdvdata->m_iLineWidth, lpdvdata->clrBar.clrBarHover);
			else
				DrawSolidBar(mHwnd, false, lpdvdata->m_iBarPos, rect.right, lpdvdata->m_iLineWidth, lpdvdata->clrBar.clrBarHover);

			TRACKMOUSEEVENT tme{};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.hwndTrack = mHwnd;
			tme.dwFlags = TME_LEAVE;
			lpdvdata->m_bMouseOver = gsl::narrow_cast<bool>(_TrackMouseEvent(&tme));
		}
		return 0L;
	}

	if (dcx_testflag(wParam, MK_LBUTTON))
	{
		RECT rect{};

		// horizontal position of cursor 
		POINT pt{ Dcx::dcxLOWORD(lParam),Dcx::dcxHIWORD(lParam) };

		GetClientRect(mHwnd, &rect);
		if (!PtInRect(&rect, pt))
			return 0L;

		Divider_CalcBarPos(mHwnd, &pt, &rect);

		if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
		{
			if (pt.x != lpdvdata->m_iOldPos)
			{
				// if TRUE is returned then dont drag.
				if (gsl::narrow_cast<BOOL>(SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_DRAG), reinterpret_cast<LPARAM>(&pt))) != FALSE)
					return 0L;

				if (const auto hdc = GetWindowDC(mHwnd); hdc)
				{
					Auto(ReleaseDC(mHwnd, hdc));

					DrawXorBar(hdc, lpdvdata->m_iOldPos - 2, 1, 4, rect.bottom - 2, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
					DrawXorBar(hdc, pt.x - 2, 1, 4, rect.bottom - 2, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
				}
				lpdvdata->m_iOldPos = pt.x;
			}
		}
		else if (pt.y != lpdvdata->m_iOldPos)
		{
			// if TRUE is returned then dont drag.
			if (gsl::narrow_cast<BOOL>(SendMessage(mHwnd, DV_CHANGEPOS, gsl::narrow_cast<WPARAM>(DVNM_DRAG_DRAG), reinterpret_cast<LPARAM>(&pt))) != FALSE)
				return 0L;

			if (const auto hdc = GetWindowDC(mHwnd); hdc)
			{
				Auto(ReleaseDC(mHwnd, hdc));

				DrawXorBar(hdc, 1, lpdvdata->m_iOldPos - 2, rect.right - 2, 4, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
				DrawXorBar(hdc, 1, pt.y - 2, rect.right - 2, 4, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
			}
			lpdvdata->m_iOldPos = pt.y;
		}
	}

	return 0L;
}

void Divider_RemoveBar(HWND mHwnd, LPCRECT rc) noexcept
{
	if (!mHwnd || !rc)
		return;

	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));
	if (!lpdvdata)
		return;

	//if (const auto hdc = GetWindowDC(mHwnd); hdc)
	//{
	//	Auto(ReleaseDC(mHwnd, hdc));
	//
	//	if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
	//		DrawXorBar(hdc, lpdvdata->m_iOldPos - 2, 1, 4, rc->bottom - 2, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
	//	else
	//		DrawXorBar(hdc, 1, lpdvdata->m_iOldPos - 2, rc->right - 2, 4, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
	//}

	if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
		DrawXorBar(mHwnd, lpdvdata->m_iOldPos - 2, 1, 4, rc->bottom - 2, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
	else
		DrawXorBar(mHwnd, 1, lpdvdata->m_iOldPos - 2, rc->right - 2, 4, lpdvdata->clrBar.clrSelBarFg, lpdvdata->clrBar.clrSelBarBg);
}

bool Divider_SetCursor(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	if (Dcx::dcxLOWORD(lParam) != HTCLIENT || to_hwnd(wParam) != mHwnd)
		return false;

	const auto lpdvdata = Dcx::dcxGetProp<LPDVCONTROLDATA>(mHwnd, TEXT("dvc_data"));
	if (!lpdvdata)
		return false;

	if (lpdvdata->m_bLocked)
		return false;

	HCURSOR hCursor{ nullptr };

	if (dcx_testflag(dcxGetWindowStyle(mHwnd), DVS_VERT))
		hCursor = LoadCursor(nullptr, IDC_SIZEWE);
	else
		hCursor = LoadCursor(nullptr, IDC_SIZENS);
	if (GetCursor() != hCursor)
		SetCursor(hCursor);
	return true;
}
