/*!
 * \file dcxwindow.h
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

#ifndef _DCXWINDOW_H_
#define _DCXWINDOW_H_

/*!
 * \brief blah
 *
 * blah
 */

class DcxWindow {

public:

  DcxWindow( HWND mHwnd, UINT mID );
  DcxWindow( UINT mID );
  virtual ~DcxWindow( );

  BOOL isStyle( LONG Styles );
  LONG removeStyle( LONG Styles );
  LONG addStyle( LONG Styles );
  LONG setStyle( LONG Styles );
  BOOL isExStyle( LONG Styles );
  LONG removeExStyle( LONG Styles );
  LONG addExStyle( LONG Styles );
  LONG setExStyle( LONG Styles );

  HWND getHwnd( );
  UINT getID( );

  void redrawWindow( );

protected:

  HWND m_Hwnd;
  UINT m_ID;

  void parseSwitchFlags( TString & switchs, XSwitchFlags * flags );

  virtual void parseCommandRequest( TString & input ) = 0;
  virtual void parseInfoRequest( TString & input, char * szReturnValue ) = 0;
  
};

#endif // _DCXWINDOW_H_