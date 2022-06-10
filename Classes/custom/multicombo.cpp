//
//

#include "defines.h"
#define BUILD_MCOMBO_CLASS 1
#include "multicombo.h"

LRESULT CALLBACK MultiComboDropWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		break;

	case WM_COMMAND:
	{
		return MultiCombo_Drop_Command(mHwnd, wParam, lParam);
	}
	break;

	//case WM_NOTIFY:
	//{
	//	dcxlParam(LPNMHDR, hdr);
//
	//	if (!hdr)
	//		break;
//
	//	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	//	if (!lpmcdata)
	//		break;
//
	//	if ((lpmcdata->m_hDropChild != hdr->hwndFrom) || (hdr->idFrom != MC_ID_DROPCHILD))
	//		break;
//
	//	switch (lpmcdata->m_Styles)
	//	{
	//	default:
	//		break;
	//	case MCS_LISTVIEW:
	//	{
	//		switch (hdr->code)
	//		{
	//		default:
	//			break;
	//		case NM_CLICK:
	//		{
	//			if (lpmcdata->m_CurrentEditColour)
	//			{
	//				DeleteBrush(lpmcdata->m_CurrentEditColour);
	//				lpmcdata->m_CurrentEditColour = nullptr;
	//				lpmcdata->m_CurrentEditTextColour = CLR_INVALID;
	//			}
//
	//			dcxlParam(LPNMITEMACTIVATE, nmia);
		//
	//			{
	//				TCHAR buf[1024]{};
	//				ListView_GetItemText(lpmcdata->m_hDropChild, nmia->iItem, nmia->iSubItem, &buf[0], std::size(buf));
//
	//				SetWindowText(lpmcdata->m_hEdit, &buf[0]);
	//			}
	//			// dunno about this, needs looked at...
//
	//			hdr->idFrom = lpmcdata->m_BaseID;
	//			hdr->hwndFrom = lpmcdata->m_hBase;
//
	//			return SendMessage(GetParent(lpmcdata->m_hBase), uMsg, wParam, lParam);
	//		}
	//		break;
//
	//		case NM_DBLCLK:
	//		{
	//		}
	//		break;
	//		}
	//	}
	//	break;
//
	//	case MCS_TREEVIEW:
	//	{
	//	}
	//	break;
	//	}
//
	//	// dunno about this, needs looked at...
//
	//	hdr->idFrom = lpmcdata->m_BaseID;
	//	hdr->hwndFrom = lpmcdata->m_hBase;
