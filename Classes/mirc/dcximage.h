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

  DcxImage( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles );
  virtual ~DcxImage( );

  LRESULT PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );
  LRESULT ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed );

  void parseInfoRequest( TString & input, char * szReturnValue );
  void parseCommandRequest( TString & input );
  void parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme );

  inline TString getType( ) { return TString( "image" ); };
  void toXml(TiXmlElement * xml);

	static void registerClass(void);

protected:

#ifdef DCX_USE_GDIPLUS
  Image * m_pImage; //!< GDI+ Image Object
	CompositingMode m_CMode; // Compositing Mode
	CompositingQuality m_CQuality; // Compositing Quality
	InterpolationMode m_IMode; // Interpolation Mode
	SmoothingMode m_SMode; // Smoothing Mode
	bool LoadGDIPlusImage(const TString &flags, TString &filename);
	void DrawGDIImage(HDC hdc, int x, int y, int w, int h);
#endif
	void DrawBMPImage(HDC hdc, int x, int y, int w, int h);
	void DrawClientArea(HDC hdc);
  HBITMAP m_hBitmap; //!< Bitmap
  HICON m_hIcon; // !< icon

  COLORREF m_clrTransColor; //!< Transparent color
  BOOL m_bIsIcon;
  // clears existing image and icon data and sets pointers to null
  void PreloadData(void);
	int m_iIconSize;
	bool m_bResizeImage; //!< Resize Image
	bool m_bTileImage; //!< Tile Image
	bool m_bBuffer; //!< Double Buffer Rendering, needed for GDI+ when WS_EX_COMPOSITED
	int m_iXOffset, m_iYOffset; //!< X & Y image offsets.
	TString m_tsFilename;	//!< The loaded images filename.
};

#endif // _DCXIMAGE_H_
