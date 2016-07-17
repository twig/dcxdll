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

/*!
 * \brief blah
 *
 * blah
 */

class DcxCheck : public DcxControl {

public:
	DcxCheck() = delete;
	DcxCheck(const DcxCheck &) = delete;
	DcxCheck &operator =(const DcxCheck &) = delete;	// No assignments!

	DcxCheck(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles);
	virtual ~DcxCheck( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("check"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::CHECK; }

	void toXml(TiXmlElement *const xml) const override;
	const TString getStyles(void) const override;

protected:
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
};

#endif // _DCXCHECK_H_
