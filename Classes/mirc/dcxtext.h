/*!
 * \file dcxtext.h
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

#ifndef _DCXTEXT_H_
#define _DCXTEXT_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
//#include <ColourString.h>

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxText : public DcxControl {

public:
	DcxText() = delete;
	DcxText(const DcxText &) = delete;
	DcxText &operator =(const DcxText &) = delete;	// No assignments!

	DcxText(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxText( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("text"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::TEXT; }

	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;
	const TString getStyles(void) const override;

//protected:
private:
	TString m_tsText;	// Edit Text
	UINT m_uiStyle;		// text style
	void DrawClientArea(HDC hdc);
};

#endif // _DCXTEXT_H_
