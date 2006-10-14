/*!
 * \file dcxtoolbar.h
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

#ifndef _DCXTOOLBAR_H_
#define _DCXTOOLBAR_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

#define TB_IML_NORMAL   0x01    //!< Toolbar Normal ImageList
#define TB_IML_DISABLE  0x02    //!< Toolbar Disabled ImageList
#define TB_IML_HOT      0x04    //!< Toolbar HotTrack ImageList
#define TB_ICO_GREY     0x08    //!< Toolbar HotTrack ImageList

#define BTNS_WIDTH      0x1000  //!< Toolbar Button Width Style
#define BTNS_BOLD       0x2000  //!< Toolbar Button Caption Bold Style
#define BTNS_UNDERLINE  0x4000  //!< Toolbar Button Caption Underline Style
#define BTNS_COLOR      0x8000  //!< Toolbar Button Caption Color Style
#define BTNS_TBKGCOLOR  0x10000 //!< Toolbar Tooltip Background Color Style
#define BTNS_TTXTCOLOR  0x20000 //!< Toolbar Tooltip Text Color Style

/*!
 * \brief blah
 *
 * blah
 */

typedef struct tagDCXTBBUTTON {

  TString tsTipText;    //!< Tooltip text
  COLORREF clrText;   //!< Button Caption Color
  BOOL bBold;          //!< Is Button Caption Bold ?
  BOOL bUline;         //!< Is Button Caption Underlined
	TString bText;			//!< Buttons text, MUST be persistant.

} DCXTBBUTTON,*LPDCXTBBUTTON;

/*!
 * \brief blah
 *
 * blah
 */

class DcxToolBar : public DcxControl {

public:

  //DcxToolBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxToolBar( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
  virtual ~DcxToolBar( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( int iImageList );
  void setImageList( HIMAGELIST himl, int iImageList );
  static HIMAGELIST createImageList( UINT iSize );

  LRESULT autoSize( );
  LRESULT insertButton( int nPos, LPTBBUTTON lpbb );
  LRESULT hitTest( LPPOINT lpt );
  LRESULT getItemRect( int iButton, LPRECT lprc );
  LRESULT getButtonCount( );
  LRESULT setButtonWidth( int cxMin, int cxMax );
  LRESULT deleteButton( int iButton );
  LRESULT setButtonInfo( int idButton, LPTBBUTTONINFO lpbi );
  LRESULT getButtonInfo( int idButton, LPTBBUTTONINFO lpbi );
  LRESULT getTooltips( );
  LRESULT getButtonText( int idButton, LPSTR lpszText );
  LRESULT getButton( int iButton, LPTBBUTTON lpb );
  LRESULT setButtonState( int idButton, UINT fState );
  LRESULT getCommandToIndex( int idButton );
  LRESULT moveButton( int iButtonFrom, int iButtonTo );
  LRESULT markButton( int iButton, bool fHighlight );
  LRESULT setButtonSize( int dxButton, int dyButton );

  void autoStretchButtons( );

  void autoPosition( int width, int height );

  inline TString getType( ) { return TString( "toolbar" ); };

protected:

  BOOL m_bAutoStretch; //!< Auto Stretch Variable

  static UINT parseImageListFlags( TString & flags );

  int getFreeButtonID( );
  int getIndexToCommand( int iIndex );

  static UINT parseButtonStateFlags( TString & flags );
  static UINT parseButtonStyleFlags( TString & flags );

  void resetContent( );
 
};

#endif // _DCXTOOLBAR_H_