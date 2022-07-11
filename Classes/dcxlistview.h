/*!
 * \file dcxlistview.h
 * \brief blah
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

#ifndef _DCXLISTVIEW_H_
#define _DCXLISTVIEW_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/dcxprogressbar.h"
#include "Classes/tinyxml/tinyxml.h"
#include "Classes\custom\ListHelper.h"

#ifndef LVM_RESETEMPTYTEXT
// Needed to reset the empty listview text.
// forces the listview to send LVN_GETEMPTYMARKUP notification. (no args)
#define LVM_RESETEMPTYTEXT (LVM_FIRST + 84)
#endif

class DcxDialog;

#define LVIS_BOLD       0x00000100	//!< ListView Caption Bold Style
#define LVIS_UNDERLINE  0x00000200	//!< ListView Caption Underline Style
#define LVIS_COLOR      0x00000400	//!< ListView Caption Color Style
#define LVIS_BGCOLOR    0x00000800	//!< ListView Caption Background Color Style
#define LVIS_PBAR       0x00001000	//!< ListView ProgressBar Style
#define LVIS_ITALIC     0x00002000	//!< ListView Caption Italic Style
#define LVIS_HASHITEM	0x00004000	//!< ListView Item takes it's text from a hashtable. (text is `table item`) (to be replaced by LVIS_HASHTABLE)
#define LVIS_HASHNUMBER	0x00008000	//!< ListView Item takes it's text from a hashtable. (text is `table N`) (to be replaced by LVIS_HASHTABLE)
#define LVIS_XML		0x00010000	//!< ListView Item takes it's text from a xml file. (text is `dataset_name filename`) (adds all items in dataset_name)
#define LVIS_HASHTABLE	0x00020000	//!< ListView Item takes it's text from a hashtable. (text is `+flags hashtable (N|N1,N2|item)`) (can add multiple items)
#define LVIS_WINDOW		0x00040000	//!< ListView Item takes it's text from a custom window. (text is `+flags window (N|N1,N2)`) (can add multiple items)
#define LVIS_CONTROL	0x00080000	//!< ListView Item takes it's text from another DCX control. (text is `+flags dialog id (N|N1,N2)`) (can add multiple items)
#define LVIS_CENTERICON	0x00100000	//!< ListView Item displays the icon centered if no text present.

// +flags mentioned above tell the parser extra details about how the items are added.
// +	:	single item is added as text only (N)
// +a	:	items added include all info for the item not just it's text.
// +n	:	Numeric range supplied N1,N2
// +i	:	item name supplied (for hashtable)
// +A	:	adds ALL items starting at N
#define LVIMF_ALLINFO	0x01
#define LVIMF_NUMERIC	0x02
#define LVIMF_NAMED		0x04
#define LVIMF_ADDALL	0x08

#define LVSS_ASC        0x01   //!< ListView Sort Ascending Style
#define LVSS_DESC       0x02   //!< ListView Sort Descending Style
#define LVSS_NUMERIC    0x04   //!< ListView Sort Numeric Style
#define LVSS_ALPHA      0x08   //!< ListView Sort Alpha-Numeric Style
#define LVSS_CUSTOM     0x10   //!< ListView Sort Custom Style
#define LVSS_CASE       0x20   //!< ListView Sort Case-sensitive Style
#define LVSS_IRC		0x40   //!< ListView Sort IRC Style (sorts with this precedence for first char: ~, &, @, %, +, all other chars)

#define LVCS_TEXT       0x01   //!< ListView Text Color Style
#define LVCS_BKGTEXT    0x02   //!< ListView Background Text Color Style
#define LVCS_BKG        0x04   //!< ListView Background Color Style
#define LVCS_OUTLINE    0x08   //!< ListView Selection Outline Color Style

#define LVSCW_AUTOSIZE_MAX	-3	// Max of LVSCW_AUTOSIZE & LVSCW_AUTOSIZE_USEHEADER

struct CharRank
{
	TCHAR c{};
	int rank{};

	bool operator ==(const CharRank& cs) const noexcept
	{
		return rank == cs.rank;
	}
	bool operator >(const CharRank& cs) const noexcept
	{
		return rank > cs.rank;
	}
	bool operator <(const CharRank& cs) const noexcept
	{
		return rank < cs.rank;
	}
	bool operator >=(const CharRank& cs) const noexcept
	{
		return !(*this < cs);
	}
	bool operator <=(const CharRank& cs) const noexcept
	{
		return !(*this > cs);
	}
};
//struct RankSet
//{
//	//(qaohvV)~&@%+-
//	const CharRank ranks[7]{
//		{ TEXT('~'), 0},
//		{ TEXT('&'), 1},
//		{ TEXT('@'), 2},
//		{ TEXT('%'), 3},
//		{ TEXT('+'), 4},
//		{ TEXT('-'), 5},
//		{ 0,6 }
//	};
//	int GetRank(TCHAR c) const noexcept
//	{
//		for (const auto& a : ranks)
//		{
//			if (a.c == c)
//				return a.rank;
//		}
//		return gsl::at(ranks, std::size(ranks) - 1).rank;
//	}
//};

/*!
 * \brief blah
 *
 * blah
 */

