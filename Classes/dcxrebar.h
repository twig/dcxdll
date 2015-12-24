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
 * � ScriptsDB.org - 2006
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
typedef struct tagDCXRBBAND {
	TString tsTipText;  //!< Tooltip text
	COLORREF clrText;   //!< Line Caption Color
	BOOL bBold;         //!< Is Line Caption Bold ?
	BOOL bUline;        //!< Is Line Caption Underlined
	TString tsMarkText; //!< Marked text, per ReBar item.
} DCXRBBAND,*LPDCXRBBAND;

/*!
 * \brief blah
 *
 * blah
 */

class DcxReBar : public DcxControl {

public:
	DcxReBar() = delete;
	DcxReBar(const DcxReBar &) = delete;
	DcxReBar &operator =(const DcxReBar &) = delete;	// No assignments!

	DcxReBar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, const TString & styles );
	virtual ~DcxReBar( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	HIMAGELIST getImageList( ) const;
	void setImageList( HIMAGELIST himl );
	static HIMAGELIST createImageList( );

	void resetContents( );

	LRESULT insertBand( const int uIndex, LPREBARBANDINFO lprbbi );
	LRESULT deleteBand( const UINT uIndex );
	LRESULT getBandInfo( const UINT uBand, LPREBARBANDINFO lprbbi ) const;
	LRESULT setBandInfo( const UINT uBand, LPREBARBANDINFO lprbbi );
	LRESULT setBarInfo( LPREBARINFO lprbi );
	LRESULT getBarInfo( LPREBARINFO lprbi ) const;
	LRESULT getRowCount( ) const;
	LRESULT hitTest( LPRBHITTESTINFO lprbht );
	LRESULT getToolTips( ) const;
	LRESULT setToolTips( const HWND hwndToolTip );
	LRESULT getIDToIndex( const UINT uBandID ) const;
	LRESULT getBandCount( ) const;
	LRESULT setReDraw( const BOOL uState );
	LRESULT showBand( const UINT uBand, const BOOL fShow );
	LRESULT moveBand( const UINT iFrom, const UINT iTo );
	LRESULT maxBand( const UINT uBand, const BOOL fIdeal );
	LRESULT minBand( const UINT uBand, const BOOL fIdeal );

	inline const TString getType() const override { return TEXT("rebar"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::REBAR; }

	const TString getStyles(void) const override;
	void toXml(TiXmlElement *const xml) const override;
	DcxControl * getControl(const int index) const;

protected:

	static UINT parseBandStyleFlags( const TString & flags );

	int m_iClickedBand;  //!< Last LClicked Band Number
	int m_iRowLimit;       //!< Rebar Band Row/Column Limit

	UINT m_iWidth;  //!< Control Width used in "change" event
	UINT m_iHeight;  //!< Control Width used in "change" event
};

#endif // _DCXREBAR_H_
