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
#include "Classes\custom\SearchHelper.h"

class DcxDialog;

//#define CBEXSEARCH_W 0x01 //!< ComboEx WildCard Search
//#define CBEXSEARCH_R 0x02 //!< ComboEx Regex Search

/*!
 * \brief blah
 *
 * blah
 */

struct DCXCOMBOEXEDIT
{
	WNDPROC OldProc{ nullptr }; //!< Subclassed Window Procedure of Combo
	HWND cHwnd{ nullptr };      //!< Parent ComboEx Handle
	HWND pHwnd{ nullptr };      //!< Dialog Handle
};
using LPDCXCOMBOEXEDIT = DCXCOMBOEXEDIT * ;

struct DCXCBITEM
{
	TString tsMark;		// Marked text
};
using LPDCXCBITEM = DCXCBITEM *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxComboEx final
	: public DcxControl
	, virtual public DcxSearchHelper
{
public:
	DcxComboEx() = delete;
	DcxComboEx(const DcxComboEx &) = delete;
	GSL_SUPPRESS(c.128) DcxComboEx &operator =(const DcxComboEx &) = delete;	// No assignments!
	DcxComboEx(DcxComboEx &&) = delete;
	GSL_SUPPRESS(c.128) DcxComboEx &operator =(DcxComboEx &&) = delete;

	DcxComboEx(const UINT ID, gsl::strict_not_null<DcxDialog* const> p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxComboEx( ) noexcept;

	LRESULT OurMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest(const TString & input) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	[[nodiscard]] HIMAGELIST getImageList( ) const noexcept;
	void setImageList( const HIMAGELIST himl ) noexcept;
	//static HIMAGELIST createImageList( );

	bool matchItemText( const int nItem, const TString &search, const DcxSearchTypes &SearchType ) const;
	bool matchItemText(const int nItem, const dcxSearchData &srch_data) const;

	bool setItem(const PCOMBOBOXEXITEM lpcCBItem) noexcept;
	LRESULT insertItem( const PCOMBOBOXEXITEM lpcCBItem ) noexcept;
	LRESULT getItem( const PCOMBOBOXEXITEM lpcCBItem ) const noexcept;
	[[nodiscard]] HWND getEditControl( ) const noexcept;
	[[nodiscard]] HWND getComboControl() const noexcept;
	LRESULT deleteItem( const int iIndex ) noexcept;
	LRESULT setCurSel( const int iIndex )  noexcept;
	LRESULT getCurSel( ) const noexcept;
	LRESULT getLBText( const int iIndex, LPSTR lps ) noexcept;
	LRESULT resetContent( ) noexcept;
	[[nodiscard]] LRESULT getCount( ) const noexcept;
	LRESULT limitText( const int iLimit ) noexcept;
	TString getSelText() const;
	void setEditboxContents(const TString& tsStr, int icon, int state, int overlay, int indent);

	int AddTokList(int nPos, int iIcon, int iState, int iIndent, int iStart, int iEnd, const TCHAR* tok, const TString& tsTokList);
	int addItems(int nPos, int iIcon, int iState, int iIndent, const TString &tsFlags, const TString &tsArgs);

	void UpdateHorizExtent();
	void UpdateHorizExtent(const int nPos);
	void UpdateHorizExtent(const TString &tsItem) noexcept;

	static LRESULT CALLBACK ComboExEditProc( HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept;

	void loadIcon(const TString& tsFlags, const TString& tsIndex, const TString& tsSrc) override;

	inline const TString getType() const final { return TEXT("comboex"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::COMBOEX; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	void fromXml(const TiXmlElement* xDcxml, const TiXmlElement* xThis) final;
	const TString getStyles(void) const final;

	static inline WNDPROC m_hDefaultClassProc{ nullptr };	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

protected:

	HWND				m_EditHwnd{ nullptr };	//!< Combo's Edit Control Handle
	HWND				m_hComboHwnd{ nullptr };	//!< Combo's handle
private:
	DCXCOMBOEXEDIT		m_exEdit{};

	[[nodiscard]] static const WindowExStyle parseComboExStyles(const TString& tsStyles);

};

#endif // _DCXCOMBOEX_H_