struct DCXLVSORT
{
	TString tsCustomAlias;  //!< Custom Sorting Alias
	UINT iSortFlags{};        //!< Sorting Flags
	HWND m_Hwnd{ nullptr };            //!< ListView Handle
	int nColumn{};            //!< Sorting Column
	TCHAR itemtext1[MIRC_BUFFER_SIZE_CCH]{};
	TCHAR itemtext2[MIRC_BUFFER_SIZE_CCH]{};
};
using LPDCXLVSORT = DCXLVSORT*;

struct DCXLVRENDERINFO
{
	DWORD		m_dFlags{};	//!< Render flags (bold etc..)
	COLORREF	m_cText{ CLR_INVALID };	//!< Text Colour
	COLORREF	m_cBg{ CLR_INVALID };		//!< Background Colour.
	COLORREF	m_cOrigText{ CLR_INVALID };
	COLORREF	m_cOrigBg{ CLR_INVALID };
};
using LPDCXLVRENDERINFO = DCXLVRENDERINFO*;

using VectorOfRenderInfo = std::vector<LPDCXLVRENDERINFO>;

/*!
* \brief blah
*
* blah
*/

#define DCX_LV_COLUMNF_AUTO			1		// uses LVSCW_AUTOSIZE
#define DCX_LV_COLUMNF_AUTOHEADER	2		// uses LVSCW_AUTOSIZE_USEHEADER
#define DCX_LV_COLUMNF_AUTOMAX		4		// uses both LVSCW_AUTOSIZE & LVSCW_AUTOSIZE_USEHEADER to find the largest.
#define DCX_LV_COLUMNF_PERCENT		8		// m_iSize is a % width between 0 & 100 (zero width columns are hidden)
#define DCX_LV_COLUMNF_FIXED		16		// m_iSize is the fixed width of the column

struct DCXLVCOLUMNINFO
{
	int			m_iColumn{};	// the column affected by this info.
	DWORD		m_dFlags{};	// size flags (autosize, % width etc..)
	int			m_iSize{};	// size of column (meaning depends on flags)
};
using LPDCXLVCOLUMNINFO = DCXLVCOLUMNINFO*;

using VectorOfColumnInfo = std::vector<LPDCXLVCOLUMNINFO>;
//using ColumnWidths = std::map<int, int>;

/*!
 * \brief blah
 *
 * blah
 */

struct DCXLVITEM
{
	TString tsTipText;	//!< Tooltip text
	TString tsMark;		// Marked text
	DcxControl* pbar{ nullptr };
	int iPbarCol{};
	VectorOfRenderInfo	vInfo;	//!< Render Info for each column
};
using LPDCXLVITEM = DCXLVITEM*;

