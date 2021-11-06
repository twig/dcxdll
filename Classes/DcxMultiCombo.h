/*
*/
#pragma once
#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/custom/multicombo.h"

class DcxDialog;

class DcxMultiCombo final
	: public DcxControl
{
public:
	DcxMultiCombo() = delete;
	DcxMultiCombo(const DcxMultiCombo&) = delete;
	DcxMultiCombo& operator =(const DcxMultiCombo&) = delete;	// No assignments!
	DcxMultiCombo(DcxMultiCombo&&) = delete;
	DcxMultiCombo& operator =(DcxMultiCombo&&) = delete;

	DcxMultiCombo(const UINT ID, DcxDialog* const p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxMultiCombo() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	inline const TString getType() const final { return TEXT("multicombo"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::MULTICOMBO; }

	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	std::unique_ptr<TiXmlElement> toXml(int blah) const;

	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	int getCount() const noexcept
	{
		return SendMessage(m_Hwnd, MC_WM_GETITEMCOUNT, 0, 0);
	}

	int getCurSel() const noexcept
	{
		return SendMessage(m_Hwnd, MC_WM_GETSEL, 0, 0);
	}

	void setCurSel(int iSel) const noexcept
	{
		SendMessage(m_Hwnd, MC_WM_SETSEL, TRUE, iSel);
	}

	UINT getCurStyle() const noexcept
	{
		return SendMessage(m_Hwnd, MC_WM_GETSTYLE, 0, 0);
	}

	int getDropState() const noexcept
	{
		return gsl::narrow_cast<int>(SendMessage(m_Hwnd, MC_WM_GETDROPSTATE, 0, 0));
	}

	MCOMBO_ITEM getColourItem(int iItem) const noexcept
	{
		MCOMBO_ITEM res;

		SendMessage(m_Hwnd, MC_WM_GETITEM, iItem, (LPARAM)&res);

		return res;
	}

	MCOMBO_ITEM getListBoxItem(int iItem) const noexcept
	{
		MCOMBO_ITEM res;

		SendMessage(m_Hwnd, MC_WM_GETITEM, iItem, (LPARAM)&res);

		return res;
	}

	void resetContent() noexcept
	{
		SendMessage(m_Hwnd, MC_WM_RESETCONTENT, 0, 0);
	}
};

