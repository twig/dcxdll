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
#include "xpopup/xpopupmenuitem.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxProgressBar : public DcxControl {

public:

  DcxProgressBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxProgressBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  LRESULT setPosition( const int nNewPos );
  LRESULT setRange( const int iLowLim, const int iHighLim );

  LRESULT setMarquee( const BOOL fStart, const int fTime );
  LRESULT stepIt( );
  LRESULT setStep( const int nStepInc );
  LRESULT setBarColor( const COLORREF clrBar );
  LRESULT setBKColor( const COLORREF clrBk );
  
  LRESULT getPosition( ) const;
  LRESULT getRange( const BOOL fWhichLimit, PPBRANGE ppBRange ) const;
  void toXml(TiXmlElement * xml);

  inline TString getType( ) { return TString( "pbar" ); };
  TString getStyles(void);

protected:

  COLORREF m_clrText;       //!< Caption Text Color
  TString m_tsText;         //!< Caption Text
  BOOL m_bIsAbsoluteValue;  //!< Caption Numerical Placeholder Format
  HFONT m_hfontVertical;
	BOOL m_bIsGrad;						//!< Draw Gradient?
	//COLORREF m_clrGrad;				//!< Gradients Color

private:
	int CalculatePosition(void) const;
	void DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam);
};

#endif // _DCXPROGRESSBAR_H_
