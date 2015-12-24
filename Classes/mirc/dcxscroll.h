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

class DcxScroll : public DcxControl {

public:
	DcxScroll() = delete;
	DcxScroll(const DcxScroll &) = delete;
	DcxScroll &operator =(const DcxScroll &) = delete;	// No assignments!

	DcxScroll(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxScroll( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("scroll"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::SCROLL; }

	const TString getStyles(void) const override;

protected:

	INT m_nPage; //!< Scroll Page Size
	INT m_nLine; //!< Scroll Line Size

};

#endif // _DCXSCROLL_H_
