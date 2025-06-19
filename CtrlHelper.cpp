//
//
//

#include "defines.h"
#include "CtrlHelper.h"

namespace Dcx
{
	// ListView

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nItem"></param>
	/// <param name="nSubItem"></param>
	/// <returns>The items text</returns>
	TString dcxListView_GetItemText(_In_ HWND hwnd, _In_ int nItem, _In_ int nSubItem)
	{
		TString tsRes(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));

		dcxListView_GetItemText(hwnd, nItem, nSubItem, tsRes.to_chr(), tsRes.capacity_cch());

		return tsRes;
	}

	size_t dcxListView_GetItemTextLength(_In_ HWND hwnd, _In_ int nItem, _In_ int nSubItem)
	{
		// Ook: why doesnt the control already have a message to do this when ListBox does?
		TString tsRes;
		LVITEM lvi{};
		lvi.iSubItem = nSubItem;

		size_t len{}, cap{}, check{};
		do {
			cap = tsRes.capacity_cch();
			check = (cap - 1);
			lvi.pszText = tsRes.to_chr();
			lvi.cchTextMax = gsl::narrow_cast<int>(cap);

			len = gsl::narrow_cast<size_t>(SendMessage(hwnd, LVM_GETITEMTEXT, nItem, reinterpret_cast<LPARAM>(&lvi)));
			if (len >= check)
				tsRes.reserve(cap * 2);

		} while (len >= check);

		return len;
	}


	/// <summary>
	/// Get the item at x,y screen position.
	/// </summary>
	/// <param name="hwnd"> = the listview control</param>
	/// <param name="x"> = the x screen position</param>
	/// <param name="y"> = the y screen position</param>
	/// <returns>the item at x,y or -1 for no item there.</returns>
	int dcxListView_GetItemAtPos(_In_ HWND hwnd, _In_ int x, _In_ int y) noexcept
	{
		if (!hwnd)
			return -1;

		LVHITTESTINFO lvhti{};
		lvhti.pt.x = x;
		lvhti.pt.y = y;

		// X & Y are relative to screen area.
		MapWindowPoints(nullptr, hwnd, &lvhti.pt, 1);
		Dcx::dcxListView_HitTest(hwnd, &lvhti);

		// Out of the ListView?
		if (lvhti.iItem == -1)
			return -1;
		// Not in an item?
		if (!dcx_testflag(lvhti.flags, LVHT_ONITEMICON) && !dcx_testflag(lvhti.flags, LVHT_ONITEMLABEL) && !dcx_testflag(lvhti.flags, LVHT_ONITEMSTATEICON))
			return -1;

		return lvhti.iItem;
	}


	/// <summary>
	/// Get the item at the cursor position.
	/// </summary>
	/// <param name="hwnd"> = the listview control.</param>
	/// <returns>the item at the cursor or -1 for no item there.</returns>
	int dcxListView_GetItemAtCursor(_In_ HWND hwnd) noexcept
	{
		if (!hwnd)
			return -1;

		POINT pt{};
		if (!GetCursorPos(&pt))
			return -1;

		return dcxListView_GetItemAtPos(hwnd, pt.x, pt.y);
	}

	LVHITTESTINFO dcxListView_CursorHitTest(_In_ HWND hwnd) noexcept
	{
		LVHITTESTINFO lvht{};
		if (!hwnd)
			return lvht;

		if (!GetCursorPos(&lvht.pt))
			return lvht;
		// X & Y are relative to screen area.
		MapWindowPoints(nullptr, hwnd, &lvht.pt, 1);
		dcxListView_SubItemHitTest(hwnd, &lvht);

		return lvht;
	}

	// ListBox

	[[nodiscard]] TString dcxListBox_GetText(_In_ HWND hwnd, _In_ int i)
	{
		TString tsBuf;

		const auto len = dcxListBox_GetTextLen(hwnd, i);
		if (len < 1)
			return tsBuf;

		tsBuf.reserve(len + 1U);

		dcxListBox_GetText(hwnd, i, tsBuf.to_chr());

		return tsBuf;
	}
	int dcxListBox_GetPointItem(_In_ HWND hListbox, _In_ POINT pt) noexcept
	{
		if (!hListbox)
			return -1;

		//// do we have any items?
		//if (const auto iCnt = dcxListBox_GetCount(hListbox); iCnt > 0)
		//{
		//	RECT rc{};
		//
		//	// check point is in client area.
		//	if (GetClientRect(hListbox, &rc))
		//	{
		//		if (!PtInRect(&rc, pt))
		//			return -1;
		//	}
		//	
		//	//ListBox_GetTopIndex(hwnd) used instead of zero to make the loop quicker.
		//
		//	// loop through items & check for a match
		//	for (int i = dcxListBox_GetTopIndex(hListbox); i < iCnt; ++i)
		//	{
		//		// get items rect
		//		if (!dcxListBox_GetItemRect(hListbox, i, &rc))
		//			break;
		//
		//		// check if point is within this item.
		//		if (PtInRect(&rc, pt))
		//			return i;
		//	}
		//}
		//return -1;

		return LBItemFromPt(hListbox, pt, FALSE);
	}
	int dcxListBox_GetHoverItem(_In_ HWND hListbox) noexcept
	{
		if (!hListbox)
			return -1;

		//return dcxListBox_GetPointItem(hListbox, Dcx::dcxCursorPos(hListbox));
		return dcxListBox_GetPointItem(hListbox, Dcx::dcxCursorPos());
	}
	bool dcxListBox_HitTest(_In_ HWND hListbox, _Inout_ LPLVHITTESTINFO phti) noexcept
	{
		if (!hListbox || !phti)
			return false;

		RECT rc{};

		// check point is in client area.
		if (!GetClientRect(hListbox, &rc))
			return false;

		if (PtInRect(&rc, phti->pt))
		{
			// in client rect
			phti->flags = LVHT_NOWHERE;
			// do we have any items?
			if (const auto iCnt = dcxListBox_GetCount(hListbox); iCnt > 0)
			{
				//ListBox_GetTopIndex(hwnd) used instead of zero to make the loop quicker.

				// loop through items & check for a match
				for (int i = dcxListBox_GetTopIndex(hListbox); i < iCnt; ++i)
				{
					// get items rect
					if (!Dcx::dcxListBox_GetItemRect(hListbox, i, &rc))
						break;

					// check if point is within this item.
					if (PtInRect(&rc, phti->pt))
					{
						phti->flags = LVHT_ONITEMLABEL;
						phti->iItem = i;
						break;
					}
				}
			}
		}
		else {
			// not in client rect
			if (phti->pt.y < rc.top)
				phti->flags |= LVHT_ABOVE;
			else if (phti->pt.y > rc.bottom)
				phti->flags |= LVHT_BELOW;

			if (phti->pt.x < rc.left)
				phti->flags |= LVHT_TOLEFT;
			else if (phti->pt.x > rc.right)
				phti->flags |= LVHT_TORIGHT;
		}

		return true;
	}

	// TreeView
	size_t dcxTreeView_GetCount(_In_ HWND hTree) noexcept
	{
		if (!hTree)
			return 0U;

		// NB: This macro returns values 0 - 32767 ok, but 32768 - 65536 are returned as negatives, & anything > 65536 returns as zero & the treeview fails to display.
		// cant use TreeView_GetCount() macro as this types the result as unsigned.
		const auto cnt = SendMessage(hTree, TVM_GETCOUNT, 0, 0);
		if (cnt < 0)
			return (gsl::narrow_cast<size_t>(std::abs(cnt)) + 32767U);

		return gsl::narrow_cast<size_t>(cnt);
	}
	GSL_SUPPRESS(type.4)
		[[nodiscard]] HTREEITEM dcxTreeView_GetLastSibling(_In_ HWND hTree, _In_ HTREEITEM child) noexcept
	{
		if (!child || !hTree)
			return nullptr;

		HTREEITEM current{};
		for (HTREEITEM tmp = dcxTreeView_GetNextSibling(hTree, child); tmp; tmp = dcxTreeView_GetNextSibling(hTree, current))
			current = tmp;
		return current;
	}
	void dcxTreeView_SetItemState(_In_ HWND hTree, _In_ HTREEITEM hItem, _In_ UINT data, _In_ UINT mask) noexcept
	{
		if (!hTree)
			return;

		//TreeView_SetItemState(m_Hwnd, hItem, data, mask);
		TVITEMEX _ms_TVi{};
		_ms_TVi.mask = TVIF_STATE | TVIF_HANDLE;
		_ms_TVi.hItem = hItem;
		_ms_TVi.state = data;
		_ms_TVi.stateMask = mask;
		dcxTreeView_SetItem(hTree, std::addressof(_ms_TVi));
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hItem"></param>
	/// <returns></returns>
	TString dcxTreeView_GetItemText(_In_ HWND hTree, _In_ HTREEITEM hItem)
	{
		TString tsRes;
		TVITEMEX tvitem{};
		TCHAR buf[MIRC_BUFFER_SIZE_CCH]{};
		tvitem.hItem = hItem;
		tvitem.mask = TVIF_TEXT | TVIF_HANDLE;
		tvitem.pszText = &buf[0];
		tvitem.cchTextMax = gsl::narrow_cast<int>(std::size(buf));
		if (dcxTreeView_GetItem(hTree, &tvitem))
			tsRes = tvitem.pszText;

		return tsRes;
	}

	void dcxTreeView_SetItemText(_In_ HWND hTree, _In_ HTREEITEM hItem, _In_ const TString& txt) noexcept
	{
		if (!hTree)
			return;

		TVITEMEX tvi{};

		tvi.mask = TVIF_TEXT | TVIF_HANDLE;
		tvi.hItem = hItem;
		GSL_SUPPRESS(type.3) tvi.pszText = const_cast<TCHAR*>(txt.to_chr());

		dcxTreeView_SetItem(hTree, &tvi);
	}

	bool dcxTreeView_DeleteItem(_In_ HWND hTree, _In_ const HTREEITEM item) noexcept
	{
		if (!hTree)
			return false;

		// If the window style for a tree-view control contains TVS_SCROLL and all items are deleted,
		// new items are not displayed until the window styles are reset.
		// The following code shows one way to ensure that items are always displayed.
		const auto styles = dcxGetWindowStyle(hTree);
		Auto(dcxSetWindowStyle(hTree, styles));

		return (TreeView_DeleteItem(hTree, item) != FALSE);
	}
	bool dcxTreeView_DeleteAllItems(_In_ HWND hTree) noexcept
	{
		if (!hTree)
			return false;

		// If the window style for a tree-view control contains TVS_SCROLL and all items are deleted,
		// new items are not displayed until the window styles are reset.
		// The following code shows one way to ensure that items are always displayed.
		const auto styles = dcxGetWindowStyle(hTree);
		if (!dcx_testflag(styles, TVS_NOSCROLL))
		{
			// If a TreeView control doesn't have the TVS_NOSCROLL style it can loose the scrollbars when all items are deleted.
			// This fixes that.
			dcxSetWindowStyle(hTree, styles | TVS_NOSCROLL);
		}
		Auto(dcxSetWindowStyle(hTree, styles));

		return (TreeView_DeleteAllItems(hTree) != FALSE);
	}

	TVHITTESTINFO dcxTreeView_GetCursorItem(_In_ HWND hTree) noexcept
	{
		TVHITTESTINFO tvh{};
		if (!hTree)
			return tvh;

		if (!GetCursorPos(&tvh.pt))
			return tvh;

		MapWindowPoints(nullptr, hTree, &tvh.pt, 1);
		dcxTreeView_HitTest(hTree, &tvh);

		return tvh;
	}

	namespace details
	{
		static HTREEITEM dcxTreeView_WalkChildItems(_In_ HWND mHwnd, _In_ HTREEITEM hParent, _Inout_ size_t& nCnt, _In_ const size_t& nIndex) noexcept
		{
			if ((!mHwnd) || (!hParent))
				return nullptr;

			for (auto hItem = Dcx::dcxTreeView_GetChild(mHwnd, hParent); hItem; hItem = Dcx::dcxTreeView_GetNextSibling(mHwnd, hItem))
			{
				if (++nCnt == nIndex)
					return hItem;

				if (auto hRes = dcxTreeView_WalkChildItems(mHwnd, hItem, nCnt, nIndex); hRes)
					return hRes;
			}
			return nullptr;
		}
	}

	[[nodiscard]] HTREEITEM dcxTreeView_MapIndexToItem(_In_ HWND hTree, _In_ size_t nIndex) noexcept
	{
		if ((!hTree) || (!IsWindow(hTree)))
			return nullptr;

		size_t nCnt{};

		for (auto hItem = Dcx::dcxTreeView_GetRoot(hTree); hItem; hItem = Dcx::dcxTreeView_GetNextSibling(hTree, hItem))
		{
			if (++nCnt == nIndex)
				return hItem;

			if (auto hRes = details::dcxTreeView_WalkChildItems(hTree, hItem, nCnt, nIndex); hRes)
				return hRes;
		}

		return nullptr;
	}

	/// <summary>
	/// Get the note text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	TString dcxButton_GetNote(_In_ HWND hwnd)
	{
		TString tsBuf;

		if (auto len = Dcx::dcxButton_GetNoteLength(hwnd); len > 0)
		{
			tsBuf.reserve(++len);

			DWORD l{ gsl::narrow_cast<DWORD>(len) };

			dcxButton_GetNote(hwnd, tsBuf.to_chr(), &l);
		}
		return tsBuf;
	}

	UINT dcxStatusBar_GetTextLength(_In_ HWND hwnd, _In_ int iPart) noexcept
	{
		return gsl::narrow_cast<UINT>(Dcx::dcxLOWORD(SendMessage(hwnd, SB_GETTEXTLENGTHW, gsl::narrow_cast<WPARAM>(iPart), 0)));
	}

	TString dcxStatusBar_GetText(HWND hwnd, int iPart)
	{
		TString tsText(gsl::narrow_cast<TString::size_type>(dcxStatusBar_GetTextLength(hwnd, iPart) + 1));
		dcxStatusBar_GetText(hwnd, iPart, tsText.to_chr());

		return tsText;
	}

	UINT dcxStatusBar_GetPartFlags(_In_ HWND hwnd, _In_ int iPart) noexcept
	{
		return gsl::narrow_cast<UINT>(Dcx::dcxHIWORD(SendMessage(hwnd, SB_GETTEXTLENGTHW, gsl::narrow_cast<WPARAM>(iPart), 0)));
	}

	TString dcxStatusBar_GetTipText(_In_ HWND hwnd, _In_ int iPart)
	{
		TString tsRes(gsl::narrow_cast<TString::size_type>(MIRC_BUFFER_SIZE_CCH));

		dcxStatusBar_GetTipText(hwnd, iPart, gsl::narrow_cast<int>(tsRes.capacity_cch()), tsRes.to_chr());

		return tsRes;
	}

	EC_ENDOFLINE dcxEdit_GetEndOfLine(_In_ HWND hwnd) noexcept
	{
		//	EC_ENDOFLINE_CRLF	one		The end - of - line character used for new linebreaks is carriage return followed by linefeed(CRLF).
		//	EC_ENDOFLINE_CR		two		The end - of - line character used for new linebreaks is carriage return (CR).
		//	EC_ENDOFLINE_LF		three	The end - of - line character used for new linebreaks is linefeed(LF).
		// When the end-of-line character used is set to EC_ENDOFLINE_DETECTFROMCONTENT using Edit_SetEndOfLine, this message will return the detected end-of-line character.
		if (hwnd && Dcx::VersInfo.isWin10())
			return Edit_GetEndOfLine(hwnd);

		// use CRLF if not win10+
		return EC_ENDOFLINE_CRLF;
	}
	bool dcxEdit_GetZoom(_In_ HWND hwnd, _In_ int* nNumerator, _In_ int* nDenominator) noexcept
	{
		//Supported in Windows 10 1809 and later. The edit control needs to have the ES_EX_ZOOMABLE extended style set, for this message to have an effect
		//(the zoom ratio is always between 1/64 and 64) NOT inclusive, 1.0 = no zoom
		if (!hwnd || !nNumerator || !nDenominator)
			return false;

		if (Dcx::VersInfo.isWin10())
			return Edit_GetZoom(hwnd, nNumerator, nDenominator);

		*nNumerator = 1;
		*nDenominator = 0;
		return true;
	}
	TString dcxEdit_GetEndOfLineCharacters(_In_ HWND hwnd)
	{
		switch (Dcx::dcxEdit_GetEndOfLine(hwnd))
		{
		case EC_ENDOFLINE_LF:
			return TEXT("\n");
		case EC_ENDOFLINE_CR:
			return TEXT("\r");
		case EC_ENDOFLINE_CRLF:
		default:
			break;
		}
		return TEXT("\r\n");
	}
	DWORD dcxEdit_GetCaretIndex(_In_ HWND hwnd) noexcept
	{
		if (!hwnd)
			return 0;

		// windows 10 only
		if (Dcx::VersInfo.isWin10())
			return Edit_GetCaretIndex(hwnd);

		DWORD hiPos{}, loPos{};
		dcxEdit_GetSel(hwnd, &loPos, &hiPos);
		if (loPos != hiPos)
			--hiPos;
		return hiPos;
	}
	void dcxEdit_SetCaretIndex2(_In_ HWND hwnd, _In_ DWORD nPos) noexcept
	{
		if (!hwnd)
			return;

		// windows 10 only
		if (Dcx::VersInfo.isWin10())
			dcxEdit_SetCaretIndex(hwnd, nPos);
		else
			dcxEdit_SetSel(hwnd, nPos, nPos);
	}
	DWORD dcxEdit_CharFromPos(_In_ HWND hwnd, _In_ LONG ihPos, _In_ LONG ivPos) noexcept
	{
		if (!hwnd)
			return 0;

		return gsl::narrow_cast<DWORD>(SNDMSG(hwnd, EM_CHARFROMPOS, 0, Dcx::dcxMAKELPARAM(ihPos, ivPos)));
	}
	DWORD dcxEdit_LineFromChar(_In_ HWND hwnd, _In_ const LONG& ich) noexcept
	{
		if (!hwnd)
			return 0;
		return gsl::narrow_cast<DWORD>(Edit_LineFromChar(hwnd, ich));
	}
	int dcxEdit_GetSel(_In_ HWND hwnd, _In_opt_ DWORD* nStart, _In_opt_ DWORD* nEnd) noexcept
	{
		if (!hwnd)
			return -1;

		return gsl::narrow_cast<int>(SendMessage(hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(nStart), reinterpret_cast<LPARAM>(nEnd)));
	}


	/// <summary>
	/// Sets the background color for a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="bUseSystem">- Specifies whether to use the system color. If this parameter is a nonzero value, the background is set to the window background system color. Otherwise, the background is set to the specified color.</param>
	/// <param name="clr">- A COLORREF structure specifying the color if wParam is zero. To generate a COLORREF, use the RGB macro.</param>
	/// <returns>The original background color.</returns>
	COLORREF dcxRichEdit_SetBkgndColor(HWND hwnd, BOOL bUseSystem, COLORREF clr) noexcept
	{
		return gsl::narrow_cast<COLORREF>(SendMessage(hwnd, EM_SETBKGNDCOLOR, gsl::narrow_cast<WPARAM>(bUseSystem), gsl::narrow_cast<LPARAM>(clr)));
	}

	COLORREF dcxRichEdit_GetBkgndColor(HWND hwnd) noexcept
	{
		return dcxRichEdit_SetBkgndColor(hwnd, FALSE, dcxRichEdit_SetBkgndColor(hwnd, TRUE, 0));
	}

	int dcxTabCtrl_GetPointItem(HWND hwnd, POINT pt) noexcept
	{
		TCHITTESTINFO tti{};
		tti.pt = pt;

		const auto iTab = TabCtrl_HitTest(hwnd, &tti);

		if (tti.flags & TCHT_ONITEM)
			return iTab;

		return -1;
	}

	int dcxTabCtrl_GetHoverItem(HWND hwnd) noexcept
	{
		return dcxTabCtrl_GetPointItem(hwnd, Dcx::dcxCursorPos(hwnd));
	}
}
