/*!
 * \file dcxdatetime.h
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

#ifndef _DCXDATETIME_H_
#define _DCXDATETIME_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDateTime final
	: public DcxControl
{
public:
	DcxDateTime() = delete;
	DcxDateTime(const DcxDateTime &) = delete;
	GSL_SUPPRESS(c.128) DcxDateTime &operator =(const DcxDateTime &) = delete;	// No assignments!
	DcxDateTime(DcxDateTime &&) = delete;
	GSL_SUPPRESS(c.128) DcxDateTime &operator =(DcxDateTime &&) = delete;

	DcxDateTime(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles);
	~DcxDateTime() noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("datetime"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::DATETIME; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
};

#endif // _DCXDATETIME_H_
