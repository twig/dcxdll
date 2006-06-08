/*!
 * \file dcxbutton.cpp
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

#include "dcxbutton.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxButton::DcxButton( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
, m_bBitmapText(FALSE)
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0, 
    DCX_BUTTONCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  ZeroMemory( &this->m_aBitmaps, 4*sizeof(HBITMAP) );
  ZeroMemory( &this->m_aColors, 4*sizeof(COLORREF) );

  this->m_bHover = FALSE;
  this->m_bSelected = FALSE;
  this->m_bTracking = FALSE;

  this->m_iIconSize = 16;
  this->m_ImageList = NULL;

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxButton::DcxButton( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0, 
    DCX_BUTTONCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  ZeroMemory( &this->m_aBitmaps, 4*sizeof(HBITMAP) );
  ZeroMemory( &this->m_aColors, 4*sizeof(COLORREF) );
  ZeroMemory( &this->m_aTransp, 4*sizeof(COLORREF) );

  this->m_bHover = FALSE;
  this->m_bSelected = FALSE;
  this->m_bTracking = FALSE;

  this->m_iIconSize = 16;
  this->m_ImageList = NULL;

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxButton::~DcxButton( ) {

  ImageList_Destroy( this->getImageList( ) );

  for ( int i = 0; i < 4; i++ ) {
    if ( this->m_aBitmaps[i] != NULL )
      DeleteObject( this->m_aBitmaps[i] );
  }

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "bitmap" )
      *Styles |= BS_BITMAP;
    else if ( styles.gettok( i , " " ) == "default" )
      *Styles |= BS_DEFPUSHBUTTON;

    i++;
  }
  this->parseGeneralControlStyles( styles, Styles, ExStyles, bNoTheme );
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxButton::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "text" ) {

    lstrcpy( szReturnValue, this->m_tsCaption.to_chr( ) );
    return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

	// xdid -c [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	if (flags.switch_flags[2] && numtok > 4) {
		UINT iColorStyles = this->parseColorFlags(input.gettok(4, " "));
		COLORREF clrColor = atol(input.gettok(5, " ").to_chr());

		if (iColorStyles & BTNCS_NORMAL)
			this->m_aColors[0] = clrColor;
		if (iColorStyles & BTNCS_HOVER)
			this->m_aColors[1] = clrColor;
		if (iColorStyles & BTNCS_SELECTED)
			this->m_aColors[2] = clrColor;
		if (iColorStyles & BTNCS_DISABLED)
			this->m_aColors[3] = clrColor;

		this->redrawWindow();
	}
	// xdid -k [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] [FILENAME]
	else if (flags.switch_flags[10] && (numtok > 5) && (this->isStyle(BS_BITMAP))) {
		UINT iColorStyles = this->parseColorFlags(input.gettok(4, " "));
		COLORREF clrColor = atol(input.gettok(5, " ").to_chr());

		TString filename = input.gettok(6, -1, " ");
		filename.trim();

		if (iColorStyles & BTNCS_NORMAL) {
			this->m_aBitmaps[0] = LoadBitmap(this->m_aBitmaps[0], filename);
			this->m_aTransp[0] = clrColor;
		}
		if (iColorStyles & BTNCS_HOVER) {
			this->m_aBitmaps[1] = LoadBitmap(this->m_aBitmaps[1], filename);
			this->m_aTransp[1] = clrColor;
		}
		if (iColorStyles & BTNCS_SELECTED) {
			this->m_aBitmaps[2] = LoadBitmap(this->m_aBitmaps[2], filename);
			this->m_aTransp[2] = clrColor;
		}
		if (iColorStyles & BTNCS_DISABLED) {
			this->m_aBitmaps[3] = LoadBitmap(this->m_aBitmaps[3], filename);
			this->m_aTransp[3] = clrColor;
		}

		this->redrawWindow( );
	}
  // xdid -l [NAME] [ID] [SWITCH] [SIZE]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    TString size = input.gettok( 4, " " );
    size.trim( );

    if ( size == "32" )
      this->m_iIconSize = 32;
    else if ( size == "24" )
      this->m_iIconSize = 24;
    else
      this->m_iIconSize = 16;

    if ( this->getImageList( ) != NULL ) {
      
      ImageList_Destroy( this->getImageList( ) );
      this->setImageList( NULL );
    }

  }
  // xdid -t [NAME] [ID] [SWITCH] ItemText
  else if ( flags.switch_flags[19] && numtok > 2 ) {
	  this->m_tsCaption = (numtok > 3 ? input.gettok( 4, -1, " " ) : "");
    this->m_tsCaption.trim( );
    this->redrawWindow( );
  }
  // xdid -w [NAME] [ID] [SWITCH] [INDEX] [FILENAME]
  else if ( flags.switch_flags[22] && numtok > 4 ) {

    HIMAGELIST himl;
    HICON icon;
    int index;

    if ( ( himl = this->getImageList( ) ) == NULL ) {

      himl = this->createImageList( );

      if ( himl )
        this->setImageList( himl );
    }

    index = atoi( input.gettok( 4, " ").to_chr( ) );
    TString filename = input.gettok( 5, -1, " " );
    if ( this->m_iIconSize > 16 )
      ExtractIconEx( filename.to_chr( ), index, &icon, NULL, 1 );
    else
      ExtractIconEx( filename.to_chr( ), index, NULL, &icon, 1 );
    ImageList_AddIcon( himl, icon );
    DestroyIcon( icon );
  }
	// xdid -m [NAME] [ID] [SWITCH] [ENABLED]
  else if (flags.switch_flags[12] && numtok > 3) {
		int b = atoi(input.gettok(4, " ").to_chr());

		this->m_bBitmapText = (b ? TRUE : FALSE);
		this->redrawWindow();
  }
  else {
    this->parseGlobalCommandRequest( input, flags );
  }
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxButton::parseColorFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'd' )
      iFlags |= BTNCS_DISABLED;
    else if ( flags[i] == 'h' )
      iFlags |= BTNCS_HOVER;
    else if ( flags[i] == 'n' )
      iFlags |= BTNCS_NORMAL;
    else if ( flags[i] == 's' )
      iFlags |= BTNCS_SELECTED;

    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxButton::getImageList(  ) {

  return this->m_ImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::setImageList( HIMAGELIST himl ) {

  this->m_ImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxButton::createImageList( ) {

  return ImageList_Create( this->m_iIconSize, this->m_iIconSize, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxButton::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_COMMAND:
      {
        //mIRCError( "Control WM_COMMAND" );

        switch ( HIWORD( wParam ) ) {

          case BN_CLICKED:
            {
              //mIRCError( "Control BN_CLICKED" );
              char ret[256];
              this->callAliasEx( ret, "%s,%d", "sclick", this->getUserID( ) );
            }
            break;
        }
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );

        if ( this->m_bTracking == FALSE ) {

          //mIRCError( "WM_MOUSEMOVE + TRACKING" );
          TRACKMOUSEEVENT tme;
          tme.cbSize = sizeof(TRACKMOUSEEVENT);
          tme.hwndTrack = this->m_Hwnd;
          tme.dwFlags = TME_LEAVE | TME_HOVER;
          tme.dwHoverTime = 1;
          this->m_bTracking = (BOOL) _TrackMouseEvent( &tme );		
        }
      }
      break;

    case WM_MOUSEHOVER:
      {
        if ( this->m_bHover == FALSE && this->m_bTracking ) {

          //mIRCError( "WM_MOUSEHOVER" );
          this->m_bHover = TRUE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_MOUSELEAVE:
      {
        if ( this->m_bTracking ) {

          //mIRCError( "WM_MOUSELEAVE" );
          this->m_bHover = FALSE;
          this->m_bTracking = FALSE;
          this->m_bSelected = FALSE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_LBUTTONDOWN:
      {
        if ( this->m_bSelected == FALSE ) {

          //mIRCError( "WM_LBUTTONDOWN" );
          this->m_bSelected = TRUE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_LBUTTONUP:
      {
        this->m_bSelected = FALSE;
      }
      break;

    case WM_CONTEXTMENU:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "rclick", this->getUserID( ) );
      }
      break;

    case WM_ENABLE:
      {
        //mIRCError( "WM_ENABLE" );
        InvalidateRect( this->m_Hwnd, NULL, TRUE );
      }
      break;

    case WM_UPDATEUISTATE:
      {
        //mIRCError( "WM_UPDATEUISTATE" );
        InvalidateRect( this->m_Hwnd, NULL, TRUE );
      }
      break;

    case WM_PAINT:
      {
        //mIRCError( "WM_PAINT" );

        PAINTSTRUCT ps; 
        HDC hdc; 

        hdc = BeginPaint( this->m_Hwnd, &ps );

        LRESULT res = 0L;
		  BOOL isBitmap = this->isStyle(BS_BITMAP);

        // Bitmapped button
        if (isBitmap) {

          RECT rcClient;
          GetClientRect( this->m_Hwnd, &rcClient );

          int nBitmap;
          HDC hdcbmp = CreateCompatibleDC( hdc );
          BITMAP bmp;

          if ( IsWindowEnabled( this->m_Hwnd ) == FALSE )
            nBitmap = 3;
          else if ( this->m_bSelected )
            nBitmap = 2;
          else if ( this->m_bHover )
            nBitmap = 1;
          else
            nBitmap = 0;
          
          if ( this->m_hBackBrush != NULL )
            FillRect( hdc, &rcClient, this->m_hBackBrush );
          else {
            HBRUSH hBrush = GetSysColorBrush( COLOR_3DFACE );
            FillRect( hdc, &rcClient, hBrush );
          }

          GetObject( this->m_aBitmaps[nBitmap], sizeof(BITMAP), &bmp );
          SelectObject( hdcbmp, this->m_aBitmaps[nBitmap] );
          TransparentBlt( hdc, rcClient.left, rcClient.top, rcClient.right - rcClient.left, 
            rcClient.bottom - rcClient.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_aTransp[nBitmap] );

          DeleteDC( hdcbmp );
        }

        // Regular button
		  if ((!isBitmap) || (this->m_bBitmapText)) {

          bParsed = TRUE;
          
			 // draw default window bg
			 if (!isBitmap)
				res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

          HFONT hFontOld = (HFONT) SelectObject( hdc, this->m_hFont );

          RECT rcTxt, rcClient;
          SetRectEmpty( &rcTxt );
          GetClientRect( this->m_Hwnd, &rcClient );

          SetBkMode( hdc, TRANSPARENT );

          HIMAGELIST himl = this->getImageList( );
          int nIcon;

          // Button colors and icon
          if ( IsWindowEnabled( this->m_Hwnd ) == FALSE )
            nIcon = 3;
          else if ( this->m_bSelected )
            nIcon = 2;
          else if ( this->m_bHover )
            nIcon = 1;
          else
            nIcon = 0;

          SetTextColor( hdc, this->m_aColors[nIcon] );

          if ( this->m_tsCaption.len( ) > 0 )
            DrawText( hdc, this->m_tsCaption.to_chr( ), -1, &rcTxt, DT_CALCRECT | DT_SINGLELINE );

          int iCenter = ( rcClient.right - rcClient.left ) / 2;
          int iVCenter = ( rcClient.bottom - rcClient.top ) / 2;
          int iTextW = ( rcTxt.right - rcTxt.left );
          int iTextH = ( rcTxt.bottom - rcTxt.top );

          int iIconLeft = 0;
          int iIconTop = 0;

          /*
          char data[500];
          wsprintf( data, "Cen %d, VCen %d, TW %d, TH, %d", iCenter, iVCenter, iTextW, iTextH );
          mIRCError( data );
          */

			 rcTxt.left = iCenter - iTextW / 2;
          rcTxt.top = iVCenter - iTextH / 2;

			 if ( rcTxt.left < BUTTON_XPAD )
            rcTxt.left = BUTTON_XPAD;

          if ( rcTxt.top < BUTTON_YPAD )
            rcTxt.top = BUTTON_YPAD;

          rcTxt.right = rcClient.right - BUTTON_XPAD;
          rcTxt.bottom = rcClient.bottom - BUTTON_YPAD;

          // If there is an icon
          if ( himl != NULL && nIcon < ImageList_GetImageCount( himl ) ) {

            iIconLeft = iCenter - ( this->m_iIconSize + ICON_XPAD + iTextW ) / 2;
            iIconTop = iVCenter - this->m_iIconSize / 2;

            if ( iIconLeft < BUTTON_XPAD )
              iIconLeft = BUTTON_XPAD;

            if ( iIconTop < BUTTON_YPAD )
              iIconTop = BUTTON_YPAD;

            rcTxt.left = iIconLeft + this->m_iIconSize + ICON_XPAD;

            if ( IsWindowEnabled( this->m_Hwnd ) == FALSE )
              ImageList_Draw( himl, nIcon, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT|ILD_BLEND50 );
            else
              ImageList_Draw( himl, nIcon, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT );
          }

          /*
          wsprintf( data, "Ileft %d, Itop %d, TL %d, TT, %d", iIconLeft, iIconTop, rcTxt.left, rcTxt.top );
          mIRCError( data );
          */

          if ( this->m_tsCaption.len( ) > 0 ) {

            DrawText( hdc, this->m_tsCaption.to_chr( ), this->m_tsCaption.len( ), 
              &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
          }

          SelectObject( hdc, hFontOld );
        }

        EndPaint( this->m_Hwnd, &ps ); 

        return res;
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    case WM_SETCURSOR:
      {
        if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {

          SetCursor( this->m_hCursor );
          bParsed = TRUE;
          return TRUE;
        }
      }
      break;

    case WM_DESTROY:
      {
        //mIRCError( "WM_DESTROY" );
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return 0L;
}