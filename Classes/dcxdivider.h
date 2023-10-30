/*!
 * \file dcxdivider.h
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

#ifndef _DCXDIVIDER_H_
#define _DCXDIVIDER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/custom/divider.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDivider final
	: public DcxControl
{
public:
	DcxDivider() = delete;
	DcxDivider(const DcxDivider &) = delete;
	GSL_SUPPRESS(c.128) DcxDivider &operator =(const DcxDivider &) = delete;	// No assignments!
	DcxDivider(DcxDivider &&) = delete;
	GSL_SUPPRESS(c.128) DcxDivider &operator =(DcxDivider &&) = delete;

	DcxDivider(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxDivider( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	LRESULT setPane( const UINT iPaneId, const LPDVPANEINFO lpdvpi ) noexcept;
	LRESULT setDivPos( const UINT iDivPos ) noexcept;

	BOOL setBarColor(COLORREF clrUnselected, COLORREF clrSelected) noexcept;
	BOOL setBarWidth(UINT nWidth) noexcept;

	UINT getDivPos() const noexcept;
	UINT getBarWidth() const noexcept;
	std::pair<COLORREF, COLORREF> getBarColours() const noexcept;

	inline const TString getType() const final { return TEXT("divider"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::DIVIDER; }

	const TString getStyles(void) const final;
	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
};

#endif // _DCXDIVIDER_H_
