/*
	* Pager Control
*/

#ifndef _DCXPAGER_H_
#define _DCXPAGER_H_

#include "../defines.h"
#include "dcxcontrol.h"

#include "layout/layoutmanager.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxPager : public DcxControl {

public:

  DcxPager( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxPager( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxPager( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "pager" ); };

	void setChild(HWND child);
	void setBkColor(COLORREF c);
	void setBorderSize(int bSize);
	void setButtonSize(int bSize);
	void reCalcSize(void);

protected:
	HWND m_ChildHWND;

};

#endif // _DCXPAGER_H_