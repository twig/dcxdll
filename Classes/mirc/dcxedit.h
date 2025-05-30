/*!
* \file dcxedit.h
* \brief blah
*
* blah
*
* \author David Legault ( clickhere at scriptsdb dot org )
* \version 1.0
*
* \b Revisions
*
* � ScriptsDB.org - 2006
*/

#ifndef _DCXEDIT_H_
#define _DCXEDIT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/custom/SearchHelper.h"

class DcxDialog;

class DcxEdit final
	: public DcxControl
	, virtual public DcxSearchHelper
{
public:
	DcxEdit() = delete;
	DcxEdit(const DcxEdit&) = delete;
	GSL_SUPPRESS(c.128) DcxEdit& operator =(const DcxEdit&) = delete;	// No assignments!
	DcxEdit(DcxEdit&&) = delete;
	GSL_SUPPRESS(c.128) DcxEdit& operator =(DcxEdit&&) = delete;

	DcxEdit(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxEdit() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	//TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("edit"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::EDIT; }

	const TString getStyles(void) const final;
	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

	TString getLine(int nLine) const;

	static DWORD parseExEditStyles(const TString& tsStyles);

private:
	TString m_tsText; // Edit Text
	TString m_tsCue; // Cue Text
	TCHAR	m_PassChar{};	// Password char

	bool m_bCueFocused{ false };
	bool m_bIgnoreRepeat{ false };
	bool m_bIgnoreUndo{ false };

#define WM_DRAW_NUMBERS (WM_USER + 1000)
#define DCX_EDIT_GUTTER_WIDTH 35

	bool m_bShowLineNumbers{ false };
	bool m_bLockGutter{ true };
	bool m_bDraggingGutter{ false };
	UINT m_GutterWidth{ DCX_EDIT_GUTTER_WIDTH };
	COLORREF m_clrGutter_selbkg{ RGB(0xFF, 0xf0, 0xff) };
	COLORREF m_clrGutter_bkg{ RGB(0xFF, 0xf0, 0xff) };
	COLORREF m_clrGutter_txt{ RGB(0xAF, 0xAF, 0xAF) };
	COLORREF m_clrGutter_seltxt{ RGB(0x0F, 0x0F, 0x0F) };
	COLORREF m_clrGutter_border{ RGB(0xFF, 0xFF, 0xFF) };

	void DrawClientRect(HDC hdc, unsigned int uMsg, LPARAM lParam);
	void DrawGutter();
	void DrawGutter(HDC hdc);
	[[nodiscard]] RECT getFmtRect() const noexcept
	{
		RECT rc{};
		if (m_Hwnd)
			Dcx::dcxEdit_GetRect(m_Hwnd, &rc);
		return rc;
	}
	void resetFmtRect() noexcept
	{
		if (m_Hwnd)
			Dcx::dcxEdit_SetRect(m_Hwnd, nullptr);
	}

	//void setFmtRect(bool bReset = false) noexcept
	//{
	//	if (bReset)
	//		resetFmtRect();
	//	else if (getFmtRect().left <= 5)
	//	{
	//		RECT rcClient{};
	//		GetClientRect(m_Hwnd, &rcClient);
	//
	//		rcClient.left += DCX_EDIT_GUTTER_WIDTH;
	//		Edit_SetRect(m_Hwnd, &rcClient);
	//	}
	//}

	void setFmtRect(bool bRedraw = true) noexcept
	{
		if (!m_Hwnd)
			return;

		if (gsl::narrow_cast<UINT>(getFmtRect().left) != (m_GutterWidth + 1)) // Edit ctrl is +1!!
		{
			// edit ctrl will shrink the fmt rect for some reason, so use client rect each time.
			RECT rcClient{};
			GetClientRect(m_Hwnd, &rcClient);

			rcClient.left += m_GutterWidth;
			if (bRedraw)
				Dcx::dcxEdit_SetRect(m_Hwnd, &rcClient);
			else
				Dcx::dcxEdit_SetRectNoPaint(m_Hwnd, &rcClient);
		}
	}
	[[nodiscard]] RECT getGutterRect() const noexcept
	{
		const RECT rcFmt{ getFmtRect() };
		return { 0,0,rcFmt.left,rcFmt.bottom };
	}
	[[nodiscard]] bool IsCursorOnGutter() const noexcept
	{
		if (!m_bShowLineNumbers)
			return false;

		const auto rc = getGutterRect();
		const Dcx::dcxCursorPos pt(m_Hwnd);

		return PtInRect(&rc, pt);
	}
	[[nodiscard]] bool IsCursorOnGutterBorder() const noexcept
	{
		if (!m_bShowLineNumbers)
			return false;

		auto rc = getFmtRect();
		constexpr int iBorderWidth = 5;
		const Dcx::dcxCursorPos pt(m_Hwnd);
		rc.right = rc.left;
		rc.left -= iBorderWidth;

		return PtInRect(&rc, pt);
	}

	[[nodiscard]] Dcx::range_t<DWORD> GetVisibleRange() const noexcept;
	[[nodiscard]] DWORD GetCaretPos() const noexcept;
	[[nodiscard]] DWORD GetCaretLine() const noexcept;
	void setCaretPos(DWORD pos) noexcept;

	[[nodiscard]] DWORD GetLineIndex(DWORD iLine) const noexcept
	{
		//if (!m_Hwnd)
		//	return 0;
		//
		//return gsl::narrow_cast<DWORD>(SendMessage(m_Hwnd, EM_LINEINDEX, gsl::narrow_cast<WPARAM>(iLine), 0));

		return Dcx::dcxEdit_GetLineIndex(m_Hwnd, iLine);
	}
	[[nodiscard]] POINTL GetPosFromChar(DWORD iLineChar) const noexcept
	{
		//POINTL pl{};
		//if (m_Hwnd)
		//	SendMessage(m_Hwnd, EM_POSFROMCHAR, reinterpret_cast<WPARAM>(&pl), gsl::narrow_cast<LPARAM>(iLineChar));
		//return pl;

		return Dcx::dcxEdit_GetPosFromChar(m_Hwnd, iLineChar);
	}

	//struct Edit_SearchResults
	//{
	//	bool m_MatchFound{};
	//	size_t m_nStart{};
	//	size_t m_nEnd{};
	//};

	//[[nodiscard]] const DcxSearchTypes CharToSearchType(const TCHAR& cType) const noexcept;
	//[[nodiscard]] std::optional<Edit_SearchResults> matchText(const UINT nLine, const UINT nSubChar, const TString& search, const DcxSearchTypes& SearchType) const;
	//[[nodiscard]] std::optional<Edit_SearchResults> matchText(const UINT nChar, const TString& search, const DcxSearchTypes& SearchType) const;
	//[[nodiscard]] std::optional<Edit_SearchResults> matchText(const UINT nChar, const dcxSearchData& srch_data) const;

};

#endif // _DCXEDIT_H_
