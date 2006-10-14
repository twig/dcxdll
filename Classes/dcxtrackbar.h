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

  //DcxTrackBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxTrackBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxTrackBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );
	static UINT parseImageFlags(TString &flags);

  LRESULT setRangeMin( LONG iLowLim );
  LRESULT getRangeMin( );
  LRESULT setRangeMax( LONG iHighLim );
  LRESULT getRangeMax( );
  LRESULT setRange( LONG iLowLim, LONG iHighLim );
  LRESULT setPos( LONG lPosition );
  LRESULT getPos( );
  LRESULT setTic( LONG lPosition );
  LRESULT setTicFreq( LONG wFreq );
  LRESULT clearTics( );
  LRESULT setTipSide( int fLocation );
  LRESULT setPageSize( LONG lPageSize );
  LRESULT getPageSize( );
  LRESULT setLineSize( LONG lLineSize );
  LRESULT getLineSize( );
  LRESULT setThumbLength( UINT iLength );
  LRESULT setSel( LONG iLowLim, LONG iHighLim );
  LRESULT getSelStart( );
  LRESULT getSelEnd( );

  inline TString getType( ) { return TString( "trackbar" ); };

protected:
	HBITMAP m_hbmp[4]; // refer to TBBMP_*
	COLORREF m_colTransparent;
};

#endif // _DCXTRACKBAR_H_