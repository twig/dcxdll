/*!
 * \file dcxradio.cpp
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

#include "dcxradio.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxRadio::DcxRadio( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    "BUTTON", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

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

DcxRadio::DcxRadio( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    "BUTTON", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

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

DcxRadio::~DcxRadio( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxRadio::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );
  *Styles |= BS_AUTORADIOBUTTON;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "rjustify" )
      *Styles |= BS_RIGHT;
    else if ( styles.gettok( i , " " ) == "center" )
      *Styles |= BS_CENTER;
    else if ( styles.gettok( i , " " ) == "ljustify" )
      *Styles |= BS_LEFT;
    else if ( styles.gettok( i , " " ) == "right" )
      *Styles |= BS_RIGHTBUTTON;
    else if ( styles.gettok( i , " " ) == "pushlike" )
      *Styles |= BS_PUSHLIKE;

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

void DcxRadio::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "state" ) {

    if ( Button_GetCheck( this->m_Hwnd ) & BST_CHECKED )
      lstrcpy( szReturnValue, "1" );
    else
      lstrcpy( szReturnValue, "0" );

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

void DcxRadio::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  //xdid -c [NAME] [ID] [SWITCH]
  if ( flags.switch_flags[2] ) {

    Button_SetCheck( this->m_Hwnd, BST_CHECKED );
  }
  //xdid -t [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[19] && numtok > 3 ) {

    TString text = input.gettok( 4, -1, " " );
    text.trim( );
    SetWindowText( this->m_Hwnd, text.to_chr( ) );
  }
  //xdid -u [NAME] [ID] [SWITCH]
  else if ( flags.switch_flags[20] ) {

    Button_SetCheck( this->m_Hwnd, BST_UNCHECKED );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxRadio::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
    case WM_COMMAND:
      {
        switch ( HIWORD( wParam ) ) {

          case BN_CLICKED:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				        this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
            }
            break;
        }
      }
      break;
	}
	return 0L;
}

LRESULT DcxRadio::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

      /*
    case WM_PAINT:
      {
        PAINTSTRUCT ps; 
        HDC hdc; 

        hdc = BeginPaint( this->m_Hwnd, &ps );

        bParsed = TRUE;

        SetTextColor( hdc, this->m_clrText );
        LRESULT res = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

        EndPaint( this->m_Hwnd, &ps ); 

        return res;
      }
      break;
      */

		case WM_NOTIFY:
			{
        LPNMHDR hdr = (LPNMHDR) lParam;
        if (!hdr)
          break;

        switch( hdr->code ) {
				case TTN_GETDISPINFO:
					{
						LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
						di->lpszText = this->m_tsToolTip.to_chr();
						di->hinst = NULL;
						bParsed = TRUE;
					}
					break;
				case TTN_LINKCLICK:
					{
						bParsed = TRUE;
						this->callAliasEx( NULL, "%s,%d", "tooltiplink", this->getUserID( ) );
					}
					break;
				}
			}
			break;

    case WM_CONTEXTMENU:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	        this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID( ) );
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
        delete this;
        bParsed = TRUE;
      }
      break;

    default:
      break;
  }

  return 0L;
}