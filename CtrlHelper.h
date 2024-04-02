#pragma once

//
//
// Helper functions for accessing standard controls.
//

#include <commctrl.h>
#include "Classes/tstring/tstring.h"

namespace Dcx
{
	// Listview
#ifndef LVSIL_FOOTER
#define LVSIL_FOOTER 4	// The image list for the footer. (undocumented)
#endif

#ifndef LVM_RESETEMPTYTEXT
	// Needed to reset the empty listview text.
	// forces the listview to send LVN_GETEMPTYMARKUP notification. (no args)
#define LVM_RESETEMPTYTEXT (LVM_FIRST + 84)
#endif

	// undocumented methods
#ifndef LVM_EDITSUBITEM
#define LVM_EDITSUBITEM  0x10B7 // 6.10 and higher translates to EditSubItem method
#endif
#ifndef LVM_ENSURESUBITEMVISIBLE
#define LVM_ENSURESUBITEMVISIBLE 0x10B8 // 6.10 and higher translates to EnsureSubItemVisible method
#endif
#ifndef LVM_QUERYINTERFACE
#define LVM_QUERYINTERFACE (LVM_FIRST + 189) // 6.10 and higher passes wParam and lParam to QueryInterface method;  returns TRUE for success, else FALSE
#endif
#ifndef LVM_SETVIEWMARGIN
#define LVM_SETVIEWMARGIN  0x105A // 6.00 and higher passes lParam to SetViewMargin method; returns TRUE for success, else FALSE
#endif
#ifndef LVM_GETVIEWMARGIN
#define LVM_GETVIEWMARGIN  0x105B // 6.00 and higher passes lParam to GetViewMargin method; returns TRUE for success, else FALSE
#endif

	/// <summary>
	/// Set an existing items details.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="plvi"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetItem(_In_ HWND hwnd, _In_ const LPLVITEM plvi) noexcept
	{
		return ListView_SetItem(hwnd, plvi);
	}

	/// <summary>
	/// Set an items text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="iSub"></param>
	/// <param name="str"></param>
	inline void dcxListView_SetItemText(_In_ HWND hwnd, _In_ const int i, _In_ const int iSub, _In_z_ PTCHAR str) noexcept
	{
		ListView_SetItemText(hwnd, i, iSub, str);
	}

	/// <summary>
	/// Set an items state.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="item"></param>
	/// <param name="state"></param>
	/// <param name="statemask"></param>
	inline void dcxListView_SetItemState(_In_ HWND hwnd, _In_ const int item, _In_ const unsigned int state, _In_ const unsigned int statemask) noexcept
	{
		ListView_SetItemState(hwnd, item, state, statemask);
	}

	/// <summary>
	/// Set an items infotip popup.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lvit"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetInfoTip(_In_ HWND hwnd, _In_ PLVSETINFOTIP lvit) noexcept
	{
		return ListView_SetInfoTip(hwnd, lvit);
	}

	/// <summary>
	/// Set items check state.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="fCheck"></param>
	inline void dcxListView_SetCheckState(_In_ HWND hwnd, _In_ const int i, _In_ const int fCheck) noexcept
	{
		ListView_SetCheckState(hwnd, i, fCheck);
	}

	/// <summary>
	/// Determines if an item in a list-view control is selected. This should be used only for list-view controls that have the LVS_EX_CHECKBOXES style.
	/// </summary>
	/// <param name="hwnd">- A handle to a list-view control.</param>
	/// <param name="i">- The index of the item for which to retrieve the check state.</param>
	/// <returns></returns>
	inline UINT dcxListView_GetCheckState(_In_ HWND hwnd, _In_ const int i) noexcept
	{
		return ListView_GetCheckState(hwnd, i);
	}

	/// <summary>
	/// Set controls extended styles.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="mask"></param>
	/// <param name="dw"></param>
	/// <returns></returns>
	inline DWORD dcxListView_SetExtendedListViewStyleEx(_In_ HWND hwnd, _In_ const WPARAM mask, _In_ const LPARAM dw) noexcept
	{
		return ListView_SetExtendedListViewStyleEx(hwnd, mask, dw);
	}

	/// <summary>
	/// Set controls empty text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pszText"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetEmptyText(_In_ HWND hwnd, _In_z_ LPCTCH pszText) noexcept
	{
		//const auto res = ListView_SetEmptyText(hwnd, pszText, cchText);
		const auto res = SetWindowText(hwnd, pszText);
		SendMessage(hwnd, LVM_RESETEMPTYTEXT, 0, 0);
		return res;
	}

	/// <summary>
	/// Insert item into control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="plvi"></param>
	/// <returns></returns>
	inline int dcxListView_InsertItem(_In_ HWND hwnd, _In_ const LPLVITEM plvi) noexcept
	{
		return ListView_InsertItem(hwnd, plvi);
	}

	/// <summary>
	/// Get information on an item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="plvi"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetItem(_In_ HWND hwnd, _Inout_ LPLVITEM const plvi) noexcept
	{
		return ListView_GetItem(hwnd, plvi);
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nItem"></param>
	/// <param name="nSubItem"></param>
	/// <param name="Buf"></param>
	/// <param name="nBufSize"></param>
	inline void dcxListView_GetItemText(_In_ HWND hwnd, _In_ const int nItem, _In_ const int nSubItem, _Inout_z_ PTCHAR const Buf, _In_ const size_t nBufSize) noexcept
	{
		ListView_GetItemText(hwnd, nItem, nSubItem, Buf, gsl::narrow_cast<int>(nBufSize));
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nItem"></param>
	/// <param name="nSubItem"></param>
	/// <returns>The items text</returns>
	TString dcxListView_GetItemText(_In_ HWND hwnd, _In_ int nItem, _In_ int nSubItem);

	/// <summary>
	/// Get the items text length.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nItem"></param>
	/// <param name="nSubItem"></param>
	/// <returns></returns>
	size_t dcxListView_GetItemTextLength(_In_ HWND hwnd, _In_ int nItem, _In_ int nSubItem);

	/// <summary>
	/// Get next item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="flags"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetNextItem(_In_ HWND hwnd, _In_ int i, _In_ unsigned int flags) noexcept
	{
		return ListView_GetNextItem(hwnd, i, flags);
	}

	/// <summary>
	/// Get an array of column order.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nCnt"></param>
	/// <param name="p"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetColumnOrderArray(_In_ HWND hwnd, _In_ const int nCnt, _Inout_ int* const p) noexcept
	{
		return ListView_GetColumnOrderArray(hwnd, nCnt, p);
	}

	/// <summary>
	/// Get a columns width.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iCol"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetColumnWidth(_In_ HWND hwnd, _In_ const int iCol) noexcept
	{
		return ListView_GetColumnWidth(hwnd, iCol);
	}

	/// <summary>
	/// Get total number of items.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetItemCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetItemCount(hwnd);
	}

	/// <summary>
	/// Get number of selected items.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline unsigned int dcxListView_GetSelectedCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetSelectedCount(hwnd);
	}

	/// <summary>
	/// Get items state.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	[[nodiscard]] inline unsigned int dcxListView_GetItemState(_In_ HWND hwnd, _In_ const int i, _In_ const unsigned int mask) noexcept
	{
		return ListView_GetItemState(hwnd, i, mask);
	}

	/// <summary>
	/// Get controls extended style.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxListView_GetExtendedListViewStyle(_In_ HWND hwnd) noexcept
	{
		return ListView_GetExtendedListViewStyle(hwnd);
	}

	/// <summary>
	/// Get items rect.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="prc"></param>
	/// <param name="code"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetItemRect(_In_ HWND hwnd, _In_ const int i, _Inout_ LPRECT prc, _In_ const int code) noexcept
	{
		GSL_SUPPRESS(es.47) return ListView_GetItemRect(hwnd, i, prc, code);
	}

	/// <summary>
	/// Get subitems rect.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="iSub"></param>
	/// <param name="code"></param>
	/// <param name="prc"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetSubItemRect(_In_ HWND hwnd, _In_ const int i, _In_ const int iSub, _In_ const int code, _Inout_ LPRECT prc) noexcept
	{
		GSL_SUPPRESS(es.47) return ListView_GetSubItemRect(hwnd, i, iSub, code, prc);
	}