//
	//	return SendMessage(GetParent(lpmcdata->m_hBase), uMsg, wParam, lParam);
	//}
	//break;

	case WM_SHOWWINDOW:
	{
		MultiCombo_Drop_ShowWindow(mHwnd, wParam, lParam);
	}
	break;

	case WM_LBUTTONUP:
	{
		MultiCombo_OnLButtonUp(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	//case WM_MOUSEMOVE:
	//{
	//	MultiCombo_OnMouseMove(mHwnd, wParam, lParam);
	//}
	//break;

	case WM_MOUSELEAVE:
	{
		MultiCombo_OnMouseLeave(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	//case WM_CTLCOLORLISTBOX:
	//{
	//	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	//	if (!lpmcdata)
	//		break;
	//
	//	if ((lpmcdata->m_Styles == MCS_COLOUR) || (lpmcdata->m_Styles == MCS_LISTBOX))
	//	{
	//		//SetBkMode((HDC)wParam, TRANSPARENT);
	//
	//		COLORREF clr{ GetSysColor(COLOR_WINDOW) };
	//		if (lpmcdata->m_hTheme)
	//			Dcx::UXModule.dcxGetThemeColor(lpmcdata->m_hTheme, CP_BACKGROUND, 0, TMT_COLOR, &clr);
	//
	//		if (lpmcdata->m_CurrentEditTextColour != CLR_INVALID)
	//			SetTextColor((HDC)wParam, lpmcdata->m_CurrentEditTextColour);
	//
	//		if (lpmcdata->m_CurrentEditBkgColour != CLR_INVALID)
	//			SetBkColor((HDC)wParam, lpmcdata->m_CurrentEditBkgColour);
	//
	//		if (lpmcdata->m_CurrentEditColour)
	//			return (LRESULT)lpmcdata->m_CurrentEditColour;
	//	}
	//}
	//break;

	case WM_SIZE:
	{
		// size drop window child to match drop window.
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			break;

		if (IsWindow(lpmcdata->m_hDropChild))
		{
			// have child window, size it...

			// doesnt size things right...
			//const auto width = Dcx::dcxLOWORD(lParam);
			//const auto height = Dcx::dcxHIWORD(lParam);
			//SetWindowPos(lpmcdata->m_hDropChild, nullptr, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER);

			if (RECT rcClient{}; GetClientRect(mHwnd, &rcClient))
			{
				const auto width = rcClient.right;
				const auto height = rcClient.bottom;
				SetWindowPos(lpmcdata->m_hDropChild, nullptr, 0, 0, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
			}
		}
	}
	break;

	case WM_ACTIVATEAPP:
	{
		// hide drop window when switching apps...
		if (wParam)
			break;

		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			break;

		if (IsWindowVisible(lpmcdata->m_hDropCtrl))
			ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
	}
	break;

	case WM_DRAWITEM:
	{
		dcxlParam(LPDRAWITEMSTRUCT, lpdis);

		if (!lpdis || lpdis->itemID == -1 || wParam == 0)
			break;

		return MultiCombo_DrawItem(mHwnd, lpdis);
	}
	break;

	case WM_MEASUREITEM:
	{
		dcxlParam(LPMEASUREITEMSTRUCT, lpmis);

		if (!lpmis)
			break;

		return MultiCombo_MeasureItem(mHwnd, lpmis);
	}
	break;
	case WM_DELETEITEM:
	{
		dcxlParam(PDELETEITEMSTRUCT, delis);

		if (!delis)
			break;

		auto lpdcxcci = reinterpret_cast<LPMCOMBO_ITEM>(delis->itemData);

		delete lpdcxcci;

		return TRUE;

	}
	break;

	case WM_VKEYTOITEM:
	{
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			break;

		if (reinterpret_cast<HWND>(lParam) != lpmcdata->m_hDropChild)
			break;

		switch (const auto vkChar = Dcx::dcxLOWORD(wParam); vkChar)
		{
		default:
			break;
		case VK_RETURN:
		{
			ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
		}
		break;
		case VK_TAB:
		{
			ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);

			//if (dcx_testflag(dcxGetWindowStyle(lpmcdata->m_hBase), WS_TABSTOP))
			//{
			//	//FindWindowEx();
			//	auto hDlgr = GetAncestor(lpmcdata->m_hBase, GA_ROOTOWNER);
			//	auto hDlg = GetWindow(lpmcdata->m_hBase, GW_OWNER);
			//	auto hNext = GetNextDlgTabItem(hDlgr, lpmcdata->m_hBase, FALSE);
			//	if (hNext)
			//		SetFocus(hNext);
			//}

			//INPUT in{};
			//in.type = INPUT_KEYBOARD;
			//in.ki.wVk = VK_TAB;
			//SendInput(1, &in, sizeof(INPUT));
		}
		break;
		}
		return -1;
	}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MultiComboWndProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		break;
	case WM_CREATE:
	{
		return MultiCombo_OnCreate(mHwnd, wParam, lParam);
	}
	break;

	case WM_SIZE:
	{
		MultiCombo_SizeBaseWindowContents(mHwnd, Dcx::dcxLOWORD(lParam), Dcx::dcxHIWORD(lParam));
		return 0L;
	}
	break;

	case WM_MOVE:
	{
		MultiCombo_OnMove(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_LBUTTONDOWN:
	{
		MultiCombo_OnLButtonDown(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_LBUTTONUP:
	{
		MultiCombo_OnLButtonUp(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_MOUSEMOVE:
	{
		MultiCombo_OnMouseMove(mHwnd, wParam, lParam);
		return 0;
	}
	break;

	case WM_MOUSELEAVE:
	{
		MultiCombo_OnMouseLeave(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_THEMECHANGED:
	{
		MultiCombo_OnThemeChange(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_PAINT:
	{
		MultiCombo_OnPaint(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_PRINTCLIENT:
	{
		MultiCombo_OnPaint(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_SETFONT:
	{
		MultiCombo_SetFont(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_GETTEXT:
	case WM_GETTEXTLENGTH:
	case WM_SETTEXT:
	{
		if (const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data")); lpmcdata)
			return SendMessage(lpmcdata->m_hEdit, uMsg, wParam, lParam);
	}
	break;

	case WM_SETCURSOR:
	{
		if ((Dcx::dcxLOWORD(lParam) != HTCLIENT) || (reinterpret_cast<HWND>(wParam) != mHwnd))
			break;

		if (const HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW); GetCursor() != hCursor)
			SetCursor(hCursor);
		return TRUE;
	}
	break;

	case WM_SETFOCUS:
	{
		MultiCombo_SetFocus(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_SHOWWINDOW:
	{
		MultiCombo_ShowWindow(mHwnd, wParam, lParam);
	}
	break;

	case WM_ERASEBKGND:
	{
		// wParam == HDC

		// paints using supplied hdc in wParam, this avoids flicker when sizing...
		MultiCombo_OnPaint(mHwnd, wParam, lParam);
		return 1L;
	}
	break;

	case WM_CTLCOLOREDIT:
	{
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			break;

		if (lpmcdata->m_Styles == MCS_COLOUR)
		{
			dcxwParam(HDC, hdc);

			SetBkMode(hdc, TRANSPARENT);

			if (lpmcdata->m_CurrentEditTextColour != CLR_INVALID)
				SetTextColor(hdc, lpmcdata->m_CurrentEditTextColour);

			if (lpmcdata->m_CurrentEditBkgColour != CLR_INVALID)
				SetBkColor(hdc, lpmcdata->m_CurrentEditBkgColour);

			if (lpmcdata->m_CurrentEditColour)
				return (LRESULT)lpmcdata->m_CurrentEditColour;
		}
	}
	break;

	case WM_PARENTNOTIFY:
	{
		MultiCombo_ParentNotify(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case WM_COMMAND:
	{
		return MultiCombo_Command(mHwnd, wParam, lParam);
	}
	break;

	case WM_DESTROY:
	{
		if (const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data")); lpmcdata)
		{
			if (lpmcdata->m_hTheme)
			{
				Dcx::UXModule.dcxCloseThemeData(lpmcdata->m_hTheme);
				lpmcdata->m_hTheme = nullptr;
			}
			if (lpmcdata->m_CurrentEditColour)
			{
				DeleteBrush(lpmcdata->m_CurrentEditColour);
				lpmcdata->m_CurrentEditColour = nullptr;
			}
			delete lpmcdata;
		}

		RemoveProp(mHwnd, TEXT("mc_data"));

		return 0L;
	}
	break;

	case MC_WM_ADDCHILD:
	{
		// wParam = zero
		// lParam = HWND of child
		MultiCombo_RemoveChild(mHwnd, wParam, lParam);

		if (dcxlParam(HWND,hChild); IsWindow(hChild))
		{
			const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
			if (!lpmcdata)
				return 0L;

			lpmcdata->m_hDropChild = hChild;
			SetParent(lpmcdata->m_hDropChild, lpmcdata->m_hDropCtrl);
			MultiCombo_AdjustDropRectPos(mHwnd, lpmcdata);
			SetWindowPos(lpmcdata->m_hDropChild, nullptr, 0, 0, (lpmcdata->m_rcDrop.right - lpmcdata->m_rcDrop.left), (lpmcdata->m_rcDrop.bottom - lpmcdata->m_rcDrop.top), SWP_NOACTIVATE);
		}
		return 0L;
	}
	break;

	case MC_WM_REMCHILD:
	{
		// wParam = zero
		// lParam = zero
		MultiCombo_RemoveChild(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case MC_WM_GETDROPRECT:
	{
		// not finished...
		// wParam = zero
		// lParam = zero
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			return 0L;

		RECT rc = lpmcdata->m_rcDrop;
		MapWindowRect(nullptr, mHwnd, &rc);

		return MAKELRESULT((rc.right - rc.left), (rc.bottom - rc.top));
	}
	break;

	case MC_WM_SETDROPRECT:
	{
		// not finished...
		// wParam = width
		// lParam = height
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			return 0L;

		RECT rc{ 0,0, gsl::narrow_cast<LONG>(wParam), lParam };
		MapWindowRect(mHwnd, nullptr, &rc);
		lpmcdata->m_rcDrop = rc;

		return 0L;
	}
	break;

	case MC_WM_UPDATEDROPRECT:
	{
		// Should be called when parent window is moved.
		// 
		// wParam = zero
		// lParam = zero
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			return 0L;

		if (!IsWindowVisible(lpmcdata->m_hDropCtrl))
			return 0l;

		MultiCombo_AdjustDropRectPos(mHwnd, lpmcdata);
		if (IsWindow(lpmcdata->m_hDropCtrl))
			MoveWindow(lpmcdata->m_hDropCtrl, lpmcdata->m_rcDrop.left, lpmcdata->m_rcDrop.top, (lpmcdata->m_rcDrop.right - lpmcdata->m_rcDrop.left), (lpmcdata->m_rcDrop.bottom - lpmcdata->m_rcDrop.top), FALSE);
		return 0L;
	}
	break;

	case MC_WM_FILLCOLOURS:
	{
		// wParam = count of items to add
		// lParam = array of colours to add. COLOURREF[wParam]
		return MultiCombo_FillColours(mHwnd, wParam, lParam);
	}
	break;

	case MC_WM_SHOWDROP:
	{
		// wParam = 1 or 0
		// lParam = zero
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			return 0L;

		if (IsWindow(lpmcdata->m_hDropCtrl))
		{
			if (wParam)
				ShowWindow(lpmcdata->m_hDropCtrl, SW_SHOW);
			else
				ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
		}
		return 0L;
	}
	break;

	case MC_WM_GETDROPSTATE:
	{
		// wParam = zero
		// lParam = zero
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			return -1L;

		if (!IsWindow(lpmcdata->m_hDropCtrl))
			return -1L;

		if (dcx_testflag(dcxGetWindowStyle(lpmcdata->m_hDropCtrl), WS_VISIBLE))
			return 1L;

		return 0L;
	}
	break;

	case MC_WM_GETITEMCOUNT:
	{
		// wParam = zero
		// lParam = zero
		return MultiCombo_GetItemCount(mHwnd);
	}
	break;

	case MC_WM_GETITEM:
	{
		// wParam = item
		// lParam = storage struct for type
		return MultiCombo_GetItem(mHwnd, wParam, lParam);
	}
	break;

	case MC_WM_GETSEL:
	{
		// wParam = zero
		// lParam = zero
		return MultiCombo_GetSelected(mHwnd);
	}
	break;

	case MC_WM_GETSTYLE:
	{
		// wParam = zero
		// lParam = zero
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		if (!lpmcdata)
			return -1;

		return lpmcdata->m_Styles;
	}
	break;

	case MC_WM_GETCOMBOINFO:
	{
		// wParam = zero
		// lParam = zero
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
		return reinterpret_cast<LRESULT>(lpmcdata);
	}
	break;

	case MC_WM_ADDITEM:
	{
		// wParam = zero
		// lParam = zero
		MultiCombo_AddItem(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case MC_WM_INSERTITEM:
	{
		// wParam = zero
		// lParam = zero
		MultiCombo_InsertItem(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case MC_WM_RESETCONTENT:
	{
		// wParam = zero
		// lParam = zero
		MultiCombo_ResetContent(mHwnd);
		return 0L;
	}
	break;

	case MC_WM_SETSEL:
	{
		// wParam = 1 or 0 to select or deselect
		// lParam = item
		MultiCombo_SetSel(mHwnd, wParam, lParam);
		return 0L;
	}
	break;

	case MC_WM_DELETEITEM:
	{
		// wParam = item
		// lParam = zero
		MultiCombo_DeleteItem(mHwnd, wParam);
		return 0L;
	}
	break;

	case MC_WM_SETITEM:
	{
		// wParam = item
		// lParam = item_data
		MultiCombo_SetItem(mHwnd, wParam, lParam);
		return 0L;
	}
	break;
	}
	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

void MultiCombo_SizeBaseWindowContents(HWND mHwnd, WORD cx, WORD cy) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	const RECT rc{ 0,0,cx,cy };

	const RECT rcEdit = MultiCombo_GetEditRect(&rc);
	if (IsWindow(lpmcdata->m_hEdit))
		MoveWindow(lpmcdata->m_hEdit, rcEdit.left, rcEdit.top, (rcEdit.right - rcEdit.left), (rcEdit.bottom - rcEdit.top), TRUE);

	lpmcdata->m_rcButton = MultiCombo_GetButtonRect(&rc);

	MultiCombo_AdjustDropRectPos(mHwnd, lpmcdata);

	if (IsWindow(lpmcdata->m_hDropCtrl))
		MoveWindow(lpmcdata->m_hDropCtrl, lpmcdata->m_rcDrop.left, lpmcdata->m_rcDrop.top, (lpmcdata->m_rcDrop.right - lpmcdata->m_rcDrop.left), (lpmcdata->m_rcDrop.bottom - lpmcdata->m_rcDrop.top), FALSE);
}

void MultiCombo_OnLButtonDown(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	lpmcdata->m_bButtonHotTracked = false;
	lpmcdata->m_bButtonPressed = true;

	InvalidateRect(mHwnd, &lpmcdata->m_rcButton, FALSE);
}

void MultiCombo_OnLButtonUp(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	//lpmcdata->m_bButtonPressed = !lpmcdata->m_bButtonPressed;

	lpmcdata->m_bButtonPressed = false;

	if (!IsWindowVisible(lpmcdata->m_hDropCtrl))
	{
		MultiCombo_AdjustDropRectPos(mHwnd, lpmcdata);

		SetWindowPos(lpmcdata->m_hDropCtrl, HWND_TOPMOST, lpmcdata->m_rcDrop.left, lpmcdata->m_rcDrop.top, (lpmcdata->m_rcDrop.right - lpmcdata->m_rcDrop.left), (lpmcdata->m_rcDrop.bottom - lpmcdata->m_rcDrop.top), SWP_NOACTIVATE);
		ShowWindow(lpmcdata->m_hDropCtrl, SW_SHOWNA);
	}
	else {
		ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
	}

	InvalidateRect(mHwnd, &lpmcdata->m_rcButton, FALSE);
}

void MultiCombo_OnMouseMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	lpmcdata->m_bButtonHotTracked = true;

	if (!lpmcdata->m_Tracking)
	{
		TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT), TME_LEAVE, mHwnd, HOVER_DEFAULT };

		lpmcdata->m_Tracking = _TrackMouseEvent(&tme);

		InvalidateRect(mHwnd, &lpmcdata->m_rcButton, FALSE);
	}
}

void MultiCombo_OnMouseLeave(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	lpmcdata->m_bButtonHotTracked = false;
	lpmcdata->m_Tracking = FALSE;

	InvalidateRect(mHwnd, &lpmcdata->m_rcButton, FALSE);
}

void MultiCombo_SetFocus(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (lpmcdata->m_hEdit)
		SetFocus(lpmcdata->m_hEdit);
}

void MultiCombo_OnThemeChange(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (lpmcdata->m_hTheme)
	{
		Dcx::UXModule.dcxCloseThemeData(lpmcdata->m_hTheme);
		lpmcdata->m_hTheme = nullptr;
	}
	Dcx::UXModule.dcxSetWindowTheme(lpmcdata->m_hEdit, L" ", L" ");
	Dcx::UXModule.dcxSetWindowTheme(lpmcdata->m_hDropCtrl, L" ", L" ");
}

RECT MultiCombo_GetEditRect(LPCRECT rcBase) noexcept
{
	RECT rc{ *rcBase };

	rc.right -= MCOMBO_EDIT_WIDTH;

	return rc;
}

RECT MultiCombo_GetButtonRect(LPCRECT rcBase) noexcept
{
	RECT rc{ *rcBase };

	rc.left = rc.right - MCOMBO_EDIT_WIDTH;

	return rc;
}

RECT MultiCombo_GetDropRect(HWND mHwnd, UINT mID) noexcept
{
	RECT rc{};
	MEASUREITEMSTRUCT mis{};

	mis.CtlType = MCOMBO_TYPE;
	mis.CtlID = mID;
	mis.itemID = MC_ID_DROP;

	SendMessage(mHwnd, WM_MEASUREITEM, mis.CtlID, reinterpret_cast<LPARAM>(&mis));

	rc.right = std::max<UINT>(mis.itemWidth, 20U);
	rc.bottom = std::max<UINT>(mis.itemHeight, 20U);

	MapWindowRect(mHwnd, nullptr, &rc);

	return rc;
}

LRESULT MultiCombo_OnCreate(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	dcxlParam(LPCREATESTRUCT,cs);

	const RECT rc{ 0,0,cs->x + cs->cx,cs->y + cs->cy };

	{
		auto lpmcdata = std::make_unique<MCOMBO_DATA>();

		lpmcdata->m_BaseID = reinterpret_cast<UINT>(cs->hMenu);

		const RECT rcEdit = MultiCombo_GetEditRect(&rc);
		lpmcdata->m_rcButton = MultiCombo_GetButtonRect(&rc);
		lpmcdata->m_rcDrop = MultiCombo_GetDropRect(cs->hwndParent, lpmcdata->m_BaseID);

		lpmcdata->m_hBase = mHwnd;
		lpmcdata->m_Styles = cs->style & MCS_STYLEMASK;
		lpmcdata->m_hTheme = Dcx::UXModule.dcxOpenThemeData(mHwnd, WC_COMBOBOXW);

		// ES_NOHIDESEL

		UINT dStyle{ WS_CHILD | WS_VISIBLE };
		if (lpmcdata->m_Styles == MCS_COLOUR)
			dStyle |= ES_CENTER /*| ES_READONLY*/;
		else
			dStyle |= ES_AUTOHSCROLL;
		if (dcx_testflag(cs->style, WS_TABSTOP))
			dStyle |= WS_TABSTOP;

		lpmcdata->m_hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, WC_EDIT, nullptr, dStyle, rcEdit.left, rcEdit.top, (rcEdit.right - rcEdit.left), (rcEdit.bottom - rcEdit.top), mHwnd, (HMENU)MC_ID_EDIT, cs->hInstance, nullptr);

		{
			WNDCLASSEX wc{};
			wc.cbSize = sizeof(WNDCLASSEX);
			if (!GetClassInfoEx(cs->hInstance, MCOMBO_DROPCLASS, &wc))
			{
				wc.cbSize = sizeof(WNDCLASSEX);
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = MultiComboDropWndProc;
				wc.cbClsExtra = 0;
				wc.cbWndExtra = 0;
				wc.hInstance = cs->hInstance;
				wc.hIcon = nullptr;
				wc.hCursor = nullptr;
				wc.hbrBackground = GetStockBrush(COLOR_WINDOWFRAME);
				wc.lpszMenuName = nullptr;
				wc.lpszClassName = MCOMBO_DROPCLASS;
				wc.hIconSm = nullptr;
				RegisterClassEx(&wc);
			}

			lpmcdata->m_hDropCtrl = CreateWindowExW(WS_EX_NOACTIVATE, MCOMBO_DROPCLASS, nullptr, WS_POPUP | WS_CLIPCHILDREN, lpmcdata->m_rcDrop.left, lpmcdata->m_rcDrop.top, (lpmcdata->m_rcDrop.right - lpmcdata->m_rcDrop.left), (lpmcdata->m_rcDrop.bottom - lpmcdata->m_rcDrop.top), mHwnd, nullptr, cs->hInstance, nullptr);
		}

		if (!lpmcdata->m_hDropCtrl || !lpmcdata->m_hEdit)
		{
			return -1L;
		}

		Dcx::UXModule.dcxSetWindowTheme(lpmcdata->m_hEdit, WC_COMBOBOXW, nullptr);
		Dcx::UXModule.dcxSetWindowTheme(lpmcdata->m_hDropCtrl, WC_COMBOBOXW, nullptr);

		SetProp(lpmcdata->m_hDropCtrl, TEXT("mc_data"), lpmcdata.get());

		SetProp(mHwnd, TEXT("mc_data"), lpmcdata.release());
	}
	{
		const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));

		switch (lpmcdata->m_Styles)
		{
		default:
			break;

		case MCS_COLOUR:
			lpmcdata->m_hDropChild = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTBOX, nullptr, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT | LBS_OWNERDRAWFIXED | LBS_MULTICOLUMN, 0, 0, (rc.right - rc.left), (rc.bottom - lpmcdata->m_rcDrop.top), lpmcdata->m_hDropCtrl, (HMENU)MC_ID_DROPCHILD, cs->hInstance, nullptr);
			break;

		case MCS_LISTBOX:
			lpmcdata->m_hDropChild = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTBOX, nullptr, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT | LBS_HASSTRINGS, 0, 0, (rc.right - rc.left), (rc.bottom - lpmcdata->m_rcDrop.top), lpmcdata->m_hDropCtrl, (HMENU)MC_ID_DROPCHILD, cs->hInstance, nullptr);
			break;

			//case MCS_LISTVIEW:
			//	lpmcdata->m_hDropChild = CreateWindowExW(WS_EX_CLIENTEDGE, WC_LISTVIEW, nullptr, WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, 0, 0, (rc.right - rc.left), (rc.bottom - lpmcdata->m_rcDrop.top), lpmcdata->m_hDropCtrl, (HMENU)MC_ID_DROPCHILD, cs->hInstance, nullptr);
			//	break;
			//case MCS_TREEVIEW:
			//	lpmcdata->m_hDropChild = CreateWindowExW(WS_EX_CLIENTEDGE, WC_TREEVIEW, nullptr, WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS, 0, 0, (rc.right - rc.left), (rc.bottom - lpmcdata->m_rcDrop.top), lpmcdata->m_hDropCtrl, (HMENU)MC_ID_DROPCHILD, cs->hInstance, nullptr);
			//	break;
		}

		if (lpmcdata->m_hDropChild)
			Dcx::UXModule.dcxSetWindowTheme(lpmcdata->m_hDropChild, WC_COMBOBOXW, nullptr);
	}

	return 0L;
}

void MultiCombo_OnPaint(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata || !mHwnd)
		return;

	// maybe add support for HDC being supplied in wParam

	//PAINTSTRUCT ps{};
	//auto hdc = BeginPaint(mHwnd, &ps);
	//
	//if (lpmcdata->m_hTheme)
	//{
	//	int iStateId{ CBXS_NORMAL };
	//
	//	if (lpmcdata->m_bButtonPressed)
	//	{
	//		iStateId = CBXS_PRESSED;
	//	}
	//	else if (!IsWindowEnabled(mHwnd))
	//	{
	//		iStateId = CBXS_DISABLED;
	//	}
	//	else if (lpmcdata->m_bButtonHotTracked)
	//	{
	//		iStateId = CBXS_HOT;
	//	}
	//
	//	Dcx::UXModule.dcxDrawThemeBackground(lpmcdata->m_hTheme, hdc, CP_DROPDOWNBUTTON, iStateId, &lpmcdata->m_rcButton, 0);
	//}
	//else {
	//	DrawFrameControl(hdc, &lpmcdata->m_rcButton, DFC_SCROLL,
	//		DFCS_SCROLLCOMBOBOX |
	//		(lpmcdata->m_bButtonPressed ? DFCS_PUSHED | DFCS_FLAT : 0) |
	//		(dcx_testflag(dcxGetWindowStyle(mHwnd), WS_DISABLED) ? DFCS_INACTIVE : 0) |
	//		(lpmcdata->m_bButtonHotTracked ? DFCS_HOT : 0));
	//}
	//EndPaint(mHwnd, &ps);

	dcxwParam(HDC,hdc);

	PAINTSTRUCT ps{};

	if (!wParam)
		hdc = BeginPaint(mHwnd, &ps);

	if (!hdc)
		return;

	if (lpmcdata->m_hTheme)
	{
		int iStateId{ CBXS_NORMAL };

		if (lpmcdata->m_bButtonPressed)
		{
			iStateId = CBXS_PRESSED;
		}
		else if (!IsWindowEnabled(mHwnd))
		{
			iStateId = CBXS_DISABLED;
		}
		else if (lpmcdata->m_bButtonHotTracked)
		{
			iStateId = CBXS_HOT;
		}

		Dcx::UXModule.dcxDrawThemeBackground(lpmcdata->m_hTheme, hdc, CP_DROPDOWNBUTTON, iStateId, &lpmcdata->m_rcButton, nullptr);
	}
	else {
		DrawFrameControl(hdc, &lpmcdata->m_rcButton, DFC_SCROLL,
			DFCS_SCROLLCOMBOBOX |
			(lpmcdata->m_bButtonPressed ? DFCS_PUSHED | DFCS_FLAT : 0) |
			(dcx_testflag(dcxGetWindowStyle(mHwnd), WS_DISABLED) ? DFCS_INACTIVE : 0) |
			(lpmcdata->m_bButtonHotTracked ? DFCS_HOT : 0));
	}
	if (!wParam)
		EndPaint(mHwnd, &ps);
}

void MultiCombo_SetFont(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (IsWindow(lpmcdata->m_hEdit))
		SendMessage(lpmcdata->m_hEdit, WM_SETFONT, wParam, lParam);

	if (IsWindow(lpmcdata->m_hDropCtrl))
		SendMessage(lpmcdata->m_hDropCtrl, WM_SETFONT, wParam, lParam);

	if (IsWindow(lpmcdata->m_hDropChild))
		SendMessage(lpmcdata->m_hDropChild, WM_SETFONT, wParam, lParam);
}

void MultiCombo_ParentNotify(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (Dcx::dcxLOWORD(wParam) == WM_DESTROY)
	{
		if (dcxlParam(HWND, hwnd); hwnd == lpmcdata->m_hEdit)
		{
			lpmcdata->m_hEdit = nullptr;
		}
		else if (hwnd == lpmcdata->m_hDropCtrl)
		{
			lpmcdata->m_hDropCtrl = nullptr;
			lpmcdata->m_hDropChild = nullptr;
		}
	}
}

void MultiCombo_AdjustDropRectPos(HWND mHwnd, LPMCOMBO_DATA lpmcdata) noexcept
{
	if (!mHwnd || !lpmcdata)
		return;

	//if (!IsWindowVisible(lpmcdata->m_hDropCtrl))
	//	return;

	HWND hParent = GetParent(mHwnd);

	const RECT rcDrop = MultiCombo_GetDropRect(hParent, lpmcdata->m_BaseID);

	RECT rcWin{};
	GetWindowRect(mHwnd, &rcWin);

	lpmcdata->m_rcDrop.left = rcWin.left;
	lpmcdata->m_rcDrop.right = /*rcWin.left + */rcDrop.right;
	lpmcdata->m_rcDrop.top = rcWin.bottom;
	lpmcdata->m_rcDrop.bottom = /*rcWin.bottom + */rcDrop.bottom;

	//OffsetRect(&lpmcdata->m_rcDrop, rcWin.left, rcWin.bottom);
}

void MultiCombo_AdjustDropRectPos(HWND mHwnd) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	MultiCombo_AdjustDropRectPos(mHwnd, lpmcdata);
}

void MultiCombo_OnMove(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (IsWindowVisible(lpmcdata->m_hDropCtrl))
		MultiCombo_AdjustDropRectPos(mHwnd, lpmcdata);
}

void MultiCombo_Drop_ShowWindow(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (wParam)
	{
		//TRACKMOUSEEVENT tme{};
		//tme.cbSize = sizeof(TRACKMOUSEEVENT);
		//tme.hwndTrack = mHwnd;
		//tme.dwFlags = TME_LEAVE;
		//_TrackMouseEvent(&tme);
		//
		//SetCapture(mHwnd);
		//
		//lpmcdata->m_bButtonHotTracked = true;
		//
		//if (dcx_testflag(lpmcdata->m_Styles, MCS_COLOUR))
		//	SendMessage(lpmcdata->m_hDropChild, LBCB_STARTTRACK, TRUE, 0);
	}
	else {
		//if (lpmcdata->m_bButtonHotTracked)
		//	ReleaseCapture();
		//lpmcdata->m_bButtonHotTracked = false;
		//lpmcdata->m_bButtonPressed = false;
		//
		//SendMessage(lpmcdata->m_hDropChild, LBCB_ENDTRACK, FALSE, 0);
	}
}

void MultiCombo_ShowWindow(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if ((!wParam) && (IsWindow(lpmcdata->m_hDropCtrl)))
		ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
}

void MultiCombo_RemoveChild(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	HWND hChild = lpmcdata->m_hDropChild;
	lpmcdata->m_hDropChild = nullptr;
	lpmcdata->m_Styles &= ~(MCS_STYLEMASK);

	if (IsWindow(hChild))
		DestroyWindow(hChild);
}

BOOL MultiCombo_DrawItem(HWND mHwnd, LPDRAWITEMSTRUCT lpdis)
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return FALSE;

	const auto lpdcxcci = reinterpret_cast<LPMCOMBO_ITEM>(lpdis->itemData);

	if (!lpdcxcci)
		return FALSE;

	if (lpmcdata->m_Styles == MCS_COLOUR)
	{
		// display colour grid
		// child control is a multi-column listbox

		const auto savedDC = SaveDC(lpdis->hDC);
		Auto(RestoreDC(lpdis->hDC, savedDC));

		const auto hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		Auto(DeleteObject(hPen));

		const auto oldPen = Dcx::dcxSelectObject<HPEN>(lpdis->hDC, hPen);
		Auto(Dcx::dcxSelectObject<HPEN>(lpdis->hDC, oldPen));

		auto rcItem = lpdis->rcItem;

		// draw selection indicator
		{
			COLORREF clr{ GetSysColor(COLOR_WINDOW) };
			if (lpmcdata->m_hTheme)
				Dcx::UXModule.dcxGetThemeColor(lpmcdata->m_hTheme, CP_BACKGROUND, 0, TMT_COLOR, &clr);

			if (dcx_testflag(lpdis->itemState, ODS_SELECTED))
				clr = GetSysColor(COLOR_MENUHILIGHT);

			SetBkColor(lpdis->hDC, clr);
		}

		ExtTextOut(lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), NULL, nullptr);

		InflateRect(&rcItem, -4, -2);

		SetBkColor(lpdis->hDC, lpdcxcci->m_clrItem);

		ExtTextOut(lpdis->hDC, rcItem.left, rcItem.top, ETO_CLIPPED | ETO_OPAQUE, &rcItem, TEXT(""), NULL, nullptr);

		TString txt(lpdcxcci->m_tsItemText);

		if (!txt.empty())
		{
			// set text colour so it will contrast nicely with the item colour.
			if (lpdcxcci->m_clrText == CLR_INVALID)
				lpdcxcci->m_clrText = GetContrastColour(lpdcxcci->m_clrItem);
			SetTextColor(lpdis->hDC, lpdcxcci->m_clrText);

			//this->ctrlDrawText(lpdis->hDC, txt, &rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			DrawText(lpdis->hDC, txt.to_chr(), txt.len(), &rcItem, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
		}

		MoveToEx(lpdis->hDC, rcItem.left, rcItem.top, nullptr);
		LineTo(lpdis->hDC, rcItem.right, rcItem.top);

		MoveToEx(lpdis->hDC, rcItem.right, rcItem.top, nullptr);
		LineTo(lpdis->hDC, rcItem.right, rcItem.bottom);

		MoveToEx(lpdis->hDC, rcItem.right, rcItem.bottom, nullptr);
		LineTo(lpdis->hDC, rcItem.left, rcItem.bottom);

		MoveToEx(lpdis->hDC, rcItem.left, rcItem.bottom, nullptr);
		LineTo(lpdis->hDC, rcItem.left, rcItem.top);
	}

	return TRUE;
}

BOOL MultiCombo_MeasureItem(HWND mHwnd, LPMEASUREITEMSTRUCT lpmis) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return FALSE;

	if (lpmcdata->m_Styles == MCS_COLOUR)
	{
		lpmis->itemHeight = 16;
		lpmis->itemWidth = 16 * 2;
	}

	return TRUE;
}

BOOL MultiCombo_FillColours(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	// wParam = count
	// lParam = array of COLORREFS
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return FALSE;

	if ((lpmcdata->m_Styles != MCS_COLOUR) || !IsWindow(lpmcdata->m_hDropChild))
		return FALSE;

	const auto clrs = reinterpret_cast<COLORREF*>(lParam);
	for (UINT i{}; i < wParam; i++)
	{
		LPMCOMBO_ITEM lpdata = new MCOMBO_ITEM;

		lpdata->m_clrItem = clrs[i];
		lpdata->m_tsItemText.addtok(i);

		ListBox_AddItemData(lpmcdata->m_hDropChild, lpdata);
	}
	return TRUE;
}

int MultiCombo_GetItemCount(HWND mHwnd) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return UINT_MAX;

	if (IsWindow(lpmcdata->m_hDropChild))
	{
		switch (lpmcdata->m_Styles)
		{
		default:
			break;
		case MCS_COLOUR:
		case MCS_LISTBOX:
			return gsl::narrow_cast<UINT>(ListBox_GetCount(lpmcdata->m_hDropChild));
			//case MCS_LISTVIEW:
			//	return gsl::narrow_cast<UINT>(ListView_GetItemCount(lpmcdata->m_hDropChild));
			//case MCS_TREEVIEW:
			//	return gsl::narrow_cast<UINT>(TreeView_GetCount(lpmcdata->m_hDropChild));
		}
	}

	return UINT_MAX;
}

int MultiCombo_GetSelected(HWND mHwnd) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return UINT_MAX;

	if (IsWindow(lpmcdata->m_hDropChild))
	{
		switch (lpmcdata->m_Styles)
		{
		default:
			break;
		case MCS_COLOUR:
		case MCS_LISTBOX:
			return gsl::narrow_cast<UINT>(ListBox_GetCurSel(lpmcdata->m_hDropChild));
			//case MCS_LISTVIEW:
			//{
			//	if (ListView_GetSelectedCount(lpmcdata->m_hDropChild) < 1)
			//		return UINT_MAX;
			//	return ListView_GetNextItem(lpmcdata->m_hDropChild, -1, LVNI_SELECTED);
			//}
			//break;
		}
	}

	return UINT_MAX;
}

BOOL MultiCombo_GetItem(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return FALSE;

	// wParam = item number
	// -1 = edit control
	// 0+ = listbox

	if (IsWindow(lpmcdata->m_hDropChild))
	{
		switch (lpmcdata->m_Styles)
		{
		default:
			break;

		case MCS_COLOUR:
		{
			// lParam = LPMCOMBO_ITEM
			dcxlParam(LPMCOMBO_ITEM, lpresult);
			if ((lpresult->m_Size != sizeof(MCOMBO_ITEM)) || (lpresult->m_Type != MCS_COLOUR))
				return FALSE;

			if (wParam == -1) // get edit control contents...
			{
				lpresult->m_clrItem = lpmcdata->m_CurrentEditBkgColour;
				lpresult->m_clrText = lpmcdata->m_CurrentEditTextColour;

				if (IsWindow(lpmcdata->m_hEdit))
					lpresult->m_tsItemText = TGetWindowText(lpmcdata->m_hEdit);
			}
			else if (auto lpdata = reinterpret_cast<LPMCOMBO_ITEM>(ListBox_GetItemData(lpmcdata->m_hDropChild, wParam)); lpdata)
			{
				*lpresult = *lpdata;
			}
			return TRUE;
		}
		break;

		case MCS_LISTBOX:
		{
			// lParam = LPMCOMBO_ITEM
			dcxlParam(LPMCOMBO_ITEM, lpresult);
			if ((lpresult->m_Size != sizeof(MCOMBO_ITEM)) || (lpresult->m_Type != MCS_COLOUR))
				return FALSE;

			lpresult->m_tsItemText.clear();
			if (wParam == -1) // get edit control contents...
			{
				if (IsWindow(lpmcdata->m_hEdit))
					lpresult->m_tsItemText = TGetWindowText(lpmcdata->m_hEdit);
			}
			else {
				if (const auto len = ListBox_GetTextLen(lpmcdata->m_hDropChild, wParam); len > 0)
				{
					lpresult->m_tsItemText.reserve(len + 1);
					ListBox_GetText(lpmcdata->m_hDropChild, wParam, lpresult->m_tsItemText.to_chr());
				}
			}
			return TRUE;
		}
		break;

		//case MCS_LISTVIEW:
		//{
		//	// lParam = LPMCOMBO_LISTVIEWITEM
		//	auto lpresult = reinterpret_cast<LPMCOMBO_LISTVIEWITEM>(lParam);
		//	if ((lpresult->m_Size != sizeof(MCOMBO_LISTVIEWITEM)) || (lpresult->m_Type != MCS_LISTVIEW))
		//		return FALSE;
		//
		//	ListView_GetItem(lpmcdata->m_hDropChild, &lpresult->m_Item);
		//	return TRUE;
		//}
		//break;
		//
		//case MCS_TREEVIEW:
		//{
		//	// lParam = LPMCOMBO_TREEVIEWITEM
		//	auto lpresult = reinterpret_cast<LPMCOMBO_TREEVIEWITEM>(lParam);
		//	if ((lpresult->m_Size != sizeof(MCOMBO_TREEVIEWITEM)) || (lpresult->m_Type != MCS_TREEVIEW))
		//		return FALSE;
		//
		//	TreeView_GetItem(lpmcdata->m_hDropChild, &lpresult->m_Item);
		//	return TRUE;
		//}
		//break;
		}
	}

	return FALSE;
}

BOOL MultiCombo_AddItem(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
		return FALSE;

	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return FALSE;

	if (!IsWindow(lpmcdata->m_hDropChild))
		return FALSE;

	// wParam = 0

	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	{
		// lParam = LPMCOMBO_COLOURITEM
		const auto lpinput = reinterpret_cast<LPMCOMBO_ITEM>(lParam);
		if ((lpinput->m_Size != sizeof(MCOMBO_ITEM)) || (lpinput->m_Type != MCS_COLOUR))
			return FALSE;

		LPMCOMBO_ITEM lpdata = new MCOMBO_ITEM;

		lpdata->m_clrItem = lpinput->m_clrItem;
		lpdata->m_clrText = lpinput->m_clrText;
		lpdata->m_tsItemText = lpinput->m_tsItemText;

		ListBox_AddItemData(lpmcdata->m_hDropChild, lpdata);

		return TRUE;
	}
	break;

	case MCS_LISTBOX:
	{
		// lParam = LPMCOMBO_LISTBOXITEM
		const auto lpinput = reinterpret_cast<LPMCOMBO_ITEM>(lParam);
		if ((lpinput->m_Size != sizeof(MCOMBO_ITEM)) || (lpinput->m_Type != MCS_COLOUR))
			return FALSE;

		ListBox_AddString(lpmcdata->m_hDropChild, lpinput->m_tsItemText.to_chr());

		return TRUE;
	}
	break;

	//case MCS_LISTVIEW:
	//{
	//	// lParam = LPMCOMBO_LISTVIEWITEM
	//	const auto lpinput = reinterpret_cast<LPMCOMBO_LISTVIEWITEM>(lParam);
	//	if ((lpinput->m_Size != sizeof(MCOMBO_LISTVIEWITEM)) || (lpinput->m_Type != MCS_LISTVIEW))
	//		return FALSE;
	//
	//	lpinput->m_Item.iItem = ListView_InsertItem(lpmcdata->m_hDropChild, &lpinput->m_Item);
	//
	//	if (lpinput->m_Item.iItem == -1)
	//		return FALSE;
	//
	//	return TRUE;
	//}
	//break;
	//
	//case MCS_TREEVIEW:
	//{
	//	// lParam = LPMCOMBO_TREEVIEWITEM
	//	const auto lpinput = reinterpret_cast<LPMCOMBO_TREEVIEWITEM>(lParam);
	//	if ((lpinput->m_Size != sizeof(MCOMBO_TREEVIEWITEM)) || (lpinput->m_Type != MCS_TREEVIEW))
	//		return FALSE;
	//
	//	TreeView_InsertItem(lpmcdata->m_hDropChild, &lpinput->m_Item);
	//	return TRUE;
	//}
	//break;
	}
	return FALSE;
}

BOOL MultiCombo_InsertItem(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0)
		return FALSE;

	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return FALSE;

	if (!IsWindow(lpmcdata->m_hDropChild))
		return FALSE;

	// wParam = 0

	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	{
		// lParam = LPMCOMBO_COLOURITEM
		const auto lpinput = reinterpret_cast<LPMCOMBO_ITEM>(lParam);
		if ((lpinput->m_Size != sizeof(MCOMBO_ITEM)) || (lpinput->m_Type != MCS_COLOUR))
			return FALSE;

		LPMCOMBO_ITEM lpdata = new MCOMBO_ITEM;

		lpdata->m_clrItem = lpinput->m_clrItem;
		lpdata->m_clrText = lpinput->m_clrText;
		lpdata->m_tsItemText = lpinput->m_tsItemText;

		ListBox_InsertItemData(lpmcdata->m_hDropChild, wParam, lpdata);

		return TRUE;
	}
	break;

	case MCS_LISTBOX:
	{
		// lParam = LPMCOMBO_LISTBOXITEM
		const auto lpinput = reinterpret_cast<LPMCOMBO_ITEM>(lParam);
		if ((lpinput->m_Size != sizeof(MCOMBO_ITEM)) || (lpinput->m_Type != MCS_COLOUR))
			return FALSE;

		ListBox_InsertString(lpmcdata->m_hDropChild, wParam, lpinput->m_tsItemText.to_chr());

		return TRUE;
	}
	break;
	}
	return FALSE;
}

void MultiCombo_SetEditToCurSel(HWND mHwnd) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;
	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	{
		if (lpmcdata->m_CurrentEditColour)
		{
			DeleteBrush(lpmcdata->m_CurrentEditColour);
			lpmcdata->m_CurrentEditColour = nullptr;
			lpmcdata->m_CurrentEditTextColour = CLR_INVALID;
			lpmcdata->m_CurrentEditBkgColour = CLR_INVALID;
		}

		if (const auto iSel = ListBox_GetCurSel(lpmcdata->m_hDropChild); iSel != LB_ERR)
		{
			if (auto lpdata = reinterpret_cast<LPMCOMBO_ITEM>(ListBox_GetItemData(lpmcdata->m_hDropChild, iSel)); lpdata)
			{
				lpmcdata->m_CurrentEditBkgColour = lpdata->m_clrItem;
				lpmcdata->m_CurrentEditColour = CreateSolidBrush(lpdata->m_clrItem);
				lpmcdata->m_CurrentEditTextColour = lpdata->m_clrText;
				SetWindowText(lpmcdata->m_hEdit, lpdata->m_tsItemText.to_chr());
			}
		}
		InvalidateRect(lpmcdata->m_hEdit, nullptr, TRUE);
	}
	break;

	case MCS_LISTBOX:
	{
		if (lpmcdata->m_CurrentEditColour)
		{
			DeleteBrush(lpmcdata->m_CurrentEditColour);
			lpmcdata->m_CurrentEditColour = nullptr;
			lpmcdata->m_CurrentEditTextColour = CLR_INVALID;
			lpmcdata->m_CurrentEditBkgColour = CLR_INVALID;
		}

		if (const auto iSel = ListBox_GetCurSel(lpmcdata->m_hDropChild); iSel != LB_ERR)
		{
			TCHAR buf[1024]{};
			ListBox_GetText(lpmcdata->m_hDropChild, iSel, &buf[0]);

			SetWindowText(lpmcdata->m_hEdit, &buf[0]);
		}
	}
	}
}

LRESULT MultiCombo_Drop_Command(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return DefWindowProc(mHwnd, WM_COMMAND, wParam, lParam);

	if ((lpmcdata->m_hDropChild == reinterpret_cast<HWND>(lParam)) && (Dcx::dcxLOWORD(wParam) == MC_ID_DROPCHILD))
	{
		switch (lpmcdata->m_Styles)
		{
		default:
			break;
		case MCS_COLOUR:
		{
			switch (Dcx::dcxHIWORD(wParam))
			{
			default:
				break;
			case LBN_SELCHANGE:
			{
				MultiCombo_SetEditToCurSel(mHwnd);

				return SendMessage(GetParent(lpmcdata->m_hBase), WM_COMMAND, MAKEWPARAM(lpmcdata->m_BaseID, MCON_SELCHANGE), reinterpret_cast<LPARAM>(lpmcdata->m_hBase));
			}
			break;

			case LBN_DBLCLK:
			{
				ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
				return SendMessage(GetParent(lpmcdata->m_hBase), WM_COMMAND, MAKEWPARAM(lpmcdata->m_BaseID, MCON_DBLCLK), reinterpret_cast<LPARAM>(lpmcdata->m_hBase));
			}
			break;
			}
		}
		break;

		case MCS_LISTBOX:
		{
			switch (Dcx::dcxHIWORD(wParam))
			{
			default:
				break;
			case LBN_SELCHANGE:
			{
				MultiCombo_SetEditToCurSel(mHwnd);

				return SendMessage(GetParent(lpmcdata->m_hBase), WM_COMMAND, MAKEWPARAM(lpmcdata->m_BaseID, MCON_SELCHANGE), reinterpret_cast<LPARAM>(lpmcdata->m_hBase));
			}
			break;

			case LBN_DBLCLK:
			{
				if (IsWindow(lpmcdata->m_hDropCtrl))
					ShowWindow(lpmcdata->m_hDropCtrl, SW_HIDE);
				return SendMessage(GetParent(lpmcdata->m_hBase), WM_COMMAND, MAKEWPARAM(lpmcdata->m_BaseID, MCON_DBLCLK), reinterpret_cast<LPARAM>(lpmcdata->m_hBase));
			}
			break;
			}
		}
		break;
		//case MCS_LISTVIEW:
		//	break;
		//case MCS_TREEVIEW:
		//	break;
		}
	}
	return DefWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
}

LRESULT MultiCombo_Command(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return DefWindowProc(mHwnd, WM_COMMAND, wParam, lParam);

	if ((lpmcdata->m_hEdit == reinterpret_cast<HWND>(lParam)) && (Dcx::dcxLOWORD(wParam) == MC_ID_EDIT))
	{
		switch (Dcx::dcxHIWORD(wParam))
		{
		default:
			break;
		case EN_CHANGE:
		{
			return SendMessage(GetParent(lpmcdata->m_hBase), WM_COMMAND, MAKEWPARAM(lpmcdata->m_BaseID, MCON_EDITCHANGE), reinterpret_cast<LPARAM>(lpmcdata->m_hBase));
		}
		break;

		}
	}
	return DefWindowProc(mHwnd, WM_COMMAND, wParam, lParam);
}

void MultiCombo_ResetContent(HWND mHwnd) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (!IsWindow(lpmcdata->m_hDropChild))
		return;

	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	case MCS_LISTBOX:
		ListBox_ResetContent(lpmcdata->m_hDropChild);
		break;
	}
}

void MultiCombo_SetSel(HWND mHwnd, WPARAM wParam, LPARAM lParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (!IsWindow(lpmcdata->m_hDropChild))
		return;

	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	case MCS_LISTBOX:
	{
		ListBox_SetCurSel(lpmcdata->m_hDropChild, lParam);
		MultiCombo_SetEditToCurSel(mHwnd);
	}
	break;

	}
}

void MultiCombo_DeleteItem(HWND mHwnd, WPARAM wParam) noexcept
{
	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (!IsWindow(lpmcdata->m_hDropChild))
		return;

	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	case MCS_LISTBOX:
		ListBox_DeleteString(lpmcdata->m_hDropChild, wParam);
		break;
	}
}

void MultiCombo_SetItem(HWND mHwnd, WPARAM wParam, LPARAM lParam)
{
	if (!mHwnd || !lParam)
		return;

	const auto lpmcdata = Dcx::dcxGetProp<LPMCOMBO_DATA>(mHwnd, TEXT("mc_data"));
	if (!lpmcdata)
		return;

	if (!IsWindow(lpmcdata->m_hDropChild))
		return;

	switch (lpmcdata->m_Styles)
	{
	default:
		break;
	case MCS_COLOUR:
	{
		if (auto lpdata = reinterpret_cast<LPMCOMBO_ITEM>(ListBox_GetItemData(lpmcdata->m_hDropChild, wParam)); lpdata)
		{
			dcxlParam(LPMCOMBO_ITEM, lpinput);
			lpdata->m_clrItem = lpinput->m_clrItem;
			lpdata->m_clrText = lpinput->m_clrText;
			lpdata->m_tsItemText = lpinput->m_tsItemText;

			if (ListBox_GetCurSel(lpmcdata->m_hDropChild) == wParam)
				MultiCombo_SetEditToCurSel(lpmcdata->m_hDropCtrl);
		}
	}
	break;
	case MCS_LISTBOX:
	{
		dcxlParam(LPMCOMBO_ITEM, lpinput);

		ListBox_SetItemData(lpmcdata->m_hDropChild, wParam, lpinput->m_tsItemText.to_chr());

		if (ListBox_GetCurSel(lpmcdata->m_hDropChild) == wParam)
			MultiCombo_SetEditToCurSel(lpmcdata->m_hDropCtrl);
	}
	break;
	}
}
