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

class DcxDateTime : public DcxControl {
public:
	DcxDateTime() = delete;
	DcxDateTime(const DcxDateTime &) = delete;
	DcxDateTime &operator =(const DcxDateTime &) = delete;	// No assignments!

	DcxDateTime(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles);
	virtual ~DcxDateTime();

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("datetime"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::DATETIME; }

	void toXml(TiXmlElement *const xml) const override;
	const TString getStyles(void) const override;
};

#endif // _DCXDATETIME_H_