namespace Dcx
{
	inline BOOL dcxListView_SetItem(_In_ HWND hwnd, _In_ const LPLVITEM plvi) noexcept
	{
		return ListView_SetItem(hwnd, plvi);
	}
	inline void dcxListView_SetItemText(_In_ HWND hwnd, const int i, const int iSub, _In_z_ PTCHAR str) noexcept
	{
		ListView_SetItemText(hwnd, i, iSub, str);
	}
	inline void dcxListView_SetItemState(_In_ HWND hwnd, const int item, const unsigned int state, const unsigned int statemask) noexcept
	{
		ListView_SetItemState(hwnd, item, state, statemask);
	}
	inline BOOL dcxListView_SetInfoTip(_In_ HWND hwnd, _In_ PLVSETINFOTIP lvit) noexcept
	{
		return ListView_SetInfoTip(hwnd, lvit);
	}
	inline void dcxListView_SetCheckState(_In_ HWND hwnd, const int i, const int fCheck) noexcept
	{
		ListView_SetCheckState(hwnd, i, fCheck);
	}
	inline DWORD dcxListView_SetExtendedListViewStyleEx(_In_ HWND hwnd, const WPARAM mask, const LPARAM dw) noexcept
	{
		return ListView_SetExtendedListViewStyleEx(hwnd, mask, dw);
	}
	inline BOOL dcxListView_SetEmptyText(_In_ HWND hwnd, _In_z_ PTCHAR pszText) noexcept
	{
		//const auto res = ListView_SetEmptyText(hwnd, pszText, cchText);
		const auto res = SetWindowText(hwnd, pszText);
		SendMessage(hwnd, LVM_RESETEMPTYTEXT, 0, 0);
		return res;
	}
	inline int dcxListView_InsertItem(_In_ HWND hwnd, _In_ const LPLVITEM plvi) noexcept
	{
		return ListView_InsertItem(hwnd, plvi);
	}
	inline BOOL dcxListView_GetItem(_In_ HWND hwnd, _Inout_ LPLVITEM const plvi) noexcept
	{
		return ListView_GetItem(hwnd, plvi);
	}
	inline void dcxListView_GetItemText(_In_ HWND hwnd, const int nItem, const int nSubItem, _Inout_z_ PTCHAR const Buf, const size_t nBufSize) noexcept
	{
		ListView_GetItemText(hwnd, nItem, nSubItem, Buf, nBufSize);
	}
	[[nodiscard]] inline int dcxListView_GetNextItem(_In_ HWND hwnd, int i, unsigned int flags) noexcept
	{
		return ListView_GetNextItem(hwnd, i, flags);
	}
	inline BOOL dcxListView_GetColumnOrderArray(_In_ HWND hwnd, const int nCnt, _Inout_ int* const p) noexcept
	{
		return ListView_GetColumnOrderArray(hwnd, nCnt, p);
	}
	[[nodiscard]] inline int dcxListView_GetColumnWidth(_In_ HWND hwnd, const int iCol) noexcept
	{
		return ListView_GetColumnWidth(hwnd, iCol);
	}
	[[nodiscard]] inline int dcxListView_GetItemCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetItemCount(hwnd);
	}
	[[nodiscard]] inline unsigned int dcxListView_GetSelectedCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetSelectedCount(hwnd);
	}
	[[nodiscard]] inline unsigned int dcxListView_GetItemState(_In_ HWND hwnd, const int i, const unsigned int mask) noexcept
	{
		return ListView_GetItemState(hwnd, i, mask);
	}
	[[nodiscard]] inline DWORD dcxListView_GetExtendedListViewStyle(_In_ HWND hwnd) noexcept
	{
		return ListView_GetExtendedListViewStyle(hwnd);
	}
	inline BOOL dcxListView_GetItemRect(_In_ HWND hwnd, const int i, _Inout_ LPRECT prc, const int code) noexcept
	{
		GSL_SUPPRESS(es.47) return ListView_GetItemRect(hwnd, i, prc, code);
	}
	inline BOOL dcxListView_GetSubItemRect(_In_ HWND hwnd, const int i, const int iSub, const int code, _Inout_ LPRECT prc) noexcept
	{
		GSL_SUPPRESS(es.47) return ListView_GetSubItemRect(hwnd, i, iSub, code, prc);
	}
	inline BOOL dcxListView_GetColumn(_In_ HWND hwnd, const int iCol, _Inout_ LPLVCOLUMN lc) noexcept
	{
		return ListView_GetColumn(hwnd, iCol, lc);
	}
	inline int dcxListView_InsertColumn(_In_ HWND hwnd, const int iCol, _In_ LPLVCOLUMN pCol) noexcept
	{
		return ListView_InsertColumn(hwnd, iCol, pCol);
	}
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxListView_GetHeader(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(lifetime.4) return ListView_GetHeader(hwnd);
	}
	GSL_SUPPRESS(lifetime.4)
		[[nodiscard]] inline HWND dcxListView_GetToolTips(_In_ HWND hwnd) noexcept
	{
		GSL_SUPPRESS(lifetime.4) return ListView_GetToolTips(hwnd);
	}
	[[nodiscard]] inline int dcxListView_GetTopIndex(_In_ HWND hwnd) noexcept
	{
		return ListView_GetTopIndex(hwnd);
	}
	inline LRESULT dcxListView_GetGroupInfo(_In_ HWND hwnd, const unsigned int gid, _Inout_ PLVGROUP pgrp) noexcept
	{
		return ListView_GetGroupInfo(hwnd, gid, pgrp);
	}
	[[nodiscard]] inline LRESULT dcxListView_GetGroupCount(_In_ HWND hwnd) noexcept
	{
		return ListView_GetGroupCount(hwnd);
	}
	[[nodiscard]] inline unsigned int dcxListView_GetGroupState(_In_ HWND hwnd, const int gid, const DWORD mask) noexcept
	{
		return ListView_GetGroupState(hwnd, gid, mask);
	}
	inline BOOL dcxListView_GetEmptyText(_In_ HWND hwnd, _Inout_z_ PTCHAR str, const int sz) noexcept
	{
		return ListView_GetEmptyText(hwnd, str, sz);
	}
	inline int dcxListView_SubItemHitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvht) noexcept
	{
		return ListView_SubItemHitTest(hwnd, lvht);
	}
	inline BOOL dcxListView_SetColumnWidth(_In_ HWND hwnd, const int iCol, const int cx) noexcept
	{
		return ListView_SetColumnWidth(hwnd, iCol, cx);
	}
	inline BOOL dcxListView_SetColumnOrderArray(_In_ HWND hwnd, const int iCount, const int *pi) noexcept
	{
		return ListView_SetColumnOrderArray(hwnd, iCount, pi);
	}
	inline DWORD dcxListView_SetView(_In_ HWND hwnd, DWORD mode) noexcept
	{
		return ListView_SetView(hwnd, mode);
	}
	[[nodiscard]] inline LRESULT dcxListView_HasGroup(_In_ HWND hwnd, const int group) noexcept
	{
		return ListView_HasGroup(hwnd, group);
	}
	[[nodiscard]] inline BOOL dcxListView_IsGroupViewEnabled(_In_ HWND hwnd) noexcept
	{
		return ListView_IsGroupViewEnabled(hwnd);
	}
	inline BOOL dcxListView_EnableGroupView(_In_ HWND hwnd, bool fEnable) noexcept
	{
		return ListView_EnableGroupView(hwnd, fEnable);
	}
	inline void dcxListView_SetGroupState(_In_ HWND hwnd, const int gid, const unsigned int mask, const unsigned int state) noexcept
	{
		ListView_SetGroupState(hwnd, gid, mask, state);
	}
	inline void dcxListView_InsertGroup(_In_ HWND hwnd, const int index, _In_ PLVGROUP pgrp) noexcept
	{
		ListView_InsertGroup(hwnd, index, pgrp);
	}
	inline void dcxListView_MoveGroup(_In_ HWND hwnd, const int gid, const int index) noexcept
	{
		ListView_MoveGroup(hwnd, gid, index);
	}
	inline void dcxListView_RemoveGroup(_In_ HWND hwnd, const int gid) noexcept
	{
		ListView_RemoveGroup(hwnd, gid);
	}
	inline void dcxListView_RemoveAllGroups(_In_ HWND hwnd) noexcept
	{
		ListView_RemoveAllGroups(hwnd);
	}
	inline BOOL dcxListView_DeleteColumn(_In_ HWND hwnd, const int column) noexcept
	{
		return ListView_DeleteColumn(hwnd, column);
	}
	inline BOOL dcxListView_DeleteAllItems(_In_ HWND hwnd) noexcept
	{
		return ListView_DeleteAllItems(hwnd);
	}
	inline BOOL dcxListView_DeleteItem(_In_ HWND hwnd, const int i) noexcept
	{
		return ListView_DeleteItem(hwnd, i);
	}
	inline BOOL dcxListView_SortItemsEx(_In_ HWND hwnd, _In_ PFNLVCOMPARE pFn, _Inout_ void *data) noexcept
	{
		return ListView_SortItemsEx(hwnd, pFn, data);
	}
	inline BOOL dcxListView_EnsureVisible(_In_ HWND hwnd, const int i, BOOL partialOK) noexcept
	{
		return ListView_EnsureVisible(hwnd, i, partialOK);
	}
	inline BOOL dcxListView_Scroll(_In_ HWND hwnd, const int dx, const int dy) noexcept
	{
		return ListView_Scroll(hwnd, dx, dy);
	}
	[[nodiscard]] inline int dcxListView_GetCountPerPage(_In_ HWND hwnd) noexcept
	{
		return ListView_GetCountPerPage(hwnd);
	}
	[[nodiscard]] inline HWND dcxListView_GetEditControl(_In_ HWND hwnd) noexcept
	{
		return ListView_GetEditControl(hwnd);
	}
	[[nodiscard]] inline COLORREF dcxListView_GetBkColor(_In_ HWND hwnd) noexcept
	{
		return ListView_GetBkColor(hwnd);
	}
	inline LRESULT dcxHeader_HitTest(_In_ HWND hwnd, _Inout_ LPHDHITTESTINFO hdti) noexcept
	{
		return SendMessage(hwnd, HDM_HITTEST, 0U, reinterpret_cast<LPARAM>(hdti));
	}
	inline HWND dcxListView_EditLabel(_In_ HWND hwnd, const int i) noexcept
	{
		return ListView_EditLabel(hwnd, i);
	}
	inline BOOL dcxHeader_GetItem(_In_ HWND hwnd, const int i, _Inout_ LPHDITEM phi) noexcept
	{
		return Header_GetItem(hwnd, i, phi);
	}
	inline BOOL dcxHeader_SetItem(_In_ HWND hwnd, const int i, _In_ const LPHDITEM phi) noexcept
	{
		return Header_SetItem(hwnd, i, phi);
	}
	inline int dcxListView_HitTest(_In_ HWND hwnd, _Inout_ LPLVHITTESTINFO lvhti) noexcept
	{
		return ListView_HitTest(hwnd, lvhti);
	}
	[[nodiscard("Memory leak")]] inline HIMAGELIST dcxListView_CreateDragImage(_In_ HWND hwnd, int i, _Inout_ LPPOINT pt) noexcept
	{
		return ListView_CreateDragImage(hwnd, i, pt);
	}
	[[nodiscard]] inline BOOL dcxListView_IsItemVisible(_In_ HWND hwnd, int i) noexcept
	{
		return ListView_IsItemVisible(hwnd, i);
	}
}

