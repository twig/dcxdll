/*!
 * \file dcxlink.h
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

#ifndef _DCXLINK_H_
#define _DCXLINK_H_

#include "../../defines.h"
#include "../dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxLink : public DcxControl {

public:

  DcxLink( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
  virtual ~DcxLink( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "link" ); };

protected:

  HICON m_hIcon; //!< Link Icon

  COLORREF m_aColors[4]; //!< Link Colors 0 = normal, 1 = hot, 2 = visited, 3 = disabled

  BOOL m_bHover; //!< Hover State
  BOOL m_bVisited; //!< Visited State
  BOOL m_bTracking; //!< Mouse Tracking Var

	//static TString UriEncode(const TString &sSrc);
	//static TString UriDecode(const TString &sSrc);
	void DrawClientArea(HDC hdc);
};

#endif // _DCXLINK_H_
