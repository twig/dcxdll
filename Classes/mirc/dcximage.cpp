/*!
 * \file dcximage.cpp
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

#include "dcximage.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxImage::DcxImage( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0, 
    "STATIC", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  //this->m_pImage = NULL;
  this->m_hBitmap = NULL;
  this->m_clrTransColor = -1;

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

DcxImage::DcxImage( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0, 
    "STATIC", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  //this->m_pImage = NULL;
  this->m_hBitmap = NULL;
  this->m_clrTransColor = -1;

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxImage::~DcxImage( ) {

  //if ( this->m_pImage != NULL )
  //  delete this->m_pImage;

  if ( this->m_hBitmap != NULL )
    delete this->m_hBitmap;

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxImage::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );
  *Styles |= SS_NOTIFY;

  /*
  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "center" )
      *Styles |= SS_CENTER;
    else if ( styles.gettok( i , " " ) == "right" )
      *Styles |= SS_RIGHT;
    else if ( styles.gettok( i , " " ) == "endellipsis" )
      *Styles |= SS_ENDELLIPSIS;

    i++;
  }
  */

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

void DcxImage::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxImage::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  //xdid -i [NAME] [ID] [SWITCH] [IMAGE]
  if ( flags.switch_flags[8] && numtok > 3 ) {

    TString filename = input.gettok( 4, -1, " " );
    filename.trim( );

    /*
    WCHAR file[257];

    MultiByteToWideChar( CP_ACP, 0, filename.to_chr( ), filename.len( )+1, file,   
     sizeof(file)/sizeof(file[0]) );

    if ( this->m_pImage != NULL )
      delete this->m_pImage;

    this->m_pImage = Image::FromFile( file );
    */

    this->m_hBitmap = (HBITMAP) LoadImage( GetModuleHandle( NULL ), 
          filename.to_chr( ), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE );

    InvalidateRect( this->m_Hwnd, NULL, TRUE );
  }
  // xdid -k [NAME] [ID] [SWITCH] [COLOR]
  if ( flags.switch_flags[10] && numtok > 3 ) {
    
    this->m_clrTransColor = atol( input.gettok( 4, " " ).to_chr( ) );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxImage::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "help", this->getUserID( ) );
      }
      break;

      
    case WM_ERASEBKGND:
      {
        /*
        mIRCError( "WM_ERASEBKGND" );
        if ( this->m_pImage != NULL ) {

          HDC hdc = (HDC) wParam;

          RECT rect;
          GetClientRect( this->m_Hwnd, &rect );

          Graphics grphx( hdc );

          HBRUSH hBrush;

          
          if ( this->m_hBackBrush != NULL )
          FillRect( hdc, &rect, this->m_hBackBrush );
          else {
          hBrush = GetSysColorBrush( COLOR_3DFACE );
          FillRect( hdc, &rect, hBrush );
          }
          

          grphx.DrawImage( this->m_pImage, 0, 0, rect.right - rect.left, rect.bottom - rect.top );

          bParsed = TRUE;
          return TRUE;
        }
        */
        bParsed = TRUE;
        return TRUE;
      }
      break;
      

    case WM_PAINT:
      {
        /*
        mIRCError( "WM_PAINT" );
        PAINTSTRUCT ps;
        HDC hdc; 

        hdc = BeginPaint( this->m_Hwnd, &ps );

        EndPaint( this->m_Hwnd, &ps ); 

        bParsed = TRUE;
        return 0L;
        */

        if ( this->m_hBitmap != NULL ) {

          PAINTSTRUCT ps; 
          HDC hdc; 

          hdc = BeginPaint( this->m_Hwnd, &ps );

          RECT rect;
          GetClientRect( this->m_Hwnd, &rect );

          /*
          Graphics grphx( hdc );

          HBRUSH hBrush;

          if ( this->m_hBackBrush != NULL )
            FillRect( hdc, &rect, this->m_hBackBrush );
            /*
          else {
            hBrush = GetSysColorBrush( COLOR_3DFACE );
            FillRect( hdc, &rect, hBrush );
          }
          
          grphx.DrawImage( this->m_pImage, 0, 0, rect.right - rect.left, rect.bottom - rect.top );
          */

          if ( this->m_hBackBrush != NULL )
            FillRect( hdc, &rect, this->m_hBackBrush );

          HDC hdcbmp = CreateCompatibleDC( hdc );
          BITMAP bmp;
          GetObject( this->m_hBitmap, sizeof(BITMAP), &bmp );
          SelectObject( hdcbmp, this->m_hBitmap );

          if ( this->m_clrTransColor != -1 ) {

            TransparentBlt( hdc, rect.left, rect.top, rect.right - rect.left, 
              rect.bottom - rect.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_clrTransColor );
          }
          else {

            StretchBlt( hdc, rect.left, rect.top, rect.right - rect.left, 
              rect.bottom - rect.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY );
          }

          DeleteDC( hdcbmp );

          EndPaint( this->m_Hwnd, &ps ); 

          bParsed = TRUE;
          return 0L;
        }
      }
      break;


    case WM_COMMAND:
      {
        //mIRCError( "Control WM_COMMAND" );

        switch ( HIWORD( wParam ) ) {

          case STN_CLICKED:
            {
              char ret[256];
              this->callAliasEx( ret, "%s,%d", "sclick", this->getUserID( ) );
            }
            break;

          case STN_DBLCLK:
            {
              char ret[256];
              this->callAliasEx( ret, "%s,%d", "dclick", this->getUserID( ) );
            }
            break;
        }
      }
      break;

    case WM_SIZE:
      {
        InvalidateRect( this->m_Hwnd, NULL, TRUE );
      }
      break;

    case WM_CONTEXTMENU:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "rclick", this->getUserID( ) );
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );
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