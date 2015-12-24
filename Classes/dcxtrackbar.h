/*!
 * \file dcxtrackbar.h
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

#ifndef _DCXTRACKBAR_H_
#define _DCXTRACKBAR_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

#define TBCS_BACK		0x01	// background
#define TBCS_THUMB		0x02	// thumb normal
#define TBCS_THUMBDRAG	0x04	// thumb dragged
#define TBCS_CHANNEL	0x08	// track

//#define TBBMP_BACK		0
//#define TBBMP_THUMB		1
//#define TBBMP_THUMBDRAG	2
//#define TBBMP_CHANNEL		3

enum TrackBarParts: UINT
{
	TBBMP_BACK,
	TBBMP_THUMB,
	TBBMP_THUMBDRAG,
	TBBMP_CHANNEL
};
class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTrackBar : public DcxControl {

public:
	DcxTrackBar() = delete;
	DcxTrackBar(const DcxTrackBar &) = delete;
	DcxTrackBar &operator =(const DcxTrackBar &) = delete;	// No assignments!

	DcxTrackBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	virtual ~DcxTrackBar( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) override;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) override;

	void parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const override;
	void parseCommandRequest( const TString & input ) override;
	void parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) override;

	static UINT parseImageFlags(const TString &flags);

	LRESULT setRangeMin( const LONG iLowLim );
	LRESULT getRangeMin( ) const;
	LRESULT setRangeMax( const LONG iHighLim );
	LRESULT getRangeMax( ) const;
	LRESULT setRange( const LONG iLowLim, const LONG iHighLim );
	LRESULT setPos( const LONG lPosition );
	LRESULT getPos( ) const;
	void setTic( const LONG lPosition );
	void setTicFreq( const LONG wFreq );
	LRESULT clearTics( );
	LRESULT setTipSide( const int fLocation );
	LRESULT setPageSize( const LONG lPageSize );
	LRESULT getPageSize( ) const;
	LRESULT setLineSize( const LONG lLineSize );
	LRESULT getLineSize( ) const;
	LRESULT setThumbLength( const UINT iLength );
	LRESULT setSel( const LONG iLowLim, const LONG iHighLim );
	LRESULT getSelStart( ) const;
	LRESULT getSelEnd( ) const;

	inline const TString getType() const override { return TEXT("trackbar"); };
	inline const DcxControlTypes getControlType() const noexcept override { return DcxControlTypes::TRACKBAR; }

private:
	bool DrawTrackBarPart(HDC hdc, const TrackBarParts iPartId, const LPRECT rc = nullptr);

protected:
	HBITMAP m_hbmp[sizeof(TrackBarParts)]; // refer to TBBMP_*
	COLORREF m_colTransparent;
	bool m_bUpdatingTooltip;
	bool m_bReserved[3];
};

#endif // _DCXTRACKBAR_H_
