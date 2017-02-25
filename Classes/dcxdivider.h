/*!
 * \file dcxdivider.h
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

#ifndef _DCXDIVIDER_H_
#define _DCXDIVIDER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/custom/divider.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxDivider : public DcxControl {

public:
	DcxDivider() = delete;
	DcxDivider(const DcxDivider &) = delete;
	DcxDivider &operator =(const DcxDivider &) = delete;	// No assignments!

	DcxDivider(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxDivider( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	LRESULT setPane( const UINT iPaneId, LPDVPANEINFO lpdvpi );
	LRESULT setDivPos( const UINT iDivPos );

	inline const TString getType() const override { return TEXT("divider"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::DIVIDER; }

	const TString getStyles(void) const override;
	void toXml(TiXmlElement *const xml) const override;
	TiXmlElement * toXml(void) const override;

protected:
	HWND m_leftPane;
	HWND m_rightPane;

};

#endif // _DCXDIVIDER_H_
