/*!
 * \file dcxstacker.h
 * \brief blah
 *
 * blah
 *
 * \author Ook
 * \version 1.0
 *
 * \b Revisions
 *
 * © ScriptsDB.org - 2006
 */

#ifndef _DCXSTACKER_H_
#define _DCXSTACKER_H_

#include "../defines.h"
#include "dcxcontrol.h"

#define MIN_STACK_HEIGHT 20 //!< Min height for a stacker item.

//class DcxDialog;

typedef struct tagDCXSITEM {
	TString tsTipText;  //!< Tooltip text
	TString tsCaption;	//!< Title Buttons text
	COLORREF clrBack;   //!< Line Background Caption Color
	COLORREF clrText;   //!< Line Caption Color
	HFONT hFont;				//!< Items font.
	DcxControl *pChild; //!< Items child control
} DCXSITEM,*LPDCXSITEM;

/*!
 * \brief blah
 *
 * blah
 */

class DcxStacker : public DcxControl {

public:

  DcxStacker( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
  virtual ~DcxStacker( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "stacker" ); };

protected:
	HWND m_hActive;

};

#endif //_DCXSTACKER_H_