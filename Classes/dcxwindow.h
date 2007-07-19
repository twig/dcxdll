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

  DcxWindow( const HWND mHwnd, const UINT mID );
  DcxWindow( const UINT mID );
  virtual ~DcxWindow( );

  BOOL isStyle( const LONG Styles ) const;
  LONG removeStyle( const LONG Styles );
  LONG addStyle( const LONG Styles );
  LONG setStyle( const LONG Styles );
  BOOL isExStyle( const LONG Styles ) const;
  LONG removeExStyle( const LONG Styles );
  LONG addExStyle( const LONG Styles );
  LONG setExStyle( const LONG Styles );

  HWND getHwnd( ) const;
  UINT getID( ) const;

  void redrawWindow( );
	void redrawBufferedWindow( );

protected:

  HWND m_Hwnd;
  UINT m_ID;

  static void parseSwitchFlags( TString & switchs, XSwitchFlags * flags );

  virtual void parseCommandRequest( TString & input ) = 0;
  virtual void parseInfoRequest( TString & input, char * szReturnValue ) = 0;
  
};

#endif // _DCXWINDOW_H_
