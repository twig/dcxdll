/*!
 * \file dcxupdown.h
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

#ifndef _DCXUPDOWN_H_
#define _DCXUPDOWN_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxUpDown
	: public DcxControl
{
public:
	DcxUpDown() = delete;
	DcxUpDown(const DcxUpDown &) = delete;
	DcxUpDown &operator =(const DcxUpDown &) = delete;
	DcxUpDown(DcxUpDown &&) = delete;
	DcxUpDown &operator =(DcxUpDown &&) = delete;

	DcxUpDown(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxUpDown( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) final;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) final;

	LRESULT setBuddy( const HWND mHwnd ) noexcept;
	LRESULT setRange32( const int iLow, const int iHigh ) noexcept;
	std::pair<int,int> getRange32() const noexcept;
	LRESULT setBase( const int iBase ) noexcept;
	LRESULT getBase( ) const noexcept;
	LRESULT setPos32( const INT nPos ) noexcept;
	LRESULT getPos32( const LPBOOL pfError ) const noexcept;

	const TString getStyles(void) const final;

	inline const TString getType() const final { return TEXT("updown"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::UPDOWN; }

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
};

#endif // _DCXUPDOWN_H_
