/*!
 * \file dcximage.h
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

#ifndef _DCXIMAGE_H_
#define _DCXIMAGE_H_

#include "../../defines.h"
#include "../dcxcontrol.h"

class DcxDialog;

/*!
 * \brief blah
 *
 * blah
 */

class DcxImage : public DcxControl {

public:

  DcxImage( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles );
  DcxImage( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles );
  virtual ~DcxImage( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "image" ); };

protected:

  //Image * m_pImage; //!< GDI+ Image Object
  HBITMAP m_hBitmap; //!< Bitmap
  HICON m_hIcon; // !< icon

  COLORREF m_clrTransColor; //!< Transparent color
  BOOL m_bIsIcon;
  // clears existing image and icon data and sets pointers to null
  void PreloadData(void);
  int m_iIconSize;
};

#endif // _DCXIMAGE_H_