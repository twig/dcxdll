/*
	* Pager Control
*/

#ifndef _DCXPAGER_H_
#define _DCXPAGER_H_

#include "defines.h"
#include "Classes/dcxcontrol.h"
#include "Classes/layout/layoutmanager.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxPager : public DcxControl {

public:

  DcxPager( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxPager( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, TCHAR * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( TEXT("pager") ); };
  TString getStyles(void);
  void toXml(TiXmlElement * xml);

	void setChild(const HWND child);
	void setBkColor(const COLORREF c);
	void setBorderSize(const int bSize);
	void setButtonSize(const int bSize);
	void reCalcSize(void) const;

protected:
	HWND m_ChildHWND;

};

#endif // _DCXPAGER_H_
