/*
*/
#pragma once
#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/custom/multicombo.h"
#include "Classes\custom\SearchHelper.h"

class DcxDialog;

class DcxMultiCombo final
	: public DcxControl
	, virtual public DcxSearchHelper
{
public:
	DcxMultiCombo() = delete;
	DcxMultiCombo(const DcxMultiCombo&) = delete;
	GSL_SUPPRESS(c.128) DcxMultiCombo& operator =(const DcxMultiCombo&) = delete;	// No assignments!
	DcxMultiCombo(DcxMultiCombo&&) = delete;
	GSL_SUPPRESS(c.128) DcxMultiCombo& operator =(DcxMultiCombo&&) = delete;

	DcxMultiCombo(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
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
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

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

		SendMessage(m_Hwnd, MC_WM_GETITEM, iItem, reinterpret_cast<LPARAM>(std::addressof(res)));

		return res;
	}

	MCOMBO_ITEM getListBoxItem(int iItem) const noexcept
	{
		MCOMBO_ITEM res;

		SendMessage(m_Hwnd, MC_WM_GETITEM, iItem, reinterpret_cast<LPARAM>(std::addressof(res)));

		return res;
	}

	void resetContent() noexcept
	{
		SendMessage(m_Hwnd, MC_WM_RESETCONTENT, 0, 0);
	}

	DcxControl* getChild() const noexcept
	{
		if (auto hChild = reinterpret_cast<HWND>(SendMessage(m_Hwnd, MC_WM_GETCHILD, 0, 0)); hChild)
		{
			if (const auto c_this = Dcx::dcxGetProp<DcxControl*>(hChild, TEXT("dcx_cthis")); c_this)
				return c_this;
		}
		return nullptr;
	}

	void addChild(HWND hwnd) noexcept
	{
		SendMessage(m_Hwnd, MC_WM_ADDCHILD, 0, reinterpret_cast<LPARAM>(hwnd));
	}

	void addItem(COLORREF clr, const TString &tsText);

	bool matchItemText(const int nItem, const dcxSearchData& srch_data) const;

};

