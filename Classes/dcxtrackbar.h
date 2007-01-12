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

#include "../defines.h"
#include "dcxcontrol.h"

#define TBCS_BACK			0x01	// backgruond
#define TBCS_THUMB		0x02	// thumb normal
#define TBCS_THUMBDRAG	0x04	// thumb dragged
#define TBCS_CHANNEL		0x08	// track

#define TBBMP_BACK		0
#define TBBMP_THUMB		1
#define TBBMP_THUMBDRAG	2
#define TBBMP_CHANNEL	3

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxTrackBar : public DcxControl {

public:

  DcxTrackBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxTrackBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );
	static UINT parseImageFlags(TString &flags);

  LRESULT setRangeMin( const LONG iLowLim );
  LRESULT getRangeMin( ) const;
  LRESULT setRangeMax( const LONG iHighLim );
  LRESULT getRangeMax( ) const;
  LRESULT setRange( const LONG iLowLim, const LONG iHighLim );
  LRESULT setPos( const LONG lPosition );
  LRESULT getPos( ) const;
  LRESULT setTic( const LONG lPosition );
  LRESULT setTicFreq( const LONG wFreq );
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

  inline TString getType( ) { return TString( "trackbar" ); };

protected:
	HBITMAP m_hbmp[4]; // refer to TBBMP_*
	COLORREF m_colTransparent;
};

#endif // _DCXTRACKBAR_H_