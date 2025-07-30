#pragma once

//
//
// Helper functions for accessing standard controls.
//

#include <commctrl.h>
#include "Classes/tstring/tstring.h"

namespace Dcx
{
	// Window

	/// <summary>
	/// Get the windows icon.
	/// </summary>
	/// <param name="hwnd">- A handle to a window.</param>
	/// <param name="uType">- The icon type wanted ICON_SMALL ICON_SMALL2 ICON_BIG</param>
	/// <param name="uDpi">- The dpi of the requested icon.</param>
	/// <returns>The icon requested or nullptr if no icon set.</returns>
	inline HICON dcxWindow_GetIcon(HWND hwnd, UINT uType, UINT uDpi) noexcept
	{
		return reinterpret_cast<HICON>(SendMessage(hwnd, WM_GETICON, uType, uDpi));
	}

	/// <summary>
	/// Get the windows icon.
	/// </summary>
	/// <param name="hwnd">- A handle to a window.</param>
	/// <param name="uType">- The icon type wanted ICON_SMALL ICON_SMALL2 ICON_BIG</param>
	/// <returns>The icon requested or nullptr if no icon set.</returns>
	inline HICON dcxWindow_GetIcon(HWND hwnd, UINT uType) noexcept
	{
		return reinterpret_cast<HICON>(SendMessage(hwnd, WM_GETICON, uType, DcxDPIModule::dcxGetDpiForWindow(hwnd)));
	}

	/// <summary>
	/// Set a windows icon.
	/// </summary>
	/// <param name="mHwnd">- A handle to a window.</param>
	/// <param name="iType">- The icon type to set ICON_SMALL, ICON_BIG, etc...</param>
	/// <param name="hIcon">- The icon to set.</param>
	/// <returns>The previous icon, if any.</returns>
	inline HICON dcxWindow_SetIcon(HWND mHwnd, UINT iType, HICON hIcon) noexcept
	{
		return reinterpret_cast<HICON>(SendMessage(mHwnd, WM_SETICON, iType, reinterpret_cast<LPARAM>(hIcon)));
	}

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
	/// <param name="hwnd">- A handle to a list-view control.</param>
	/// <param name="plvi"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetItem(_In_ HWND hwnd, _In_ const LPLVITEM plvi) noexcept
	{
		return ListView_SetItem(hwnd, plvi);
	}

	/// <summary>
	/// Set an items text.
	/// </summary>
	/// <param name="hwnd">- A handle to a list-view control.</param>
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
	/// <param name="hwnd">- A handle to a list-view control.</param>
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
	/// <param name="hwnd">- A handle to a list-view control.</param>
	/// <param name="lvit"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetInfoTip(_In_ HWND hwnd, _In_ PLVSETINFOTIP lvit) noexcept
	{
		return ListView_SetInfoTip(hwnd, lvit);
	}

	/// <summary>
	/// Set items check state.
	/// </summary>
	/// <param name="hwnd">- A handle to a list-view control.</param>
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
	/// <param name="hwnd">- A handle to a list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="plvi"></param>
	/// <returns></returns>
	inline int dcxListView_InsertItem(_In_ HWND hwnd, _In_ const LPLVITEM plvi) noexcept
	{
		return ListView_InsertItem(hwnd, plvi);
	}

	/// <summary>
	/// Get information on an item.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="plvi"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetItem(_In_ HWND hwnd, _Inout_ LPLVITEM const plvi) noexcept
	{
		return ListView_GetItem(hwnd, plvi);
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="nItem"></param>
	/// <param name="nSubItem"></param>
	/// <returns>The items text</returns>
	TString dcxListView_GetItemText(_In_ HWND hwnd, _In_ int nItem, _In_ int nSubItem);

	/// <summary>
	/// Get the items text length.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="nItem"></param>
	/// <param name="nSubItem"></param>
	/// <returns></returns>
	size_t dcxListView_GetItemTextLength(_In_ HWND hwnd, _In_ int nItem, _In_ int nSubItem);

	/// <summary>
	/// Get next item.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iCol"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetColumnWidth(_In_ HWND hwnd, _In_ const int iCol) noexcept
	{
		return ListView_GetColumnWidth(hwnd, iCol);
	}

	/// <summary>
	/// Get total number of items.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetItemCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetItemCount(hwnd);
	}

