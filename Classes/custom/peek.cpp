//
//

#include "defines.h"
#define BUILD_PEEK_CLASS 1
#include "peek.h"

namespace
{
	LPPEEK_DATA Peek_GetWindowData(HWND hWnd) noexcept
	{
		return Dcx::dcxGetProp<LPPEEK_DATA>(hWnd, TEXT("peek_data"));
	}

	void Peek_DeleteWindowData(HWND hWnd) noexcept
	{
		if (const auto lpmcdata = Peek_GetWindowData(hWnd); lpmcdata)
		{
			//if (lpmcdata->m_hThumb)
			//{
			//	DcxDWMModule::dcxDwmUnregisterThumbnail(lpmcdata->m_hThumb);
			//	lpmcdata->m_hThumb = nullptr;
			//}
			if (lpmcdata->m_hTheme)
			{
				DcxUXModule::dcxCloseThemeData(lpmcdata->m_hTheme);
				lpmcdata->m_hTheme = nullptr;
			}

			RemoveProp(hWnd, TEXT("peek_data"));

			delete lpmcdata;
		}
	}
}

/// <summary>
/// The window proc of the main parent control.
/// </summary>
/// <param name="mHwnd">- The editbox and button are children of this HWND.</param>
/// <param name="uMsg"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK PeekWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		break;
	case WM_CREATE:
	{
		return Peek_OnCreate(mHwnd, wParam, lParam);
	}
	break;

	case WM_THEMECHANGED:
	{
		Peek_OnThemeChange(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_PAINT:
	{
		Peek_OnPaint(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_PRINTCLIENT:
	{
		Peek_OnPaint(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_SETTEXT:
	{
		return Peek_SetTitle(mHwnd, reinterpret_cast<LPCTSTR>(lParam), -1);
	}
	break;

	case WM_SETCURSOR:
	{
		if ((Dcx::dcxLOWORD(lParam) != HTCLIENT) || (to_hwnd(wParam) != mHwnd))
			break;

		if (const HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW); GetCursor() != hCursor)
			SetCursor(hCursor);
		return TRUE;
	}
	break;

	case WM_ERASEBKGND:
	{
		// wParam == HDC

		// paints using supplied hdc in wParam, this avoids flicker when sizing...
		Peek_OnPaint(mHwnd, wParam, lParam);
		return 1L;
	}
	break;

	case WM_DESTROY:
	{
		Peek_DeleteWindowData(mHwnd);
		return 0L;
	}
	break;

	case PC_WM_SETSOURCE:
	{
		// wParam = hwnd
		// lParam = 0
		return Peek_SetSource(mHwnd, reinterpret_cast<HWND>(wParam));
	}
	break;

	case PC_WM_REMSOURCE:
	{
		// wParam = 0
		// lParam = 0
		return Peek_RemSource(mHwnd);
	}
	break;

	case PC_WM_SETDESC:
	{
		// wParam = description string
		// lParam = length of string
		return Peek_SetDescription(mHwnd, reinterpret_cast<LPCTSTR>(wParam), gsl::narrow_cast<int>(lParam));
	}
	break;

	case PC_WM_SETMINMAX:
	{
		// wParam = pointer to min size LPSIZE.
		// lParam = pointer to max size LPSIZE.

		if (auto lpszMin = reinterpret_cast<LPSIZE>(wParam); lpszMin)
			Peek_SetMin(mHwnd, lpszMin);

		if (auto lpszMax = reinterpret_cast<LPSIZE>(lParam); lpszMax)
			Peek_SetMax(mHwnd, lpszMax);

		return TRUE;
	}
	break;

	case PC_WM_SETDATA:
	{
		// wParam = pointer to peek data structure.
		// lParam = 0
		return Peek_SetData(mHwnd, reinterpret_cast<pkData*>(wParam));
	}
	break;

	case PC_WM_SHOW:
	{
		// wParam = BOOL showhdc
		// lParam = x & y pos
		return Peek_Show(mHwnd, gsl::narrow_cast<bool>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	break;
	}
	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

//void Peek_SizeWindow(HWND mHwnd, WORD cx, WORD cy) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//	//const RECT rc{ 0,0,cx,cy };
//
//	//if (lpmcdata->m_hThumb)
//	//{
//	//	DWM_THUMBNAIL_PROPERTIES dskThumbProps{};
//
//	//	dskThumbProps.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_RECTDESTINATION;
//	//	dskThumbProps.fSourceClientAreaOnly = FALSE;
//	//	dskThumbProps.fVisible = TRUE;
//	//	dskThumbProps.opacity = (255 * 70) / 100;
//	//	dskThumbProps.rcDestination = rc;
//
//	//	DcxDWMModule::dcxDwmUpdateThumbnailProperties(lpmcdata->m_hThumb, &dskThumbProps);
//	//}
//
//}
//
//void Peek_OnLButtonDown(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//}
//
//void Peek_OnLButtonUp(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//}
//
//void Peek_OnMouseMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//	if (!lpmcdata->m_Tracking)
//	{
//		TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, mHwnd, HOVER_DEFAULT };
//
//		lpmcdata->m_Tracking = _TrackMouseEvent(&tme);
//	}
//}
//
//void Peek_OnMouseLeave(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//	lpmcdata->m_Tracking = FALSE;
//}
//
//void Peek_SetFocus(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//}

void Peek_OnThemeChange(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!mHwnd)
		return;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return;

	if (lpmcdata->m_hTheme)
	{
		DcxUXModule::dcxCloseThemeData(lpmcdata->m_hTheme);
		lpmcdata->m_hTheme = nullptr;
		//lpmcdata->m_hTheme = DcxUXModule::dcxOpenThemeData(mHwnd, L"Composited::Window");
		lpmcdata->m_hTheme = DcxUXModule::dcxOpenThemeData(mHwnd, L"TEXTSTYLE");
	}
}

LRESULT Peek_OnCreate(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (!mHwnd)
		return -1L;

	dcxlParam(LPCREATESTRUCT, cs);

	if (!cs)
		return -1L;

	{
		auto lpmcdata = std::make_unique<PEEK_DATA>();

		lpmcdata->m_BaseID = gsl::narrow_cast<UINT>(reinterpret_cast<size_t>(cs->hMenu));

		//lpmcdata->m_hBase = mHwnd;
		//lpmcdata->m_Styles = cs->style /*& MCS_STYLEMASK*/;
		//lpmcdata->m_hTheme = DcxUXModule::dcxOpenThemeData(mHwnd, L"Composited::Window");
		lpmcdata->m_hTheme = DcxUXModule::dcxOpenThemeData(mHwnd, L"TEXTSTYLE");

		// toplevel only!
		//if (DcxDWMModule::dcxDwmRegisterThumbnail(mHwnd, cs->hwndParent, &lpmcdata->m_hThumb) != S_OK)
		//	return -1L;

		SetPropW(mHwnd, TEXT("peek_data"), lpmcdata.release());
	}
	{
		const auto lpmcdata = Peek_GetWindowData(mHwnd);

		if (!mHwnd || !lpmcdata)
			return -1L;
	}

	return 0L;
}

static void GetTextRect(HDC hdc, LPRECT rcTxt, LPPEEK_DATA lpmcdata) noexcept
{
	constexpr DWORD textFlags = DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT;

	if (lpmcdata->m_hTheme)
	{
		DTTOPTS dtt{};
		dtt.dwSize = sizeof(DTTOPTS);
		dtt.dwFlags = DTT_CALCRECT;

		if (!_ts_isEmpty(lpmcdata->m_Title))
		{
			RECT rcTmp{};
			DcxUXModule::dcxDrawThemeTextEx(lpmcdata->m_hTheme, hdc, TEXT_BODYTITLE, 0, &lpmcdata->m_Title[0], -1, textFlags, &rcTmp, &dtt);
			const int txtSize = std::max(12L, (rcTmp.bottom - rcTmp.top));
			rcTxt->bottom = rcTxt->top + txtSize + 1;
			rcTxt->right = std::max(rcTmp.right, rcTxt->right);
		}
		if (!_ts_isEmpty(lpmcdata->m_Description))
		{
			RECT rcTmp{};
			DcxUXModule::dcxDrawThemeTextEx(lpmcdata->m_hTheme, hdc, TEXT_BODYTITLE, 0, &lpmcdata->m_Title[0], -1, textFlags, &rcTmp, &dtt);
			const int txtSize = std::max(12L, (rcTmp.bottom - rcTmp.top));
			rcTxt->bottom += txtSize + 1;
			rcTxt->right = std::max(rcTmp.right, rcTxt->right);
		}
	}
	else {
		if (!_ts_isEmpty(lpmcdata->m_Title))
		{
			RECT rcTmp{};
			DrawText(hdc, &lpmcdata->m_Title[0], -1, &rcTmp, textFlags);
			const int txtSize = (rcTmp.bottom - rcTmp.top);
			rcTxt->bottom = rcTxt->top + txtSize + 1;
			rcTxt->right = std::max(rcTmp.right, rcTxt->right);
		}
		if (!_ts_isEmpty(lpmcdata->m_Description))
		{
			RECT rcTmp{};
			DrawText(hdc, &lpmcdata->m_Title[0], -1, &rcTmp, textFlags);
			const int txtSize = (rcTmp.bottom - rcTmp.top);
			rcTxt->bottom += txtSize + 1;
			rcTxt->right = std::max(rcTmp.right, rcTxt->right);
		}
	}
}

void Peek_OnPaint(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	if (!mHwnd)
		return;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return;

	dcxwParam(HDC, hdc);

	RECT rcClient{};
	GetClientRect(mHwnd, &rcClient);
	RECT rcDest{ rcClient };

	PAINTSTRUCT ps{};

	if (!wParam)
		hdc = BeginPaint(mHwnd, &ps);
	//else
	//	CopyRect(&ps.rcPaint, &rcClient);

	if (!hdc)
		return;

	Auto({ if (!wParam) EndPaint(mHwnd, &ps); });

	if (lpmcdata->m_clrBkg != CLR_INVALID)
		Dcx::FillRectColour(hdc, &rcClient, lpmcdata->m_clrBkg);
	else
		FillRect(hdc, &rcClient, GetSysColorBrush(COLOR_WINDOW));

	{
		RECT rcTxt{ rcClient };
		//SetBkMode(hdc, OPAQUE);

		constexpr DWORD textFlags = DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
		// draw title & description
		if (lpmcdata->m_hTheme)
		{
			DTTOPTS dtt{};
			dtt.dwSize = sizeof(DTTOPTS);
			dtt.dwFlags = DTT_CALCRECT;

			if (!_ts_isEmpty(lpmcdata->m_Title))
			{
				RECT rcTmp{};
				DcxUXModule::dcxDrawThemeTextEx(lpmcdata->m_hTheme, hdc, TEXT_BODYTITLE, 0, &lpmcdata->m_Title[0], -1, textFlags | DT_CALCRECT, &rcTmp, &dtt);
				const int txtSize = std::max(12L, (rcTmp.bottom - rcTmp.top));
				rcTxt.bottom = rcTxt.top + txtSize;

				if (lpmcdata->m_clrTitle != CLR_INVALID)
				{
					dtt.dwFlags = DTT_TEXTCOLOR;
					dtt.crText = lpmcdata->m_clrTitle;
				}
				DcxUXModule::dcxDrawThemeTextEx(lpmcdata->m_hTheme, hdc, TEXT_BODYTITLE, 0, &lpmcdata->m_Title[0], -1, textFlags, &rcTxt, &dtt);
				if (!_ts_isEmpty(lpmcdata->m_Description))
					OffsetRect(&rcTxt, 0, txtSize + 1);
			}
			if (!_ts_isEmpty(lpmcdata->m_Description))
			{
				RECT rcTmp{};
				DcxUXModule::dcxDrawThemeTextEx(lpmcdata->m_hTheme, hdc, TEXT_BODYTITLE, 0, &lpmcdata->m_Description[0], -1, textFlags | DT_CALCRECT, &rcTmp, &dtt);
				const int txtSize = std::max(12L, (rcTmp.bottom - rcTmp.top));
				rcTxt.bottom = rcTxt.top + txtSize + PEEK_TXT_BOTTOM_PAD;

				if (lpmcdata->m_clrDesc != CLR_INVALID)
				{
					dtt.dwFlags = DTT_TEXTCOLOR;
					dtt.crText = lpmcdata->m_clrDesc;
				}
				DcxUXModule::dcxDrawThemeTextEx(lpmcdata->m_hTheme, hdc, TEXT_BODYTEXT, 0, &lpmcdata->m_Description[0], -1, textFlags, &rcTxt, &dtt);
			}
		}
		else {
			if (!_ts_isEmpty(lpmcdata->m_Title))
			{
				COLORREF oldClr{ CLR_INVALID };
				if (lpmcdata->m_clrTitle != CLR_INVALID)
					oldClr = SetTextColor(hdc, lpmcdata->m_clrTitle);

				RECT rcTmp{};
				DrawText(hdc, &lpmcdata->m_Title[0], -1, &rcTmp, textFlags | DT_CALCRECT);
				const int txtSize = (rcTmp.bottom - rcTmp.top);
				rcTxt.bottom = rcTxt.top + txtSize;

				DrawText(hdc, &lpmcdata->m_Title[0], -1, &rcTxt, textFlags);
				if (!_ts_isEmpty(lpmcdata->m_Description))
					OffsetRect(&rcTxt, 0, txtSize + 1);

				if (oldClr != CLR_INVALID)
					SetTextColor(hdc, oldClr);
			}
			if (!_ts_isEmpty(lpmcdata->m_Description))
			{
				COLORREF oldClr{ CLR_INVALID };
				if (lpmcdata->m_clrDesc != CLR_INVALID)
					oldClr = SetTextColor(hdc, lpmcdata->m_clrDesc);

				RECT rcTmp{};
				DrawText(hdc, &lpmcdata->m_Description[0], -1, &rcTmp, textFlags | DT_CALCRECT);
				const int txtSize = (rcTmp.bottom - rcTmp.top);
				rcTxt.bottom = rcTxt.top + txtSize + PEEK_TXT_BOTTOM_PAD;

				DrawText(hdc, &lpmcdata->m_Description[0], -1, &rcTxt, textFlags);

				if (oldClr != CLR_INVALID)
					SetTextColor(hdc, oldClr);
			}
		}
		rcDest.top = rcTxt.bottom + 1;
	}

	if (!IsWindow(lpmcdata->m_hCopyHwnd))
		return;

	RECT rcSrc{};
	GetClientRect(lpmcdata->m_hCopyHwnd, &rcSrc);

	HDC hdcSrc = GetDC(lpmcdata->m_hCopyHwnd);
	if (!hdcSrc)
		return;

	Auto(ReleaseDC(lpmcdata->m_hCopyHwnd, hdcSrc));

	// cant use DcxUXModule::dcxBeginBufferedPaint() etc.. as we need StretchBlt()
	if (auto hbm = CreateCompatibleBitmap(hdc, (rcSrc.right - rcSrc.left), (rcSrc.bottom - rcSrc.top)); hbm)
	{
		Auto(DeleteBitmap(hbm));

		if (auto hdcPaint = CreateCompatibleDC(hdc); hdcPaint)
		{
			Auto(DeleteDC(hdcPaint));

			auto oldbm = SelectBitmap(hdcPaint, hbm);
			Auto(SelectBitmap(hdcPaint, oldbm));

			const auto hOldFont = CopyHDCSettings(hdcSrc, hdcPaint);
			Auto(SelectFont(hdcPaint, hOldFont));

			SendMessage(lpmcdata->m_hCopyHwnd, WM_PRINT, reinterpret_cast<WPARAM>(hdcPaint), PRF_CLIENT | PRF_CHILDREN /*| PRF_NONCLIENT*/ | PRF_ERASEBKGND);

			SetGraphicsMode(hdc, GM_ADVANCED);
			SetStretchBltMode(hdc, HALFTONE);
			SetBrushOrgEx(hdc, 0, 0, nullptr);

			StretchBlt(hdc, rcDest.left, rcDest.top, (rcDest.right - rcDest.left), (rcDest.bottom - rcDest.top), hdcPaint, 0, 0, (rcSrc.right - rcSrc.left), (rcSrc.bottom - rcSrc.top), SRCCOPY);
		}
	}

}

//void Peek_SetFont(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//}
//
//void Peek_ParentNotify(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//	if (Dcx::dcxLOWORD(wParam) == WM_DESTROY)
//	{
//	}
//}
//
//void Peek_OnMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//}
//
//void Peek_ShowWindow(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return;
//
//}
//
//LRESULT Peek_Command(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
//{
//	if (!mHwnd)
//		return 0;
//
//	const auto lpmcdata = Peek_GetWindowData(mHwnd);
//	if (!lpmcdata)
//		return DefWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
//
//	return DefWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
//}

LRESULT Peek_SetSource(HWND mHwnd, HWND hSrc) noexcept
{
	if (!mHwnd)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	lpmcdata->m_hCopyHwnd = hSrc;
	if (IsWindow(lpmcdata->m_hCopyHwnd))
	{
		GetClientRect(lpmcdata->m_hCopyHwnd, &lpmcdata->m_rcSrc);
		MapWindowRect(lpmcdata->m_hCopyHwnd, nullptr, &lpmcdata->m_rcSrc);
	}
	else
		SetRectEmpty(&lpmcdata->m_rcSrc);

	InvalidateRect(mHwnd, nullptr, TRUE);
	//UpdateWindow(mHwnd);
	return TRUE;
}

LRESULT Peek_RemSource(HWND mHwnd) noexcept
{
	if (!mHwnd)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	lpmcdata->m_hCopyHwnd = nullptr;

	InvalidateRect(mHwnd, nullptr, TRUE);
	UpdateWindow(mHwnd);
	return TRUE;
}

LRESULT Peek_SetDescription(HWND mHwnd, LPCTSTR str, int length) noexcept
{
	if (!mHwnd || !str)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	if ((length < 0) || (gsl::narrow_cast<size_t>(length) > std::size(lpmcdata->m_Description)))
		length = std::size(lpmcdata->m_Description);

	_ts_strcpyn(&lpmcdata->m_Description[0], str, length);
	lpmcdata->m_Description[63] = 0;

	return TRUE;
}

LRESULT Peek_SetTitle(HWND mHwnd, LPCTSTR str, int length) noexcept
{
	if (!mHwnd || !str)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	if ((length < 0) || (gsl::narrow_cast<size_t>(length) > std::size(lpmcdata->m_Title)))
		length = std::size(lpmcdata->m_Title);

	_ts_strcpyn(&lpmcdata->m_Title[0], str, length);
	lpmcdata->m_Title[63] = 0;

	return TRUE;
}

LRESULT Peek_SetData(HWND mHwnd, pkData* pkd) noexcept
{
	if (!mHwnd || !pkd)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	if (dcx_testflag(pkd->m_dwMask, PCF_TITLE))
		Peek_SetTitle(mHwnd, &pkd->m_Title[0], -1);
	if (dcx_testflag(pkd->m_dwMask, PCF_DESC))
		Peek_SetDescription(mHwnd, &pkd->m_Description[0], -1);
	if (dcx_testflag(pkd->m_dwMask, PCF_MIN))
		Peek_SetMin(mHwnd, &pkd->m_szMin);
	if (dcx_testflag(pkd->m_dwMask, PCF_MAX))
		Peek_SetMax(mHwnd, &pkd->m_szMax);
	if (dcx_testflag(pkd->m_dwMask, PCF_HWND))
		Peek_SetSource(mHwnd, pkd->m_hSrc);
	if (dcx_testflag(pkd->m_dwMask, PCF_BKGCOLOUR))
		lpmcdata->m_clrBkg = pkd->m_clrBkg;
	if (dcx_testflag(pkd->m_dwMask, PCF_TITLECOLOUR))
		lpmcdata->m_clrTitle = pkd->m_clrTitle;
	if (dcx_testflag(pkd->m_dwMask, PCF_DESCCOLOUR))
		lpmcdata->m_clrDesc = pkd->m_clrDescription;
	if (dcx_testflag(pkd->m_dwMask, PCF_ROUNDED))
		lpmcdata->m_bRounded = pkd->m_bRoundedWindow;

	return TRUE;
}

LRESULT Peek_Show(HWND mHwnd, bool bShowHDC, int x, int y) noexcept
{
	if (!mHwnd)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	int w{}, h{};

	// we want to show hdc, but can only do so if window is valid.
	if (bShowHDC)
		bShowHDC = (!!IsWindow(lpmcdata->m_hCopyHwnd));

	if (bShowHDC)
	{
		w = std::clamp((lpmcdata->m_rcSrc.right - lpmcdata->m_rcSrc.left), lpmcdata->m_szMin.cx, lpmcdata->m_szMax.cx);
		h = std::clamp((lpmcdata->m_rcSrc.bottom - lpmcdata->m_rcSrc.top), lpmcdata->m_szMin.cy, lpmcdata->m_szMax.cy);
	}
	else {
		RECT rcTxt{};
		if (auto hdc = GetDC(mHwnd); hdc)
		{
			GetTextRect(hdc, &rcTxt, lpmcdata);

			ReleaseDC(mHwnd, hdc);

			MapWindowRect(mHwnd, nullptr, &rcTxt);
		}
		w = std::clamp((rcTxt.right - rcTxt.left), lpmcdata->m_szMin.cx, lpmcdata->m_szMax.cx);
		h = std::clamp((rcTxt.bottom - rcTxt.top), 24L, 30L) + PEEK_TXT_BOTTOM_PAD;
	}

	SetWindowPos(mHwnd, nullptr, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
	Peek_SetRounded(mHwnd, lpmcdata->m_bRounded);

	return TRUE;
}

LRESULT Peek_SetMin(HWND mHwnd, LPSIZE szMin) noexcept
{
	if (!mHwnd || !szMin)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	lpmcdata->m_szMin = *szMin;

	return TRUE;
}

LRESULT Peek_SetMax(HWND mHwnd, LPSIZE szMax) noexcept
{
	if (!mHwnd || !szMax)
		return FALSE;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return FALSE;

	lpmcdata->m_szMax = *szMax;

	return TRUE;
}

LRESULT Peek_SetRounded(HWND mHwnd, bool bRounded) noexcept
{
	HRGN hRgn{};

	if (bRounded)
	{
		RECT rcWin{};
		GetWindowRect(mHwnd, &rcWin);

		hRgn = CreateRoundRectRgn(0, 0, (rcWin.right - rcWin.left), (rcWin.bottom - rcWin.top), 5, 5);
	}

	SetWindowRgn(mHwnd, hRgn, TRUE);

	return TRUE;
}

void Peek_ResetContent(HWND mHwnd) noexcept
{
	if (!mHwnd)
		return;

	const auto lpmcdata = Peek_GetWindowData(mHwnd);
	if (!lpmcdata)
		return;

	lpmcdata->m_Description[0] = 0;
	lpmcdata->m_Title[0] = 0;
	lpmcdata->m_hCopyHwnd = nullptr;
}
