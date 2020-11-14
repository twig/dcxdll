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
#include "Classes\custom\ListHelper.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxList final
	: public DcxControl
	, public DcxListHelper
{

public:
	DcxList() = delete;
	DcxList(const DcxList &) = delete;
	DcxList &operator =(const DcxList &) = delete;	// No assignments!
	DcxList(DcxList &&) = delete;
	DcxList &operator =(DcxList &&) = delete;

	DcxList(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxList( );

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("list"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::LIST; }

	inline const UINT &getDragListId() const noexcept { return m_iDragList; };

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	void DrawDragLine(const int location) noexcept;
	bool matchItemText(const int nItem, const TString &search, const DcxSearchTypes &SearchType) const;

	//static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart, int *iEnd);
	//static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);

	//void StrLenToExtent(int *nLineExtent);

	void UpdateHorizExtent();
	void UpdateHorizExtent(const int nPos);

	UINT m_iDragList{};
	int m_iLastDrawnLine{};
	bool m_bUseDrawInsert{ true };
	bool m_bReserved[3]{ false };
};

#endif // _DCXLIST_H_
