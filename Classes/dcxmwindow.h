/*!
 * \file dcxmwindow.h
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

#ifndef _DCXMWINDOW_H_
#define _DCXMWINDOW_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxMWindow : public DcxControl {

public:
	DcxMWindow() = delete;
	DcxMWindow(const DcxMWindow &) = delete;
	DcxMWindow &operator =(const DcxMWindow &) = delete;	// No assignments!

	DcxMWindow( const HWND cHwnd, const HWND pHwnd, const UINT ID, DcxDialog *const p_Dialog, const RECT *const rc, const TString & styles );
	virtual ~DcxMWindow( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("window"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::WINDOW; }

protected:

	LONG m_OrigStyles;      //!< Window Original Styles
	LONG m_OrigExStyles;    //!< Window Original Extended Styles
	HWND m_OrigParentHwnd;  //!< Window Original Parent Handle
	UINT m_OrigID;          //!< Window Original Control ID
	TString m_OrigName;     //!< Dialog Original Name
};

#endif // _DCXMWINDOW_H_
