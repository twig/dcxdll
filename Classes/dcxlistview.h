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
#include "Classes/custom/SearchHelper.h"

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

using VectorOfRenderInfo = std::vector<DCXLVRENDERINFO>;

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

struct DCXLVITEM
{
	TString tsTipText;	//!< Tooltip text
	TString tsMark;		// Marked text
	DcxControl* pbar{ nullptr };
	int iPbarCol{};
	VectorOfRenderInfo	vInfo;	//!< Render Info for each column
};
using LPDCXLVITEM = DCXLVITEM*;
using VectorOfDcxItems = std::vector<DCXLVITEM>;
using MapOfDcxItems = std::map<LPARAM, DCXLVITEM>;

class DcxListView final
	: public DcxControl
	, virtual public DcxSearchHelper
{
public:
	DcxListView() = delete;
	DcxListView(const DcxListView&) = delete;
	GSL_SUPPRESS(c.128) DcxListView& operator =(const DcxListView&) = delete;	// No assignments!
	DcxListView(DcxListView&&) = delete;
	GSL_SUPPRESS(c.128) DcxListView& operator =(DcxListView&&) = delete;

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

	/// <summary>
	/// Count the columns in the control.
	/// This only works in report style, otherwise it returns zero.
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] const int& getColumnCount() const noexcept;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("listview"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::LISTVIEW; }

	[[nodiscard]] int getTopIndex() const noexcept;
	[[nodiscard]] int getBottomIndex() const noexcept;

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	std::unique_ptr<TiXmlElement> toXml(int blah) const;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	const TString getStyles(void) const final;

	void HandleDragDrop(int x, int y) noexcept override;
	void HandleDragMove(int x, int y) noexcept override;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	WNDPROC m_OrigEditProc{ nullptr }; //!< Label Edit Control Orignal Procedure

	[[nodiscard]] static UINT parseIconFlagOptions(const TString& flags);
	[[nodiscard]] static UINT parseItemFlags(const TString& flags);
	[[nodiscard]] static TString parseItemFlags(UINT uFlags);
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
	[[nodiscard]] gsl::strict_not_null<HIMAGELIST> initImageList(const int iImageList);

	bool xmlLoadListview(const int nPos, const TString& dataset, TString& filename);
	bool xmlLoadListview(const int nPos, const TiXmlElement* xElm);

	bool xmlSaveListview(const int nPosStart, int iPosEnd, const TString& dataset, TString& filename) const;
	bool xmlSaveListview(const int nPosStart, int iPosEnd, TiXmlElement* xElm) const;

	bool ctrlLoadListview(const int nPos, const TString& tsData) noexcept;
	bool xLoadListview(const int nPos, const TString& tsData, const TCHAR* sTest, const TCHAR* sCount, const TCHAR* sGet, const TCHAR* sGetNamed);
	bool xSaveListview(const int nStartPos, const int nEndPos, const TString& tsData, const TCHAR* sTestCommand, const TCHAR* sStoreCommand);

	void massSetItem(const int nPos, const TString& input);
	void setHeaderStyle(HWND h, const int nCol, const TString& info);

	void addColumn(int nColumn, int iOrder, const TString& tsFlags, int iIcon, int iWidth, const TString& tsText);
	void DeleteColumns(const int nColumn) noexcept;

	[[nodiscard]] TString ItemToString(int nItem, int iColumns) const;
	[[nodiscard]] TiXmlElement* ItemToXml(int nItem, int iColumns) const;

	void DrawEmpty(HDC hdc, const TString &tsBuf);
	void DrawMargin(HDC hdc) noexcept;
	void DrawMargin() noexcept;
	void DrawClientArea(HDC hdc);
	void DrawClientArea();

	/// <summary>
	/// Only called inside NM_CUSTOMDRAW.
	/// Draws the current item.
	/// </summary>
	/// <param name="lplvcd"></param>
	/// <returns></returns>
	LRESULT DrawItem(LPNMLVCUSTOMDRAW lplvcd);

	/// <summary>
	/// Draw the groups header text.
	/// </summary>
	/// <param name="hdc"></param>
	/// <param name="hTheme"></param>
	/// <param name="iStateId"></param>
	/// <param name="rc"></param>
	/// <param name="tsText"></param>
	/// <param name="uTextFlags"></param>
	/// <param name="uAlign"></param>
	/// <param name="bCustomText"></param>
	/// <param name="iCol"></param>
	void DrawGroupHeaderText(HDC hdc, HTHEME hTheme, int iStateId, LPCRECT rc, const TString &tsText, UINT uTextFlags, UINT uAlign, bool bCustomText, int iCol);

	/// <summary>
	/// Only called inside NM_CUSTOMDRAW.
	/// Draw current group.
	/// </summary>
	/// <param name="lplvcd"></param>
	/// <returns></returns>
	LRESULT DrawGroup(LPNMLVCUSTOMDRAW lplvcd);

	/// <summary>
	/// Only called inside NM_CUSTOMDRAW.
	/// </summary>
	/// <returns>Draw state of current group</returns>
	int getGroupDrawState() noexcept;

	/// <summary>
	/// Get the groups header text if any.
	/// </summary>
	/// <param name="gid"></param>
	/// <returns>The header text</returns>
	TString getGroupHeader(int gid);

	/// <summary>
	/// Test if a group is collapsible.
	/// </summary>
	/// <param name="gid"></param>
	/// <returns></returns>
	bool IsGroupCollapsible(int gid) const noexcept
	{
		if (!m_Hwnd)
			return false;

		return (Dcx::dcxListView_GetGroupState(m_Hwnd, gid, LVGS_COLLAPSIBLE) != 0);
	}

	/// <summary>
	/// Test if a group is currently collapsed.
	/// </summary>
	/// <param name="gid"></param>
	/// <returns></returns>
	bool IsGroupCollapsed(int gid) const noexcept
	{
		if (!m_Hwnd)
			return false;
		constexpr auto iState = LVGS_COLLAPSED | LVGS_COLLAPSIBLE;
		return (Dcx::dcxListView_GetGroupState(m_Hwnd, gid, iState) == iState );
	}

	/// <summary>
	/// Get the groups header collapse button rect.
	/// </summary>
	/// <param name="rcHeader"></param>
	/// <returns></returns>
	RECT GetHeaderButtonRect(_In_ LPCRECT rcHeader) const noexcept
	{
		RECT rcButton{ *rcHeader };
		rcButton.left = rcButton.right - (rcHeader->bottom - rcHeader->top);
		InflateRect(&rcButton, 0, -2);
		//OffsetRect(&rcButton, -10, 0);
		OffsetRect(&rcButton, -2, 0);
		return rcButton;
	}

	/// <summary>
	/// Get the groups header collapse button rect, based on a theme.
	/// </summary>
	/// <param name="hTheme"></param>
	/// <param name="hdc"></param>
	/// <param name="iStateId"></param>
	/// <param name="rcHeader"></param>
	/// <returns></returns>
	RECT GetHeaderButtonRect(_In_ HTHEME hTheme, _In_ HDC hdc, _In_ int iStateId, _In_ LPCRECT rcHeader) const noexcept
	{
		RECT rcButton{};
		const RECT rcBounds{ *rcHeader };
		DcxUXModule::dcxGetThemeBackgroundContentRect(hTheme, hdc, LVP_COLLAPSEBUTTON, iStateId, &rcBounds, &rcButton);

		return rcButton;
	}

	/// <summary>
	/// Adds a group to the control.
	/// </summary>
	/// <param name="tsInput"></param>
	void addGroup(const TString &tsInput);

	/// <summary>
	/// Adds a group to the control.
	/// </summary>
	/// <param name="index"></param>
	/// <param name="tsFlags"></param>
	/// <param name="gid"></param>
	void addGroup(int index, const TString& tsFlags, int gid, TString& tsText);

	/// <summary>
	/// Only called inside NM_CUSTOMDRAW.
	/// Draw current groups selection rect if any.
	/// </summary>
	/// <param name="hTheme"></param>
	/// <param name="hdc"></param>
	/// <param name="rc"></param>
	/// <param name="iStateId"></param>
	void DrawGroupSelectionRect(HTHEME hTheme, HDC hdc, LPCRECT rc, int iStateId) noexcept;

	/// <summary>
	/// Copy iSrc to iDest.
	/// iSrc is maintained.
	/// </summary>
	/// <param name="iSrc"></param>
	/// <param name="iDest"></param>
	void CopyItem(int iSrc, int iDest);

	/// <summary>
	/// Move iSrc to iDest.
	/// iSrc is deleted.
	/// </summary>
	/// <param name="iSrc"></param>
	/// <param name="iDest"></param>
	void MoveItem(int iSrc, int iDest) noexcept;

	/// <summary>
	/// Get the margin rect.
	/// </summary>
	/// <returns></returns>
	RECT getListRect() const noexcept;

	void Command_a(const TString& input);
	void Command_b(const TString& input);
	void Command_c(const TString& input);
	void Command_d(const TString& input);
	void Command_e(const TString& input);
	void Command_f(const TString& input);
	void Command_g(const TString& input);
	void Command_h(const TString& input);
	void Command_i(const TString& input);
	void Command_j(const TString& input);
	void Command_k(const TString& input);
	void Command_l(const TString& input);
	void Command_m(const TString& input);
	void Command_n(const TString& input);
	void Command_o(const TString& input);
	void Command_p(const TString& input);
	void Command_q(const TString& input);
	void Command_u(const TString& input);
	void Command_r(const TString& input);
	void Command_s(const TString& input);
	void Command_t(const TString& input);
	void Command_v(const TString& input);
	void Command_w(const TString& input);
	void Command_x(const TString& input);
	void Command_y(const TString& input);
	void Command_z(const TString& input);

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
		// check if item supplied was -1 (now -2), 1 beyond last...
		else if (nItem == -2)
			nItem = Dcx::dcxListView_GetItemCount(m_Hwnd);

		return nItem;
	}

	// static functions.
	[[nodiscard]] static UINT parseMassItemFlags(const TString& flags);
	static void parseText2Item(const TString& tsTxt, TString& tsItem, const TString& tsData);
	static int CALLBACK sortItemsEx(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static LRESULT CALLBACK EditLabelProc(gsl::not_null<HWND> mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	[[nodiscard]] static const WindowExStyle parseListviewExStyles(const TString& styles) noexcept;

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

	/// <summary>
	/// Get the colour for group header text.
	/// </summary>
	/// <param name="hTheme"></param>
	/// <param name="iStateId"></param>
	/// <returns></returns>
	static COLORREF getThemeGroupTextColour(HTHEME hTheme, int iStateId) noexcept
	{
		COLORREF clr{ RGB(0, 51, 153) };
		if (hTheme)
			DcxUXModule::dcxGetThemeColor(hTheme, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, TMT_HEADING1TEXTCOLOR, &clr);
		return clr;
	}
	/// <summary>
	/// Get the group header fill colour.
	/// </summary>
	/// <param name="hTheme"></param>
	/// <param name="iStateId"></param>
	/// <returns></returns>
	static COLORREF getThemeGroupBkgFillColour(HTHEME hTheme, int iStateId) noexcept
	{
		COLORREF clr{ RGB(185, 229, 242) };
		if (hTheme)
			DcxUXModule::dcxGetThemeColor(hTheme, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, TMT_ACCENTCOLORHINT, &clr);
		return clr;
	}
	/// <summary>
	/// Get the group header border colour.
	/// </summary>
	/// <param name="hTheme"></param>
	/// <param name="iStateId"></param>
	/// <returns></returns>
	static COLORREF getThemeGroupBkgBorderColour(HTHEME hTheme, int iStateId) noexcept
	{
		COLORREF clr{ RGB(94, 131, 191) };
		if (hTheme)
			DcxUXModule::dcxGetThemeColor(hTheme, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, TMT_FILLCOLORHINT, &clr);
		return clr;
	}
	/// <summary>
	/// Get the group headers font.
	/// </summary>
	/// <param name="hTheme"></param>
	/// <param name="iStateId"></param>
	/// <param name="hdc"></param>
	/// <returns></returns>
	[[nodiscard("Memory Leak")]] static HFONT getThemeGroupFont(HTHEME hTheme, int iStateId, HDC hdc) noexcept
	{
		if (hTheme)
		{
			LOGFONT logFont{};

			if (DcxUXModule::dcxGetThemeFont(hTheme, hdc, LISTVIEWPARTS::LVP_GROUPHEADER, iStateId, TMT_HEADING1FONT, &logFont) == S_OK)
				return CreateFontIndirectW(&logFont);
		}
		return nullptr;
	}

	//
	HFONT m_hItemFont{ nullptr };		// Font used for specific item changes.
	HFONT m_hOldItemFont{ nullptr };	// Font used for specific item changes.
	int m_iSelectedItem{};				// Items currently selected.
	int m_iSelectedSubItem{};			// SubItems currently selected.
	mutable int m_iColumnCount{ -1 };	// the number of columns in the listview, a -1 value mean "dont know"
	VectorOfColumnInfo	m_vWidths;		// column widths for dynamic sizing of columns.

	// Ranks for sorting nicknames
	//(qaohvV)~&@%+-
	static constexpr CharRank m_ranks[7] = {
	{ TEXT('~'), 0},	//
	{ TEXT('&'), 1},	// channel owner
	{ TEXT('@'), 2},	// channel ops
	{ TEXT('%'), 3},	// channel half-ops
	{ TEXT('+'), 4},	// channel voices
	{ TEXT('-'), 5},	// not allowed voice in channel?
	{ 0,6 }				// all other characters
	};

	// data for drawing custom margins
	struct dcxListViewMarginSideData
	{
		COLORREF m_clrBkg{ RGB(155, 55, 55) };
		COLORREF m_clrTxt{ RGB(0, 255, 0) };
		COLORREF m_clrBorder{ RGB(100, 100, 0) };
		DcxControl* m_Ctrl{};
		TString m_Text;
	};
	struct dcxListViewMarginData
	{
		dcxListViewMarginSideData m_Left;
		dcxListViewMarginSideData m_Right;
		dcxListViewMarginSideData m_Top;
		dcxListViewMarginSideData m_Bottom;
	};
	dcxListViewMarginData m_MarginData;

	bool m_bHasPBars{ false };			// true if listview has pbars at all, if it does, a slower update is used that checks & moves pbars. (better system needed)
	bool m_bCustomGroups{ false };		// true if custom drawn group headers is enabled.
	bool m_bReserved[2]{ false };
};

#endif // _DCXLISTVIEW_H_
