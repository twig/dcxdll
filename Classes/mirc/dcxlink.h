/*!
 * \file dcxlink.h
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

#ifndef _DCXLINK_H_
#define _DCXLINK_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxLink : public DcxControl {

public:
	DcxLink() = delete;
	DcxLink(const DcxLink &) = delete;
	DcxLink &operator =(const DcxLink &) = delete;	// No assignments!

	DcxLink( const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxLink( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("link"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::LINK; }

	void toXml(TiXmlElement *const xml) const override;

protected:

	HICON m_hIcon; //!< Link Icon

	COLORREF m_aColors[4]; //!< Link Colors 0 = normal, 1 = hot, 2 = visited, 3 = disabled

	bool m_bHover; //!< Hover State
	bool m_bVisited; //!< Visited State
	bool m_bReserved[2];
	BOOL m_bTracking; //!< Mouse Tracking Var

	//static TString UriEncode(const TString &sSrc);
	//static TString UriDecode(const TString &sSrc);
	void DrawClientArea(HDC hdc);
};

#endif // _DCXLINK_H_