	/// <summary>
	/// Get number of selected items.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline unsigned int dcxListView_GetSelectedCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetSelectedCount(hwnd);
	}

	/// <summary>
	/// Get items state.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxListView_GetExtendedListViewStyle(_In_ HWND hwnd) noexcept
	{
		return ListView_GetExtendedListViewStyle(hwnd);
	}

	/// <summary>
	/// Get items rect.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxListView_GetHeader(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(lifetime.4) return ListView_GetHeader(hwnd);
	}

	/// <summary>
	/// Get ToolTip child control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxListView_GetToolTips(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(lifetime.4) return ListView_GetToolTips(hwnd);
	}

	/// <summary>
	/// Get index of top item.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// Get a groups information.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="gid"></param>
	/// <param name="pgrp"></param>
	/// <returns></returns>
	inline int dcxListView_GetGroupInfo(_In_ HWND hwnd, _In_ const unsigned int gid, _Inout_ PLVGROUP pgrp) noexcept
	{
		return gsl::narrow_cast<int>(ListView_GetGroupInfo(hwnd, gid, pgrp));
	}

	/// <summary>
	/// Get a groups information.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iIndex"></param>
	/// <param name="pgrp"></param>
	/// <returns></returns>
	inline LRESULT dcxListView_GetGroupInfoByIndex(_In_ HWND hwnd, _In_ const unsigned int iIndex, _Inout_ PLVGROUP pgrp) noexcept
	{
		return ListView_GetGroupInfoByIndex(hwnd, iIndex, pgrp);
	}

	/// <summary>
	/// Get number of groups.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetGroupCount(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<int>(ListView_GetGroupCount(hwnd));
	}

	/// <summary>
	/// Get groups state.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="gid"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	[[nodiscard]] inline unsigned int dcxListView_GetGroupState(_In_ HWND hwnd, _In_ const int gid, _In_ const DWORD mask) noexcept
	{
		return ListView_GetGroupState(hwnd, gid, mask);
	}

	/// <summary>
	/// Get groups size.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="gid"></param>
	/// <param name="type"></param>
	/// <param name="prc"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetGroupRect(_In_ HWND hwnd, _In_ const int gid, _In_ const long type, _Out_ LPRECT prc) noexcept
	{
		GSL_SUPPRESS(es.47) return gsl::narrow_cast<BOOL>(ListView_GetGroupRect(hwnd, gid, type, prc));
	}

	/// <summary>
	/// Get the text displayed when the control is empty.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="str"></param>
	/// <param name="sz"></param>
	/// <returns></returns>
	inline BOOL dcxListView_GetEmptyText(_In_ HWND hwnd, _Inout_z_ PTCHAR str, _In_ const int sz) noexcept
	{
		return ListView_GetEmptyText(hwnd, str, sz);
	}

	/// <summary>
	/// Hit test subitem.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="lvht"></param>
	/// <returns></returns>
	inline int dcxListView_SubItemHitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvht) noexcept
	{
		return ListView_SubItemHitTest(hwnd, lvht);
	}

	/// <summary>
	/// Hit test group.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="lvht"></param>
	/// <returns></returns>
	inline int dcxListView_SubItemGroupHitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvht) noexcept
	{
		// setting wparam to -1 causes it to return the iGroup info.
		return gsl::narrow_cast<int>(SendMessageW(hwnd, LVM_SUBITEMHITTEST, UINT_MAX, reinterpret_cast<LPARAM>(lvht)));
	}

	/// <summary>
	/// Set column width.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iCol"></param>
	/// <param name="cx"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetColumnWidth(_In_ HWND hwnd, _In_ const int iCol, _In_ const int cx) noexcept
	{
		return ListView_SetColumnWidth(hwnd, iCol, cx);
	}

	/// <summary>
	/// Set column order.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iCount"></param>
	/// <param name="pi"></param>
	/// <returns></returns>
	inline bool dcxListView_SetColumnOrderArray(_In_ HWND hwnd, _In_ const int iCount, _In_ const int* pi) noexcept
	{
		return !!ListView_SetColumnOrderArray(hwnd, iCount, pi);
	}

	/// <summary>
	/// Set current view mode.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="mode"></param>
	/// <returns></returns>
	inline DWORD dcxListView_SetView(_In_ HWND hwnd, _In_ DWORD mode) noexcept
	{
		return ListView_SetView(hwnd, mode);
	}

	/// <summary>
	/// Check if group exists.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="group"></param>
	/// <returns></returns>
	[[nodiscard]] inline LRESULT dcxListView_HasGroup(_In_ HWND hwnd, _In_ const int group) noexcept
	{
		return ListView_HasGroup(hwnd, group);
	}

	/// <summary>
	/// Check if group view is enabled.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline BOOL dcxListView_IsGroupViewEnabled(_In_ HWND hwnd) noexcept
	{
		return ListView_IsGroupViewEnabled(hwnd);
	}

	/// <summary>
	/// Enable/Disable group view.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="fEnable"></param>
	/// <returns></returns>
	inline BOOL dcxListView_EnableGroupView(_In_ HWND hwnd, _In_ bool fEnable) noexcept
	{
		return gsl::narrow_cast<BOOL>(ListView_EnableGroupView(hwnd, fEnable));
	}

	/// <summary>
	/// Set a groups state.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="gid"></param>
	/// <param name="mask"></param>
	/// <param name="state"></param>
	inline void dcxListView_SetGroupState(_In_ HWND hwnd, _In_ const int gid, _In_ const unsigned int mask, _In_ const unsigned int state) noexcept
	{
		ListView_SetGroupState(hwnd, gid, mask, state);
	}

	/// <summary>
	/// Insert a group.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="index"></param>
	/// <param name="pgrp"></param>
	inline void dcxListView_InsertGroup(_In_ HWND hwnd, _In_ const int index, _In_ PLVGROUP pgrp) noexcept
	{
		ListView_InsertGroup(hwnd, index, pgrp);
	}

	/// <summary>
	/// Set a groups information.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iGID"></param>
	/// <param name="pgrp"></param>
	/// <returns></returns>
	inline int dcxListView_SetGroupInfo(_In_ HWND hwnd, _In_ const int iGID, _In_ PLVGROUP pgrp) noexcept
	{
		return gsl::narrow_cast<int>(ListView_SetGroupInfo(hwnd, iGID, pgrp));
	}

	/// <summary>
	/// Move the position of a group.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="gid"></param>
	/// <param name="index"></param>
	inline void dcxListView_MoveGroup(_In_ HWND hwnd, _In_ const int gid, _In_ const int index) noexcept
	{
		ListView_MoveGroup(hwnd, gid, index);
	}

	/// <summary>
	/// Moves an item to a specified group.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="index"></param>
	/// <param name="gid"></param>
	inline void dcxListView_MoveItemToGroup(_In_ HWND hwnd, _In_ const int index, _In_ const int gid) noexcept
	{
		ListView_MoveItemToGroup(hwnd, index, gid);
	}

	/// <summary>
	/// Removes a group.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="gid"></param>
	inline void dcxListView_RemoveGroup(_In_ HWND hwnd, _In_ const int gid) noexcept
	{
		ListView_RemoveGroup(hwnd, gid);
	}

	/// <summary>
	/// Removes all groups from the control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// Sort the items.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="pFn"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SortItemsEx(_In_ HWND hwnd, _In_ PFNLVCOMPARE pFn, _Inout_ void* data) noexcept
	{
		return ListView_SortItemsEx(hwnd, pFn, data);
	}

	/// <summary>
	/// Ensure a specific item is visible.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="i"></param>
	/// <param name="partialOK"></param>
	/// <returns></returns>
	inline BOOL dcxListView_EnsureVisible(_In_ HWND hwnd, _In_ const int i, _In_ BOOL partialOK) noexcept
	{
		return ListView_EnsureVisible(hwnd, i, partialOK);
	}

	/// <summary>
	/// Scroll the display.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="dx"></param>
	/// <param name="dy"></param>
	/// <returns></returns>
	inline BOOL dcxListView_Scroll(_In_ HWND hwnd, _In_ const int dx, _In_ const int dy) noexcept
	{
		return ListView_Scroll(hwnd, dx, dy);
	}

	/// <summary>
	/// Get the number of items per page.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxListView_GetCountPerPage(_In_ HWND hwnd) noexcept
	{
		return ListView_GetCountPerPage(hwnd);
	}

	/// <summary>
	/// Get the edit control used to edit label.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline HWND dcxListView_GetEditControl(_In_ HWND hwnd) noexcept
	{
		return ListView_GetEditControl(hwnd);
	}

	/// <summary>
	/// Get background colour.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline COLORREF dcxListView_GetBkColor(_In_ HWND hwnd) noexcept
	{
		return ListView_GetBkColor(hwnd);
	}

	/// <summary>
	/// Edit items text.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="i"></param>
	/// <returns></returns>
	inline HWND dcxListView_EditLabel(_In_ HWND hwnd, _In_ const int i) noexcept
	{
		return ListView_EditLabel(hwnd, i);
	}

	/// <summary>
	/// Hit test control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="lvhti"></param>
	/// <returns></returns>
	inline int dcxListView_HitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvhti) noexcept
	{
		return ListView_HitTest(hwnd, lvhti);
	}

	/// <summary>
	/// Hit test control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="lvhti"></param>
	/// <returns></returns>
	inline int dcxListView_HitTestEx(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvhti) noexcept
	{
		return ListView_HitTestEx(hwnd, lvhti);
	}

	/// <summary>
	/// Update an item.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="nItem"></param>
	/// <returns></returns>
	inline BOOL dcxListView_Update(_In_ HWND hwnd, _In_ int nItem) noexcept
	{
		return ListView_Update(hwnd, nItem);
	}

	/// <summary>
	/// Redraw a range of items.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="nFirst"></param>
	/// <param name="nLast"></param>
	/// <returns></returns>
	inline BOOL dcxListView_RedrawItems(_In_ HWND hwnd, _In_ int nFirst, _In_ int nLast) noexcept
	{
		return ListView_RedrawItems(hwnd, nFirst, nLast);
	}

	/// <summary>
	/// Set the text colour.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="clr">- The colour to use.</param>
	/// <returns>The previously used colour.</returns>
	inline COLORREF dcxListView_SetTextColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetTextColor(hwnd, clr);
	}

	/// <summary>
	/// Set the text background colour.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="clr">- The colour to use.</param>
	/// <returns>The previously used colour.</returns>
	inline COLORREF dcxListView_SetTextBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetTextBkColor(hwnd, clr);
	}

	/// <summary>
	/// Set the background colour.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="clr">- The colour to use.</param>
	/// <returns>The previously used colour.</returns>
	inline COLORREF dcxListView_SetBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetBkColor(hwnd, clr);
	}

	/// <summary>
	/// Sets the background image in a list-view control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="lplvbki">- A pointer to an LVBKIMAGE structure that contains the new background image information.</param>
	/// <returns>Returns true if successful, or false otherwise. Returns false if the ulFlags member of the LVBKIMAGE structure is LVBKIF_SOURCE_NONE.</returns>
	/// <remarks>
	/// <para>Because the list-view control uses OLE COM to manipulate the background images, the calling application must call CoInitialize or OleInitialize before using this macro.</para>
	/// <para>It is best to call one of these functions when the application is initialized and call either CoUninitialize or OleUninitialize when the application is terminating.</para>
	/// </remarks>
	inline bool dcxListView_SetBkImage(_In_ HWND hwnd, _In_ LPLVBKIMAGE lplvbki) noexcept
	{
		return !!ListView_SetBkImage(hwnd, lplvbki);
	}

	/// <summary>
	/// Set the outline colour.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="clr">- The colour to use.</param>
	/// <returns>The previously used colour.</returns>
	inline COLORREF dcxListView_SetOutlineColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return ListView_SetOutlineColor(hwnd, clr);
	}

	/// <summary>
	/// Set the insert mark position.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="lplvim"></param>
	/// <returns></returns>
	inline BOOL dcxListView_SetInsertMark(_In_ HWND hwnd, _In_ LPLVINSERTMARK lplvim) noexcept
	{
		return ListView_SetInsertMark(hwnd, lplvim);
	}

	/// <summary>
	/// Clear the insert mark.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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
	/// Test if an item is visible.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="i"></param>
	/// <returns></returns>
	[[nodiscard]] inline BOOL dcxListView_IsItemVisible(_In_ HWND hwnd, _In_ int i) noexcept
	{
		return ListView_IsItemVisible(hwnd, i);
	}

	/// <summary>
	/// Gets the handle to an image list used for drawing list-view items.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="iImageList">
	/// <para>- The image list to retrieve. This parameter can be one of the following values:</para>
	/// <para>LVSIL_NORMAL		-	Image list with large icons.</para>
	/// <para>LVSIL_SMALL		-	Image list with small icons.</para>
	/// <para>LVSIL_STATE		-	Image list with state images.</para>
	/// <para>LVSIL_GROUPHEADER	-	Image list for group header.</para>
	/// </param>
	/// <returns>Returns the handle to the specified image list if successful, or nullptr otherwise.</returns>
	[[nodiscard]] inline HIMAGELIST dcxListView_GetImageList(_In_ HWND hwnd, _In_ int iImageList) noexcept
	{
		return ListView_GetImageList(hwnd, iImageList);
	}

	/// <summary>
	/// Assigns an image list to a list-view control.
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
	/// <param name="himl">- A handle to the image list to assign.</param>
	/// <param name="iImageList">
	/// <para>- The image type of image list. This parameter can be one of the following values:</para>
	/// <para>LVSIL_NORMAL		-	Image list with large icons.</para>
	/// <para>LVSIL_SMALL		-	Image list with small icons.</para>
	/// <para>LVSIL_STATE		-	Image list with state images.</para>
	/// <para>LVSIL_GROUPHEADER	-	Image list for group header.</para>
	/// </param>
	/// <returns>Returns the handle to the image list previously associated with the control if successful, or nullptr otherwise.</returns>
	/// <remarks>
	/// The current image list will be destroyed when the list-view control is destroyed unless the LVS_SHAREIMAGELISTS style is set.
	/// If you use this message to replace one image list with another, your application must explicitly destroy all image lists other than the current one.
	/// </remarks>
	inline HIMAGELIST dcxListView_SetImageList(_In_ HWND hwnd, _In_ HIMAGELIST himl, _In_ int iImageList) noexcept
	{
		return ListView_SetImageList(hwnd, himl, iImageList);
	}

	/// <summary>
	/// Undocumented message.
	/// Set a subitem for editing (used mostly when setting embeded control in explorer)
	/// </summary>
	/// <param name="hwnd">- A handle to the list-view control.</param>
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

	/// <summary>
	/// Get the controls current view setting.
	/// </summary>
	/// <param name="hwnd">- the listview control</param>
	/// <returns></returns>
	inline DWORD dcxListView_GetView(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, LVM_GETVIEW, 0, 0));
	}

	/// <summary>
	/// Get the item at x,y screen position.
	/// </summary>
	/// <param name="hwnd">- the listview control</param>
	/// <param name="x">- the x screen position</param>
	/// <param name="y">- the y screen position</param>
	/// <returns>the item at x,y or -1 for no item there.</returns>
	int dcxListView_GetItemAtPos(_In_ HWND hwnd, _In_ int x, _In_ int y) noexcept;

	/// <summary>
	/// Get the item at the cursor position.
	/// </summary>
	/// <param name="hwnd">- the listview control.</param>
	/// <returns>the item at the cursor or -1 for no item there.</returns>
	inline int dcxListView_GetItemAtCursor(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// Test where cursor is over control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns></returns>
	LVHITTESTINFO dcxListView_CursorHitTest(_In_ HWND hwnd) noexcept;

	// Header

	/// <summary>
	/// Tests a point to determine which header item, if any, is at the specified point.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="hdti">- A pointer to an HDHITTESTINFO structure that contains the position to test and receives information about the results of the test.</param>
	/// <returns>Returns the index of the item at the specified position, if any, or -1 otherwise.</returns>
	inline int dcxHeader_HitTest(_In_ HWND hwnd, _Inout_ LPHDHITTESTINFO hdti) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, HDM_HITTEST, 0U, reinterpret_cast<LPARAM>(hdti)));
	}

	/// <summary>
	/// Gets information about an item in a header control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="i">- The index of the item for which information is to be retrieved.</param>
	/// <param name="phi">- A pointer to an HDITEM structure. When the message is sent, the mask member indicates the type of information being requested. When the message returns, the other members receive the requested information. If the mask member specifies zero, the message returns TRUE but copies no information to the structure.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxHeader_GetItem(_In_ HWND hwnd, _In_ const int i, _Inout_ LPHDITEM phi) noexcept
	{
		return !!Header_GetItem(hwnd, i, phi);
	}

	/// <summary>
	/// Sets the attributes of the specified item in a header control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="i">- The current index of the item whose attributes are to be changed.</param>
	/// <param name="phi">- A pointer to an HDITEM structure that contains item information. When this message is sent, the mask member of the structure must be set to indicate which attributes are being set.</param>
	/// <returns>Returns true if successful, or false otherwise.</returns>
	inline bool dcxHeader_SetItem(_In_ HWND hwnd, _In_ const int i, _In_ const LPHDITEM phi) noexcept
	{
		return !!Header_SetItem(hwnd, i, phi);
	}

	/// <summary>
	/// Gets a count of the items in a header control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>Returns the number of items if successful, or -1 otherwise.</returns>
	inline int dcxHeader_GetItemCount(_In_ HWND hwnd) noexcept
	{
		return Header_GetItemCount(hwnd);
	}

	// ListBox

	/// <summary>
	/// Adds a string to a list box at the specified location.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos">- The zero-based index in the list at which to insert the item data, or –1 to add it to the end of the list.</param>
	/// <param name="txt">- The string to add.</param>
	/// <returns>The return value is the zero-based index of the string in the list box. If an error occurs, the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.</returns>
	inline int dcxListBox_InsertString(_In_ HWND hwnd, _In_ int nPos, _In_ const TString& txt) noexcept
	{
		return ListBox_InsertString(hwnd, nPos, txt.to_chr());
	}

	/// <summary>
	/// Inserts item data to a list box at the specified location.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos">- The zero-based index in the list at which to insert the item data, or –1 to add it to the end of the list.</param>
	/// <param name="data">- A pointer to the item data to insert.</param>
	/// <returns>The return value is the zero-based index of the item in the list box. If an error occurs, the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.</returns>
	inline int dcxListBox_InsertItemData(_In_ HWND hwnd, _In_ int nPos, _In_ LPARAM data) noexcept
	{
		return ListBox_InsertItemData(hwnd, nPos, data);
	}

	/// <summary>
	/// Removes all items from a list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	inline void dcxListBox_ResetContent(_In_ HWND hwnd) noexcept
	{
		ListBox_ResetContent(hwnd);
	}

	/// <summary>
	/// Allocates memory for storing list box items. This message is used before an application adds a large number of items to a list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nItems"></param>
	/// <param name="nMem"></param>
	/// <returns></returns>
	inline int dcxListBox_InitStorge(_In_ HWND hwnd, _In_ int nItems, _In_ UINT nMem) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, LB_INITSTORAGE, nItems, nMem));
	}

	/// <summary>
	/// Get an items rect.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos"></param>
	/// <param name="prc"></param>
	/// <returns>true on success</returns>
	inline bool dcxListBox_GetItemRect(_In_ HWND hwnd, _In_ int nPos, _Out_ LPRECT prc) noexcept
	{
		return (ListBox_GetItemRect(hwnd, nPos, prc) != LB_ERR);
	}

	/// <summary>
	/// Gets the application-defined value associated with the specified list box item.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos">- The zero-based index of the item.</param>
	/// <returns>The item data.</returns>
	inline LRESULT dcxListBox_GetItemData(_In_ HWND hwnd, _In_ int nPos) noexcept
	{
		return ListBox_GetItemData(hwnd, nPos);
	}

	/// <summary>
	/// Retrieves the height of items in a list box. If the list box has the LBS_OWNERDRAWVARIABLE style, this macro gets the height of the specified item; otherwise, it gets the height of all items.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos">- The zero-based index of the item. If the list box does not have the LBS_OWNERDRAWVARIABLE style, set this parameter to zero.</param>
	/// <returns>The height, in pixels, of the item or items, or LB_ERR if an error occurs.</returns>
	inline int dcxListBox_GetItemHeight(_In_ HWND hwnd, _In_ int nPos) noexcept
	{
		return ListBox_GetItemHeight(hwnd, nPos);
	}

	/// <summary>
	/// Get the length of an items text.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos">- The zero-based index of the item.</param>
	/// <returns>The count of characters in the string.</returns>
	inline int dcxListBox_GetTextLen(_In_ HWND hwnd, _In_ int nPos) noexcept
	{
		return ListBox_GetTextLen(hwnd, nPos);
	}

	/// <summary>
	/// Gets the width that a list box can be scrolled horizontally (the scrollable width) if the list box has a horizontal scroll bar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>The scrollable width, in pixels, of the list box.</returns>
	inline int dcxListBox_GetHorizontalExtent(_In_ HWND hwnd) noexcept
	{
		return ListBox_GetHorizontalExtent(hwnd);
	}

	/// <summary>
	/// Set the width by which a list box can be scrolled horizontally (the scrollable width). If the width of the list box is smaller than this value, the horizontal scroll bar horizontally scrolls items in the list box. If the width of the list box is equal to or greater than this value, the horizontal scroll bar is hidden.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nExtent">- The number of pixels by which the list box can be scrolled.</param>
	inline void dcxListBox_SetHorizontalExtent(_In_ HWND hwnd, _In_ int nExtent) noexcept
	{
		ListBox_SetHorizontalExtent(hwnd, nExtent);
	}

	/// <summary>
	/// Sets the application-defined value associated with the specified list box item.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="index">- The zero-based index of the item.</param>
	/// <param name="data">- The item data to set.</param>
	/// <returns>If an error occurs, the return value is LB_ERR.</returns>
	inline int dcxListBox_SetItemData(_In_ HWND hwnd, _In_ int index, _In_ LPARAM data) noexcept
	{
		return ListBox_SetItemData(hwnd, index, data);
	}

	/// <summary>
	/// Sets the height of items in a list box. If the list box has the LBS_OWNERDRAWVARIABLE style, this macro sets the height of the specified item; otherwise, it sets the height of all items.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="index">- The zero-based index of the item.</param>
	/// <param name="cy">- The height of the item or items, in pixels.</param>
	/// <returns>If the index or height is invalid, the return value is LB_ERR.</returns>
	inline int dcxListBox_SetItemHeight(_In_ HWND hwnd, _In_ int index, _In_ LPARAM cy) noexcept
	{
		return ListBox_SetItemData(hwnd, index, cy);
	}

	/// <summary>
	/// Selects or deselects an item in a multiple-selection list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="fSelect">- TRUE to select the item, or FALSE to deselect it.</param>
	/// <param name="index">- The zero-based index of the item.</param>
	/// <returns>If an error occurs, the return value is LB_ERR.</returns>
	inline int dcxListBox_SetSel(_In_ HWND hwnd, _In_ BOOL fSelect, _In_ int index) noexcept
	{
		return ListBox_SetSel(hwnd, fSelect, index);
	}

	/// <summary>
	/// Sets the tab-stop positions in a list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="cTabs">- The number of elements in the lpTabs array.</param>
	/// <param name="lpTabs">- A pointer to an array of integers containing the tab stops. The integers represent the number of quarters of the average character width for the font that is selected into the list box. For example, a tab stop of 4 is placed at 1.0 character units, and a tab stop of 6 is placed at 1.5 average character units. However, if the list box is part of a dialog box, the integers are in dialog template units. The tab stops must be sorted in ascending order; backward tabs are not allowed.</param>
	/// <returns>If all the specified tabs are set, the return value is TRUE; otherwise, it is FALSE.</returns>
	inline bool dcxListBox_SetTabStops(_In_ HWND hwnd, _In_ int cTabs, _In_reads_(cTabs) const int* lpTabs) noexcept
	{
		return (ListBox_SetTabStops(hwnd, cTabs, lpTabs) != FALSE);
	}

	/// <summary>
	/// Searches a list box for an item that has the specified item data. If a matching item is found, the item is selected.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="indexStart">- The zero-based index of the item before the first item to be searched. When the search reaches the bottom of the list box, it continues searching from the top of the list box back to the item specified by the indexStart parameter. If indexStart is –1, the entire list box is searched from the beginning.</param>
	/// <param name="data">- The item data to find.</param>
	/// <returns>If the search is successful, the return value is the index of the selected item. If the search is unsuccessful, the return value is LB_ERR and the current selection is not changed.</returns>
	inline int dcxListBox_SelectItemData(_In_ HWND hwnd, _In_ int indexStart, _In_ LPARAM data) noexcept
	{
		return ListBox_SelectItemData(hwnd, indexStart, data);
	}

	/// <summary>
	/// Searches a list box for an item that begins with the characters in a specified string. If a matching item is found, the item is selected.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="indexStart">- The zero-based index of the item before the first item to be searched. When the search reaches the bottom of the list box, it continues searching from the top of the list box back to the item specified by the indexStart parameter. If indexStart is –1, the entire list box is searched from the beginning.</param>
	/// <param name="lpszFind">- The string to find.</param>
	/// <returns>If the search is successful, the return value is the index of the selected item. If the search is unsuccessful, the return value is LB_ERR and the current selection is not changed.</returns>
	inline int dcxListBox_SelectString(_In_ HWND hwnd, _In_ int indexStart, _In_ LPCTSTR lpszFind) noexcept
	{
		return ListBox_SelectString(hwnd, indexStart, lpszFind);
	}

	/// <summary>
	/// Selects or deselects one or more consecutive items in a multiple-selection list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="fSelect">- TRUE to select the range of items, or FALSE to deselect it.</param>
	/// <param name="first">- The zero-based index of the first item to select.</param>
	/// <param name="last">- The zero-based index of the last item to select.</param>
	/// <returns>If an error occurs, the return value is LB_ERR.</returns>
	inline int dcxListBox_SelItemRange(_In_ HWND hwnd, _In_ BOOL fSelect, _In_ int first, _In_ int last) noexcept
	{
		return ListBox_SelItemRange(hwnd, fSelect, first, last);
	}

	/// <summary>
	/// Sets the focus rectangle to the item at the specified index in a multiple-selection list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="index">- the zero-based index of the list box item that is to receive the focus rectangle.</param>
	/// <returns>If an error occurs, the return value is LB_ERR.</returns>
	/// <remarks>The contents of the list box are scrolled till the item is fully visible.</remarks>
	inline int dcxListBox_SetCaretIndex(_In_ HWND hwnd, _In_ int index) noexcept
	{
		return ListBox_SetCaretIndex(hwnd, index);
	}

	/// <summary>
	/// Sets the width of all columns in a multiple-column list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="cxColumn">- The width, in pixels, of all columns.</param>
	inline void dcxListBox_SetColumnWidth(_In_ HWND hwnd, _In_ int cxColumn) noexcept
	{
		ListBox_SetColumnWidth(hwnd, cxColumn);
	}

	/// <summary>
	/// Sets the currently selected item in a single-selection list box.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="index">- The zero-based index of the item to select, or –1 to clear the selection.</param>
	/// <returns>If an error occurs, the return value is LB_ERR. If the index parameter is –1, the return value is LB_ERR even though no error occurred.</returns>
	inline int dcxListBox_SetCurSel(_In_ HWND hwnd, _In_ int index) noexcept
	{
		return ListBox_SetCurSel(hwnd, index);
	}

	/// <summary>
	/// Get the total count of items.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns></returns>
	inline int dcxListBox_GetCount(_In_ HWND hwnd) noexcept
	{
		return ListBox_GetCount(hwnd);
	}

	/// <summary>
	/// Set the top visible item.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iIndex">- The zero-based index of the item to put at the top of the visible part of the list.</param>
	/// <returns>If an error occurs, the return value is LB_ERR.</returns>
	/// <remarks>The list box contents are scrolled so that either the specified item appears at the top of the list box or the maximum scroll range has been reached.</remarks>
	inline int dcxListBox_SetTopIndex(_In_ HWND hwnd, _In_ int iIndex) noexcept
	{
		return ListBox_SetTopIndex(hwnd, iIndex);
	}

	/// <summary>
	/// Get the top visible item.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>The zero-based index of the first visible item.</returns>
	inline int dcxListBox_GetTopIndex(_In_ HWND hwnd) noexcept
	{
		return ListBox_GetTopIndex(hwnd);
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="i">- The zero-based index of the item.</param>
	/// <param name="buf">- A pointer to the buffer that will receive the string. The buffer must have sufficient space for the string and a terminating null character. Before allocating the buffer, you can call dcxListBox_GetTextLen to retrieve the length of the string.</param>
	/// <returns>The count of characters in the string, excluding the terminating null character. If index does not specify a valid item, the return value is LB_ERR.</returns>
	inline int dcxListBox_GetText(_In_ HWND hwnd, _In_ int i, _Inout_z_ TCHAR* buf) noexcept
	{
		return ListBox_GetText(hwnd, i, buf);
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="i">- The zero-based index of the item.</param>
	/// <returns></returns>
	[[nodiscard]] TString dcxListBox_GetText(_In_ HWND hwnd, _In_ int i);

	/// <summary>
	/// Get the item at a specific point.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="pt">Position in screen coordinates.</param>
	/// <returns>Item number or -1 if no item</returns>
	int dcxListBox_GetPointItem(_In_ HWND hListbox, _In_ POINT pt) noexcept;

	/// <summary>
	/// Get the item the mouse is currently over
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <returns></returns>
	int dcxListBox_GetHoverItem(_In_ HWND hListbox) noexcept;

	/// <summary>
	/// Test where a point is
	/// uses the same return codes as Listviews LVM_HITTEST, and the same structure.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="phti"></param>
	/// <returns>true on success</returns>
	bool dcxListBox_HitTest(_In_ HWND hListbox, _Inout_ LPLVHITTESTINFO phti) noexcept;

	/// <summary>
	/// Adds the specified filename to a list box that contains a directory listing.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="szFile">- A pointer to a buffer that specifies the name of the file to add.</param>
	/// <returns></returns>
	inline int dcxListBox_AddFile(_In_ HWND hListbox, _In_z_ TCHAR* szFile) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hListbox, LB_ADDFILE, 0, reinterpret_cast<LPARAM>(szFile)));
	}

	/// <summary>
	/// Adds a string to a list box. If the list box does not have the LBS_SORT style, the string is added to the end of the list. Otherwise, the string is inserted into the list and the list is sorted.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="szStr">- A pointer to the null-terminated string that is to be added.</param>
	/// <returns>The return value is the zero-based index of the string in the list box. If an error occurs, the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.</returns>
	inline int dcxListBox_AddString(_In_ HWND hListbox, _In_z_ TCHAR* szStr) noexcept
	{
		return ListBox_AddString(hListbox, szStr);
	}

	/// <summary>
	/// Adds item data to the list box at the specified location.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="data">- A pointer to the item data to add.</param>
	/// <returns>The return value is the zero-based index of the item in the list. If an error occurs, the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.</returns>
	/// <remarks>Use this macro for a list box with an owner-drawn style but without the LBS_HASSTRINGS style.</remarks>
	inline int dcxListBox_AddItemData(_In_ HWND hListbox, _In_ LPARAM data) noexcept
	{
		return ListBox_AddItemData(hListbox, data);
	}

	/// <summary>
	/// Deletes a string in a list box.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="iIndex">- The zero-based index of the string to be deleted.</param>
	/// <returns>The return value is a count of the strings remaining in the list. The return value is LB_ERR if the index parameter specifies an index greater than the number of items in the list.</returns>
	inline int dcxListBox_DeleteString(_In_ HWND hListbox, _In_ int iIndex) noexcept
	{
		return ListBox_DeleteString(hListbox, iIndex);
	}

	/// <summary>
	/// Adds names to the list displayed by a list box. The message adds the names of directories and files that match a specified string and set of file attributes. LB_DIR can also add mapped drive letters to the list box.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="attrs">- The attributes of the files or directories to be added to the list box.</param>
	/// <param name="szFile">- A pointer to the null - terminated string that specifies an absolute path, relative path, or filename.An absolute path can begin with a drive letter(for example, d:) or a UNC name(for example, \\ machinename\ sharename). If the string specifies a filename or directory that has the attributes specified by the wParam parameter, the filename or directory is added to the list.If the filename or directory name contains wildcard characters(? or *), all files or directories that match the wildcard expression and have the attributes specified by the wParam parameter are added to the list.</param>
	/// <returns>If the message succeeds, the return value is the zero-based index of the last name added to the list. If an error occurs, the return value is LB_ERR.If there is insufficient space to store the new strings, the return value is LB_ERRSPACE.</returns>
	inline int dcxListBox_Dir(_In_ HWND hListbox, _In_ UINT attrs, _In_z_ LPCTSTR szFile) noexcept
	{
		return ListBox_Dir(hListbox, attrs, szFile);
	}

	/// <summary>
	/// Enables or disables a list box control.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="bEnable">- TRUE to enable the control, or FALSE to disable it.</param>
	/// <returns>TRUE if the window was previously disabled, or FALSE if it was previously enabled.</returns>
	inline int dcxListBox_Enable(_In_ HWND hListbox, _In_ bool bEnable) noexcept
	{
		return ListBox_Enable(hListbox, bEnable);
	}

	/// <summary>
	/// Finds the first item in a list box that has the specified item data.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="indexStart">- The zero-based index of the item before the first item to be searched. When the search reaches the bottom of the list box, it continues searching from the top of the list box back to the item specified by the indexStart parameter. If indexStart is –1, the entire list box is searched from the beginning.</param>
	/// <param name="data">- The data to find.</param>
	/// <returns>The index of the matching item, or LB_ERR if the search was unsuccessful.</returns>
	inline int dcxListBox_FindItemData(_In_ HWND hListbox, _In_ int indexStart, _In_ LPARAM data) noexcept
	{
		return ListBox_FindItemData(hListbox, indexStart, data);
	}

	/// <summary>
	/// Finds the first string in a list box that begins with the specified string.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="indexStart">- The zero-based index of the item before the first item to be searched. When the search reaches the bottom of the list box, it continues searching from the top of the list box back to the item specified by the indexStart parameter. If indexStart is –1, the entire list box is searched from the beginning.</param>
	/// <param name="lpszFind">- The string to find.</param>
	/// <returns>The index of the matching item, or LB_ERR if the search was unsuccessful.</returns>
	inline int dcxListBox_FindString(_In_ HWND hListbox, _In_ int indexStart, _In_z_ LPCTSTR lpszFind) noexcept
	{
		return ListBox_FindString(hListbox, indexStart, lpszFind);
	}

	/// <summary>
	/// Finds the first list box string that exactly matches the specified string, except that the search is not case sensitive.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="indexStart">- The zero-based index of the item before the first item to be searched. When the search reaches the bottom of the list box, it continues searching from the top of the list box back to the item specified by the indexStart parameter. If indexStart is –1, the entire list box is searched from the beginning.</param>
	/// <param name="lpszFind">- The string to find.</param>
	/// <returns>The index of the matching item, or LB_ERR if the search was unsuccessful.</returns>
	inline int dcxListBox_FindStringExact(_In_ HWND hListbox, _In_ int indexStart, _In_z_ LPCTSTR lpszFind) noexcept
	{
		return ListBox_FindStringExact(hListbox, indexStart, lpszFind);
	}

	/// <summary>
	/// Gets the selection state of an item.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="iIndex">- The zero-based index of the item.</param>
	/// <returns>If the item is selected, the return value is greater than zero; otherwise, it is zero. If an error occurs, the return value is LB_ERR.</returns>
	inline int dcxListBox_GetSel(_In_ HWND hListbox, _In_ int iIndex) noexcept
	{
		return ListBox_GetSel(hListbox, iIndex);
	}

	/// <summary>
	/// Gets the count of selected items in a multiple-selection list box.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <returns>The number of selected items. If the list box is a single-selection list box, the return value is LB_ERR.</returns>
	inline int dcxListBox_GetSelCount(_In_ HWND hListbox) noexcept
	{
		return ListBox_GetSelCount(hListbox);
	}

	/// <summary>
	/// Gets the indexes of selected items in a multiple-selection list box.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <param name="cItems">- The maximum number of selected items whose item numbers are to be placed in the buffer.</param>
	/// <param name="lpItems">- A pointer to a buffer large enough for the number of integers specified by cItems.</param>
	/// <returns></returns>
	inline int dcxListBox_GetSelItems(_In_ HWND hListbox, _In_ int cItems, _Inout_ int* lpItems) noexcept
	{
		return ListBox_GetSelItems(hListbox, cItems, lpItems);
	}

	/// <summary>
	/// Gets the index of the currently selected item in a single-selection list box.
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <returns>The zero-based index of the selected item. If there is no selection, the return value is LB_ERR.</returns>
	inline int dcxListBox_GetCurSel(_In_ HWND hListbox) noexcept
	{
		return ListBox_GetCurSel(hListbox);
	}

	/// <summary>
	/// Retrieves the index of the list box item that has the focus rectangle in a multiple-selection list box. The item may or may not be selected. 
	/// </summary>
	/// <param name="hListbox">- A handle to the control.</param>
	/// <returns>The zero-based index of the focused item, or 0 if no item has the focus.</returns>
	/// <remarks>The contents of the list box are scrolled till the item is fully visible.</remarks>
	inline int dcxListBox_GetCaretIndex(_In_ HWND hListbox) noexcept
	{
		return ListBox_GetCaretIndex(hListbox);
	}

	//Toolbar

	/// <summary>
	/// Sets the size of buttons on a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dx">- The width in pixels, of the buttons</param>
	/// <param name="dy">- The height in pixels, of the buttons</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxToolbar_SetButtonSize(_In_ HWND hwnd, _In_ int dx, _In_ int dy) noexcept
	{
		return (SendMessage(hwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(dx, dy)) != FALSE);
	}

	/// <summary>
	/// Sets the minimum and maximum button widths in the toolbar control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="xMin">- Minimum button width, in pixels. Toolbar buttons will never be narrower than this value.</param>
	/// <param name="xMax">- Maximum button width, in pixels. If button text is too wide, the control displays it with ellipsis points.</param>
	/// <returns>Returns nonzero if successful, or zero otherwise.</returns>
	inline bool dcxToolbar_SetButtonWidth(_In_ HWND hwnd, _In_ int xMin, _In_ int xMax) noexcept
	{
		return (SendMessage(hwnd, TB_SETBUTTONWIDTH, 0, MAKELPARAM(xMin, xMax)) != FALSE);
	}

	/// <summary>
	/// Adds one or more images to the list of button images available for a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iCnt">- Number of button images in the bitmap. If lParam specifies a system-defined bitmap, this parameter is ignored.</param>
	/// <param name="lpbm">- Pointer to a TBADDBITMAP structure that contains the identifier of a bitmap resource and the handle to the module instance with the executable file that contains the bitmap resource.</param>
	/// <returns>Returns the index of the first new image if successful, or -1 otherwise.</returns>
	inline int dcxToolbar_AddBitmap(_In_ HWND hwnd, _In_ int iCnt, _In_ LPTBADDBITMAP lpbm) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_ADDBITMAP, iCnt, reinterpret_cast<LPARAM>(lpbm)));
	}

	/// <summary>
	/// Adds one or more buttons to a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iCnt">- Number of buttons to add.</param>
	/// <param name="lpbm">- Pointer to an array of TBBUTTON structures that contain information about the buttons to add. There must be the same number of elements in the array as buttons specified by wParam.</param>
	/// <returns></returns>
	/// <remarks>If the toolbar was created using the CreateWindowEx function, you must send the TB_BUTTONSTRUCTSIZE message to the toolbar before sending TB_ADDBUTTONS.</remarks>
	inline int dcxToolbar_AddButtons(_In_ HWND hwnd, _In_ int iCnt, _In_ LPCTBBUTTON lpbm) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_ADDBUTTONS, iCnt, reinterpret_cast<LPARAM>(lpbm)));
	}

	/// <summary>
	/// Adds a new string to the toolbar's string pool.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="hInst">- Handle to the module instance with an executable file that contains the string resource. If lParam instead points to a character array with one or more strings, set this parameter to NULL.</param>
	/// <param name="lpStrs">- Resource identifier for the string resource, or a pointer to a TCHAR array. See Remarks.</param>
	/// <returns>Returns the index of the first new string if successful, or -1 otherwise.</returns>
	/// <remarks>
	/// <para>If wParam is NULL, lParam points to a character array with one or more null-terminated strings.</para>
	/// <para>The last string in the array must be terminated with two null characters.</para>
	/// <para>If wParam is the HINSTANCE of the application or of another module containing a string resource, lParam is the resource identifier of the string.
	/// <para>Each item in the string must begin with an arbitrary separator character,</para>
	/// <para>and the string must end with two such characters. For example, the text for three buttons might appear in the string table as "/New/Open/Save//".</para>
	/// <para>The message returns the index of "New" in the toolbar's string pool, and the other items are in consecutive positions.</para>
	/// </remarks>
	inline int dcxToolbar_AddString(_In_ HWND hwnd, _In_ HINSTANCE hInst, _In_ TCHAR *lpStrs) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_ADDSTRING, reinterpret_cast<WPARAM>(hInst), reinterpret_cast<LPARAM>(lpStrs)));
	}

	/// <summary>
	/// Retrieves a count of the buttons currently in the toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>Returns the count of the buttons.</returns>
	inline int dcxToolbar_ButtonCount(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_BUTTONCOUNT, 0, 0));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns></returns>
	inline HWND dcxToolbar_GetTooltips(_In_ HWND hwnd) noexcept
	{
		return to_hwnd(SendMessage(hwnd, TB_ADDBUTTONS, 0, 0));
	}

	/// <summary>
	/// Retrieves the bounding rectangle of a button in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of the button for which to retrieve information.</param>
	/// <param name="lprc">- Pointer to a RECT structure that receives the client coordinates of the bounding rectangle.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxToolbar_GetItemRect(_In_ HWND hwnd, _In_ UINT uIndex, _In_ LPRECT lprc) noexcept
	{
		return !!SendMessage(hwnd, TB_GETITEMRECT, uIndex, reinterpret_cast<LPARAM>(lprc));
	}

	/// <summary>
	/// Retrieves the index of the hot item in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>Returns the index of the hot item, or -1 if no hot item is set. Toolbar controls that do not have the TBSTYLE_FLAT style do not have hot items.</returns>
	inline int dcxToolbar_GetHotItem(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_GETHOTITEM, 0, 0));
	}

	/// <summary>
	/// Determines where a point lies in a toolbar control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="lpPT">- Pointer to a POINT structure that contains the x-coordinate of the hit test in the x member and the y-coordinate of the hit test in the y member. The coordinates are relative to the toolbar's client area.</param>
	/// <returns>Returns an integer value. If the return value is zero or a positive value, it is the zero-based index of the nonseparator item in which the point lies. If the return value is negative, the point does not lie within a button. The absolute value of the return value is the index of a separator item or the nearest nonseparator item.</returns>
	inline int dcxToolbar_HitTest(_In_ HWND hwnd, _In_ const LPPOINT lpPT) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_HITTEST, 0, reinterpret_cast<LPARAM>(lpPT)));
	}

	/// <summary>
	/// Deletes a button from the toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of the button to delete.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxToolbar_DeleteButton(_In_ HWND hwnd, _In_ UINT uIndex) noexcept
	{
		return !!SendMessage(hwnd, TB_DELETEBUTTON, uIndex, 0);
	}

	/// <summary>
	/// Inserts a button in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of a button. The message inserts the new button to the left of this button.</param>
	/// <param name="lpButton">- Pointer to a TBBUTTON structure containing information about the button to insert.</param>
	/// <returns></returns>
	inline bool dcxToolbar_InsertButton(_In_ HWND hwnd, _In_ UINT uIndex, _In_ LPCTBBUTTON lpButton) noexcept
	{
		return !!SendMessage(hwnd, TB_INSERTBUTTON, uIndex, reinterpret_cast<LPARAM>(lpButton));
	}

	/// <summary>
	/// Sets the information for an existing button in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of a button.</param>
	/// <param name="lpButton">- Pointer to a TBBUTTONINFO structure that contains the new button information. The cbSize and dwMask members of this structure must be filled in prior to sending this message.</param>
	/// <returns>Returns nonzero if successful, or zero otherwise.</returns>
	inline bool dcxToolbar_SetButtonInfo(_In_ HWND hwnd, _In_ UINT uIndex, _In_ LPTBBUTTONINFO lpButton) noexcept
	{
		return !!SendMessage(hwnd, TB_SETBUTTONINFO, uIndex, reinterpret_cast<LPARAM>(lpButton));
	}

	/// <summary>
	/// Retrieves extended information for a button in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of a button.</param>
	/// <param name="lpButton">- Pointer to a TBBUTTONINFO structure that receives the button information. The cbSize and dwMask members of this structure must be filled in prior to sending this message.</param>
	/// <returns>Returns the zero-based index of the button, or -1 if an error occurs.</returns>
	inline int dcxToolbar_GetButtonInfo(_In_ HWND hwnd, _In_ UINT uIndex, _In_ LPTBBUTTONINFO lpButton) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_GETBUTTONINFO, uIndex, reinterpret_cast<LPARAM>(lpButton)));
	}

	/// <summary>
	/// Retrieves the display text of a button on a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Command identifier of the button whose text is to be retrieved.</param>
	/// <param name="szBuf">- Pointer to a buffer that receives the button text.</param>
	/// <returns>
	/// Returns the length, in characters, of the string pointed to by lParam.
	/// The length does not include the terminating null character.
	/// If unsuccessful, the return value is -1.
	/// </returns>
	/// <remarks>
	/// Security Warning: Using this message incorrectly might compromise the security of your program.
	/// This message does not provide a way for you to know the size of the buffer.
	/// If you use this message, first call the message passing NULL in the lParam, this returns the number of characters, excluding NULL that are required.
	/// Then call the message a second time to retrieve the string.
	/// </remarks>
	inline int dcxToolbar_GetButtonText(_In_ HWND hwnd, _In_ UINT uIndex, _In_ TCHAR *szBuf) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_GETBUTTONTEXT, uIndex, reinterpret_cast<LPARAM>(szBuf)));
	}

	TString dcxToolbar_GetButtonText(_In_ HWND hwnd, _In_ UINT uIndex);

	/// <summary>
	/// Retrieves information about the specified button in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of the button for which to retrieve information.</param>
	/// <param name="lpButton">- Pointer to the TBBUTTON structure that receives the button information.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxToolbar_GetButton(_In_ HWND hwnd, _In_ UINT uIndex, _In_ LPTBBUTTON lpButton) noexcept
	{
		return !!SendMessage(hwnd, TB_GETBUTTON, uIndex, reinterpret_cast<LPARAM>(lpButton));
	}

	/// <summary>
	/// Retrieves the zero-based index for the button associated with the specified command identifier.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of the button for which to retrieve information.</param>
	/// <returns>Returns the zero-based index for the button or -1 if the specified command identifier is invalid.</returns>
	inline int dcxToolbar_CommandToIndex(_In_ HWND hwnd, _In_ UINT uIndex) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, TB_COMMANDTOINDEX, uIndex, 0));
	}

	/// <summary>
	/// Moves a button from one index to another.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uIndex">- Zero-based index of the button for which to retrieve information.</param>
	/// <param name="uMoveIndex">- Zero-based index where the button will be moved.</param>
	/// <returns>Returns nonzero if successful, or zero otherwise.</returns>
	inline bool dcxToolbar_MoveButton(_In_ HWND hwnd, _In_ UINT uIndex, _In_ UINT uMoveIndex) noexcept
	{
		return !!SendMessage(hwnd, TB_MOVEBUTTON, gsl::narrow_cast<WPARAM>(uIndex), gsl::narrow_cast<LPARAM>(uMoveIndex));
	}

	/// <summary>
	/// Sets the highlight state of a given button in a toolbar control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uCmdId">- Command identifier for a toolbar button.</param>
	/// <param name="fHighlight">- Indicates the new highlight state. If TRUE, the button is highlighted. If FALSE, the button is set to its default state.</param>
	/// <returns>Returns nonzero if successful, or zero otherwise.</returns>
	int dcxToolbar_MarkButton(_In_ HWND hwnd, _In_ UINT uCmdId, bool fHighlight) noexcept;

	/// <summary>
	/// Sets the state for the specified button in a toolbar.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="uCmdId">- Command identifier for a toolbar button.</param>
	/// <param name="uFlags">- A combination of values listed in Toolbar Button States.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	bool dcxToolbar_SetState(_In_ HWND hwnd, _In_ UINT uCmdId, _In_ UINT uFlags) noexcept;

	/// <summary>
	/// Sets the extended styles for a toolbar control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="exStyle">- Value specifying the new extended styles.</param>
	/// <returns>Returns a DWORD that represents the previous extended styles.</returns>
	inline DWORD dcxToolbar_SetExtendedStyle(_In_ HWND hwnd, _In_ DWORD exStyle) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, TB_SETEXTENDEDSTYLE, 0, exStyle));
	}

	/// <summary>
	/// Specifies the size of the TBBUTTON structure.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	inline void dcxToolbar_SetButtonStructSize(_In_ HWND hwnd) noexcept
	{
		SendMessage(hwnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
	}

	/// <summary>
	/// Resize controls contents to match current settings.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	inline void dcxToolbar_AutoSize(_In_ HWND hwnd) noexcept
	{
		SendMessage(hwnd, TB_AUTOSIZE, 0, 0);
	}

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
	/// <param name="hTree">- Handle to a tree-view control.</param>
	/// <returns></returns>
	size_t dcxTreeView_GetCount(_In_ HWND hTree) noexcept;

	/// <summary>
	/// Get the controls image list.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="type"></param>
	/// <returns></returns>
	[[nodiscard]] inline HIMAGELIST dcxTreeView_GetImageList(_In_ HWND hwnd, _In_ int type) noexcept
	{
		return TreeView_GetImageList(hwnd, gsl::narrow_cast<WPARAM>(type));
	}

	/// <summary>
	/// Retrieves the amount, in pixels, that child items are indented relative to their parent items.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns></returns>
	inline UINT dcxTreeView_GetIndent(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetIndent(hwnd);
	}

	/// <summary>
	/// Retrieves the color used to draw the insertion mark for the tree view.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_GetInsertMarkColor(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetInsertMarkColor(hwnd);
	}

	/// <summary>
	/// Retrieves the incremental search string for a tree-view control. The tree-view control uses the incremental search string to select an item based on characters typed by the user.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="lpsz"></param>
	/// <returns></returns>
	inline bool dcxTreeView_GetISearchString(_In_ HWND hwnd, _Inout_z_ LPTSTR lpsz) noexcept
	{
		return (TreeView_GetISearchString(hwnd, lpsz) != FALSE);
	}

	/// <summary>
	/// Sets the normal or state image list for a tree-view control and redraws the control using the new images.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="himl">- The HIMAGELIST handle to the image list. If himl is NULL, the message removes the specified image list from the tree-view control.</param>
	/// <param name="type">
	/// <para>- Type of image list to set. This parameter can be one of the following values:</para>
	///	<para>TVSIL_NORMAL - Indicates the normal image list, which contains selected, nonselected, and overlay images for the items of a tree - view control.</para>
	///	<para>TVSIL_STATE - Indicates the state image list.You can use state images to indicate application - defined item states.A state image is displayed to the left of an item's selected or nonselected image.</para>
	///	</param>
	/// <returns>Returns the HIMAGELIST handle to the previous image list, if any, or NULL otherwise.</returns>
	/// <remarks>The tree-view control will not destroy the image list specified with this message. Your application must destroy the image list when it is no longer needed.</remarks>
	inline HIMAGELIST dcxTreeView_SetImageList(_In_ HWND hwnd, _In_ HIMAGELIST himl, _In_ int type) noexcept
	{
		return TreeView_SetImageList(hwnd, himl, gsl::narrow_cast<WPARAM>(type));
	}

	/// <summary>
	/// Insert a treeview item.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="ptvins">- Pointer to a TVINSERTSTRUCT structure that specifies the attributes of the tree-view item.</param>
	/// <returns>Returns the HTREEITEM handle to the new item if successful, or NULL otherwise.</returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_InsertItem(_In_ HWND hwnd, _In_ const TVINSERTSTRUCT* const ptvins) noexcept
	{
		return TreeView_InsertItem(hwnd, ptvins);
	}

	/// <summary>
	/// Retrieves the handle to the child tooltip control used by a tree-view control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns>Returns the handle to the child tooltip control, or NULL if the control is not using tooltips.</returns>
	/// <remarks>
	/// <para>When created, tree-view controls automatically create a child tooltip control.</para>
	/// <para>To prevent a tree-view control from using tooltips, create the control with the TVS_NOTOOLTIPS style.</para>
	/// </remarks>
	[[nodiscard]] inline HWND dcxTreeView_GetToolTips(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetToolTips(hwnd);
	}

	/// <summary>
	/// Retrieves the Unicode character format flag for the control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns>
	/// <para>Returns the Unicode format flag for the control.</para>
	/// <para>If this value is nonzero, the control is using Unicode characters.</para>
	/// <para>If this value is zero, the control is using ANSI characters.</para>
	/// </returns>
	inline bool dcxTreeView_GetUnicodeFormat(_In_ HWND hwnd) noexcept
	{
		return (TreeView_GetUnicodeFormat(hwnd) != FALSE);
	}

	/// <summary>
	/// Obtains the number of items that can be fully visible in the client window of a tree-view control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns>Returns the number of items that can be fully visible in the client window of the tree-view control.</returns>
	/// <remarks>
	/// <para>The number of items that can be fully visible may be greater than the number of items in the control.</para>
	/// <para>The control calculates this value by dividing the height of the client window by the height of an item.</para>
	/// <para>Note that the return value is the number of items that can be fully visible.</para>
	/// <para>If you can see all of 20 items and part of one more item, the return value is 20.</para>
	/// </remarks>
	inline UINT dcxTreeView_GetVisibleCount(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetVisibleCount(hwnd);
	}

	/// <summary>
	/// Get an items info.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="ptvi"></param>
	/// <returns></returns>
	inline bool dcxTreeView_GetItem(_In_ HWND hwnd, _Inout_ LPTVITEMEX ptvi) noexcept
	{
		return (TreeView_GetItem(hwnd, ptvi) != FALSE);
	}

	/// <summary>
	/// Retrieves the current height of the tree-view items.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns></returns>
	inline int dcxTreeView_GetItemHeight(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetItemHeight(hwnd);
	}

	/// <summary>
	/// Set an items info.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="ptvi"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SetItem(_In_ HWND hwnd, _In_ const LPTVITEMEX ptvi) noexcept
	{
		return (TreeView_SetItem(hwnd, ptvi) != FALSE);
	}

	/// <summary>
	/// Get the root item.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetRoot(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(es.47) return TreeView_GetRoot(hwnd);
	}

	/// <summary>
	/// Retrieves the maximum scroll time for the tree-view control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns>Returns the maximum scroll time, in milliseconds.</returns>
	/// <remarks>
	/// The maximum scroll time is the longest amount of time that a scroll operation can take.<br/>
	/// Scrolling will be adjusted so that the scroll will take place within the maximum scroll time.<br/>
	/// A scroll operation may take less time than the maximum.
	/// </remarks>
	[[nodiscard]] inline UINT dcxTreeView_GetScrollTime(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(es.47) return TreeView_GetScrollTime(hwnd);
	}

	/// <summary>
	/// Get items parent.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetParent(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetParent(hwnd, hItem);
	}

	/// <summary>
	/// Get items first child.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetChild(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetChild(hwnd, hItem);
	}

	/// <summary>
	/// Get next sibling item.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem">- Handle to an item.</param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns>Returns the handle to the item if successful, or NULL otherwise.</returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetNextSibling(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetNextSibling(hwnd, hItem);
	}

	/// <summary>
	/// Retrieves the next visible item that follows a specified item in a tree-view control. 
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem">- Handle to an item.</param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetNextVisible(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetNextVisible(hwnd, hItem);
	}

	/// <summary>
	/// Get previous sibling item.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem"></param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetPrevSibling(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetPrevSibling(hwnd, hItem);
	}

	/// <summary>
	/// Retrieves the first visible item that precedes a specified item in a tree-view control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem"></param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetPrevVisible(_In_ HWND hwnd, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_GetPrevVisible(hwnd, hItem);
	}

	/// <summary>
	/// Get controls extended styles.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns></returns>
	[[nodiscard]] inline DWORD dcxTreeView_GetExtendedStyle(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetExtendedStyle(hwnd);
	}

	/// <summary>
	/// Retrieves the first visible item in a tree-view control window.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns></returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetFirstVisible(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetFirstVisible(hwnd);
	}

	/// <summary>
	/// Set controls extended styles.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
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
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="exstyle"></param>
	/// <param name="mask"></param>
	/// <returns></returns>
	inline DWORD dcxTreeView_SetExtendedStyle(_In_ HWND hwnd, _In_ WindowExStyle exstyle, _In_ WindowExStyle mask) noexcept
	{
		return TreeView_SetExtendedStyle(hwnd, gsl::narrow_cast<DWORD>(exstyle), gsl::narrow_cast<DWORD>(mask));
	}

	/// <summary>
	/// Sets the hot item for a tree-view control. (internal only dont use)
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem">- Handle to the new hot item. If this value is NULL, the tree-view control will be set to have no hot item.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	/// <remarks>
	/// The hot item is the item that the mouse is hovering over. The TVM_SETHOT message sent by this macro makes an item look like it is the hot item even if the mouse is not hovering over it.
	/// The TVM_SETHOT message has no visible effect if the TVS_TRACKSELECT style is not set.
	///	If it succeeds, the TVM_SETHOT message causes the hot item to be redrawn.
	///	The TVM_SETHOT message is ignored if hitem is NULL and the tree - view control is tracking the mouse.
	/// </remarks>
	inline bool dcxTreeView_SetHot(_In_ HWND hwnd, _In_opt_ HTREEITEM hItem) noexcept
	{
		return (TreeView_SetHot(hwnd, hItem) != FALSE);
	}

	/// <summary>
	/// Set text colour.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetTextColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetTextColor(hwnd, clr);
	}

	/// <summary>
	/// Set background colour.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="clr">- COLORREF value that contains the new background color. If this argument is -1, the control will revert to using the system color for the background color.</param>
	/// <returns>Returns a COLORREF value that represents the previous background color. If this value is -1, the control was using the system color for the background color.</returns>
	inline COLORREF dcxTreeView_SetBkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetBkColor(hwnd, clr);
	}

	/// <summary>
	/// Sets information used to determine auto-scroll characteristics.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="uPixPerSec">- Specifies pixels per second. The offset to scroll is divided by the uPixPerSec to determine the total duration of the auto-scroll.</param>
	/// <param name="uUpdateTime">- Specifies the redraw time interval. Redraw at every elapsed interval, until the item is scrolled into view. Given uPixPerSec, the location of the item is calculated and a repaint occurs. Set this value to create smooth scrolling.</param>
	inline void dcxTreeView_SetAutoScrollInfo(_In_ HWND hwnd, _In_ UINT uPixPerSec, _In_ UINT uUpdateTime) noexcept
	{
		TreeView_SetAutoScrollInfo(hwnd, uPixPerSec, uUpdateTime);
	}

	/// <summary>
	/// Set line colour.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetLineColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetLineColor(hwnd, clr);
	}

	/// <summary>
	/// Sets the maximum scroll time for the tree-view control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="uTime">- New maximum scroll time, in milliseconds. If this value is less than 100, it will be rounded up to 100.</param>
	/// <returns>Returns the previous scroll time, in milliseconds.</returns>
	/// <remarks>
	/// The maximum scroll time is the longest amount of time that a scroll operation can take.
	/// Scrolling will be adjusted so that the scroll will take place within the maximum scroll time.
	/// A scroll operation may take less time than the maximum.
	/// </remarks>
	inline UINT dcxTreeView_SetScrollTime(_In_ HWND hwnd, _In_ UINT uTime) noexcept
	{
		return TreeView_SetScrollTime(hwnd, uTime);
	}

	/// <summary>
	/// Set insert mark colour.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="clr"></param>
	/// <returns></returns>
	inline COLORREF dcxTreeView_SetInsertMarkColor(_In_ HWND hwnd, _In_ COLORREF clr) noexcept
	{
		return TreeView_SetInsertMarkColor(hwnd, clr);
	}

	/// <summary>
	/// Get items state.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
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
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti"></param>
	/// <returns></returns>
	inline bool dcxTreeView_EnsureVisible(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return (TreeView_EnsureVisible(hwnd, hti) != FALSE);
	}

	/// <summary>
	/// Start editing the items label.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti"></param>
	/// <returns>The edit control for editing.</returns>
	inline HWND dcxTreeView_EditLabel(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return TreeView_EditLabel(hwnd, hti);
	}

	/// <summary>
	/// Ends the editing of a tree-view item's label.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="bCancel"></param>
	/// <returns></returns>
	inline bool dcxTreeView_EndEditLabelNow(_In_ HWND hwnd, _In_ bool bCancel) noexcept
	{
		return (TreeView_EndEditLabelNow(hwnd, bCancel) != FALSE);
	}

	/// <summary>
	/// Selects the specified tree-view item, scrolls the item into view, or redraws the item in the style used to indicate the target of a drag-and-drop operation.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti">- Handle to an item. If the hitem parameter is NULL, the control is set to have no selected item.</param>
	/// <param name="code">
	/// <para>- Action flag. This parameter can be one of the following values:</para>
	///	<para>TVGN_CARET - Sets the selection to the given item.The control's parent window receives the TVN_SELCHANGING and TVN_SELCHANGED notification codes.</para>
	///	<para>TVGN_DROPHILITE - Redraws the given item in the style used to indicate the target of a drag - and -drop operation.</para>
	///	<para>TVGN_FIRSTVISIBLE - Ensures that the specified item is visible, and, if possible, displays it at the top of the control's window. Tree-view controls display as many items as will fit in the window. If the specified item is near the bottom of the control's hierarchy of items, it might not become the first visible item, depending on how many items fit in the window.</para>
	/// </param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	/// <remarks>
	/// <para>If the specified item is the child of a collapsed parent item, the parent's list of child items is expanded to reveal the specified item.</para>
	/// <para>In this case, the parent window receives the TVN_ITEMEXPANDING and TVN_ITEMEXPANDED notification codes.</para>
	/// </remarks>
	inline bool dcxTreeView_Select(_In_ HWND hwnd, _In_opt_ HTREEITEM hti, _In_ UINT code) noexcept
	{
		return (TreeView_Select(hwnd, hti, code) != FALSE);
	}

	/// <summary>
	/// Redraws a specified tree-view control item in the style used to indicate the target of a drag-and-drop operation.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti">- Handle to an item. If the hitem parameter is NULL, the control is set to have no selected item.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	/// <remarks>
	/// <para>If the specified item is the child of a collapsed parent item, the parent's list of child items is expanded to reveal the specified item.</para>
	/// <para>In this case, the parent window receives the TVN_ITEMEXPANDING and TVN_ITEMEXPANDED notification codes.</para>
	/// </remarks>
	/// <seealso cref="dcxTreeView_Select"/>
	inline bool dcxTreeView_SelectDropTarget(_In_ HWND hwnd, _In_opt_ HTREEITEM hti) noexcept
	{
		return (TreeView_SelectDropTarget(hwnd, hti) != FALSE);
	}

	/// <summary>
	/// Set item as selected.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti">- Handle to an item. If the hitem parameter is NULL, the control is set to have no selected item.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	/// <remarks>
	/// <para>When you call the TreeView_SelectItem macro, the control's parent window receives the TVN_SELCHANGING and TVN_SELCHANGED notification codes.</para>
	/// <para>Also, if the specified item is the child of a collapsed parent item, the parent's list of child items is expanded to reveal the specified item.</para>
	/// <para>In this case, the parent window receives the TVN_ITEMEXPANDING and TVN_ITEMEXPANDED notification codes.</para>
	/// </remarks>
	/// <seealso cref="dcxTreeView_Select"/>
	inline bool dcxTreeView_SelectItem(_In_ HWND hwnd, _In_opt_ HTREEITEM hti) noexcept
	{
		return (TreeView_SelectItem(hwnd, hti) != FALSE);
	}

	/// <summary>
	/// Scrolls the tree-view control vertically to ensure that the specified item is visible. If possible, the specified item becomes the first visible item at the top of the control's window.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti">- Handle to an item. If the hitem parameter is NULL, the control is set to have no selected item.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	/// <remarks>
	///  <para>Tree - view controls display as many items as will fit in the window.</para>
	///  <para>If the specified item is near the bottom of the control's hierarchy of items, it might not become the first visible item, depending on how many items fit in the window.</para>
	///  <para>If the specified item is the child of a collapsed parent item, the parent's list of child items is expanded to reveal the specified item.</para>
	///  <para>In this case, the parent window receives the TVN_ITEMEXPANDING and TVN_ITEMEXPANDED notification codes.</para>
	/// </remarks>
	/// <seealso cref="dcxTreeView_Select"/>
	inline bool dcxTreeView_SelectSetFirstVisible(_In_ HWND hwnd, _In_opt_ HTREEITEM hti) noexcept
	{
		return (TreeView_SelectSetFirstVisible(hwnd, hti) != FALSE);
	}

	/// <summary>
	/// Set height for all items.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="height"></param>
	/// <returns></returns>
	inline int dcxTreeView_SetItemHeight(_In_ HWND hwnd, _In_ int height) noexcept
	{
		return TreeView_SetItemHeight(hwnd, height);
	}

	/// <summary>
	/// Set border size around control. (internal only dont use)
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="dwFlags"></param>
	/// <param name="iLeft"></param>
	/// <param name="iTop"></param>
	/// <returns></returns>
	inline int dcxTreeView_SetBorder(_In_ HWND hwnd, _In_ DWORD dwFlags, _In_ int iLeft, _In_ int iTop) noexcept
	{
		return TreeView_SetBorder(hwnd, dwFlags, iLeft, iTop);
	}

	/// <summary>
	/// Sets the item's state image to "checked" or "unchecked."
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti">- Handle to the item.</param>
	/// <param name="fCheck">- Value that indicates which state image is displayed. Set fCheck to TRUE to display the checked state image or FALSE to display the unchecked image.</param>
	inline void dcxTreeView_SetCheckState(_In_ HWND hwnd, _In_ HTREEITEM hti, _In_ bool fCheck) noexcept
	{
		TreeView_SetCheckState(hwnd, hti, fCheck);
	}
	
	/// <summary>
	/// Expand items children.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti">- Handle to the parent item that will be expanded or collapsed.</param>
	/// <param name="iCode">- Action flag:
	/// <para>TVE_COLLAPSE		-	Collapses the list.</para>
	/// <para>TVE_COLLAPSERESET	-	Collapses the list and removes the child items.The TVIS_EXPANDEDONCE state flag is reset.This flag must be used with the TVE_COLLAPSE flag.</para>
	/// <para>TVE_EXPAND		-	Expands the list.</para>
	/// <para>TVE_EXPANDPARTIAL	-	Version 4.70.Partially expands the list.In this state the child items are visible and the parent item's plus sign (+), indicating that it can be expanded, is displayed. This flag must be used in combination with the TVE_EXPAND flag.</para>
	/// <para>TVE_TOGGLE		-	Collapses the list if it is expanded or expands it if it is collapsed.</para>
	/// </param>
	/// <returns>Returns true if the operation was successful, or false otherwise.</returns>
	/// <remarks>
	/// <para>Expanding a node that is already expanded, or collapsing a node that is already collapsed is considered a successful operation and the macro returns a nonzero value.</para>
	/// <para>Attempting to expand or collapse a node that has no children is considered a failure and the return value is zero.</para>
	/// <para>When an item is first expanded by a TVM_EXPAND message, the action generates TVN_ITEMEXPANDING and TVN_ITEMEXPANDED notification codes and the item's TVIS_EXPANDEDONCE state flag is set.</para>
	/// <para>As long as this state flag remains set, subsequent TVM_EXPAND messages do not generate TVN_ITEMEXPANDING or TVN_ITEMEXPANDED notifications.</para>
	/// <para>To reset the TVIS_EXPANDEDONCE state flag, you must send a TVM_EXPAND message with the TVE_COLLAPSE and TVE_COLLAPSERESET flags set.</para>
	/// <para>Attempting to explicitly set TVIS_EXPANDEDONCE will result in unpredictable behavior.</para>
	/// <para>The expand operation may fail if the owner of the treeview control denies the operation in response to a TVN_ITEMEXPANDING notification.</para>
	/// </remarks>
	inline bool dcxTreeView_Expand(_In_ HWND hwnd, _In_ HTREEITEM hti, _In_ DWORD iCode) noexcept
	{
		return (TreeView_Expand(hwnd, hti, iCode) != FALSE);
	}

	/// <summary>
	/// Sort items.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
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
	/// <param name="hwnd">- Handle to a tree-view control.</param>
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
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hti"></param>
	/// <returns></returns>
	inline UINT dcxTreeView_GetCheckState(_In_ HWND hwnd, _In_ HTREEITEM hti) noexcept
	{
		return TreeView_GetCheckState(hwnd, hti);
	}

	/// <summary>
	/// Get the edit control being used to edit labels.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
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
		GSL_SUPPRESS(es.47) return TreeView_GetDropHilight(hwnd);
	}

	/// <summary>
	/// Retrieves the bounding rectangle for a tree-view item and indicates whether the item is visible.
	/// </summary>
	/// <param name="hwnd">- Handle to the tree-view control.</param>
	/// <param name="hItem">- Handle to the tree-view item.</param>
	/// <param name="prc">- Pointer to a RECT structure that receives the bounding rectangle. The coordinates are relative to the upper-left corner of the tree-view control.</param>
	/// <param name="bCode">- Value specifying the portion of the item for which to retrieve the bounding rectangle. If this parameter is TRUE, the bounding rectangle includes only the text of the item. Otherwise, it includes the entire line that the item occupies in the tree-view control.</param>
	/// <returns>If the item is visible and the bounding rectangle is successfully retrieved, the return value is TRUE. Otherwise, the TVM_GETITEMRECT message returns FALSE and does not retrieve the bounding rectangle.</returns>
	inline bool dcxTreeView_GetItemRect(_In_ HWND hwnd, _In_ HTREEITEM hItem, _In_ LPRECT prc, _In_ bool bCode) noexcept
	{
		return (TreeView_GetItemRect(hwnd, hItem, prc, bCode) != FALSE);
	}

	/// <summary>
	/// Retrieves the largest possible bounding rectangle that constitutes the "hit zone" for a specified part of an item.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <param name="hItem"></param>
	/// <param name="prc">- Pointer to a RECT structure that receives the bounding rectangle. The caller is responsible for allocating this structure. The coordinates received are relative to the upper-left corner of the tree-view control.</param>
	/// <param name="partid">- ID of the item part. This value must be TVGIPR_BUTTON (0x0001).</param>
	/// <returns></returns>
	inline bool dcxTreeView_GetItemPartRect(_In_ HWND hwnd, _In_ HTREEITEM hItem, _Inout_ LPRECT prc, _In_ TVITEMPART partid) noexcept
	{
		const TVGETITEMPARTRECTINFO pri{ hItem, prc, partid };
		return (gsl::narrow_cast<BOOL>(SendMessageW(hwnd, TVM_GETITEMPARTRECT, 0, reinterpret_cast<LPARAM>(&pri))) != FALSE);

		//return (TreeView_GetItemPartRect(hwnd, hItem, prc, partid) != FALSE);
	}

	/// <summary>
	/// Retrieves the current background color of the control.
	/// </summary>
	/// <param name="hwnd">- Handle to a tree-view control.</param>
	/// <returns>Returns a COLORREF value that represents the current background color. If this value is -1, the control is using the system color for the background color.</returns>
	inline COLORREF dcxTreeView_GetBkColor(_In_ HWND hwnd) noexcept
	{
		return TreeView_GetBkColor(hwnd);
	}

	/// <summary>
	/// Get the number of items selected?
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <returns>Returns the number of items selected.</returns>
	inline DWORD dcxTreeView_GetSelectedCount(_In_ HWND hTree) noexcept
	{
		return TreeView_GetSelectedCount(hTree);
	}

	/// <summary>
	/// Get the currently selected item.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns>Returns the handle to the item if successful, or NULL otherwise.</returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetSelection(_In_ HWND hTree) noexcept
	{
		if (!hTree)
			return nullptr;

		GSL_SUPPRESS(es.47) GSL_SUPPRESS(lifetime.4) return TreeView_GetSelection(hTree);
	}

	/// <summary>
	/// Retrieves the current text color of the control.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <returns>Returns a COLORREF value that represents the current text color. If this value is -1, the control is using the system color for the text color.</returns>
	inline COLORREF dcxTreeView_GetTextColor(_In_ HWND hTree) noexcept
	{
		return TreeView_GetTextColor(hTree);
	}

	/// <summary>
	/// Retrieves the tree-view item that bears the specified relationship to a specified item.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem">- Handle to an item.</param>
	/// <param name="iCode">- Flag specifying the item to retrieve.</param>
	/// <returns>Returns the handle to the item if successful. For most cases, the message returns a NULL value to indicate an error.</returns>
	/// <remarks>This macro will return NULL if the item being retrieved is the root node of the tree. For example, if you use this macro with the TVGN_PARENT flag on a first-level child of the tree view's root node, the macro will return NULL.</remarks>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetNextItem(_In_ HWND hTree, _In_opt_ HTREEITEM hItem, _In_ int iCode) noexcept
	{
		if (!hTree)
			return nullptr;

		GSL_SUPPRESS(es.47) GSL_SUPPRESS(lifetime.4) return TreeView_GetNextItem(hTree, hItem, iCode);
	}

	/// <summary>
	/// Retrieves the tree-view item that bears the TVGN_NEXTSELECTED relationship to a specified tree item.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem">- Handle to an item.</param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns>Handle to an item, or NULL if no item is found with the TVGN_NEXTSELECTED relationship.</returns>
	/// <remarks>Used to find the next selected item when there are multiple items selected.</remarks>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetNextSelected(_In_ HWND hTree, _In_opt_ HTREEITEM hItem) noexcept
	{
		if (!hTree)
			return nullptr;

		GSL_SUPPRESS(es.47) GSL_SUPPRESS(lifetime.4) return TreeView_GetNextSelected(hTree, hItem);
	}

	/// <summary>
	/// Show an items info tip.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	inline DWORD dcxTreeView_ShowInfoTip(_In_ HWND hTree, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_ShowInfoTip(hTree, hItem);
	}

	/// <summary>
	/// Set the control to unicode format or not.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="bUnicode"></param>
	/// <returns></returns>
	inline bool dcxTreeView_SetUnicodeFormat(_In_ HWND hTree, _In_ bool bUnicode) noexcept
	{
		return (TreeView_SetUnicodeFormat(hTree, bUnicode) != FALSE);
	}

	/// <summary>
	/// Sets a tree-view control's child tooltip control.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
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
	/// <param name="lpht">
	/// <para>- Pointer to a TVHITTESTINFO structure.</para>
	/// <para>When the message is sent, the pt member specifies the coordinates of the point to test.</para>
	/// <para>When the message returns, the hItem member is the handle to the item at the specified point or NULL if no item occupies the point.</para>
	/// <para>Also, when the message returns, the flags member is a hit test value that indicates the location of the specified point.</para>
	/// </param>
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
	/// Creates a dragging bitmap for the specified item in a tree-view control. The macro also creates an image list for the bitmap and adds the bitmap to the image list. An application can display the image when dragging the item by using the image list functions.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem"></param>
	/// <returns></returns>
	[[nodiscard("Memory Leak")]] inline HIMAGELIST dcxTreeView_CreateDragImage(_In_ HWND hTree, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_CreateDragImage(hTree, hItem);
	}

	/// <summary>
	/// <para>Retrieves the last expanded item in a tree-view control.</para>
	/// <para>This does not retrieve the last item visible in the tree-view window.</para>
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <seealso cref="dcxTreeView_GetNextItem"/>
	/// <returns>Returns the handle to the item if successful, or NULL otherwise.</returns>
	[[nodiscard]] inline HTREEITEM dcxTreeView_GetLastVisible(_In_ HWND hTree) noexcept
	{
		return TreeView_GetLastVisible(hTree);
	}

	/// <summary>
	/// Gets the current line color.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <returns>Returns the current line color, or the CLR_DEFAULT value if none has been specified.</returns>
	inline COLORREF dcxTreeView_GetLineColor(_In_ HWND hTree) noexcept
	{
		return TreeView_GetLineColor(hTree);
	}

	/// <summary>
	/// Get an items last sibling.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="child"></param>
	/// <returns></returns>
	[[nodiscard]] HTREEITEM dcxTreeView_GetLastSibling(_In_ HWND hTree, _In_ HTREEITEM child) noexcept;

	/// <summary>
	/// Set an items state.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
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
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem"></param>
	/// <param name="txt"></param>
	void dcxTreeView_SetItemText(_In_ HWND hTree, _In_ HTREEITEM hItem, _In_ const TString& txt) noexcept;

	/// <summary>
	/// Delete a specified item. This function includes the fix for items not being displayed after a delete.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="item"></param>
	/// <returns></returns>
	bool dcxTreeView_DeleteItem(_In_ HWND hTree, _In_ const HTREEITEM item) noexcept;

	/// <summary>
	/// Delete all items. This function includes the fix for items not being displayed after a delete.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <returns></returns>
	bool dcxTreeView_DeleteAllItems(_In_ HWND hTree) noexcept;

	/// <summary>
	/// Get the HitTest info about the current cursor position.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <returns></returns>
	TVHITTESTINFO dcxTreeView_GetCursorItem(_In_ HWND hTree) noexcept;

	/// <summary>
	/// Map an index to an item.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="nIndex"></param>
	/// <returns>The matching item if any, or nullptr</returns>
	[[nodiscard]] HTREEITEM dcxTreeView_MapIndexToItem(_In_ HWND hTree, _In_ size_t nIndex) noexcept;

	/// <summary>
	/// Maps an accessibility ID to an HTREEITEM.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="id">The accessibility ID to map to an HTREEITEM.</param>
	/// <returns>Returns the HTREEITEM to which the specified accessibility ID is mapped.</returns>
	/// <remarks>
	/// <para>To use TreeView_MapAccIDToHTREEITEM, specify Comctl32.dll version 6 in the manifest.</para>
	/// <para>The accessibility ID is not the same as that mentioned in IAccessibleObject.</para>
	/// <para>This is a unique ID used for treeview items as long as treeitems do not exceed the max limit of UINT.</para>
	/// </remarks>
	inline HTREEITEM dcxTreeView_MapAccIDToHTREEITEM(_In_ HWND hTree, _In_ UINT id) noexcept
	{
		return TreeView_MapAccIDToHTREEITEM(hTree, id);
	}

	/// <summary>
	/// Maps an HTREEITEM to an accessibility ID.
	/// </summary>
	/// <param name="hTree">- Handle to the tree-view control.</param>
	/// <param name="hItem">- The value to be mapped.</param>
	/// <returns>Returns an accessibility ID.</returns>
	/// <remarks>
	/// <para>To use TreeView_MapHTREEITEMToAccID, specify Comctl32.dll version 6 in the manifest.</para>
	/// <para>The accessibility ID is not the same as that mentioned in IAccessibleObject.</para>
	/// <para>This is a unique ID used for treeview items as long as treeitems do not exceed the max limit of UINT.</para>
	/// </remarks>
	inline UINT dcxTreeView_MapHTREEITEMToAccID(_In_ HWND hTree, _In_ HTREEITEM hItem) noexcept
	{
		return TreeView_MapHTREEITEMToAccID(hTree, hItem);
	}

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
	/// <param name="hwndCtl"></param>
	/// <param name="bModified"></param>
	inline void dcxEdit_SetModify(_In_ HWND hwndCtl, _In_ BOOL bModified) noexcept
	{
		Edit_SetModify(hwndCtl, bModified);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwndCtl"></param>
	/// <returns></returns>
	inline bool dcxEdit_GetModify(_In_ HWND hwndCtl) noexcept
	{
		return !!Edit_GetModify(hwndCtl);
	}

	/// <summary>
	/// Sets the zoom ratio for a multiline edit control or a rich edit control. The ratio must be a value between 1/64 and 64. The edit control needs to have the ES_EX_ZOOMABLE extended style set, for this message to have an effect, see Edit Control Extended Styles.
	/// </summary>
	/// <param name="hwndCtl"></param>
	/// <param name="iNumerator">- Numerator of the zoom ratio.</param>
	/// <param name="iDenomerator">- Denominator of the zoom ratio. These parameters can have the following values.</param>
	/// <returns><para>If the new zoom setting is accepted, the return value is TRUE.</para>
	/// <para>If the new zoom setting is not accepted, the return value is FALSE.</para></returns>
	inline bool dcxEdit_SetZoom(_In_ HWND hwndCtl, _In_ int iNumerator, _In_ int iDenomerator) noexcept
	{
		return !!Edit_SetZoom(hwndCtl, iNumerator, iDenomerator);
	}

	/// <summary>
	/// Gets the zero-based index of the uppermost visible line in a multiline edit control. You can send this message to either an edit control or a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns><para>The return value is the zero-based index of the uppermost visible line in a multiline edit control.</para>
	///  <para>Edit controls : For single - line edit controls, the return value is the zero - based index of the first visible character.</para>
	///	<para>Rich edit controls : For single - line rich edit controls, the return value is zero.</para></returns>
	inline DWORD dcxEdit_GetFirstVisibleLine(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(Edit_GetFirstVisibleLine(hwnd));
	}

	/// <summary>
	/// Gets the number of lines in a multiline edit control. You can send this message to either an edit control or a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>The return value is an integer specifying the total number of text lines in the multiline edit control or rich edit control. If the control has no text, the return value is 1. The return value will never be less than 1.</returns>
	[[nodiscard]] inline int dcxEdit_GetLineCount(_In_ HWND hwnd) noexcept
	{
		return Edit_GetLineCount(hwnd);
	}

	/// <summary>
	/// Retrieves the length, in characters, of a line in an edit control. You can send this message to either an edit control or a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iLine">- The character index of a character in the line whose length is to be retrieved. If this parameter is greater than the number of characters in the control, the return value is zero.
	///	This parameter can be - 1. In this case, the message returns the number of unselected characters on lines containing selected characters.For example, if the selection extended from the fourth character of one line through the eighth character from the end of the next line, the return value would be 10 (three characters on the first line and seven on the next).
	///	</param>
	/// <returns>For multiline edit controls, the return value is the length, in TCHARs, of the line specified by the wParam parameter. For ANSI text, this is the number of bytes; for Unicode text, this is the number of characters. It does not include the carriage-return character at the end of the line.
	/// For single - line edit controls, the return value is the length, in TCHARs, of the text in the edit control.
	///	If wParam is greater than the number of characters in the control, the return value is zero.</returns>
	[[nodiscard]] inline int dcxEdit_LineLength(_In_ HWND hwnd, _In_ int iLine) noexcept
	{
		return Edit_LineLength(hwnd, iLine);
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iLine"></param>
	/// <returns></returns>
	[[nodiscard]] inline int dcxEdit_GetLineIndex(_In_ HWND hwnd, _In_ int iLine) noexcept
	{
		if (!hwnd)
			return 0;

		return gsl::narrow_cast<int>(SendMessage(hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(iLine), 0));
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
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
	/// Copies a line of text from an edit control and places it in a specified buffer. You can send this message to either an edit control or a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iLine">- The zero-based index of the line. This parameter is ignored by a single-line edit control.</param>
	/// <param name="lpch">- A pointer to a buffer that receives the string.</param>
	/// <param name="cchMax">- The maximum number of characters to be copied to the buffer.</param>
	/// <returns>The return value is the number of TCHARs copied. The return value is zero if the line number specified by the wParam parameter is greater than the number of lines in the edit control.</returns>
	inline int dcxEdit_GetLine(_In_ HWND hwnd, _In_ int iLine, _Inout_z_ LPTSTR lpch, _In_ int cchMax) noexcept
	{
		return Edit_GetLine(hwnd, iLine, lpch, cchMax);
	}

	/// <summary>
	/// Gets the character index of the first character of a specified line in a multiline edit control. A character index is the zero-based index of the character from the beginning of the edit control. You can send this message to either an edit control or a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="iLine">- The zero-based line number. A value of -1 specifies the current line number (the line that contains the caret).</param>
	/// <returns>The return value is the character index of the line specified in the iLine parameter, or it is -1 if the specified line number is greater than the number of lines in the edit control.</returns>
	inline int dcxEdit_LineIndex(_In_ HWND hwnd, _In_ int iLine) noexcept
	{
		return Edit_LineIndex(hwnd, iLine);
	}

	/// <summary>
	/// Selects a range of characters in an edit control. You can send this message to either an edit control or a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nStart">- The starting character position of the selection.</param>
	/// <param name="nEnd">- The ending character position of the selection.</param>
	inline void dcxEdit_SetSel(_In_ HWND hwnd, _In_ DWORD nStart, _In_ DWORD nEnd) noexcept
	{
		Edit_SetSel(hwnd, nStart, nEnd);
	}

	/// <summary>
	/// Replaces the selected text in an edit control or a rich edit control with the specified text.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="bUndoable">- Specifies whether the replacement operation can be undone. If this is TRUE, the operation can be undone. If this is FALSE , the operation cannot be undone.</param>
	/// <param name="strReplacement">- A pointer to a null-terminated string containing the replacement text.</param>
	inline void dcxEdit_ReplaceSel(_In_ HWND hwnd, _In_ BOOL bUndoable, _In_z_ LPCTSTR strReplacement) noexcept
	{
		SendMessage(hwnd, EM_REPLACESEL, bUndoable, reinterpret_cast<LPARAM>(strReplacement));
	}

	/// <summary>
	/// Sets the textual cue, or tip, that is displayed by the edit control to prompt the user for information.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="strReplacement">- A pointer to a Unicode string that contains the text to display as the textual cue.</param>
	/// <param name="bFocused">- TRUE if the cue banner should show even when the edit control has focus; otherwise, FALSE. FALSE is the default behavior the cue banner disappears when the user clicks in the control.</param>
	/// <returns>If the message succeeds, it returns TRUE. Otherwise it returns FALSE.</returns>
	inline bool dcxEdit_SetCueBannerTextFocused(_In_ HWND hwnd, _In_z_ LPCWSTR strReplacement, bool bFocused) noexcept
	{
		return !!SendMessage(hwnd, EM_SETCUEBANNER, bFocused, reinterpret_cast<LPARAM>(strReplacement));
	}

	/// <summary>
	/// Sets the textual cue, or tip, that is displayed by the edit control to prompt the user for information.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="strReplacement">- A pointer to a Unicode string that contains the text to display as the textual cue.</param>
	/// <returns>If the message succeeds, it returns TRUE. Otherwise it returns FALSE.</returns>
	inline bool dcxEdit_SetCueBannerText(_In_ HWND hwnd, _In_z_ LPCWSTR strReplacement) noexcept
	{
		return !!SendMessage(hwnd, EM_SETCUEBANNER, 0, reinterpret_cast<LPARAM>(strReplacement));
	}

	/// <summary>
	/// Gets the end of line character used for the content of the edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>One of the EC_ENDOFLINE values.</returns>
	EC_ENDOFLINE dcxEdit_GetEndOfLine(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// Gets the current zoom ratio for a multiline edit control or a rich edit control. The zoom ration is always between 1/64 and 64.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nNumerator">- Receives the numerator of the zoom ratio.</param>
	/// <param name="nDenominator">- Receives the denominator of the zoom ratio.</param>
	/// <returns>true if message is processed, which it will be if both nNumerator and nDenominator are not NULL.</returns>
	bool dcxEdit_GetZoom(_In_ HWND hwnd, _In_ int* nNumerator, _In_ int* nDenominator) noexcept;

	/// <summary>
	/// Gets the characters used to mark the end of line.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns></returns>
	TString dcxEdit_GetEndOfLineCharacters(_In_ HWND hwnd);

	/// <summary>
	/// Gets the character index of the caret location for a given edit control.
	/// NB: Works on winXP+
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>The return value is a zero-based index value of the position of the caret.</returns>
	DWORD dcxEdit_GetCaretIndex(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nPos"></param>
	void dcxEdit_SetCaretIndex2(_In_ HWND hwnd, _In_ DWORD nPos) noexcept;

	/// <summary>
	/// Gets information about the character closest to a specified point in the client area of an edit control.
	/// NB: MUST NOT be used with a RichEdit
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="ihPos">- Contains the horizontal coordinate.</param>
	/// <param name="ivPos">- Contains the vertical coordinate.</param>
	/// <returns>The LOWORD specifies the zero-based index of the character nearest the specified point. This index is relative to the beginning of the control, not the beginning of the line. If the specified point is beyond the last character in the edit control, the return value indicates the last character in the control. The HIWORD specifies the zero-based index of the line that contains the character. For single-line edit controls, this value is zero. The index indicates the line delimiter if the specified point is beyond the last visible character in a line.</returns>
	DWORD dcxEdit_CharFromPos(_In_ HWND hwnd, _In_ LONG ihPos, _In_ LONG ivPos) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="ich"></param>
	/// <returns></returns>
	DWORD dcxEdit_LineFromChar(_In_ HWND hwnd, _In_ const LONG& ich) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="nStart"></param>
	/// <param name="nEnd"></param>
	/// <returns>
	/// <para>The return value is a zero-based value with the starting position of the selection in the LOWORD and the position of the first TCHAR after the last selected TCHAR in the HIWORD. If either of these values exceeds 65,535, the return value is -1.</para>
	/// <para>It is better to use the values returned in nStart and nEnd because they are full 32 - bit values.</para>
	/// </returns>
	int dcxEdit_GetSel(_In_ HWND hwnd, _In_opt_ DWORD* nStart, _In_opt_ DWORD* nEnd) noexcept;

	// RichEdit

	/// <summary>
	/// Set a rich edit controls event mask.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwMask">The previous event mask.</param>
	/// <returns></returns>
	inline DWORD dcxRichEdit_SetEventMask(_In_ HWND hwnd, _In_ DWORD dwMask) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_SETEVENTMASK, 0, dwMask));
	}

	/// <summary>
	/// Retrieves the starting and ending character positions of the selection in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="lprng">- A CHARRANGE structure that receives the selection range.</param>
	inline void dcxRichEdit_ExGetSel(_In_ HWND hwnd, _Inout_ CHARRANGE* lprng) noexcept
	{
		SendMessage(hwnd, EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(lprng));
	}

	/// <summary>
	/// Selects a range of characters or Component Object Model (COM) objects in a Microsoft Rich Edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="lprng">- A CHARRANGE structure that sets the selection range.</param>
	/// <returns>The return value is the selection that is actually set.</returns>
	inline DWORD dcxRichEdit_ExSetSel(_In_ HWND hwnd, _In_ CHARRANGE* lprng) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_EXSETSEL, 0, reinterpret_cast<LPARAM>(lprng)));
	}

	/// <summary>
	/// Enables or disables automatic detection of hyperlinks by a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwType">-
	/// <para>Specify 0 to disable automatic link detection, or one of the following values to enable various kinds of detection.</para>
	/// <para>Value 					Meaning</para>
	///	<para>AURL_DISABLEMIXEDLGC		Windows 8: Disable recognition of domain names that contain labels with characters belonging to more than one of the following scripts : Latin, Greek, and Cyrillic.</para>
	///	<para>AURL_ENABLEDRIVELETTERS	Windows 8: Recognize file names that have a leading drive specification, such as c : \temp.</para>
	///	<para>AURL_ENABLEEA				This value is deprecated; use AURL_ENABLEEAURLS instead.</para>
	///	<para>AURL_ENABLEEAURLS			Recognize URLs that contain East Asian characters.</para>
	///	<para>AURL_ENABLEEMAILADDR		Windows 8: Recognize email addresses.</para>
	///	<para>AURL_ENABLETELNO			Windows 8: Recognize telephone numbers.</para>
	///	<para>AURL_ENABLEURL			Windows 8: Recognize URLs that include the path.</para>
	///	</param>
	/// <param name="pstrScheme"><para>This parameter determines the URL schemes recognized if AURL_ENABLEURL is active.</para>
	/// <para>If pstrScheme is NULL, the default scheme name list is used (see Remarks).</para>
	/// <para>Alternatively, pstrScheme can point to a null-terminated string consisting of up to 50 colon-terminated scheme names that supersede the default scheme name list.</para>
	/// <para>For example, the string could be "news:http:ftp:telnet:". The scheme name syntax is defined in the Uniform Resource Identifiers (URI): Generic Syntax document on The Internet Engineering Task Force (IETF) website.</para>
	/// <para>Specifically, a scheme name can contain up to 13 characters (including the colon), must start with an ASCII alphabetic, and can be followed by a mixture of ASCII alphabetics, digits, and the three punctuation characters: ".", "+", and "-". The string type can be either char* or WCHAR*; the rich edit control automatically detects the type.</para>
	/// </param>
	/// <returns>
	/// <para>If the message succeeds, the return value is zero.</para>
	/// <para>If the message fails, the return value is a nonzero value. For example, the message might fail due to insufficient memory, an invalid detection option, or an invalid scheme - name string.</para>
	///	<para>If pstrScheme contains more than 50 scheme names, the message fails with a return value of E_INVALIDARG.</para>
	///	</returns>
	inline DWORD dcxRichEdit_AutoUrlDetect(_In_ HWND hwnd, _In_ DWORD dwType, _In_opt_z_ LPCTSTR pstrScheme) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_AUTOURLDETECT, dwType, reinterpret_cast<LPARAM>(pstrScheme)));
	}

	/// <summary>
	/// Calls the autocorrect callback function that is stored by the EM_SETAUTOCORRECTPROC message, provided that the text preceding the insertion point is a candidate for autocorrection.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="chr">- If this character is a tab (U+0009), and the character preceding the insertion point isn't a tab, then the character preceding the insertion point is treated as part of the autocorrect candidate string instead of as a string delimiter; otherwise, chr has no effect.</param>
	/// <returns>The return value is zero if the message succeeds, or nonzero if an error occurs.</returns>
	inline bool dcxRichEdit_CallAutoCorrectProc(_In_ HWND hwnd, _In_ WCHAR chr) noexcept
	{
		return (gsl::narrow_cast<BOOL>(SendMessage(hwnd, EM_CALLAUTOCORRECTPROC, chr, 0)) != FALSE);
	}

	/// <summary>
	/// Determines whether a rich edit control can paste a specified clipboard format.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="fmts">- Specifies the Clipboard Formats to try. To try any format currently on the clipboard, set this parameter to zero.</param>
	/// <returns>
	/// <para>If the clipboard format can be pasted, the return value is a nonzero value.</para>
	/// <para>If the clipboard format cannot be pasted, the return value is zero.</para>
	///	</returns>
	inline bool dcxRichEdit_CanPaste(_In_ HWND hwnd, _In_ UINT fmts) noexcept
	{
		return (gsl::narrow_cast<BOOL>(SendMessage(hwnd, EM_CANPASTE, fmts, 0)) != FALSE);
	}

	/// <summary>
	/// Determines whether there are any actions in the control redo queue.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>
	/// <para>If there are actions in the control redo queue, the return value is a nonzero value.</para>
	/// <para>If the redo queue is empty, the return value is zero.</para>
	/// </returns>
	/// <remarks>To redo the most recent undo operation, send the EM_REDO message.</remarks>
	inline bool dcxRichEdit_CanRedo(_In_ HWND hwnd) noexcept
	{
		return (gsl::narrow_cast<BOOL>(SendMessage(hwnd, EM_CANREDO, 0, 0)) != FALSE);
	}

	/// <summary>
	/// Hides or shows the selection in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="bHide">- Value specifying whether to hide or show the selection. If this parameter is FALSE, the selection is shown. Otherwise, the selection is hidden.</param>
	inline void dcxRichEdit_HideSelection(_In_ HWND hwnd, _In_ BOOL bHide) noexcept
	{
		SendMessage(hwnd, EM_HIDESELECTION, gsl::narrow_cast<WPARAM>(bHide), 0);
	}

	/// <summary>
	/// Determines the character formatting in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwRange">-
	/// <para>Specifies the range of text from which to retrieve formatting. It can be one of the following values.</para>
	///	<para>Value 		-	Meaning</para>
	///	<para>SCF_DEFAULT	-	The default character formatting.</para>
	///	<para>SCF_SELECTION	-	The current selection's character formatting.</para>
	///	</param>
	/// <param name="chrFmt">- 
	///	<para>A CHARFORMAT2 structure that receives the attributes of the first character.</para>
	/// <para>The dwMask member specifies which attributes are consistent throughout the entire selection.</para>
	/// <para>For example, if the entire selection is either in italics or not in italics, CFM_ITALIC is set; if the selection is partly in italics and partly not, CFM_ITALIC is not set.</para>
	///	<para>Microsoft Rich Edit 2.0 and later: This parameter is a CHARFORMAT2 structure, which is an extension of the CHARFORMAT structure.</para>
	/// <para>The structure's cbSize member is set to indicate the version of the structure.</para>
	///	</param>
	inline DWORD dcxRichEdit_GetCharFormat(_In_ HWND hwnd, _In_ UINT dwRange, _Inout_ CHARFORMAT2& chrFmt) noexcept
	{
		chrFmt.cbSize = sizeof(CHARFORMAT2);
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_GETCHARFORMAT, dwRange, reinterpret_cast<LPARAM>(&chrFmt)));
	}

	/// <summary>
	/// Determines the character formatting in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwRange">-
	/// <para>Specifies the range of text from which to retrieve formatting. It can be one of the following values.</para>
	///	<para>Value 		-	Meaning</para>
	///	<para>SCF_DEFAULT	-	The default character formatting.</para>
	///	<para>SCF_SELECTION	-	The current selection's character formatting.</para>
	///	</param>
	/// <param name="chrFmt">- 
	///	<para>A CHARFORMAT structure that receives the attributes of the first character.</para>
	/// <para>The dwMask member specifies which attributes are consistent throughout the entire selection.</para>
	/// <para>For example, if the entire selection is either in italics or not in italics, CFM_ITALIC is set; if the selection is partly in italics and partly not, CFM_ITALIC is not set.</para>
	/// <para>The structure's cbSize member is set to indicate the version of the structure.</para>
	///	</param>
	inline DWORD dcxRichEdit_GetCharFormat(_In_ HWND hwnd, _In_ UINT dwRange, _Inout_ CHARFORMAT& chrFmt) noexcept
	{
		chrFmt.cbSize = sizeof(CHARFORMAT);
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_GETCHARFORMAT, dwRange, reinterpret_cast<LPARAM>(&chrFmt)));
	}

	/// <summary>
	/// Retrieves the current edit style flags.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>
	/// <para>Returns the current edit style flags, which can include one or more of the following values:</para>
	/// <para>SES_BEEPONMAXTEXT			-	Rich Edit will call the system beeper if the user attempts to enter more than the maximum characters.</para>
	///	<para>SES_BIDI					-	Turns on bidirectional processing.This is automatically turned on by Rich Edit if any of the following window styles are active : WS_EX_RIGHT, WS_EX_RTLREADING, WS_EX_LEFTSCROLLBAR.However, this setting is useful for handling these window styles when using a custom implementation of ITextHost(default: 0).</para>
	///	<para>SES_CTFALLOWEMBED			-	Windows XP with SP1 : Allow embedded objects to be inserted using TSF(default: 0).</para>
	///	<para>SES_CTFALLOWPROOFING		-	Windows XP with SP1 : Allows TSF proofing tips(default: 0).</para>
	///	<para>SES_CTFALLOWSMARTTAG		-	Windows XP with SP1 : Allows TSF SmartTag tips(default: 0).</para>
	///	<para>SES_CTFNOLOCK				-	Windows 8 : Do not allow the TSF lock read / write access.This pauses TSF input.</para>
	///	<para>SES_DEFAULTLATINLIGA		-	Windows 8 : Fonts with an fi ligature are displayed with default OpenType features resulting in improved typography(default: 0).</para>
	///	<para>SES_DRAFTMODE				-	Windows XP with SP1 : Use draft mode fonts to display text.Draft mode is an accessibility option where the control displays the text with a single font; the font is determined by the system setting for the font used in message boxes.For example, accessible users may read text easier if it is uniform, rather than a mix of fonts and styles(default: 0).</para>
	///	<para>SES_EMULATE10				-	Windows 8: Emulate RichEdit 1.0 behavior.</para>
	///	<para>Note : If you really want this behavior, use the Windows riched32.dll instead of riched20.dll or msftedit.dll.Riched32.dll had more functionality.</para>
	///	<para>SES_EMULATESYSEDIT		-	When this bit is on, rich edit attempts to emulate the system edit control(default: 0).</para>
	///	<para>SES_EXTENDBACKCOLOR		-	Extends the background color all the way to the edges of the client rectangle(default: 0).</para>
	///	<para>SES_HIDEGRIDLINES			-	Windows XP with SP1 : If the width of table gridlines is zero, gridlines are not displayed.This is equivalent to the hide gridlines feature in Word's table menu (default: 0).</para>
	///	<para>SES_HYPERLINKTOOLTIPS		-	Windows 8 : When the cursor is over a link, display a tooltip with the target link address(default: 0).</para>
	///	<para>SES_LOGICALCARET			-	Windows 8 : Provide logical caret information instead of a caret bitmap as described in ITextHost::TxSetCaretPos(default: 0).</para>
	///	<para>SES_LOWERCASE				-	Converts all input characters to lowercase(default: 0).</para>
	///	<para>SES_MAPCPS				-	Obsolete.Do not use.</para>
	///	<para>SES_MULTISELECT			-	Windows 8: Enable multiselection with individual mouse selections made while the Ctrl key is pressed(default: 0).</para>
	///	<para>SES_NOEALINEHEIGHTADJUST	-	Windows 8 : Do not adjust line height for East Asian text(default: 0 which adjusts the line height by 15 %).</para>
	///	<para>SES_NOFOCUSLINKNOTIFY		-	Sends EN_LINK notification from links that do not have focus.</para>
	///	<para>SES_NOIME					-	Disallows IMEs for this instance of the rich edit control(default: 0).</para>
	///	<para>SES_NOINPUTSEQUENCECHK	-	When this bit is on, rich edit does not verify the sequence of typed text.Some languages(such as Thai and Vietnamese) require verifying the input sequence order before submitting it to the backing store(default: 0).</para>
	///	<para>SES_SCROLLONKILLFOCUS		-	When KillFocus occurs, scroll to the beginning of the text(character position equal to 0) (default: 0).</para>
	///	<para>SES_SMARTDRAGDROP			-	Windows 8: Add or delete a space according to the context when dropping text(default: 0).</para>
	///	<para>SES_USECRLF				-	Obsolete.Do not use.</para>
	///	<para>SES_WORDDRAGDROP			-	Windows 8: If word select is active, ensure that the drop location is at a word boundary(default: 0).</para>
	///	<para>SES_UPPERCASE				-	Converts all input characters to uppercase(default: 0).</para>
	///	<para>SES_USEAIMM				-	Uses the Active IMM input method component that ships with Internet Explorer 4.0 or later(default: 0).</para>
	///	<para>SES_USEATFONT				-	Windows XP with SP1 : Uses an @ font, which is designed for vertical text; this is used with the ES_VERTICAL window style.The name of an @ font begins with the @ symbol, for example, "@Batang" (default: 0, but is automatically turned on for vertical text layout).</para>
	///	<para>SES_USECTF				-	Windows XP with SP1 : Turns on TSF support. (default: 0)</para>
	///	<para>SES_XLTCRCRLFTOCR			-	Turns on translation of CRCRLFs to CRs.When this bit is on and a file is read in, all instances of CRCRLF will be converted to hard CRs internally.This will affect the text wrapping.Note that if such a file is saved as plain text, the CRs will be replaced by CRLFs.This is the.txt standard for plain text(default: 0, which deletes CRCRLFs on input).</para>
	/// </returns>
	inline DWORD dcxRichEdit_GetEditStyle(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_GETEDITSTYLE, 0, 0));
	}

	/// <summary>
	/// Retrieves the current extended edit style flags.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>
	/// <para>Returns the extended edit style flags, which can include one or more of the following values.</para>
	/// <para>SES_EX_HANDLEFRIENDLYURL	-	Display friendly name links with the same text color and underlining as automatic links, provided that temporary formatting isn t used or uses text autocolor(default: 0).</para>
	/// <para>SES_EX_MULTITOUCH			-	Enable touch support in Rich Edit.This includes selection, caret placement, and context - menu invocation.When this flag is not set, touch is emulated by mouse commands, which do not take touch - mode specifics into account(default: 0).</para>
	///	<para>SES_EX_NOACETATESELECTION	-	Display selected text using classic Windows selection text and background colors instead of background acetate color(default: 0).</para>
	///	<para>SES_EX_NOMATH				-	Disable insertion of math zones(default: 1).To enable math editing and display, send the EM_SETEDITSTYLEEX message with wParam set to 0, and lParam set to SES_EX_NOMATH.</para>
	///	<para>SES_EX_NOTABLE			-	Disable insertion of tables.The EM_INSERTTABLE message returns E_FAIL and RTF tables are skipped(default: 0).</para>
	///	<para>SES_EX_USESINGLELINE		-	Enable a multiline control to act like a single - line control with the ability to scroll vertically when the single - line height is greater than the window height(default: 0).</para>
	///	<para>SES_HIDETEMPFORMAT		-	Hide temporary formatting that is created when ITextFont.Reset is called with tomApplyTmp.For example, such formatting is used by spell checkers to display a squiggly underline under possibly misspelled words.</para>
	///	<para>SES_EX_USEMOUSEWPARAM		-	Use wParam when handling the WM_MOUSEMOVE message and do not call GetAsyncKeyState.</para>
	/// </returns>
	inline DWORD dcxRichEdit_GetEditStyleEx(_In_ HWND hwnd) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_GETEDITSTYLEEX, 0, 0));
	}

	/// <summary>
	/// Sets character formatting in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwRange">- 
	/// <para>Character formatting that applies to the control. If this parameter is zero, the default character format is set. Otherwise, it can be one of the following values.</para>
	///	<para>Value					-	Meaning</para>
	///	<para>SCF_ALL				-	Applies the formatting to all text in the control.Not valid with SCF_SELECTION or SCF_WORD.</para>
	///	<para>SCF_ASSOCIATEFONT		-	RichEdit 4.1: Associates a font to a given script, thus changing the default font for that script.To specify the font, use the following members of CHARFORMAT2 : yHeight, bCharSet, bPitchAndFamily, szFaceName, and lcid.</para>
	///	<para>SCF_ASSOCIATEFONT2	-	RichEdit 4.1 : Associates a surrogate(plane - 2) font to a given script, thus changing the default font for that script.To specify the font, use the following members of CHARFORMAT2 : yHeight, bCharSet, bPitchAndFamily, szFaceName, and lcid.</para>
	///	<para>SCF_CHARREPFROMLCID	-	Gets the character repertoire from the LCID.</para>
	///	<para>SCF_DEFAULT			-	RichEdit 4.1: Sets the default font for the control.</para>
	///	<para>SPF_DONTSETDEFAULT	-	Prevents setting the default paragraph format when the rich edit control is empty.</para>
	///	<para>SCF_NOKBUPDATE		-	RichEdit 4.1: Prevents keyboard switching to match the font.For example, if an Arabic font is set, normally the automatic keyboard feature for Bidi languages changes the keyboard to an Arabic keyboard.</para>
	///	<para>SCF_SELECTION			-	Applies the formatting to the current selection.If the selection is empty, the character formatting is applied to the insertion point, and the new character format is in effect only until the insertion point changes.</para>
	///	<para>SPF_SETDEFAULT		-	Sets the default paragraph formatting attributes.</para>
	///	<para>SCF_SMARTFONT			-	Apply the font only if it can handle script.</para>
	///	<para>SCF_USEUIRULES		-	RichEdit 4.1: Used with SCF_SELECTION.Indicates that format came from a toolbar or other UI tool, so UI formatting rules should be used instead of literal formatting.</para>
	///	<para>SCF_WORD				-	Applies the formatting to the selected word or words.If the selection is empty but the insertion point is inside a word, the formatting is applied to the word.The SCF_WORD value must be used in conjunction with the SCF_SELECTION value.</para>
	///	</param>
	/// <param name="chrFmt">-
	/// <para>A CHARFORMAT structure specifying the character formatting to use. Only the formatting attributes specified by the dwMask member are changed.</para>
	/// <para>The szFaceName and bCharSet members may be overruled when invalid for characters, for example: Arial on kanji characters.</para>
	/// <para>The structure's cbSize member is set to sizeof(CHARFORMAT)</para>
	/// </param>
	/// <returns><para>If the operation succeeds, the return value is true.</para>
	///	<para>If the operation fails, the return value is false.</para>
	///	</returns>
	inline bool dcxRichEdit_SetCharFormat(_In_ HWND hwnd, _In_ UINT dwRange, _Inout_ CHARFORMAT& chrFmt) noexcept
	{
		chrFmt.cbSize = sizeof(CHARFORMAT);
		return !!SendMessage(hwnd, EM_GETCHARFORMAT, dwRange, reinterpret_cast<LPARAM>(&chrFmt));
	}

	/// <summary>
	/// Sets character formatting in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwRange">- 
	/// <para>Character formatting that applies to the control. If this parameter is zero, the default character format is set. Otherwise, it can be one of the following values.</para>
	///	<para>Value					-	Meaning</para>
	///	<para>SCF_ALL				-	Applies the formatting to all text in the control.Not valid with SCF_SELECTION or SCF_WORD.</para>
	///	<para>SCF_ASSOCIATEFONT		-	RichEdit 4.1: Associates a font to a given script, thus changing the default font for that script.To specify the font, use the following members of CHARFORMAT2 : yHeight, bCharSet, bPitchAndFamily, szFaceName, and lcid.</para>
	///	<para>SCF_ASSOCIATEFONT2	-	RichEdit 4.1 : Associates a surrogate(plane - 2) font to a given script, thus changing the default font for that script.To specify the font, use the following members of CHARFORMAT2 : yHeight, bCharSet, bPitchAndFamily, szFaceName, and lcid.</para>
	///	<para>SCF_CHARREPFROMLCID	-	Gets the character repertoire from the LCID.</para>
	///	<para>SCF_DEFAULT			-	RichEdit 4.1: Sets the default font for the control.</para>
	///	<para>SPF_DONTSETDEFAULT	-	Prevents setting the default paragraph format when the rich edit control is empty.</para>
	///	<para>SCF_NOKBUPDATE		-	RichEdit 4.1: Prevents keyboard switching to match the font.For example, if an Arabic font is set, normally the automatic keyboard feature for Bidi languages changes the keyboard to an Arabic keyboard.</para>
	///	<para>SCF_SELECTION			-	Applies the formatting to the current selection.If the selection is empty, the character formatting is applied to the insertion point, and the new character format is in effect only until the insertion point changes.</para>
	///	<para>SPF_SETDEFAULT		-	Sets the default paragraph formatting attributes.</para>
	///	<para>SCF_SMARTFONT			-	Apply the font only if it can handle script.</para>
	///	<para>SCF_USEUIRULES		-	RichEdit 4.1: Used with SCF_SELECTION.Indicates that format came from a toolbar or other UI tool, so UI formatting rules should be used instead of literal formatting.</para>
	///	<para>SCF_WORD				-	Applies the formatting to the selected word or words.If the selection is empty but the insertion point is inside a word, the formatting is applied to the word.The SCF_WORD value must be used in conjunction with the SCF_SELECTION value.</para>
	///	</param>
	/// <param name="chrFmt">-
	/// <para>A CHARFORMAT2 structure specifying the character formatting to use. Only the formatting attributes specified by the dwMask member are changed.</para>
	/// <para>The szFaceName and bCharSet members may be overruled when invalid for characters, for example: Arial on kanji characters.</para>
	/// <para>The structure's cbSize member is set to sizeof(CHARFORMAT2)</para>
	/// </param>
	/// <returns><para>If the operation succeeds, the return value is true.</para>
	///	<para>If the operation fails, the return value is false.</para>
	///	</returns>
	inline bool dcxRichEdit_SetCharFormat(_In_ HWND hwnd, _In_ UINT dwRange, _Inout_ CHARFORMAT2& chrFmt) noexcept
	{
		chrFmt.cbSize = sizeof(CHARFORMAT2);
		return !!SendMessage(hwnd, EM_SETCHARFORMAT, dwRange, reinterpret_cast<LPARAM>(&chrFmt));
	}

	/// <summary>
	/// Determines which line contains the specified character in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwIndex">- Zero-based index of the character.</param>
	/// <returns>The zero-based index of the line.</returns>
	[[nodiscard]] inline DWORD dcxRichEdit_ExLineFromChar(_In_ HWND hwnd, _In_ DWORD dwIndex) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_EXLINEFROMCHAR, 0, gsl::narrow_cast<LPARAM>(dwIndex)));
	}

	/// <summary>
	/// Gets information about the character closest to a specified point in the client area of a RichEdit control.
	/// NB: You must NOT use the edit controls version of this message with richedit.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="pos">- A pointer to a POINTL structure that contains the horizontal and vertical coordinates.</param>
	/// <returns>The return value specifies the zero-based character index of the character nearest the specified point. The return value indicates the last character in the edit control if the specified point is beyond the last character in the control.</returns>
	inline DWORD dcxRichEdit_CharFromPos(_In_ HWND hwnd, _In_ const PPOINTL pos) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_CHARFROMPOS, 0, reinterpret_cast<LPARAM>(pos)));
	}

	/// <summary>
	/// Retrieves the currently selected text in a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="lpstr">- Pointer to a buffer that receives the selected text. The calling application must ensure that the buffer is large enough to hold the selected text.</param>
	/// <returns>This message returns the number of characters copied, not including the terminating null character.</returns>
	inline int dcxRichEdit_GetSelText(_In_ HWND hwnd, _Inout_z_ LPTSTR lpstr) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, EM_GETSELTEXT, 0, reinterpret_cast<LPARAM>(lpstr)));
	}

	/// <summary>
	/// Causes a rich edit control to pass its contents to an application defined EditStreamCallback callback function. The callback function can then write the stream of data to a file or any other location that it chooses.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwFlags">- Specifies the data format and replacement options.
	/// <para>This value must be one of the following values.</para>
	///	<para>Value 		Meaning</para>
	///	<para>SF_RTF		RTF.</para>
	///	<para>SF_RTFNOOBJS	RTF with spaces in place of COM objects.</para>
	///	<para>SF_TEXT		Text with spaces in place of COM objects.</para>
	///	<para>SF_TEXTIZED	Text with a text representation of COM objects.</para>
	///	<para>The SF_RTFNOOBJS option is useful if an application stores COM objects itself, as RTF representation of COM objects is not very compact.The control word, \objattph, followed by a space denotes the object position.</para>
	///	<para>In addition, you can specify the following flags.</para>
	/// <para>Value 			Meaning</para>
	///	<para>SFF_PLAINRTF		If specified, the rich edit control streams out only the keywords common to all languages, ignoring language - specific keywords. If not specified, the rich edit control streams out all keywords.You can combine this flag with the SF_RTF or SF_RTFNOOBJS flag.</para>
	///	<para>SFF_SELECTION		If specified, the rich edit control streams out only the contents of the current selection. If not specified, the control streams out the entire contents.You can combine this flag with any of data format values.</para>
	///	<para>SF_UNICODE		Microsoft Rich Edit 2.0 and later: Indicates Unicode text.You can combine this flag with the SF_TEXT flag.</para>
	///	<para>SF_USECODEPAGE	Rich Edit 3.0 and later : Generates UTF - 8 RTF and text using other code pages.The code page is set in the high word of wParam. For example, for UTF - 8 RTF, set wParam to(CP_UTF8 &lt;&lt; 16) | SF_USECODEPAGE | SF_RTF.</para>
	///	</param>
	/// <param name="lpstrm">- Pointer to an EDITSTREAM structure. On input, the pfnCallback member of this structure must point to an application defined EditStreamCallback function. On output, the dwError member can contain a nonzero error code if an error occurred.</param>
	/// <returns>This message returns the number of characters written to the data stream.</returns>
	inline int dcxRichEdit_StreamOut(_In_ HWND hwnd, _In_ DWORD dwFlags, _Inout_ EDITSTREAM* lpstrm) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, EM_STREAMOUT, dwFlags, reinterpret_cast<LPARAM>(lpstrm)));
	}

	/// <summary>
	/// The reverse of streamout
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="dwFlags"></param>
	/// <param name="lpstrm"></param>
	/// <returns></returns>
	inline int dcxRichEdit_StreamIn(_In_ HWND hwnd, _In_ DWORD dwFlags, _Inout_ EDITSTREAM* lpstrm) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, EM_STREAMIN, dwFlags, reinterpret_cast<LPARAM>(lpstrm)));
	}

	/// <summary>
	/// Replaces the selection with a blob that displays an image.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="lpRIP">- A pointer to a RICHEDIT_IMAGE_PARAMETERS structure that contains the image blob.</param>
	/// <returns>Returns S_OK if successful, or one of the following error codes.</returns>
	inline LRESULT dcxRichEdit_InsertImage(_In_ HWND hwnd, _In_ RICHEDIT_IMAGE_PARAMETERS* lpRIP) noexcept
	{
		return SendMessage(hwnd, EM_INSERTIMAGE, 0, reinterpret_cast<LPARAM>(lpRIP));
	}

	/// <summary>
	/// Sets the background color for a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="bUseSystem">- Specifies whether to use the system color. If this parameter is a nonzero value, the background is set to the window background system color. Otherwise, the background is set to the specified color.</param>
	/// <param name="clr">- A COLORREF structure specifying the color if wParam is zero. To generate a COLORREF, use the RGB macro.</param>
	/// <returns>The original background color.</returns>
	COLORREF dcxRichEdit_SetBkgndColor(_In_ HWND hwnd, _In_ BOOL bUseSystem, _In_ COLORREF clr) noexcept;

	/// <summary>
	/// Gets the background color for a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <returns>The background color.</returns>
	COLORREF dcxRichEdit_GetBkgndColor(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// Formats a range of text in a rich edit control for a specific device.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="bRender">- Specifies whether to render the text. If this parameter is not zero, the text is rendered. Otherwise, the text is just measured.</param>
	/// <param name="lpFR">- A FORMATRANGE structure containing information about the output device, or NULL to free information cached by the control.</param>
	/// <returns>This message returns the index of the last character that fits in the region, plus 1.</returns>
	/// <remarks>
	/// This message is typically used to format the content of rich edit control for an output device such as a printer.
	/// After using this message to format a range of text, it is important that you free cached information by sending EM_FORMATRANGE again,
	/// but with lParam set to NULL; otherwise, a memory leak will occur.
	/// Also, after using this message for one device, you must free cached information before using it again for a different device.
	/// </remarks>
	inline int dcxRichEdit_FormatRange(_In_ HWND hwnd, _In_ BOOL bRender, _In_ FORMATRANGE* lpFR) noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(hwnd, EM_FORMATRANGE, gsl::narrow_cast<WPARAM>(bRender), reinterpret_cast<LPARAM>(lpFR)));
	}

	/// <summary>
	/// Retrieves a specified range of characters from a rich edit control.
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
	/// <param name="rng">- A TEXTRANGE structure that specifies the range of characters to retrieve and a buffer to copy the characters to.</param>
	/// <returns>The message returns the number of characters copied, not including the terminating null character.</returns>
	inline DWORD dcxRichEdit_GetTextRange(_In_ HWND hwnd, _Inout_ TEXTRANGE& rng) noexcept
	{
		return gsl::narrow_cast<DWORD>(SendMessage(hwnd, EM_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&rng)));
	}

	// Animate

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- A handle to the control.</param>
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
		GSL_SUPPRESS(es.47) return !!Animate_Close(hwnd);
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

	/// <summary>
	/// Set the tab padding.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="cx"></param>
	/// <param name="cy"></param>
	inline void dcxTabCtrl_SetPadding(_In_ HWND hwnd, _In_ UINT cx, _In_ UINT cy) noexcept
	{
		TabCtrl_SetPadding(hwnd, cx, cy);
	}

	/// <summary>
	/// Determines the currently selected tab in a tab control.
	/// </summary>
	/// <param name="hwnd">- Handle to the tab control.</param>
	/// <returns>Returns the index of the selected tab if successful, or -1 if no tab is selected.</returns>
	inline int dcxTabCtrl_GetCurSel(_In_ HWND hwnd) noexcept
	{
		return TabCtrl_GetCurSel(hwnd);
	}

	/// <summary>
	/// Calculates a tab control's display area given a window rectangle, or calculates the window rectangle that would correspond to a specified display area.
	/// </summary>
	/// <param name="hwnd">- Handle to the tab control.</param>
	/// <param name="bLarger">- Operation to perform. If this parameter is TRUE, prc specifies a display rectangle and receives the corresponding window rectangle. If this parameter is FALSE, prc specifies a window rectangle and receives the corresponding display area.</param>
	/// <param name="prc">- Pointer to a RECT structure that specifies the given rectangle and receives the calculated rectangle.</param>
	inline void dcxTabCtrl_AdjustRect(_In_ HWND hwnd, _In_ bool bLarger, _Inout_ LPRECT prc) noexcept
	{
		TabCtrl_AdjustRect(hwnd, bLarger, prc);
	}

	/// <summary>
	/// Removes all items from a tab control.
	/// </summary>
	/// <param name="hwnd">- Handle to the tab control.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxTabCtrl_DeleteAllItems(_In_ HWND hwnd) noexcept
	{
		return (TabCtrl_DeleteAllItems(hwnd) != FALSE);
	}

	/// <summary>
	/// Removes an item from a tab control.
	/// </summary>
	/// <param name="hwnd">- Handle to the tab control.</param>
	/// <param name="i">- Index of the item to delete.</param>
	/// <returns>Returns TRUE if successful, or FALSE otherwise.</returns>
	inline bool dcxTabCtrl_DeleteItem(_In_ HWND hwnd, _In_ int i) noexcept
	{
		return (TabCtrl_DeleteItem(hwnd, i) != FALSE);
	}

	int dcxTabCtrl_GetPointItem(_In_ HWND hwnd, _In_ POINT pt) noexcept;

	int dcxTabCtrl_GetHoverItem(_In_ HWND hwnd) noexcept;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="hwnd">- Handle to the tab control.</param>
	/// <param name="i"></param>
	/// <param name="prc"></param>
	/// <returns></returns>
	inline bool dcxTabCtrl_GetItemRect(_In_ HWND hwnd, _In_ int i, _Out_ LPRECT prc) noexcept
	{
		return (TabCtrl_GetItemRect(hwnd, i, prc) != FALSE);
	}

}