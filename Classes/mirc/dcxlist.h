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

class DcxList
	: public DcxControl
	, public DcxListHelper
{

public:
	DcxList() = delete;
	DcxList(const DcxList &) = delete;
	DcxList &operator =(const DcxList &) = delete;	// No assignments!

	DcxList(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxList( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("list"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::LIST; }

	inline const UINT &getDragListId() const noexcept { return m_iDragList; };
	const TString getStyles(void) const override;

protected:

	void DrawDragLine(const int location);
	bool matchItemText(const int nItem, const TString &search, const DcxSearchTypes &SearchType) const;

	//static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart, int *iEnd);
	//static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);

	//void StrLenToExtent(int *nLineExtent);

	void UpdateHorizExtent();
	void UpdateHorizExtent(const int nPos);

	UINT m_iDragList;
	int m_iLastDrawnLine;
	bool m_bUseDrawInsert;
	bool m_bReserved[3];
};

#endif // _DCXLIST_H_
