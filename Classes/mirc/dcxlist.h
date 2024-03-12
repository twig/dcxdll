/*!
 * \file dcxlist.h
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

#ifndef _DCXLIST_H_
#define _DCXLIST_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes\custom\SearchHelper.h"

// Test fix for drawing when transparent.
#define USE_FIX_01 1
// this enables a fix for listboxes with >64k items
#define USE_FIX_SCROLL 1

class DcxDialog;

namespace Dcx {
	/// <summary>
	/// Add a string to the listbox
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="nPos"></param>
	/// <param name="txt"></param>
	/// <returns></returns>
	inline int dcxListBox_InsertString(HWND hwnd, int nPos, const TString& txt) noexcept
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
	inline bool dcxListBox_GetItemRect(HWND hwnd, int nPos, LPRECT prc) noexcept
	{
		return (ListBox_GetItemRect(hwnd, nPos, prc) != LB_ERR);
	}

	/// <summary>
	/// Get an items text.
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="i"></param>
	/// <returns></returns>
	TString dcxListBox_GetText(HWND hwnd, int i);

	/// <summary>
	/// Get the item at a specific point.
	/// </summary>
	/// <param name="hListbox"></param>
	/// <param name="pt"></param>
	/// <returns>Item number or -1 if no item</returns>
	int dcxListBox_GetPointItem(HWND hListbox, POINT pt) noexcept;

	/// <summary>
	/// Get the item the mouse is currently over
	/// </summary>
	/// <param name="hListbox"></param>
	/// <returns></returns>
	int dcxListBox_GetHoverItem(HWND hListbox) noexcept;
}

/*!
 * \brief blah
 *
 * blah
 */

class DcxList final
	: public DcxControl
	, virtual public DcxSearchHelper
{

public:
	DcxList() = delete;
	DcxList(const DcxList &) = delete;
	GSL_SUPPRESS(c.128) DcxList &operator =(const DcxList &) = delete;	// No assignments!
	DcxList(DcxList &&) = delete;
	GSL_SUPPRESS(c.128) DcxList &operator =(DcxList &&) = delete;

	DcxList(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxList( );

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("list"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::LIST; }

	inline const UINT &getDragListId() const noexcept { return m_iDragList; };

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	void DrawDragLine(const int location) noexcept;
	bool matchItemText(const int nItem, const TString &search, const DcxSearchTypes &SearchType) const;
	bool matchItemText(const int nItem, const dcxSearchData &srch_data) const;

	void UpdateHorizExtent();
	void UpdateHorizExtent(const int nPos);

	/// <summary>
	/// Add items to the list.
	/// </summary>
	/// <param name="nPos">- Position at which to add this item.</param>
	/// <param name="tsFlags">- Flags to define what tsArgs means. MUST start with a +</param>
	/// <param name="tsArgs">- The item data to add.</param>
	/// <returns>The next position to add an item at.</returns>
	int addItems(int nPos, const TString &tsFlags, const TString &tsArgs);

	UINT m_iDragList{};
	int m_iLastDrawnLine{};
	bool m_bUseDrawInsert{ true };
	bool m_bReserved[3]{ false };
};

#endif // _DCXLIST_H_
