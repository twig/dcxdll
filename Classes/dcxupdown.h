/*!
 * \file dcxupdown.h
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

#ifndef _DCXUPDOWN_H_
#define _DCXUPDOWN_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

class DcxUpDown final
	: public DcxControl
{
public:
	DcxUpDown() = delete;
	DcxUpDown(const DcxUpDown&) = delete;
	GSL_SUPPRESS(c.128) DcxUpDown& operator =(const DcxUpDown&) = delete;
	DcxUpDown(DcxUpDown&&) = delete;
	GSL_SUPPRESS(c.128) DcxUpDown& operator =(DcxUpDown&&) = delete;

	DcxUpDown(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT* const rc, const TString& styles);
	~DcxUpDown() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString& input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH>& szReturnValue) const final;
	void parseCommandRequest(const TString& input) final;
	dcxWindowStyles parseControlStyles(const TString& tsStyles) final;

	DcxControl* getBuddy() const noexcept;
	HWND setBuddy(const HWND mHwnd) noexcept;
	void setRange32(const int iLow, const int iHigh) noexcept;
	void setRange32(const Dcx::range_t<int> &r) noexcept;
	//std::pair<int,int> getRange32() const noexcept;
	Dcx::range_t<int> getRange32() const noexcept;
	int setBase(const int iBase) noexcept;
	int getBase() const noexcept;
	int setPos32(const INT nPos) noexcept;
	int getPos32(const LPBOOL pfError) const noexcept;

	const TString getStyles(void) const final;
	void toXml(TiXmlElement* const xml) const final;
	TiXmlElement* toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	inline const TString getType() const final { return TEXT("updown"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::UPDOWN; }

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
};

#endif // _DCXUPDOWN_H_
