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
#include "Classes\custom\ListHelper.h"

class DcxDialog;

//#define CBEXSEARCH_W 0x01 //!< ComboEx WildCard Search
//#define CBEXSEARCH_R 0x02 //!< ComboEx Regex Search

/*!
 * \brief blah
 *
 * blah
 */

struct DCXCOMBOEXEDIT {

	WNDPROC OldProc{ nullptr }; //!< Subclassed Window Procedure of Combo
	HWND cHwnd{ nullptr };      //!< Parent ComboEx Handle
	HWND pHwnd{ nullptr };      //!< Dialog Handle

	//DCXCOMBOEXEDIT() noexcept : OldProc(nullptr), cHwnd(nullptr), pHwnd(nullptr) {}
};
using LPDCXCOMBOEXEDIT = DCXCOMBOEXEDIT * ;

struct DCXCBITEM {
	TString tsMark;		// Marked text
};
using LPDCXCBITEM = DCXCBITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxComboEx
	: public DcxControl
	, public DcxListHelper
{
public:
	DcxComboEx() = delete;
	DcxComboEx(const DcxComboEx &) = delete;
	DcxComboEx &operator =(const DcxComboEx &) = delete;	// No assignments!
	DcxComboEx(DcxComboEx &&) = delete;
	DcxComboEx &operator =(DcxComboEx &&) = delete;

	DcxComboEx(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxComboEx( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) final;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) final;

	HIMAGELIST getImageList( ) const noexcept;
	void setImageList( const HIMAGELIST himl ) noexcept;
	//static HIMAGELIST createImageList( );

	bool matchItemText( const int nItem, const TString &search, const DcxSearchTypes &SearchType ) const;

	LRESULT insertItem( const PCOMBOBOXEXITEM lpcCBItem ) noexcept;
	LRESULT getItem( const PCOMBOBOXEXITEM lpcCBItem ) const noexcept;
	HWND getEditControl( ) const noexcept;
	LRESULT deleteItem( const int iIndex ) noexcept;
	LRESULT setCurSel( const int iIndex )  noexcept;
	LRESULT getCurSel( ) const noexcept;
	LRESULT getLBText( const int iIndex, LPSTR lps ) noexcept;
	LRESULT resetContent( ) noexcept;
	LRESULT getCount( ) const noexcept;
	LRESULT limitText( const int iLimit ) noexcept;

	//static void getItemRange(const TString &tsItems, const int nItemCnt, int *iStart, int *iEnd);
	//static std::pair<int, int> getItemRange(const TString &tsItems, const int nItemCnt);
	static LRESULT CALLBACK ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept;

	inline const TString getType() const final { return TEXT("comboex"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::COMBOEX; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	HWND				m_EditHwnd{ nullptr };	//!< Combo's Edit Control Handle
	HWND				m_hComboHwnd{ nullptr };	//!< Combo's handle
	TString				m_tsSelected;
private:
	DCXCOMBOEXEDIT		m_exEdit{};

};

#endif // _DCXCOMBOEX_H_
