/*!
 * \file dcxipaddress.h
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

#ifndef _DCXIPADDRESS_H_
#define _DCXIPADDRESS_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxIpAddress : public DcxControl {

public:

  DcxIpAddress( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxIpAddress( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  LRESULT setRange( const int nField, const BYTE iMin, const BYTE iMax );
  LRESULT setFocus( const int nField );
  LRESULT setAddress( const DWORD dwIpAddress );
  LRESULT getAddress( LPDWORD lpdwIpAddress ) const;
  LRESULT clearAddress( );

  inline TString getType( ) { return TString( "ipaddress" ); };
  void toXml(TiXmlElement * xml);

protected:
  
};

#endif // _DCXIPADDRESS_H_
