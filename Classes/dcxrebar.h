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

#include "../defines.h"
#include "dcxcontrol.h"

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

} DCXRBBAND,*LPDCXRBBAND;

/*!
 * \brief blah
 *
 * blah
 */

class DcxReBar : public DcxControl {

public:

  DcxReBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxReBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxReBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  HIMAGELIST createImageList( );

  void resetContents( );

  LRESULT insertBand( int uIndex, LPREBARBANDINFO lprbbi );
  LRESULT deleteBand( UINT uIndex );
  LRESULT getBandInfo( UINT uBand, LPREBARBANDINFO lprbbi );
  LRESULT setBandInfo( UINT uBand, LPREBARBANDINFO lprbbi );
  LRESULT setBarInfo( LPREBARINFO lprbi );
  LRESULT getBarInfo( LPREBARINFO lprbi );
  LRESULT getRowCount( );
  LRESULT hitTest( LPRBHITTESTINFO lprbht );
  LRESULT getToolTips( );
  LRESULT setToolTips( HWND hwndToolTip );
  LRESULT getIDToIndex( UINT uBandID );
  LRESULT getBandCount( );
  LRESULT setReDraw( BOOL uState );
  LRESULT showBand( UINT uBand, BOOL fShow );
  LRESULT moveBand( UINT iFrom, UINT iTo );
  LRESULT maxBand( UINT uBand, BOOL fIdeal );
  LRESULT minBand( UINT uBand, BOOL fIdeal );

  inline TString getType( ) { return TString( "rebar" ); };

protected:

  UINT parseBandStyleFlags( TString & flags );

  int m_iClickedBand;  //!< Last LClicked Band Number
  int m_iRowLimit;       //!< Rebar Band Row/Column Limit

  UINT m_iWidth;  //!< Control Width used in "change" event
  UINT m_iHeight;  //!< Control Width used in "change" event

};

#endif // _DCXREBAR_H_