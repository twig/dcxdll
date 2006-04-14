/*!
 * \file dcxbutton.h
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

#ifndef _DCXBUTTON_H_
#define _DCXBUTTON_H_

#include "../defines.h"
#include "dcxcontrol.h"

class DcxDialog;

#define ICON_XPAD 2 //!< Icon X Padding

#define BUTTON_YPAD 3 //!< Button Top/Bottom Padding
#define BUTTON_XPAD 4 //!< Button Left/Right Padding
  
#define BTNCS_NORMAL    0x01 //!< Button Normal Color Style
#define BTNCS_SELECTED  0x02 //!< Button Selected Color Style
#define BTNCS_HOVER     0x04 //!< Button Hover Color Style
#define BTNCS_DISABLED  0x08 //!< Button Disabled Color Style

/*!
 * \brief blah
 *
 * blah
 */

class DcxButton : public DcxControl {

public:

  DcxButton( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxButton( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxButton( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  HIMAGELIST getImageList( );
  void setImageList( HIMAGELIST himl );
  HIMAGELIST createImageList( );

  inline TString getType( ) { return TString( "button" ); };

protected:

  HIMAGELIST m_ImageList; //!< Button Image List (normal, hover, pushed, disabled)
  COLORREF m_aColors[4];  //!< Button Colors (normal, hover, pushed, disabled)

  HBITMAP m_aBitmaps[4];  //!< Button Bitmaps (normal, hover, pushed, disabled)

  TString m_tsCaption; //!< Button Caption Text

  BOOL m_bTracking; //!< Button Tracking Mouse Event State
  BOOL m_bHover;    //!< Button Hovering State
  BOOL m_bSelected; //!< Button Selected State

  UINT m_iIconSize; //!< Button Icon Size 16,24,32

  UINT parseColorFlags( TString & flags );
};

#endif // _DCXBUTTON_H_