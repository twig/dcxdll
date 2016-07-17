/*!
 * \file dcxline.h
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

#ifndef _DCXLINE_H_
#define _DCXLINE_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxLine : public DcxControl {

public:
	DcxLine() = delete;
	DcxLine(const DcxLine &) = delete;
	DcxLine &operator =(const DcxLine &) = delete;	// No assignments!

	DcxLine( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxLine( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("line"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::LINE; }

	void toXml(TiXmlElement *const xml) const override;
	const TString getStyles(void) const override;

protected:

	void DrawClientArea(HDC hdc);

	TString m_sText;
	bool m_bVertical;
};

#endif // _DCXLINE_H_
