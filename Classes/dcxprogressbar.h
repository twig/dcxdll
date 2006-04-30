/*!
 * \file dcxprogressbar.h
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

#ifndef _DCXPROGRESSBAR_H_
#define _DCXPROGRESSBAR_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxProgressBar : public DcxControl {

public:

  DcxProgressBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxProgressBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxProgressBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  LRESULT setPosition( int nNewPos );
  LRESULT setRange( int iLowLim, int iHighLim );

  LRESULT setMarquee( BOOL fStart, int fTime );
  LRESULT stepIt( );
  LRESULT setStep( int nStepInc );
  LRESULT setBarColor( COLORREF clrBar );
  LRESULT setBKColor( COLORREF clrBk );
  
  LRESULT getPosition( );
  LRESULT getRange( BOOL fWhichLimit, PPBRANGE ppBRange );

  inline TString getType( ) { return TString( "pbar" ); };

protected:

  COLORREF m_clrText;       //!< Caption Text Color
  TString m_tsText;         //!< Caption Text
  BOOL m_bIsAbsoluteValue;  //!< Caption Numerical Placeholder Format
  HFONT m_hfontVertical;

private:
	int CalculatePosition(void);
};

#endif // _DCXPROGRESSBAR_H_