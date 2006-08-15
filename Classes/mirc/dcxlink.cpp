/*!
 * \file dcxlink.cpp
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

#include "dcxlink.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxLink::DcxLink( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
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

  this->m_hIcon = NULL;
  this->m_aColors[0] = RGB( 0, 0, 255 );
  this->m_aColors[1] = RGB( 255, 0, 0 );
  this->m_aColors[2] = RGB( 0, 0, 255 );
  this->m_aColors[3] = RGB( 128, 128, 128 );
  this->m_bHover = FALSE;
  this->m_bTracking = FALSE;
  this->m_bVisited = FALSE;

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips"," ")) {

			this->m_ToolTipHWND = p_Dialog->getToolTip();

			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

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

DcxLink::DcxLink( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
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

  this->m_hIcon = NULL;
  this->m_aColors[0] = RGB( 0, 0, 255 );
  this->m_aColors[1] = RGB( 255, 0, 0 );
  this->m_aColors[2] = RGB( 0, 0, 255 );
  this->m_aColors[3] = RGB( 128, 128, 128 );
  this->m_bHover = FALSE;
  this->m_bTracking = FALSE;
  this->m_bVisited = FALSE;

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips"," ")) {

			this->m_ToolTipHWND = p_Dialog->getToolTip();

			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxLink::~DcxLink( ) {

  if ( this->m_hIcon != NULL )
    DestroyIcon( this->m_hIcon );

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxLink::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
	//unsigned int i = 1, numtok = styles.numtok( " " );
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

	this->parseGeneralControlStyles(styles, Styles, ExStyles, bNoTheme);
}

/*!
 * \brief $xdid Parsing Function
 *
 * \param input [NAME] [ID] [PROP] (OPTIONS)
 * \param szReturnValue mIRC Data Container
 *
 * \return > void
 */

void DcxLink::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
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

void DcxLink::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );
  
  // xdid -l [NAME] [ID] [SWITCH] [N] [COLOR]
  if ( flags.switch_flags[11] && numtok > 4 ) {

    int nColor = atoi( input.gettok( 4, " " ).to_chr( ) ) - 1;

    if ( nColor > -1 && nColor < 4 )
      this->m_aColors[nColor] = atol( input.gettok( 5, " " ).to_chr( ) );
  }
  // xdid -q [NAME] [ID] [SWITCH] [COLOR1] ... [COLOR4]
  else if ( flags.switch_flags[16] && numtok > 3 ) {

    int i = 0, len = input.gettok( 4, -1, " " ).numtok( " " );
    while ( i < len && i < 4 ) {

      this->m_aColors[i] = atol( input.gettok( 4 + i, " " ).to_chr( ) );

      i++;
    }
  }
  //xdid -t [NAME] [ID] [SWITCH] (TEXT)
  else if ( flags.switch_flags[19] ) {

    TString text = input.gettok( 4, -1, " " );
    //text.trim( );
    SetWindowText( this->m_Hwnd, text.to_chr( ) );
    this->redrawWindow( );
  }
  // xdid -w [NAME] [ID] [SWITCH] [INDEX] [FILENAME]
  else if ( flags.switch_flags[22] && numtok > 4 ) {

    int index;
    index = atoi( input.gettok( 4, " ").to_chr( ) );

    if ( this->m_hIcon != NULL )
      DestroyIcon( this->m_hIcon );

    TString filename = input.gettok( 5, -1, " " );
    ExtractIconEx( filename.to_chr( ), index, NULL, &this->m_hIcon, 1 );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxLink::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_MOUSEMOVE:
      {
        this->m_pParentDialog->setMouseControl( this->getUserID( ) );

        if ( !this->m_bTracking ) {

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
        if ( !this->m_bHover && this->m_bTracking ) {

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
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_LBUTTONDOWN:
      {
        if ( this->m_bVisited == FALSE ) {

          //mIRCError( "WM_LBUTTONDOWN" );
          this->m_bVisited = TRUE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_ENABLE:
      {
        //mIRCError( "WM_ENABLE" );
        InvalidateRect( this->m_Hwnd, NULL, FALSE );
      }
      break;

	 case WM_SETCURSOR:
		 {
			 if (this->m_hCursor) {
				 SetCursor(this->m_hCursor);
				 bParsed = TRUE;
				 return TRUE;
			 }
			 else if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd ) {
				 HCURSOR hCursor = LoadCursor( NULL, IDC_HAND );
				 SetCursor( hCursor );
				 bParsed = TRUE;
				 return TRUE;
			 }
		 }
		 break;

    case WM_PAINT:
      {

        PAINTSTRUCT ps; 
        HDC hdc; 

        hdc = BeginPaint( this->m_Hwnd, &ps );

        RECT rect;
        GetClientRect( this->m_Hwnd, &rect );

        HBRUSH hBrush;

        if ( this->m_hBackBrush != NULL )
          hBrush = this->m_hBackBrush;
        else
          hBrush = GetSysColorBrush( COLOR_3DFACE );

        FillRect( hdc, &rect, hBrush );

        HFONT hFont = (HFONT) GetStockObject( DEFAULT_GUI_FONT /*SYSTEM_FONT*/ );

        LOGFONT lf;
        GetObject( hFont, sizeof( LOGFONT ), &lf );

        lf.lfUnderline = TRUE;

        HFONT hNewFont = CreateFontIndirect( &lf );
        HFONT hOldFont = (HFONT) SelectObject( hdc, hNewFont );

        SetBkMode( hdc, TRANSPARENT );

        if ( this->m_hIcon != NULL ) {

          int y = ( rect.top + rect.bottom - 16 ) / 2;
          DrawIconEx( hdc, rect.left, y, this->m_hIcon, 0, 0, NULL, NULL, DI_NORMAL );

          OffsetRect( &rect, 20, 0 );
        }

        if ( IsWindowEnabled( this->m_Hwnd ) == FALSE )
          SetTextColor( hdc, this->m_aColors[3] );
        else if ( this->m_bHover )
          SetTextColor( hdc, this->m_aColors[1] );
        else if ( this->m_bVisited )
          SetTextColor( hdc, this->m_aColors[2] );
        else
          SetTextColor( hdc, this->m_aColors[0] );

        int nText = GetWindowTextLength( this->m_Hwnd );
        char * text = new char[nText+2];
        GetWindowText( this->m_Hwnd, text, nText+1 );

        DrawText( hdc, text, nText, &rect, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );

        delete []text;

        SelectObject( hdc, hOldFont );
        DeleteObject( hNewFont );

        EndPaint( this->m_Hwnd, &ps ); 

        bParsed = TRUE;
        return 0L;
      }
      break;

    case WM_COMMAND:
      {
        //mIRCError( "Control WM_COMMAND" );

        switch ( HIWORD( wParam ) ) {

          case STN_CLICKED:
            {
              this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
            }
            break;

          case STN_DBLCLK:
            {
              this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
            }
            break;
        }
      }
      break;

    case WM_CONTEXTMENU:
      {
        this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID( ) );
      }
      break;

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    case WM_CLOSE: 
      {

        mIRCError( "WM_CLOSE" );
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