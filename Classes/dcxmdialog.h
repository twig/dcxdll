/*!
 * \file dcxmdialog.h
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

#ifndef _DCXMDIALOG_H_
#define _DCXMDIALOG_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;


/*!
 * \brief blah
 *
 * blah
 */

class DcxMDialog : public DcxControl {

public:
	DcxMDialog() = delete;
	DcxMDialog(const DcxMDialog &) = delete;
	DcxMDialog &operator =(const DcxMDialog &) = delete;	// No assignments!

	DcxMDialog(const HWND cHwnd, const HWND pHwnd, const UINT ID, DcxDialog *const p_Dialog, const RECT *const rc, const TString & styles );
	virtual ~DcxMDialog( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	inline const TString getType() const override { return TEXT("dialog"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::DIALOG; }

protected:

	LONG m_OrigStyles;      //!< Dialog Original Styles
	LONG m_OrigExStyles;    //!< Dialog Original Extended Styles
	HWND m_OrigParentHwnd;  //!< Dialog Original Parent Handle
	UINT m_OrigID;          //!< Dialog Original Control ID
	TString m_OrigName;     //!< Dialog Original Name
	bool m_DeleteByDestroy; //!< is true if control is deleted because docked dialog is destroyed (won't repaint dialog on delete)
};

#endif // _DCXMDIALOG_H_
