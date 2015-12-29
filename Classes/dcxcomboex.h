/*!
 * \file dcxcomboex.h
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

#ifndef _DCXCOMBOEX_H_
#define _DCXCOMBOEX_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

enum ComboEx_SearchTypes : UINT {
	CBEXSEARCH_W = 0x01,	//!< ComboEx WildCard Search
	CBEXSEARCH_R,			//!< ComboEx Regex Search
	CBEXSEARCH_E			//!< ComboEx Exact Match Search
};

//#define CBEXSEARCH_W 0x01 //!< ComboEx WildCard Search
//#define CBEXSEARCH_R 0x02 //!< ComboEx Regex Search

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXCOMBOEXEDIT {

  WNDPROC OldProc; //!< Subclassed Window Procedure of Combo
  HWND cHwnd;      //!< Parent ComboEx Handle
  HWND pHwnd;      //!< Dialog Handle

} DCXCOMBOEXEDIT, *LPDCXCOMBOEXEDIT;


typedef struct tagDCXCBITEM {
	TString tsMark;		// Marked text
} DCXCBITEM,*LPDCXCBITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxComboEx : public DcxControl {

public:
	DcxComboEx() = delete;
	DcxComboEx(const DcxComboEx &) = delete;
	DcxComboEx &operator =(const DcxComboEx &) = delete;	// No assignments!

	DcxComboEx(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxComboEx( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;

	void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const override;
	void parseCommandRequest(const TString & input) override;
	void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) override;

	HIMAGELIST getImageList( ) const;
	void setImageList( HIMAGELIST himl );
	static HIMAGELIST createImageList( );

	bool matchItemText( const int nItem, const TString * search, const ComboEx_SearchTypes SearchType ) const;
	static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart, int *iEnd);

	LRESULT insertItem( const PCOMBOBOXEXITEM lpcCBItem );
	LRESULT getItem( PCOMBOBOXEXITEM lpcCBItem ) const;
	LRESULT getEditControl( ) const;
	LRESULT deleteItem( const int iIndex );
	LRESULT setCurSel( const int iIndex );
	LRESULT getCurSel( ) const;
	LRESULT getLBText( const int iIndex, LPSTR lps );
	LRESULT resetContent( );
	LRESULT getCount( ) const;
	LRESULT limitText( const int iLimit );

	static LRESULT CALLBACK ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	inline const TString getType() const override { return TEXT("comboex"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::COMBOEX; }

	const TString getStyles(void) const override;

protected:

	HWND				m_EditHwnd;	//!< Combo's Edit Control Handle
	HWND				m_hComboHwnd;	//!< Combo's handle
	TString				m_tsSelected;
private:
	DCXCOMBOEXEDIT		m_exEdit;
};

#endif // _DCXCOMBOEX_H_
