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

class DcxMWindow final
	: public DcxControl
{
public:
	DcxMWindow() = delete;
	DcxMWindow(const DcxMWindow &) = delete;
	DcxMWindow &operator =(const DcxMWindow &) = delete;	// No assignments!
	DcxMWindow(DcxMWindow &&) = delete;
	DcxMWindow &operator =(DcxMWindow &&) = delete;

	DcxMWindow( const HWND cHwnd, const HWND pHwnd, const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const RECT *const rc, const TString & styles );
	~DcxMWindow( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) noexcept final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	inline const TString getType() const final { return TEXT("window"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::WINDOW; }

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	WindowStyle m_OrigStyles{};      //!< Window Original Styles
	WindowExStyle m_OrigExStyles{};    //!< Window Original Extended Styles
	HWND m_OrigParentHwnd{ nullptr };  //!< Window Original Parent Handle
	UINT m_OrigID{};          //!< Window Original Control ID
	TString m_OrigName;     //!< Dialog Original Name
};

#endif // _DCXMWINDOW_H_
