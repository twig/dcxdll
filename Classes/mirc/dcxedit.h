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
	DcxEdit(const UINT ID, DcxDialog *p_Dialog, const HWND mParentHwnd, const RECT *rc, TString &styles);
	virtual ~DcxEdit();

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bParsed);

	void parseInfoRequest(TString &input, TCHAR *szReturnValue);
	void parseCommandRequest(TString &input);
	void parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme);

	inline TString getType() { return TString(TEXT("edit")); };
	TString getStyles(void);
	void toXml(TiXmlElement * xml);

	static void registerClass(void);

protected:
	TString m_tsText; // Edit Text
	TString m_tsCue; // Cue Text

	BOOL m_bIgnoreRepeat;
};

#endif // _DCXEDIT_H_
