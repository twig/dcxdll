/*!
 * \file dcxcolorcombo.h
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

#ifndef _DCXCOLORCOMBO_H_
#define _DCXCOLORCOMBO_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

struct DCXCCOMBOITEM {

  COLORREF clrItem;     //!< Item Color
  TString tsItemText;   //!< Item Text

  DCXCCOMBOITEM() noexcept : DCXCCOMBOITEM(CLR_INVALID) {}
  explicit DCXCCOMBOITEM(const COLORREF &clr) noexcept : clrItem(clr) {}
};
using LPDCXCCOMBOITEM = DCXCCOMBOITEM *;

// the height for each item
#define DCX_COLORCOMBO_ITEM_HEIGHT 16

/*!
 * \brief blah
 *
 * blah
 */

class DcxColorCombo
	: public DcxControl
{
public:
	DcxColorCombo() = delete;
	DcxColorCombo(const DcxColorCombo &) = delete;
	DcxColorCombo &operator =(const DcxColorCombo &) = delete;	// No assignments!
	DcxColorCombo(DcxColorCombo &&) = delete;
	DcxColorCombo &operator =(DcxColorCombo &&) = delete;

	DcxColorCombo(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxColorCombo( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) final;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) final;

	void setmIRCPalette( );

	int insertItem( const int nPos, const DCXCCOMBOITEM *dci) noexcept;
	int getCount( ) const noexcept;
	int setCurSel( const int nPos ) noexcept;
	LPDCXCCOMBOITEM getItemData( const int nItem ) const noexcept;
	int getCurSel( ) const noexcept;
	int deleteItem( const int nItem ) noexcept;
	int resetContent( ) noexcept;

	inline const TString getType() const final { return TEXT("colorcombo"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::COLORCOMBO; }

	using DcxControl::toXml;
	using DcxControl::getStyles;

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;
};

#endif // _DCXCOLORCOMBO_H_
