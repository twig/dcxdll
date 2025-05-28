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


class DcxMDialog final
	: public DcxControl
{
public:
	DcxMDialog() = delete;
	DcxMDialog(const DcxMDialog &) = delete;
	GSL_SUPPRESS(c.128) DcxMDialog &operator =(const DcxMDialog &) = delete;	// No assignments!
	DcxMDialog(DcxMDialog &&) = delete;
	GSL_SUPPRESS(c.128) DcxMDialog &operator =(DcxMDialog &&) = delete;

	DcxMDialog(const HWND cHwnd, const HWND pHwnd, const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const RECT *const rc, const TString & styles );
	~DcxMDialog( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("dialog"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::DIALOG; }

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	WindowStyle m_OrigStyles{};      //!< Dialog Original Styles
	WindowExStyle m_OrigExStyles{};    //!< Dialog Original Extended Styles
	HWND m_OrigParentHwnd{ nullptr };  //!< Dialog Original Parent Handle
	UINT m_OrigID{};          //!< Dialog Original Control ID
	TString m_OrigName;     //!< Dialog Original Name
	bool m_DeleteByDestroy{ false }; //!< is true if control is deleted because docked dialog is destroyed (won't repaint dialog on delete)
};

#endif // _DCXMDIALOG_H_
