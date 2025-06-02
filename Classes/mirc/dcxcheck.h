/*!
 * \file dcxcheck.h
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

#ifndef _DCXCHECK_H_
#define _DCXCHECK_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

class DcxCheck final
	: public DcxControl
{
public:
	DcxCheck() = delete;
	DcxCheck(const DcxCheck &) = delete;
	GSL_SUPPRESS(c.128) DcxCheck &operator =(const DcxCheck &) = delete;	// No assignments!
	DcxCheck(DcxCheck &&) = delete;
	GSL_SUPPRESS(c.128) DcxCheck &operator =(DcxCheck &&) = delete;

	DcxCheck(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles);
	~DcxCheck( );

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	TString parseInfoRequest(const TString& input) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("check"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::CHECK; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	std::unique_ptr<TiXmlElement> toXml(int blah) const;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	const TString getStyles(void) const final;

	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure

private:
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
	void ctrlDrawBackground(HTHEME hTheme, int iState, HDC hdcPaint, LPRECT rc, bool bFocus);
	void ctrlDrawCheckBox(HTHEME hTheme, int iState, HDC hdcPaint, LPRECT rc, bool bFocus, bool b3State) noexcept;
	void ctrlDrawCheckText(HTHEME hTheme, int iState, HDC hdcPaint, LPRECT rc, bool bFocus);

	bool m_bCustom{};
	bool m_bRoundedCheckBox{};
	bool m_bTracking{ false };	//!< Tracking Mouse Event State
	bool m_bHover{ false };		//!< Hovering State
	bool m_bHot{ false };		//!< Hot State
	bool m_bSelected{ false };	//!< Selected State

	clrCheckBox m_Colours;
};

#endif // _DCXCHECK_H_