	/// <summary>
	/// Get columns info.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iCol"></param>
	/// <param name="lc"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetColumn(_In_ HWND hwnd, _In_ const int iCol, _Inout_ LPLVCOLUMN lc) noexcept
	{
		return ListView_GetColumn(hwnd, iCol, lc);
	}

	/// <summary>
	/// Insert column into control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iCol"></param>
	/// <param name="pCol"></param>
	/// <returns></returns>
	inline int dcxListView_InsertColumn(_In_ HWND hwnd, _In_ const int iCol, _In_ LPLVCOLUMN pCol) noexcept
	{
		return ListView_InsertColumn(hwnd, iCol, pCol);
	}

	/// <summary>
	/// Get Header child control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxListView_GetHeader(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(lifetime.4) return ListView_GetHeader(hwnd);
	}

	/// <summary>
	/// Get ToolTip child control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxListView_GetToolTips(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(lifetime.4) return ListView_GetToolTips(hwnd);
	}

	/// <summary>
	/// Get index of top item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetTopIndex(_In_ HWND hwnd) noexcept
	{
		return ListView_GetTopIndex(hwnd);
	}

	/// <summary>
	/// Calculates the approximate width and height required to display a given number of items.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iWidth">- The proposed x-dimension of the control, in pixels.<para/> This parameter can be -1 to allow the message to use the current width value.</param>
	/// <param name="iHeight">- The proposed y-dimension of the control, in pixels.<para/> This parameter can be -1 to allow the message to use the current height value.</param>
	/// <param name="iCount">- The number of items to be displayed in the control.<para/> If this parameter is -1, the message uses the total number of items in the control.</param>
	/// <returns>Returns a DWORD value that holds the approximate width (in the LOWORD) and height (in the HIWORD) needed to display the items, in pixels.</returns>
	inline DWORD dcxListView_ApproximateViewRect(_In_ HWND hwnd, _In_ int iWidth, _In_ int iHeight, _In_ int iCount) noexcept
	{
		return ListView_ApproximateViewRect(hwnd, iWidth, iHeight, iCount);
	}

	/// <summary>
	/// Arranges items in icon view.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="uCode">
	///	<para>- One of the following values that specifies alignment:</para>
	///		<para>Value - Meaning</para>
	///		<para>LVA_ALIGNLEFT - Not implemented. Apply the LVS_ALIGNLEFT style instead.</para>
	///		<para>LVA_ALIGNTOP - Not implemented. Apply the LVS_ALIGNTOP style instead.</para>
	///		<para>LVA_DEFAULT - Aligns items according to the list - view control's current alignment styles (the default value).</para>
	///		<para>LVA_SNAPTOGRID - Snaps all icons to the nearest grid position.</para>
	///	</param>
	/// <returns>Returns true if successful; otherwise, false.</returns>
	inline bool dcxListView_Arrange(_In_ HWND hwnd, _In_ UINT uCode) noexcept
	{
		return !!ListView_Arrange(hwnd, uCode);
	}

	/// <summary>
	/// Cancels an item text editing operation.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	inline void dcxListView_CancelEditLabel(_In_ HWND hwnd) noexcept
	{
		ListView_CancelEditLabel(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="gid"></param>
	/// <param name="pgrp"></param>
	/// <returns></returns>
	inline int dcxListView_GetGroupInfo(_In_ HWND hwnd, _In_ const unsigned int gid, _Inout_ PLVGROUP pgrp) noexcept
	{
		return gsl::narrow_cast<int>(ListView_GetGroupInfo(hwnd, gid, pgrp));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iIndex"></param>
	/// <param name="pgrp"></param>
	/// <returns></returns>
	inline LRESULT dcxListView_GetGroupInfoByIndex(_In_ HWND hwnd, _In_ const unsigned int iIndex, _Inout_ PLVGROUP pgrp) noexcept
	{
		return ListView_GetGroupInfoByIndex(hwnd, iIndex, pgrp);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetGroupCount(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<int>(ListView_GetGroupCount(hwnd));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="gid"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	[[nodiscard]] inline unsigned int dcxListView_GetGroupState(_In_ HWND hwnd, _In_ const int gid, _In_ const DWORD mask) noexcept
	{
		return ListView_GetGroupState(hwnd, gid, mask);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="gid"></param>
	/// <param name="type"></param>
	/// <param name="prc"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetGroupRect(_In_ HWND hwnd, _In_ const int gid, _In_ const long type, _Out_ LPRECT prc) noexcept
	{
		GSL_SUPPRESS(es.47) return gsl::narrow_cast<BOOL>(ListView_GetGroupRect(hwnd, gid, type, prc));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="str"></param>
	/// <param name="sz"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetEmptyText(_In_ HWND hwnd, _Inout_z_ PTCHAR str, _In_ const int sz) noexcept
	{
		return ListView_GetEmptyText(hwnd, str, sz);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lvht"></param>
	/// <returns></returns>
	inline int dcxListView_SubItemHitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvht) noexcept
	{
		return ListView_SubItemHitTest(hwnd, lvht);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lvht"></param>
	/// <returns></returns>
	inline int dcxListView_SubItemGroupHitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvht) noexcept
	{
		// setting wparam to -1 causes it to return the iGroup info.
		return gsl::narrow_cast<int>(SendMessageW(hwnd, LVM_SUBITEMHITTEST, UINT_MAX, reinterpret_cast<LPARAM>(lvht)));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iCol"></param>
	/// <param name="cx"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetColumnWidth(_In_ HWND hwnd, _In_ const int iCol, _In_ const int cx) noexcept
	{
		return ListView_SetColumnWidth(hwnd, iCol, cx);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iCount"></param>
	/// <param name="pi"></param>
	/// <returns></returns>
	inline bool dcxListView_SetColumnOrderArray(_In_ HWND hwnd, _In_ const int iCount, _In_ const int* pi) noexcept
	{
		return !!ListView_SetColumnOrderArray(hwnd, iCount, pi);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="mode"></param>
	/// <returns></returns>
	inline DWORD dcxListView_SetView(_In_ HWND hwnd, _In_ DWORD mode) noexcept
	{
		return ListView_SetView(hwnd, mode);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="group"></param>
	/// <returns></returns>
	[[nodiscard]] inline LRESULT dcxListView_HasGroup(_In_ HWND hwnd, _In_ const int group) noexcept
	{
		return ListView_HasGroup(hwnd, group);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline BOOL dcxListView_IsGroupViewEnabled(_In_ HWND hwnd) noexcept
	{
		return ListView_IsGroupViewEnabled(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="fEnable"></param>
	/// <returns></returns>
	inline BOOL dcxListView_EnableGroupView(_In_ HWND hwnd, _In_ bool fEnable) noexcept
	{
		return gsl::narrow_cast<BOOL>(ListView_EnableGroupView(hwnd, fEnable));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="gid"></param>
	/// <param name="mask"></param>
	/// <param name="state"></param>
	inline void dcxListView_SetGroupState(_In_ HWND hwnd, _In_ const int gid, _In_ const unsigned int mask, _In_ const unsigned int state) noexcept
	{
		ListView_SetGroupState(hwnd, gid, mask, state);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="index"></param>
	/// <param name="pgrp"></param>
	inline void dcxListView_InsertGroup(_In_ HWND hwnd, _In_ const int index, _In_ PLVGROUP pgrp) noexcept
	{
		ListView_InsertGroup(hwnd, index, pgrp);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iGID"></param>
	/// <param name="pgrp"></param>
	/// <returns></returns>
	inline int dcxListView_SetGroupInfo(_In_ HWND hwnd, _In_ const int iGID, _In_ PLVGROUP pgrp) noexcept
	{
		return gsl::narrow_cast<int>(ListView_SetGroupInfo(hwnd, iGID, pgrp));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="gid"></param>
	/// <param name="index"></param>
	inline void dcxListView_MoveGroup(_In_ HWND hwnd, _In_ const int gid, _In_ const int index) noexcept
	{
		ListView_MoveGroup(hwnd, gid, index);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="index"></param>
	/// <param name="gid"></param>
	inline void dcxListView_MoveItemToGroup(_In_ HWND hwnd, _In_ const int index, _In_ const int gid) noexcept
	{
		ListView_MoveItemToGroup(hwnd, index, gid);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="gid"></param>
	inline void dcxListView_RemoveGroup(_In_ HWND hwnd, _In_ const int gid) noexcept
	{
		ListView_RemoveGroup(hwnd, gid);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	inline void dcxListView_RemoveAllGroups(_In_ HWND hwnd) noexcept
	{
		ListView_RemoveAllGroups(hwnd);
	}

	/// <summary>
	/// Removes a column from a list-view control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="column">- An index of the column to delete.</param>
	/// <returns>Returns true if successful, or false otherwise.</returns>
	inline BOOL dcxListView_DeleteColumn(_In_ HWND hwnd, _In_ const int column) noexcept
	{
		return ListView_DeleteColumn(hwnd, column);
	}

	/// <summary>
	/// Removes all items from a list-view control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	inline BOOL dcxListView_DeleteAllItems(_In_ HWND hwnd) noexcept
	{
		return ListView_DeleteAllItems(hwnd);
	}

	/// <summary>
	/// Removes an item from a list-view control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="i">- An index of the list-view item to delete.</param>
	/// <returns></returns>
	inline BOOL dcxListView_DeleteItem(_In_ HWND hwnd, _In_ const int i) noexcept
	{
		return ListView_DeleteItem(hwnd, i);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pFn"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SortItemsEx(_In_ HWND hwnd, _In_ PFNLVCOMPARE pFn, _Inout_ void* data) noexcept
	{
		return ListView_SortItemsEx(hwnd, pFn, data);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="partialOK"></param>
	/// <returns></returns>
	inline BOOL dcxListView_EnsureVisible(_In_ HWND hwnd, _In_ const int i, _In_ BOOL partialOK) noexcept
	{
		return ListView_EnsureVisible(hwnd, i, partialOK);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="dx"></param>
	/// <param name="dy"></param>
	/// <returns></returns>
	inline BOOL dcxListView_Scroll(_In_ HWND hwnd, _In_ const int dx, _In_ const int dy) noexcept
	{
		return ListView_Scroll(hwnd, dx, dy);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetCountPerPage(_In_ HWND hwnd) noexcept
	{
		return ListView_GetCountPerPage(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline HWND dcxListView_GetEditControl(_In_ HWND hwnd) noexcept
	{
		return ListView_GetEditControl(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline COLORREF dcxListView_GetBkColor(_In_ HWND hwnd) noexcept
	{
		return ListView_GetBkColor(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <returns></returns>
	inline HWND dcxListView_EditLabel(_In_ HWND hwnd, _In_ const int i) noexcept
	{
		return ListView_EditLabel(hwnd, i);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lvhti"></param>
	/// <returns></returns>
	inline int dcxListView_HitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvhti) noexcept
	{
		return ListView_HitTest(hwnd, lvhti);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lvhti"></param>
	/// <returns></returns>
	inline int dcxListView_HitTestEx(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvhti) noexcept
	{
		return ListView_HitTestEx(hwnd, lvhti);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nItem"></param>
	/// <returns></returns>
	inline BOOL dcxListView_Update(_In_ HWND hwnd, _In_ int nItem) noexcept
	{
		return ListView_Update(hwnd, nItem);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nFirst"></param>
	/// <param name="nLast"></param>
	/// <returns></returns>
	inline BOOL dcxListView_RedrawItems(_In_ HWND hwnd, _In_ int nFirst, _In_ int nLast) noexcept
	{
		return ListView_RedrawItems(hwnd, nFirst, nLast);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxListView_SetTextColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetTextColor(hwnd, clr);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxListView_SetTextBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetTextBkColor(hwnd, clr);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxListView_SetBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetBkColor(hwnd, clr);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxListView_SetOutlineColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetOutlineColor(hwnd, clr);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lplvim"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetInsertMark(_In_ HWND hwnd, _In_ LPLVINSERTMARK lplvim) noexcept
	{
		return ListView_SetInsertMark(hwnd, lplvim);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline BOOL dcxListView_ClearInsertMark(_In_ HWND hwnd) noexcept
	{
		LVINSERTMARK lvim{};
		lvim.cbSize = sizeof(LVINSERTMARK);
		lvim.iItem = -1;

		return Dcx::dcxListView_SetInsertMark(hwnd, &lvim);
	}

	/// <summary>
	/// Creates a drag image list for the specified item.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="i">- The index of the item.</param>
	/// <param name="pt">- A pointer to a POINT structure that receives the initial location of the upper-left corner of the image, in view coordinates.</param>
	/// <returns>Returns the handle to the drag image list if successful, or NULL otherwise.</returns>
	[[nodiscard("Memory leak")]] inline HIMAGELIST dcxListView_CreateDragImage(_In_ HWND hwnd, _In_ int i, _Inout_ LPPOINT pt) noexcept
	{
		return ListView_CreateDragImage(hwnd, i, pt);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <returns></returns>
	[[nodiscard]] inline BOOL dcxListView_IsItemVisible(_In_ HWND hwnd, _In_ int i) noexcept
	{
		return ListView_IsItemVisible(hwnd, i);
	}
	/// <summary>
	/// Undocumented message.
	/// Set a subitem for editing (used mostly when setting embeded control in explorer)
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iItem"></param>
	/// <param name="iSubItem"></param>
	/// <returns></returns>
	inline LRESULT dcxListView_EditSubItem(_In_ HWND hwnd, _In_ int iItem, _In_ int iSubItem) noexcept
	{
		return SendMessage(hwnd, LVM_EDITSUBITEM, iItem, iSubItem);
	}
	/// <summary>
	/// Undocumented message.
	/// Ensure a specified subitem is visible.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iItem"></param>
	/// <param name="iSubItem"></param>
	/// <returns></returns>
	inline LRESULT dcxListView_EnsureSubItemVisible(_In_ HWND hwnd, _In_ int iItem, _In_ int iSubItem) noexcept
	{
		return SendMessage(hwnd, LVM_ENSURESUBITEMVISIBLE, iItem, iSubItem);
	}
	/// <summary>
	/// Undocumented message.
	/// Get the ListViews COM interface.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iid"></param>
	/// <param name="pUnkn"></param>
	/// <returns></returns>
	inline BOOL dcxListView_QueryInterface(_In_ HWND hwnd, _In_ REFIID iid, _Inout_ IUnknown** pUnkn) noexcept
	{
		if ((!hwnd) || (!pUnkn))
			return FALSE;

		return gsl::narrow_cast<BOOL>(SendMessage(hwnd, LVM_QUERYINTERFACE, reinterpret_cast<WPARAM>(&iid), reinterpret_cast<LPARAM>(pUnkn)));
	}
	/// <summary>
	/// Undocumented message.
	/// Set the ListViews left, right, top, bottom margins.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pRC"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetViewMargin(_In_ HWND hwnd, _In_ LPCRECT pRC) noexcept
	{
		return gsl::narrow_cast<BOOL>(SendMessage(hwnd, LVM_SETVIEWMARGIN, 0, reinterpret_cast<LPARAM>(pRC)));
	}
	/// <summary>
	/// Undocumented message.
	/// Get the ListViews left, right, top, bottom margins.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pRC"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetViewMargin(_In_ HWND hwnd, _Inout_ LPRECT pRC) noexcept
	{
		return gsl::narrow_cast<BOOL>(SendMessage(hwnd, LVM_GETVIEWMARGIN, 0, reinterpret_cast<LPARAM>(pRC)));
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pRC"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetViewRect(_In_ HWND hwnd, _Inout_ LPRECT pRC) noexcept
	{
		return gsl::narrow_cast<BOOL>(SendMessage(hwnd, LVM_GETVIEWRECT, 0, reinterpret_cast<LPARAM>(pRC)));
	}
	inline DWORD dcxListView_GetView(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, LVM_GETVIEW, 0, 0));
	}
	/// <summary>
	/// Get the item at x,y screen position.
	/// </summary>
	/// <param name="hwnd"> = the listview control</param>
	/// <param name="x"> = the x screen position</param>
	/// <param name="y"> = the y screen position</param>
	/// <returns>the item at x,y or -1 for no item there.</returns>
	int dcxListView_GetItemAtPos(_In_ HWND hwnd, _In_ int x, _In_ int y) noexcept;
	/// <summary>
	/// Get the item at the cursor position.
	/// </summary>
	/// <param name="hwnd"> = the listview control.</param>
	/// <returns>the item at the cursor or -1 for no item there.</returns>
	inline int dcxListView_GetItemAtCursor(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// Test where cursor is over control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	LVHITTESTINFO dcxListView_CursorHitTest(_In_ HWND hwnd) noexcept;

	// Header

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hdti"></param>
	/// <returns></returns>
	inline LRESULT dcxHeader_HitTest(_In_ HWND hwnd, _Inout_ LPHDHITTESTINFO hdti) noexcept
	{
		return SendMessage(hwnd, HDM_HITTEST, 0U, reinterpret_cast<LPARAM>(hdti));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="phi"></param>
	/// <returns></returns>
	inline BOOL dcxHeader_GetItem(_In_ HWND hwnd, _In_ const int i, _Inout_ LPHDITEM phi) noexcept
	{
		return Header_GetItem(hwnd, i, phi);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <param name="phi"></param>
	/// <returns></returns>
	inline BOOL dcxHeader_SetItem(_In_ HWND hwnd, _In_ const int i, _In_ const LPHDITEM phi) noexcept
	{
		return Header_SetItem(hwnd, i, phi);
	}

	// ListBox

	/// <summary>
	/// Add a string to the listbox
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nPos"></param>
	/// <param name="txt"></param>
	/// <returns></returns>
	inline int dcxListBox_InsertString(_In_ HWND hwnd, _In_ int nPos, _In_ const TString& txt) noexcept
	{
		return ListBox_InsertString(hwnd, nPos, txt.to_chr());
	}

	/// <summary>
	/// Get an items rect.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nPos"></param>
	/// <param name="prc"></param>
	/// <returns>true on success</returns>
	inline bool dcxListBox_GetItemRect(_In_ HWND hwnd, _In_ int nPos, _Out_ LPRECT prc) noexcept
	{
		return (ListBox_GetItemRect(hwnd, nPos, prc) != LB_ERR);
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <returns></returns>
	[[nodiscard]] TString dcxListBox_GetText(_In_ HWND hwnd, _In_ int i);

	/// <summary>
	/// Get the item at a specific point.
	/// </summary>
	/// <param name="hListbox"></param>
	/// <param name="pt"></param>
	/// <returns>Item number or -1 if no item</returns>
	int dcxListBox_GetPointItem(_In_ HWND hListbox, _In_ POINT pt) noexcept;

	/// <summary>
	/// Get the item the mouse is currently over
	/// </summary>
	/// <param name="hListbox"></param>
	/// <returns></returns>
	int dcxListBox_GetHoverItem(_In_ HWND hListbox) noexcept;

	/// <summary>
	/// Test where a point is
	/// uses the same return codes as Listviews LVM_HITTEST, and the same structure.
	/// </summary>
	/// <param name="hListbox"></param>
	/// <param name="phti"></param>
	/// <returns>true on success</returns>
	bool dcxListBox_HitTest(_In_ HWND hListbox, _Inout_ LPLVHITTESTINFO phti) noexcept;

	// ToolTip

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pti"></param>
	/// <returns></returns>
	inline bool dcxToolTip_GetCurrentTool(_In_ HWND hwnd, _Inout_ LPTOOLINFO pti) noexcept
	{
		return (SendMessage(hwnd, TTM_GETCURRENTTOOL, 0, reinterpret_cast<LPARAM>(pti)) != 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="bActivate"></param>
	/// <param name="pti"></param>
	inline void dcxToolTip_TrackActivate(_In_ HWND hwnd, _In_ BOOL bActivate, _In_ LPTOOLINFO pti) noexcept
	{
		SendMessage(hwnd, TTM_TRACKACTIVATE, bActivate, reinterpret_cast<LPARAM>(pti));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	inline void dcxToolTip_Pop(_In_ HWND hwnd) noexcept
	{
		SendMessage(hwnd, TTM_POP, 0, 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	inline void dcxToolTip_PopUp(_In_ HWND hwnd) noexcept
	{
		SendMessage(hwnd, TTM_POPUP, 0, 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pti"></param>
	inline void dcxToolTip_SetToolInfo(_In_ HWND hwnd, _Inout_ LPTOOLINFO pti) noexcept
	{
		SendMessage(hwnd, TTM_SETTOOLINFO, 0, reinterpret_cast<LPARAM>(pti));
	}

	enum class dcxToolTipTitleIcons
		: int
	{
		eTTI_NONE,
		eTTI_INFO,
		eTTI_WARNING,
		eTTI_ERROR,
		eTTI_INFO_LARGE,
		eTTI_WARNING_LARGE,
		eTTI_ERROR_LARGE
	};

	/// <summary>
	/// Set the tooltips title text and icon.
	/// </summary>
	/// <param name="hwnd">- The tooltip hwnd.</param>
	/// <param name="hIcon">- The icon to use.</param>
	/// <param name="str">- The string to use as a title. (Must not exceed 100 chars including the NULL)</param>
	/// <returns>true on success</returns>
	inline bool dcxToolTip_SetTitle(_In_ HWND hwnd, _In_ HICON hIcon, _In_z_ LPWSTR str) noexcept
	{
		//When calling TTM_SETTITLE, the string pointed to by lParam must not exceed 100 TCHARs in length, including the terminating NULL.
		return (SendMessage(hwnd, TTM_SETTITLE, reinterpret_cast<WPARAM>(hIcon), reinterpret_cast<LPARAM>(str)) == TRUE);
	}

	/// <summary>
	/// Set the tooltips title text and icon.
	/// </summary>
	/// <param name="hwnd">- The tooltip hwnd.</param>
	/// <param name="eIcon">- The default icon to use.</param>
	/// <param name="str">- The string to use as a title. (Must not exceed 100 chars including the NULL)</param>
	/// <returns>true on success</returns>
	inline bool dcxToolTip_SetTitle(_In_ HWND hwnd, _In_ dcxToolTipTitleIcons eIcon, _In_z_ LPWSTR str) noexcept
	{
		//When calling TTM_SETTITLE, the string pointed to by lParam must not exceed 100 TCHARs in length, including the terminating NULL.
		return (SendMessage(hwnd, TTM_SETTITLE, gsl::narrow_cast<WPARAM>(eIcon), reinterpret_cast<LPARAM>(str)) == TRUE);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	inline void dcxToolTip_TrackPosition(_In_ HWND hwnd, _In_ int x, _In_ int y) noexcept
	{
		SendMessage(hwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(x + 10, y - 20));	// Dcx::dcxMAKELPARAM() not available here.
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pti"></param>
	inline void dcxToolTip_UpdateTipText(_In_ HWND hwnd, _In_ LPTOOLINFO pti) noexcept
	{
		SendMessage(hwnd, TTM_UPDATETIPTEXT, 0, reinterpret_cast<LPARAM>(pti));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="ctrl"></param>
	/// <param name="id"></param>
	/// <param name="str"></param>
	inline void dcxToolTip_UpdateTipText(_In_ HWND hwnd, _In_ HWND ctrl, _In_ UINT_PTR id, _In_z_ LPWSTR str) noexcept
	{
		TOOLINFO ti{};

		ti.cbSize = sizeof(TOOLINFO);
		ti.hinst = GetModuleHandle(nullptr);
		ti.hwnd = ctrl;
		ti.uId = id;
		ti.lpszText = str;

		dcxToolTip_UpdateTipText(hwnd, &ti);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="bState"></param>
	inline void dcxToolTip_Activate(_In_ HWND hwnd, _In_ BOOL bState) noexcept
	{
		SendMessage(hwnd, TTM_ACTIVATE, bState, 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pti"></param>
	/// <returns></returns>
	inline bool dcxToolTip_AddTool(_In_ HWND hwnd, _In_ LPTOOLINFO pti) noexcept
	{
		return (SendMessage(hwnd, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(pti)) == TRUE);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="bState"></param>
	/// <param name="prc"></param>
	/// <returns></returns>
	inline bool dcxToolTip_AdjustRect(_In_ HWND hwnd, _In_ BOOL bState, _Inout_ LPRECT prc) noexcept
	{
		return (SendMessage(hwnd, TTM_ADJUSTRECT, bState, reinterpret_cast<LPARAM>(prc)) != 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline COLORREF dcxToolTip_GetTipBkColor(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<COLORREF>(SendMessage(hwnd, TTM_GETTIPBKCOLOR, 0, 0));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline COLORREF dcxToolTip_GetTipTextColor(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<COLORREF>(SendMessage(hwnd, TTM_GETTIPTEXTCOLOR, 0, 0));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxToolTip_SetTipBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return gsl::narrow_cast<COLORREF>(SendMessage(hwnd, TTM_SETTIPBKCOLOR, gsl::narrow_cast<WPARAM>(clr), 0));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxToolTip_SetTipTextColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return gsl::narrow_cast<COLORREF>(SendMessage(hwnd, TTM_SETTIPTEXTCOLOR, gsl::narrow_cast<WPARAM>(clr), 0));
	}

	// TreeView

	/// <summary>
	/// Get the number of items in the control.
	/// The control does not support >65536 items.
	/// </summary>
	/// <param name="hTree"></param>
	/// <returns></returns>
	size_t dcxTreeView_GetCount(_In_ HWND hTree) noexcept;

	/// <summary>
	/// Get the controls image list.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="type"></param>
	/// <returns></returns>
	[[nodiscard]] inline HIMAGELIST dcxTreeView_GetImageList(_In_ HWND hwnd, _In_ int type) noexcept
	{
		return TreeView_GetImageList(hwnd, gsl::narrow_cast<WPARAM>(type));
	}

	/// <summary>
	/// Sets the controls image list.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="himl"></param>
	/// <param name="type"></param>
	/// <returns></returns>
	inline HIMAGELIST dcxTreeView_SetImageList(_In_ HWND hwnd, _In_ HIMAGELIST himl, _In_ int type) noexcept
	{
		return TreeView_SetImageList(hwnd, himl, gsl::narrow_cast<WPARAM>(type));
	}

	/// <summary>
	/// Insert a treeview item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="ptvins"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_InsertItem(_In_ HWND hwnd, _In_ const TVINSERTSTRUCT* const ptvins) noexcept
	{
		return TreeView_InsertItem(hwnd, ptvins);
	}

	/// <summary>
	/// Get the tooltip control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline HWND dcxTreeView_GetToolTips(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetToolTips(hwnd);
	}

	/// <summary>
	/// Get an items info.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="ptvi"></param>
	/// <returns></returns>
	inline bool dcxTreeView_GetItem(_In_ HWND hwnd, _Inout_ LPTVITEMEX ptvi) noexcept
	{
		return (TreeView_GetItem(hwnd, ptvi) != FALSE);
	}

	/// <summary>
	/// Set an items info.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="ptvi"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SetItem(_In_ HWND hwnd, _In_ const LPTVITEMEX ptvi) noexcept
	{
		return (TreeView_SetItem(hwnd, ptvi) != FALSE);
	}

	/// <summary>
	/// Get the root item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetRoot(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(es.47) return TreeView_GetRoot(hwnd);
	}

	/// <summary>
	/// Get items parent.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetParent(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetParent(hwnd, hItem);
	}

	/// <summary>
	/// Get items first child.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetChild(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetChild(hwnd, hItem);
	}

	/// <summary>
	/// Get next sibling item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetNextSibling(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetNextSibling(hwnd, hItem);
	}

	/// <summary>
	/// Get previous sibling item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetPrevSibling(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetPrevSibling(hwnd, hItem);
	}

	/// <summary>
	/// Get controls extended styles.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxTreeView_GetExtendedStyle (_In_ HWND hwnd) noexcept
	{
		return TreeView_GetExtendedStyle(hwnd);
	}

	/// <summary>
	/// Set controls extended styles.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="exstyle"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	inline DWORD dcxTreeView_SetExtendedStyle(_In_ HWND hwnd, _In_ DWORD exstyle, _In_ DWORD mask) noexcept
	{
		return TreeView_SetExtendedStyle(hwnd, exstyle, mask);
	}

	/// <summary>
	/// Set controls extended styles.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="exstyle"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	inline DWORD dcxTreeView_SetExtendedStyle(_In_ HWND hwnd, _In_ WindowExStyle exstyle, _In_ WindowExStyle mask) noexcept
	{
		return TreeView_SetExtendedStyle(hwnd, gsl::narrow_cast<DWORD>(exstyle), gsl::narrow_cast<DWORD>(mask));
	}

	/// <summary>
	/// Set text colour.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetTextColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetTextColor(hwnd, clr);
	}

	/// <summary>
	/// Set background colour.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetBkColor(hwnd, clr);
	}

	/// <summary>
	/// Set line colour.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetLineColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetLineColor(hwnd, clr);
	}

	/// <summary>
	/// Set insert mark colour.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetInsertMarkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetInsertMarkColor(hwnd, clr);
	}

	/// <summary>
	/// Get items state.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hti"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	inline UINT dcxTreeView_GetItemState(_In_ HWND hwnd, _In_ HTREEITEM hti, _In_ UINT mask) noexcept
	{
		return TreeView_GetItemState(hwnd, hti, mask);
	}

	/// <summary>
	/// Ensure item is visible.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hti"></param>
	/// <returns></returns>
	inline bool dcxTreeView_EnsureVisible(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return (TreeView_EnsureVisible(hwnd, hti) != FALSE);
	}

	/// <summary>
	/// Start editing the items label.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hti"></param>
	/// <returns>The edit control for editing.</returns>
	inline HWND dcxTreeView_EditLabel(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return TreeView_EditLabel(hwnd, hti);
	}

	/// <summary>
	/// Set items as selected.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hti"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SelectItem(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return (TreeView_SelectItem(hwnd, hti) != FALSE);
	}

	/// <summary>
	/// Set height for all items.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="height"></param>
	/// <returns></returns>
	inline int dcxTreeView_SetItemHeight(_In_ HWND hwnd, _In_ int height) noexcept
	{
		return TreeView_SetItemHeight(hwnd, height);
	}

	/// <summary>
	/// Set border size around control.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="dwFlags"></param>
	/// <param name="iLeft"></param>
	/// <param name="iTop"></param>
	/// <returns></returns>
	inline int dcxTreeView_SetBorder(_In_ HWND hwnd, _In_ DWORD dwFlags, _In_ int iLeft, _In_ int iTop) noexcept
	{
		return TreeView_SetBorder(hwnd, dwFlags, iLeft, iTop);
	}

	/// <summary>
	/// Expand items children.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="Style"></param>
	/// <param name="lpstr"></param>
	inline bool dcxTreeView_Expand(_In_ HWND hwnd, _In_ HTREEITEM hti, _In_ DWORD iCode) noexcept
	{
		return (TreeView_Expand(hwnd, hti, iCode) != FALSE);
	}

	/// <summary>
	/// Sort items.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hItem"></param>
	/// <param name="bRecurse"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SortChildren(_In_ HWND hwnd, _In_ HTREEITEM hItem, _In_ bool bRecurse) noexcept
	{
		return (TreeView_SortChildren(hwnd, hItem, bRecurse) != FALSE);
	}

	/// <summary>
	/// Sort items via callback function.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="ptscb"></param>
	/// <param name="bRecurse"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SortChildrenCB(_In_ HWND hwnd, _In_ LPTVSORTCB ptscb, _In_ bool bRecurse) noexcept
	{
		return (TreeView_SortChildrenCB(hwnd, ptscb, bRecurse) != FALSE);
	}

	/// <summary>
	/// Get items check state.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hti"></param>
	/// <returns></returns>
	inline UINT dcxTreeView_GetCheckState(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return TreeView_GetCheckState(hwnd, hti);
	}

	/// <summary>
	/// Get the edit control being used to edit labels.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline HWND dcxTreeView_GetEditControl(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetEditControl(hwnd);
	}

	/// <summary>
	/// Retrieves the tree-view item that is the target of a drag-and-drop operation.
	/// </summary>
	/// <param name="hwnd">- Handle to the tree-view control.</param>
	/// <returns>Returns the handle to the item if successful. For most cases, the message returns a NULL value to indicate an error.</returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetDropHilight(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetDropHilight(hwnd);
	}

	/// <summary>
	/// Retrieves the bounding rectangle for a tree-view item and indicates whether the item is visible.
	/// </summary>
	/// <param name="hwnd">- Handle to the tree-view control.</param>
	/// <param name="hItem">- Handle to the tree-view item.</param>
	/// <param name="prc">- Pointer to a RECT structure that receives the bounding rectangle. The coordinates are relative to the upper-left corner of the tree-view control.</param>
	/// <param name="bCode">- Value specifying the portion of the item for which to retrieve the bounding rectangle. If this parameter is TRUE, the bounding rectangle includes only the text of the item. Otherwise, it includes the entire line that the item occupies in the tree-view control.</param>
	/// <returns></returns>
	inline bool dcxTreeView_GetItemRect(_In_ HWND hwnd, _In_ HTREEITEM hItem, _In_ LPRECT prc, _In_ bool bCode) noexcept
	{
		return (TreeView_GetItemRect(hwnd, hItem, prc, bCode) != FALSE);
	}

	/// <summary>
	/// Get the currently selected item.
	/// </summary>
	/// <param name="hTree"></param>
	/// <see cref="dcxTreeView_GetNextItem"/>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetSelection(_In_ HWND hTree) noexcept
	{
		if (!hTree)
			return nullptr;

		GSL_SUPPRESS(es.47) GSL_SUPPRESS(lifetime.4) return TreeView_GetSelection(hTree);
	}

	/// <summary>
	/// Retrieves the tree-view item that bears the specified relationship to a specified item.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem">- Handle to an item.</param>
	/// <param name="iCode">- Flag specifying the item to retrieve.</param>
	/// <returns></returns>
	/// <remarks>This macro will return NULL if the item being retrieved is the root node of the tree. For example, if you use this macro with the TVGN_PARENT flag on a first-level child of the tree view's root node, the macro will return NULL.</remarks>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetNextItem(_In_ HWND hTree, _In_opt_ HTREEITEM hItem, _In_ int iCode) noexcept
	{
		if (!hTree)
			return nullptr;

		GSL_SUPPRESS(es.47) GSL_SUPPRESS(lifetime.4) return TreeView_GetNextItem(hTree, hItem, iCode);
	}

	/// <summary>
	/// Show an items info tip.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	inline DWORD dcxTreeView_ShowInfoTip(_In_ HWND hTree, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_ShowInfoTip(hTree, hItem);
	}

	/// <summary>
	/// Set the control to unicode format or not.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="bUnicode"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SetUnicodeFormat(_In_ HWND hTree, _In_ bool bUnicode) noexcept
	{
		return (TreeView_SetUnicodeFormat(hTree, bUnicode) != FALSE);
	}

	/// <summary>
	/// Sets a tree-view control's child tooltip control.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="hTooltip"></param>
	/// <returns></returns>
	inline HWND dcxTreeView_SetToolTips(_In_ HWND hTree, _In_ HWND hTooltip) noexcept
	{
		return TreeView_SetToolTips(hTree, hTooltip);
	}

	/// <summary>
	/// Determines the location of the specified point relative to the client area of a tree-view control.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="lpht">- Pointer to a TVHITTESTINFO structure. When the message is sent, the pt member specifies the coordinates of the point to test. When the message returns, the hItem member is the handle to the item at the specified point or NULL if no item occupies the point. Also, when the message returns, the flags member is a hit test value that indicates the location of the specified point.</param>
	/// <returns>The items under the point or NULL</returns>
	inline HTREEITEM dcxTreeView_HitTest(_In_ HWND hTree, _Inout_ LPTVHITTESTINFO lpht) noexcept
	{
		return TreeView_HitTest(hTree, lpht);
	}

	/// <summary>
	/// Sets the insertion mark in a tree-view control.
	/// </summary>
	/// <param name="hTree">- Handle to a tree-view control.</param>
	/// <param name="hItem">- HTREEITEM that specifies at which item the insertion mark will be placed. If this argument is NULL, the insertion mark is removed.</param>
	/// <param name="bAfter">- bool value that specifies if the insertion mark is placed before or after the specified item. If this argument is nonzero, the insertion mark will be placed after the item. If this argument is zero, the insertion mark will be placed before the item.</param>
	/// <returns>true on success</returns>
	inline bool dcxTreeView_SetInsertMark(_In_ HWND hTree, _In_opt_ HTREEITEM hItem, _In_ bool bAfter) noexcept
	{
		return (TreeView_SetInsertMark(hTree, hItem, bAfter) != FALSE);
	}

	/// <summary>
	/// Sets the width of indentation for a tree-view control and redraws the control to reflect the new width.
	/// </summary>
	/// <param name="hTree">- Handle to a tree-view control.</param>
	/// <param name="iIndent">- Width, in pixels, of the indentation. If this parameter is less than the system-defined minimum width, the new width is set to the system-defined minimum.</param>
	/// <remarks>The system-defined minimum indent value is typically five pixels, but it is not fixed. To retrieve the exact value of the minimum indent on a particular system, use the <c>TreeView_SetIndent</c> macro with indent set to zero. Then use the <c>TreeView_GetIndent</c> macro to retrieve the minimum indent value.</remarks>
	/// <returns>true on success.</returns>
	inline bool dcxTreeView_SetIndent(_In_ HWND hTree, _In_ int iIndent) noexcept
	{
		return (TreeView_SetIndent(hTree, iIndent) != FALSE);
	}

	/// <summary>
	/// Get an items last sibling.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="child"></param>
	/// <returns></returns>
	[[nodiscard]] HTREEITEM dcxTreeView_GetLastSibling(_In_ HWND hTree, _In_ HTREEITEM child) noexcept;

	/// <summary>
	/// Set an items state.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="hItem"></param>
	/// <param name="data"></param>
	/// <param name="mask"></param>
	void dcxTreeView_SetItemState(_In_ HWND hTree, _In_ HTREEITEM hItem, _In_ UINT data, _In_ UINT mask) noexcept;

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hItem"></param>
	/// <returns>The items text as a TString</returns>
	TString dcxTreeView_GetItemText(_In_ HWND hTree, _In_ HTREEITEM hItem);

	/// <summary>
	/// Set an items text.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="hItem"></param>
	/// <param name="txt"></param>
	void dcxTreeView_SetItemText(_In_ HWND hTree, _In_ HTREEITEM hItem, _In_ const TString& txt) noexcept;

	/// <summary>
	/// Delete a specified item. This function includes the fix for items not being displayed after a delete.
	/// </summary>
	/// <param name="hTree"></param>
	/// <param name="item"></param>
	/// <returns></returns>
	bool dcxTreeView_DeleteItem(_In_ HWND hTree, _In_ const HTREEITEM item) noexcept;

	/// <summary>
	/// Delete all items. This function includes the fix for items not being displayed after a delete.
	/// </summary>
	/// <param name="hTree"></param>
	/// <returns></returns>
	bool dcxTreeView_DeleteAllItems(_In_ HWND hTree) noexcept;

	/// <summary>
	/// Get the HitTest info about the current cursor position.
	/// </summary>
	/// <param name="hTree"></param>
	/// <returns></returns>
	TVHITTESTINFO dcxTreeView_GetCursorItem(_In_ HWND hTree) noexcept;

	// Button

	/// <summary>
	/// Get the length of the note text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline LRESULT dcxButton_GetNoteLength(_In_ HWND hwnd) noexcept
	{
		if (!hwnd)
			return 0;

		return Button_GetNoteLength(hwnd);
	}

	/// <summary>
	/// Get the note text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="szBuf"></param>
	/// <param name="l"></param>
	/// <returns></returns>
	inline bool dcxButton_GetNote(_In_ HWND hwnd, _Inout_z_ TCHAR* szBuf, _Inout_ DWORD* l) noexcept
	{
		if (!hwnd || !szBuf || !l)
			return false;

		return (Button_GetNote(hwnd, szBuf, l) != FALSE); // NB: two POINTERS
	}

	/// <summary>
	/// Get the note text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	TString dcxButton_GetNote(_In_ HWND hwnd);

	/// <summary>
	/// Set a buttons note text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="szBuf"></param>
	inline void dcxButton_SetNote(_In_ HWND hwnd, _In_z_ const TCHAR* szBuf) noexcept
	{
		if (!hwnd || !szBuf)
			return;

		Button_SetNote(hwnd, szBuf);
	}

	/// <summary>
	/// Set a buttons text, this just maps to SetWindowText
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="szBuf"></param>
	inline void dcxButton_SetText(_In_ HWND hwnd, _In_z_ const TCHAR* szBuf) noexcept
	{
		if (!hwnd || !szBuf)
			return;

		Button_SetText(hwnd, szBuf);
	}

	/// <summary>
	/// Set a buttons style.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="dwStyle"></param>
	/// <param name="bRedraw"></param>
	inline void dcxButton_SetStyle(_In_ HWND hwnd, _In_ DWORD dwStyle, BOOL bRedraw) noexcept
	{
		if (!hwnd)
			return;

		Button_SetStyle(hwnd, dwStyle, bRedraw);
	}

	// ComboEx

	/// <summary>
	/// Set controls extended styles.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="mask"></param>
	/// <param name="dw"></param>
	/// <returns></returns>
	inline DWORD dcxComboEx_SetExtendedStyles(_In_ HWND hwnd, _In_ const WPARAM mask, _In_ const LPARAM dw) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, CBEM_SETEXTENDEDSTYLE, mask, dw));
	}

	/// <summary>
	/// Get controls extended styles.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxComboEx_GetExtendedStyles(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, CBEM_GETEXTENDEDSTYLE, 0, 0));
	}

	/// <summary>
	/// Set the controls horizontal extent, This adjusts the horizontal scroll bar.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iWidth"></param>
	/// <returns></returns>
	inline DWORD dcxCombo_SetHorizExtent(_In_ HWND hwnd, _In_ const WPARAM iWidth) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, CB_SETHORIZONTALEXTENT, iWidth, 0));
	}

	/// <summary>
	/// Get the controls horizontal extent.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxCombo_GetHorizExtent(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, CB_GETHORIZONTALEXTENT, 0, 0));
	}

	/// <summary>
	/// Set an existing items info.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lpcCBItem"></param>
	/// <returns></returns>
	inline bool dcxComboEx_SetItem(_In_ HWND hwnd, _In_ const PCOMBOBOXEXITEM lpcCBItem) noexcept
	{
		return (SendMessage(hwnd, CBEM_SETITEM, 0, reinterpret_cast<LPARAM>(lpcCBItem)) != 0);
	}

	/// <summary>
	/// Get an items info.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lpcCBItem"></param>
	/// <returns></returns>
	inline LRESULT dcxComboEx_GetItem(_In_ HWND hwnd, const PCOMBOBOXEXITEM lpcCBItem) noexcept
	{
		return SendMessage(hwnd, CBEM_GETITEM, 0U, reinterpret_cast<LPARAM>(lpcCBItem));
	}

	/// <summary>
	/// Add/Insert an item.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="lpcCBItem"></param>
	/// <returns></returns>
	inline LRESULT dcxComboEx_InsertItem(_In_ HWND hwnd, _In_ const PCOMBOBOXEXITEM lpcCBItem) noexcept
	{
		return SendMessage(hwnd, CBEM_INSERTITEM, 0, reinterpret_cast<LPARAM>(lpcCBItem));
	}

	/// <summary>
	/// Get edit control part.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxComboEx_GetEditControl(_In_ HWND hwnd) noexcept
	{
		return to_hwnd(SendMessage(hwnd, CBEM_GETEDITCONTROL, 0U, 0));
	}

	/// <summary>
	/// Get the combo control part.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxComboEx_GetComboControl(_In_ HWND hwnd) noexcept
	{
		return to_hwnd(SendMessage(hwnd, CBEM_GETCOMBOCONTROL, 0U, 0));
	}

	// Statusbar

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <returns></returns>
	UINT dcxStatusBar_GetTextLength(_In_ HWND hwnd, _In_ int iPart) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="lpstr"></param>
	/// <returns></returns>
	inline LRESULT dcxStatusBar_GetText(_In_ HWND hwnd, _In_ int iPart, _Inout_z_ LPWSTR lpstr) noexcept
	{
		return SendMessage(hwnd, SB_GETTEXTW, gsl::narrow_cast<WPARAM>(iPart), reinterpret_cast<LPARAM>(lpstr));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="clrBk"></param>
	/// <returns></returns>
	inline COLORREF dcxStatusBar_SetBkColor(_In_ HWND hwnd, _In_ COLORREF clrBk) noexcept
	{
		return gsl::narrow_cast<COLORREF>(SendMessage(hwnd, SB_SETBKCOLOR, 0U, gsl::narrow_cast<LPARAM>(clrBk)));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nParts"></param>
	/// <param name="aWidths"></param>
	/// <returns></returns>
	inline bool dcxStatusBar_SetParts(_In_ HWND hwnd, _In_ UINT nParts, _In_ const LPINT aWidths) noexcept
	{
		return (SendMessage(hwnd, SB_SETPARTS, gsl::narrow_cast<WPARAM>(nParts), reinterpret_cast<LPARAM>(aWidths)) != FALSE);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nParts"></param>
	/// <param name="aWidths"></param>
	/// <returns></returns>
	inline int dcxStatusBar_GetParts(_In_ HWND hwnd, _In_ UINT nParts, _In_ const LPINT aWidths) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, SB_GETPARTS, gsl::narrow_cast<WPARAM>(nParts), reinterpret_cast<LPARAM>(aWidths)));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="Style"></param>
	/// <param name="lpstr"></param>
	/// <returns></returns>
	inline LRESULT dcxStatusBar_SetText(_In_ HWND hwnd, _In_ int iPart, _In_ int Style, _In_z_ const WCHAR* const lpstr) noexcept
	{
		return SendMessage(hwnd, SB_SETTEXTW, gsl::narrow_cast<WPARAM>(iPart | Style), reinterpret_cast<LPARAM>(lpstr));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <returns></returns>
	UINT dcxStatusBar_GetPartFlags(_In_ HWND hwnd, _In_ int iPart) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <returns></returns>
	TString dcxStatusBar_GetText(_In_ HWND hwnd, _In_ int iPart);

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <returns></returns>
	template <typename T>
	T dcxStatusBar_GetPartInfo(_In_ HWND hwnd, _In_ int iPart) noexcept
	{
		if (dcx_testflag(dcxStatusBar_GetPartFlags(hwnd, iPart), SBT_OWNERDRAW))
			return reinterpret_cast<T>(SendMessage(hwnd, SB_GETTEXT, gsl::narrow_cast<WPARAM>(iPart), 0));

		return nullptr;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="Style"></param>
	/// <param name="pPart"></param>
	/// <returns></returns>
	template <typename T>
	LRESULT dcxStatusBar_SetPartInfo(_In_ HWND hwnd, _In_ int iPart, _In_ int Style, _In_ const T pPart) noexcept
	{
		return SendMessage(hwnd, SB_SETTEXT, gsl::narrow_cast<WPARAM>(iPart | (Style | SBT_OWNERDRAW)), reinterpret_cast<LPARAM>(pPart));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="lpstr"></param>
	inline void dcxStatusBar_SetTipText(_In_ HWND hwnd, _In_ int iPart, _In_z_ const WCHAR* const lpstr) noexcept
	{
		SendMessage(hwnd, SB_SETTIPTEXTW, gsl::narrow_cast<WPARAM>(iPart), reinterpret_cast<LPARAM>(lpstr));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="nSize"></param>
	/// <param name="lpstr"></param>
	inline void dcxStatusBar_GetTipText(_In_ HWND hwnd, _In_ int iPart, _In_ int nSize, _In_z_ const LPWSTR lpstr) noexcept
	{
		SendMessage(hwnd, SB_GETTIPTEXTW, MAKEWPARAM(iPart, nSize), reinterpret_cast<LPARAM>(lpstr));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <returns></returns>
	TString dcxStatusBar_GetTipText(_In_ HWND hwnd, _In_ int iPart);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="lprc"></param>
	/// <returns></returns>
	inline bool dcxStatusBar_GetRect(_In_ HWND hwnd, _In_ int iPart, _Inout_ LPRECT lprc) noexcept
	{
		return (SendMessage(hwnd, SB_GETRECT, gsl::narrow_cast<WPARAM>(iPart), reinterpret_cast<LPARAM>(lprc)) == TRUE);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <param name="hIcon"></param>
	/// <returns></returns>
	inline bool dcxStatusBar_SetIcon(_In_ HWND hwnd, _In_ int iPart, _In_ const HICON hIcon) noexcept
	{
		return (SendMessage(hwnd, SB_SETICON, gsl::narrow_cast<WPARAM>(iPart), reinterpret_cast<LPARAM>(hIcon)) != 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPart"></param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		inline HICON dcxStatusBar_GetIcon(_In_ HWND hwnd, _In_ int iPart) noexcept
	{
		return reinterpret_cast<HICON>(SendMessage(hwnd, SB_GETICON, gsl::narrow_cast<WPARAM>(iPart), gsl::narrow_cast<LPARAM>(0)));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="h"></param>
	inline void dcxStatusBar_SetMinHeight(_In_ HWND hwnd, _In_ int h) noexcept
	{
		SendMessage(hwnd, SB_SETMINHEIGHT, gsl::narrow_cast<WPARAM>(h), 0);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="aWidths"></param>
	/// <returns></returns>
	inline bool dcxStatusBar_GetBorders(_In_ HWND hwnd, _In_ LPINT aWidths) noexcept
	{
		return (SendMessage(hwnd, SB_GETBORDERS, 0U, reinterpret_cast<LPARAM>(aWidths)) == TRUE);
	}

	// Edit

	// version of Edit_SetCaretIndex in the sdk is broken as it uses newCaretIndex instead of the defined newCaretPosition
#ifdef Edit_SetCaretIndex
#undef Edit_SetCaretIndex
#endif
#define Edit_SetCaretIndex(hwndCtl, newCaretPosition) \
        (BOOL)SNDMSG((hwndCtl), EM_SETCARETINDEX, (WPARAM)(newCaretPosition), 0)

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="bEnable"></param>
	/// <returns></returns>
	inline bool dcxEdit_SetReadOnly(_In_ HWND hwnd, _In_ BOOL bEnable) noexcept
	{
		return !!Edit_SetReadOnly(hwnd, bEnable);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="prc"></param>
	inline void dcxEdit_SetRect(_In_ HWND hwnd, _In_ LPCRECT prc) noexcept
	{
		Edit_SetRect(hwnd, prc);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="prc"></param>
	inline void dcxEdit_SetRectNoPaint(_In_ HWND hwnd, _In_ LPCRECT prc) noexcept
	{
		Edit_SetRectNoPaint(hwnd, prc);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="dwStyle"></param>
	/// <param name="dwMask"></param>
	inline void dcxEdit_SetExtendedStyle(_In_ HWND hwnd, _In_ DWORD dwStyle, _In_ DWORD dwMask) noexcept
	{
		Edit_SetExtendedStyle(hwnd, dwStyle, dwMask);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iMax"></param>
	inline void dcxEdit_LimitText(_In_ HWND hwnd, _In_ int iMax) noexcept
	{
		Edit_LimitText(hwnd, iMax);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="chr"></param>
	inline void dcxEdit_SetPasswordChar(_In_ HWND hwnd, _In_ TCHAR chr) noexcept
	{
		Edit_SetPasswordChar(hwnd, chr);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline TCHAR dcxEdit_GetPasswordChar(_In_ HWND hwnd) noexcept
	{
		return Edit_GetPasswordChar(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="prc"></param>
	inline void dcxEdit_GetRect(_In_ HWND hwnd, _In_ LPRECT prc) noexcept
	{
		Edit_GetRect(hwnd, prc);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline bool dcxEdit_ScrollCaret(_In_ HWND hwnd) noexcept
	{
		return !!Edit_ScrollCaret(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwndCtl"></param>
	/// <param name="newCaretPosition"></param>
	/// <returns></returns>
	inline bool dcxEdit_SetCaretIndex(_In_ HWND hwndCtl, _In_ DWORD newCaretPosition) noexcept
	{
		return (Edit_SetCaretIndex(hwndCtl, newCaretPosition) != FALSE);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline DWORD dcxEdit_GetFirstVisibleLine(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(Edit_GetFirstVisibleLine(hwnd));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iLine"></param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxEdit_GetLineIndex(_In_ HWND hwnd, _In_ DWORD iLine) noexcept
	{
		if (!hwnd)
			return 0;

		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(iLine), 0));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iLineChar"></param>
	/// <returns></returns>
	[[nodiscard]] inline POINTL dcxEdit_GetPosFromChar(_In_ HWND hwnd, _In_ DWORD iLineChar) noexcept
	{
		POINTL pl{};
		if (hwnd)
			SendMessage(hwnd, EM_POSFROMCHAR, reinterpret_cast<WPARAM>(&pl), gsl::narrow_cast<LPARAM>(iLineChar));
		return pl;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	int dcxEdit_GetEndOfLine(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nNumerator"></param>
	/// <param name="nDenominator"></param>
	/// <returns></returns>
	bool dcxEdit_GetZoom(_In_ HWND hwnd, _In_ int* nNumerator, _In_ int* nDenominator) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	TString dcxEdit_GetEndOfLineCharacters(_In_ HWND hwnd);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	DWORD dcxEdit_GetCaretIndex(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nPos"></param>
	void dcxEdit_SetCaretIndex2(_In_ HWND hwnd, _In_ DWORD nPos) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="iPos"></param>
	/// <returns></returns>
	DWORD dcxEdit_CharFromPos(_In_ HWND hwnd, _In_ const LONG& iPos) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="ich"></param>
	/// <returns></returns>
	DWORD dcxEdit_LineFromChar(_In_ HWND hwnd, _In_ const LONG& ich) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nStart"></param>
	/// <param name="nEnd"></param>
	void dcxEdit_GetSel(_In_ HWND hwnd, _In_opt_ DWORD* nStart, _In_opt_ DWORD* nEnd) noexcept;

	// Animate

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="pszName"></param>
	/// <returns></returns>
	inline bool dcxAnimate_Open(_In_ HWND hwnd, _In_z_ LPTSTR pszName) noexcept
	{
		return !!Animate_Open(hwnd, pszName);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="hInst"></param>
	/// <param name="pszName"></param>
	/// <returns></returns>
	inline bool dcxAnimate_OpenEx(_In_ HWND hwnd, _In_ HINSTANCE hInst, _In_z_ LPTSTR pszName) noexcept
	{
		return !!Animate_OpenEx(hwnd, hInst, pszName);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline bool dcxAnimate_Close(_In_ HWND hwnd) noexcept
	{
		return !!Animate_Close(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <returns></returns>
	inline bool dcxAnimate_IsPlaying(_In_ HWND hwnd) noexcept
	{
		return !!Animate_IsPlaying(hwnd);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="uFrom"></param>
	/// <param name="uTo"></param>
	/// <param name="uRep"></param>
	/// <returns></returns>
	inline bool dcxAnimate_Play(_In_ HWND hwnd, _In_ UINT uFrom, _In_ UINT uTo, _In_ UINT uRep) noexcept
	{
		return !!Animate_Play(hwnd, uFrom, uTo, uRep);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="uFrame"></param>
	/// <returns></returns>
	inline bool dcxAnimate_Seek(_In_ HWND hwnd, _In_ UINT uFrame) noexcept
	{
		return !!Animate_Seek(hwnd, uFrame);
	}

	/// <summary>
	/// Stops playing an AVI clip in an animation control.
	/// </summary>
	/// <param name="hwnd">- A handle to the animation control.</param>
	/// <returns>true on success.</returns>
	inline bool dcxAnimate_Stop(_In_ HWND hwnd) noexcept
	{
		return !!Animate_Stop(hwnd);
	}
}