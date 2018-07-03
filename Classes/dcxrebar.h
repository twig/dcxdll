/*!
 * \file dcxrebar.h
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

#ifndef _DCXREBAR_H_
#define _DCXREBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

#define RBBS_COLOR     0x00001000 //!< Rebar Caption Color Style
#define RBBS_BOLD      0x00002000 //!< Rebar Caption Bold Style
#define RBBS_UNDERLINE 0x00004000 //!< Rebar Caption Underline Style

/*!
 * \brief blah
 *
 * blah
 */
struct DCXRBBAND {
	TString tsTipText;  //!< Tooltip text
	TString tsMarkText; //!< Marked text, per ReBar item.
	COLORREF clrText{ CLR_INVALID };   //!< Line Caption Color
	bool bBold{ false };         //!< Is Line Caption Bold ?
	bool bUline{ false };        //!< Is Line Caption Underlined

	//DCXRBBAND()
	//	: tsTipText()
	//	, tsMarkText()
	//	, clrText(CLR_INVALID)
	//	, bBold(false)
	//	, bUline(false)
	//{}
};
using LPDCXRBBAND = DCXRBBAND *;

/*!
 * \brief blah
 *
 * blah
 */

class DcxReBar
	: public DcxControl
{
public:
	DcxReBar() = delete;
	DcxReBar(const DcxReBar &) = delete;
	DcxReBar &operator =(const DcxReBar &) = delete;	// No assignments!
	DcxReBar(DcxReBar &&) = delete;
	DcxReBar &operator =(DcxReBar &&) = delete;

	DcxReBar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles );
	~DcxReBar( ) noexcept;

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	dcxWindowStyles parseControlStyles(const TString & tsStyles) final;

	HIMAGELIST getImageList( ) const noexcept;
	void setImageList( HIMAGELIST himl ) noexcept;
	//static HIMAGELIST createImageList( );

	void resetContents( ) noexcept;

	LRESULT insertBand( const int uIndex, const LPREBARBANDINFO lprbbi ) noexcept;
	LRESULT deleteBand( const UINT uIndex ) noexcept;
	LRESULT getBandInfo( const UINT uBand, const LPREBARBANDINFO lprbbi ) const noexcept;
	LRESULT setBandInfo( const UINT uBand, const LPREBARBANDINFO lprbbi ) noexcept;
	LRESULT setBarInfo( const LPREBARINFO lprbi ) noexcept;
	LRESULT getBarInfo( const LPREBARINFO lprbi ) const noexcept;
	LRESULT getRowCount( ) const noexcept;
	LRESULT hitTest( const LPRBHITTESTINFO lprbht ) const noexcept;
	LRESULT getToolTips( ) const noexcept;
	LRESULT setToolTips( const HWND hwndToolTip ) noexcept;
	LRESULT getIDToIndex( const UINT uBandID ) const noexcept;
	LRESULT getBandCount( ) const noexcept;
	LRESULT setReDraw( const BOOL uState ) noexcept;
	LRESULT showBand( const UINT uBand, const BOOL fShow ) noexcept;
	LRESULT moveBand( const UINT iFrom, const UINT iTo ) noexcept;
	LRESULT maxBand( const UINT uBand, const BOOL fIdeal ) noexcept;
	LRESULT minBand( const UINT uBand, const BOOL fIdeal ) noexcept;

	inline const TString getType() const final { return TEXT("rebar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::REBAR; }

	const TString getStyles(void) const final;
	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept final;

	DcxControl * getControl(const int index) const noexcept;

protected:

	static UINT parseBandStyleFlags( const TString & flags ) noexcept;

	int m_iClickedBand{ -1 };  //!< Last LClicked Band Number
	int m_iRowLimit{};       //!< Rebar Band Row/Column Limit

	UINT m_iWidth{};  //!< Control Width used in "change" event
	UINT m_iHeight{};  //!< Control Width used in "change" event
};

#endif // _DCXREBAR_H_
