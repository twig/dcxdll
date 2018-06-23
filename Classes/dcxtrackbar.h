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

class DcxTrackBar
	: public DcxControl
{
public:
	DcxTrackBar() = delete;
	DcxTrackBar(const DcxTrackBar &) = delete;
	DcxTrackBar &operator =(const DcxTrackBar &) = delete;	// No assignments!
	DcxTrackBar(DcxTrackBar &&) = delete;
	DcxTrackBar &operator =(DcxTrackBar &&) = delete;

	DcxTrackBar(const UINT ID, DcxDialog *const p_Dialog, const HWND mParentHwnd, const RECT *const rc, const TString & styles );
	~DcxTrackBar( );

	LRESULT PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed) final;
	LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) final;

	//void parseInfoRequest(const TString & input, PTCHAR szReturnValue) const final;
	void parseInfoRequest(const TString & input, const refString<TCHAR, MIRC_BUFFER_SIZE_CCH> &szReturnValue) const final;
	void parseCommandRequest( const TString & input ) final;
	//void parseControlStyles(const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) final;
	std::tuple<NoTheme, WindowStyle, WindowExStyle> parseControlStyles(const TString & tsStyles) final;

	static UINT parseImageFlags(const TString &flags) noexcept;

	LRESULT setRangeMin( const LONG iLowLim ) noexcept;
	LRESULT getRangeMin( ) const noexcept;
	LRESULT setRangeMax( const LONG iHighLim ) noexcept;
	LRESULT getRangeMax( ) const noexcept;
	LRESULT setRange( const LONG iLowLim, const LONG iHighLim ) noexcept;
	LRESULT setPos( const LONG lPosition ) noexcept;
	LRESULT getPos( ) const noexcept;
	void setTic( const LONG lPosition ) noexcept;
	void setTicFreq( const LONG wFreq ) noexcept;
	LRESULT clearTics( ) noexcept;
	LRESULT setTipSide( const int fLocation ) noexcept;
	LRESULT setPageSize( const LONG lPageSize ) noexcept;
	LRESULT getPageSize( ) const noexcept;
	LRESULT setLineSize( const LONG lLineSize ) noexcept;
	LRESULT getLineSize( ) const noexcept;
	LRESULT setThumbLength( const UINT iLength ) noexcept;
	LRESULT setSel( const LONG iLowLim, const LONG iHighLim ) noexcept;
	LRESULT getSelStart( ) const noexcept;
	LRESULT getSelEnd( ) const noexcept;

	inline const TString getType() const final { return TEXT("trackbar"); };
	inline const DcxControlTypes getControlType() const noexcept final { return DcxControlTypes::TRACKBAR; }

	void toXml(TiXmlElement *const xml) const final;
	TiXmlElement * toXml(void) const final;
	const TString getStyles(void) const final;

	static WNDPROC m_hDefaultClassProc;	//!< Default window procedure
	LRESULT CallDefaultClassProc(const UINT uMsg, WPARAM wParam, LPARAM lParam)  noexcept final;

private:
	bool DrawTrackBarPart(HDC hdc, const TrackBarParts iPartId, const RECT *const rc = nullptr);

	HBITMAP m_hbmp[sizeof(TrackBarParts)]{ nullptr }; // refer to TBBMP_*
	COLORREF m_colTransparent{ CLR_INVALID };
	bool m_bUpdatingTooltip{ false };
	bool m_bReserved[3]{ false };
};

#endif // _DCXTRACKBAR_H_
