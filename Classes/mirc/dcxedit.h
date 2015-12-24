/*!
* \file dcxedit.h
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

#ifndef _DCXEDIT_H_
#define _DCXEDIT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
* \brief blah
*
* blah
*/

class DcxEdit : public DcxControl {

public:
	DcxEdit() = delete;
	DcxEdit(const DcxEdit &) = delete;
	DcxEdit &operator =(const DcxEdit &) = delete;	// No assignments!

	DcxEdit(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString &styles);
	virtual ~DcxEdit();

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("edit"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::EDIT; }

	const TString getStyles(void) const override;
	void toXml(TiXmlElement *const xml) const override;

protected:
	TString m_tsText; // Edit Text
	TString m_tsCue; // Cue Text
	TCHAR	m_PassChar;	// Password char

	bool m_bIgnoreRepeat;
	bool m_bReserved;
};

#endif // _DCXEDIT_H_
