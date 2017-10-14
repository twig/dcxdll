/*!
 * \file dcxradio.h
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

#ifndef _DCXRADIO_H_
#define _DCXRADIO_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxRadio : public DcxControl {

public:
	DcxRadio() = delete;
	DcxRadio(const DcxRadio &) = delete;
	DcxRadio &operator =(const DcxRadio &) = delete;	// No assignments!

	DcxRadio(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxRadio( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) override;

	inline const TString getType() const override { return TEXT("radio"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::RADIO; }

	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;
	const TString getStyles(void) const override;

protected:

	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);

};

#endif // _DCXRADIO_H_