/*!
 * \brief blah
 *
 * blah
 */

class DcxListView final
	: public DcxControl
	, public DcxListHelper
{
public:
	DcxListView() = delete;
	DcxListView(const DcxListView&) = delete;
	DcxListView& operator =(const DcxListView&) = delete;	// No assignments!
	DcxListView(DcxListView&&) = delete;
	DcxListView& operator =(DcxListView&&) = delete;

	DcxListView(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxListView() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	[[nodiscard]] HIMAGELIST getImageList(const int iImageList) const noexcept;
	void setImageList(const HIMAGELIST himl, const int iImageList) noexcept;

	[[nodiscard]] bool isListViewStyle(const WindowStyle dwView) const noexcept;

	[[nodiscard]] const int& getColumnCount() const noexcept;

	inline const TString getType() const final { return TEXT("listview"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::LISTVIEW; }

	[[nodiscard]] int getTopIndex() const noexcept;
	[[nodiscard]] int getBottomIndex() const noexcept;

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	std::unique_ptr<TiXmlElement> toXml(int blah) const;

	const TString getStyles(void) const final;

	void HandleDragDrop(int x, int y) noexcept override;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	WNDPROC m_OrigEditProc{ nullptr }; //!< Label Edit Control Orignal Procedure

	[[nodiscard]] static UINT parseIconFlagOptions(const TString& flags);
	[[nodiscard]] static UINT parseItemFlags(const TString& flags);
	[[nodiscard]] static UINT parseHeaderFlags(const TString& flags);
	[[nodiscard]] static INT parseHeaderFlags2(const TString& flags);
	[[nodiscard]] static INT parseHeaderFlags2(const XSwitchFlags& xflags);
	[[nodiscard]] static UINT parseSortFlags(const TString& flags);
	[[nodiscard]] static UINT parseColorFlags(const TString& flags);
	[[nodiscard]] static UINT parseImageFlags(const TString& flags);
	[[nodiscard]] static UINT parseGroupFlags(const TString& flags);
	[[nodiscard]] static UINT parseGroupState(const TString& flags);

	[[nodiscard]] bool matchItemText(const int nItem, const int nSubItem, const TString& search, const DcxSearchTypes& SearchType) const;
	[[nodiscard]] bool matchItemText(const int nItem, const int nSubItem, const dcxSearchData &srch_data) const;

	void autoSize(const int nColumn, const TString& flags);
	void autoSize(const int nColumn, const int iFlags, const int iWidth = 0) noexcept;

	bool m_bAllowDrag{ false }; //!< Dragging Items ?

private:
	DcxControl * CreatePbar(LPLVITEM lvi, const TString & style);
	void ScrollPbars(const int row, const int nCols, const int iTop, const int iBottom, LPLVITEM lvi) noexcept;
	void UpdateScrollPbars(void);
	[[nodiscard]] HIMAGELIST initImageList(const int iImageList);
	bool xmlLoadListview(const int nPos, const TString& dataset, TString& filename);
	void xmlSetItem(const int nItem, const int nSubItem, const TiXmlElement* xNode, LPLVITEM lvi, LPDCXLVITEM lpmylvi, TString& tsBuf);
	bool ctrlLoadListview(const int nPos, const TString& tsData) noexcept;
	bool xLoadListview(const int nPos, const TString& tsData, const TCHAR* sTest, const TCHAR* sCount, const TCHAR* sGet, const TCHAR* sGetNamed);
	void massSetItem(const int nPos, const TString& input);
	void setHeaderStyle(HWND h, const int nCol, const TString& info);
	void DeleteColumns(const int nColumn) noexcept;
	[[nodiscard]] TString ItemToString(int nItem, int iColumns);
	bool xSaveListview(const int nStartPos, const int nEndPos, const TString& tsData, const TCHAR* sTestCommand, const TCHAR* sStoreCommand);

	void DrawEmpty(HDC hdc, const TString &tsBuf);

	void CopyItem(int iSrc, int iDest);
	void MoveItem(int iSrc, int iDest) noexcept;

	// static functions.
	//static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart_range, int *iEnd_range);
	//static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);

	[[nodiscard]] static UINT parseMassItemFlags(const TString& flags);
	static void parseText2Item(const TString& tsTxt, TString& tsItem, const TString& tsData);
	static int CALLBACK sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static LRESULT CALLBACK EditLabelProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	//static HIMAGELIST createImageList(const bool bIcons);
	//static void parseListviewExStyles(const TString & styles, LONG * ExStyles) noexcept;

	[[nodiscard]] static const WindowExStyle parseListviewExStyles(const TString& styles) noexcept;

	/// <summary>
	/// Convert a string to an item number.
	/// </summary>
	/// <param name="ts">- The string to convert.</param>
	/// <returns>A zero index item number</returns>
	[[nodiscard]] int StringToItemNumber(const TString& ts) const
	{
		auto nItem = ts.to_int() - 1;

		// check if item supplied was 0 (now -1), last item in list
		if (nItem == -1)
			nItem = Dcx::dcxListView_GetItemCount(m_Hwnd) - 1;

		return nItem;
	}

	/// <summary>
	/// Get a characters rank for IRC sort.
	/// </summary>
	/// <param name="c">- The character to check.</param>
	/// <returns>A rank value >= 0</returns>
	[[nodiscard]] static int GetRank(TCHAR c) noexcept
	{
		for (const auto& a : m_ranks)
		{
			if (a.c == c)
				return a.rank;
		}
		return gsl::at(m_ranks, std::size(m_ranks) - 1).rank;
	}

	//
	HFONT m_hItemFont{ nullptr };					// Font used for specific item changes.
	HFONT m_hOldItemFont{ nullptr };				// Font used for specific item changes.
	int m_iSelectedItem{};				// Items currently selected.
	int m_iSelectedSubItem{};				// SubItems currently selected.
	mutable int m_iColumnCount{ -1 };			// the number of columns in the listview, a -1 value mean "dont know"
	VectorOfColumnInfo	m_vWidths;		// column widths for dynamic sizing of columns.
	//(qaohvV)~&@%+-
	static constexpr CharRank m_ranks[7] = {
	{ TEXT('~'), 0},	//
	{ TEXT('&'), 1},	//
	{ TEXT('@'), 2},	// channel ops
	{ TEXT('%'), 3},	// channel half-ops
	{ TEXT('+'), 4},	// channel voices
	{ TEXT('-'), 5},	// not allowed voice in channel?
	{ 0,6 }				// all other characters
	};
	bool m_bHasPBars{ false };					// true if listview has pbars at all, if it does, a slower update is used that checks & moves pbars. (better system needed)
	bool m_bReserved[3]{ false };
};

#endif // _DCXLISTVIEW_H_
