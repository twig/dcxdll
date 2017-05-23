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

  DCXCCOMBOITEM() : clrItem(CLR_INVALID), tsItemText() {}
};
using LPDCXCCOMBOITEM = DCXCCOMBOITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxColorCombo : public DcxControl {

public:
	DcxColorCombo() = delete;
	DcxColorCombo(const DcxColorCombo &) = delete;
	DcxColorCombo &operator =(const DcxColorCombo &) = delete;	// No assignments!

	DcxColorCombo(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxColorCombo( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	void setmIRCPalette( );

	LRESULT insertItem( const int nPos, const LPARAM lParam );
	LRESULT getCount( ) const;
	LRESULT setCurSel( const int nPos );
	LRESULT getItemData( const int nItem ) const;
	LRESULT getCurSel( ) const;
	LRESULT deleteItem( const int nItem );
	LRESULT resetContent( );

	inline const TString getType() const override { return TEXT("colorcombo"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::COLORCOMBO; }

	using DcxControl::toXml;
	using DcxControl::getStyles;
};

#endif // _DCXCOLORCOMBO_H_
