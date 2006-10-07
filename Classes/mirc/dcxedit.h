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

#include "../../defines.h"
#include "../dcxcontrol.h"

class DcxDialog;

/*!
* \brief blah
*
* blah
*/

class DcxEdit : public DcxControl {

public:
	DcxEdit(UINT ID, DcxDialog *p_Dialog, RECT *rc, TString &styles);
	DcxEdit(UINT ID, DcxDialog *p_Dialog, HWND mParentHwnd, RECT *rc, TString &styles);
	virtual ~DcxEdit();

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);

	void parseInfoRequest(TString &input, char *szReturnValue);
	void parseCommandRequest(TString &input);
	void parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme);
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);

	inline TString getType() {
		return TString("edit");
	};

protected:
	TString m_tsText; // Edit Text
	TString m_tsCue; // Cue Text
};

#endif // _DCXEDIT_H_