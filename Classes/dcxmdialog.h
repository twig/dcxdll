/*!
 * \file dcxmdialog.h
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

#ifndef _DCXMDIALOG_H_
#define _DCXMDIALOG_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxMDialog : public DcxControl {

public:

  DcxMDialog( HWND cHwnd, HWND pHwnd, UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  virtual ~DcxMDialog( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme);

  inline TString getType( ) { return TString( "dialog" ); };

protected:

  LONG m_OrigStyles;      //!< Dialog Original Styles
  LONG m_OrigExStyles;    //!< Dialog Original Extended Styles
  HWND m_OrigParentHwnd;  //!< Dialog Original Parent Handle
  UINT m_OrigID;          //!< Dialog Original Control ID
  TString m_OrigName;     //!< Dialog Original Name

};

#endif // _DCXMDIALOG_H_