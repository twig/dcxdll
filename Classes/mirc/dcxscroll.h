/*!
 * \file dcxscroll.h
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

#ifndef _DCXSCROLL_H_
#define _DCXSCROLL_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxScroll final
	: public DcxControl
{
public:
	DcxScroll() = delete;
	DcxScroll(const DcxScroll &) = delete;
	DcxScroll &operator =(const DcxScroll &) = delete;	// No assignments!
	DcxScroll(DcxScroll &&) = delete;
	DcxScroll &operator =(DcxScroll &&) = delete;

	DcxScroll(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxScroll( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("scroll"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::SCROLL; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

private:
	INT m_nPage{ 5 }; //!< Scroll Page Size
	INT m_nLine{ 1 }; //!< Scroll Line Size
};

#endif // _DCXSCROLL_H_
