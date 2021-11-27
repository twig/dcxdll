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
* © ScriptsDB.org - 2006
*/

#ifndef _DCXEDIT_H_
#define _DCXEDIT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
* \brief blah
*
* blah
*/

class DcxEdit final
	: public DcxControl
{
public:
	DcxEdit() = delete;
	DcxEdit(const DcxEdit&) = delete;
	DcxEdit& operator =(const DcxEdit&) = delete;	// No assignments!
	DcxEdit(DcxEdit&&) = delete;
	DcxEdit& operator =(DcxEdit&&) = delete;

	DcxEdit(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
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

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

private:
	TString m_tsText; // Edit Text
	TString m_tsCue; // Cue Text
	TCHAR	m_PassChar{};	// Password char

	bool m_bIgnoreRepeat{ false };

	bool m_bShowLineNumbers{ false };
	COLORREF m_clrGutter_selbkg{ RGB(0xFF, 0xf0, 0xff) };
	COLORREF m_clrGutter_bkg{ RGB(0xFF, 0xf0, 0xff) };
	COLORREF m_clrGutter_txt{ RGB(0xAF, 0xAF, 0xAF) };
	COLORREF m_clrGutter_seltxt{ RGB(0x0F, 0x0F, 0x0F) };
	COLORREF m_clrGutter_border{ RGB(0xFF, 0xFF, 0xFF) };

#define WM_DRAW_NUMBERS (WM_USER + 1000)
#define DCX_EDIT_GUTTER_WIDTH 35

	void DrawGutter();
	void DrawGutter(HDC hdc);
	RECT getFmtRect() const noexcept
	{
		RECT rc{};
		Edit_GetRect(m_Hwnd, &rc);
		return rc;
	}
	void setFmtRect(bool bReset = false) noexcept
	{
		//if (!m_bShowLineNumbers)
		//	return;

		RECT rcClient{};
		GetClientRect(m_Hwnd, &rcClient);
		if (bReset)
		{
			Edit_SetRect(m_Hwnd, &rcClient);
		}
		else {
			rcClient.left += DCX_EDIT_GUTTER_WIDTH;
			RECT rcFmt = getFmtRect();
			//if (!EqualRect(&rcClient, &rcFmt))
			if (rcFmt.left <= 5)
			{
				rcFmt = rcClient;
				Edit_SetRect(m_Hwnd, &rcFmt);
			}
		}
	}
	RECT getGutterRect() const noexcept
	{
		const RECT rcFmt{ getFmtRect() };
		//RECT rcClient{};
		//GetClientRect(m_Hwnd, &rcClient);
		return { 0,0,rcFmt.left,rcFmt.bottom };
	}

	Dcx::range_t<DWORD> GetVisibleRange() noexcept;
	DWORD GetCaretPos() noexcept;
	DWORD GetCaretLine() noexcept;
	void setCaretPos(DWORD pos) noexcept;
};

#endif // _DCXEDIT_H_
