/*!
 * \file dcxtext.cpp
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

#include "dcxtext.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxText::DcxText( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
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

DcxText::DcxText( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
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

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxText::~DcxText( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxText::parseControlStyles(TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme) {
	unsigned int i = 1, numtok = styles.numtok(" ");
	*Styles |= SS_NOTIFY;

	while (i <= numtok) {
		if (styles.gettok(i , " ") == "nowrap")
			*Styles |= SS_LEFTNOWORDWRAP;
		else if (styles.gettok(i, " ") == "center")
			*Styles |= SS_CENTER;
		else if (styles.gettok(i, " ") == "right")
			*Styles |= SS_RIGHT;
		else if (styles.gettok(i, " ") == "noprefix")
			*Styles |= SS_NOPREFIX;
		else if (styles.gettok(i, " ") == "endellipsis")
			*Styles |= SS_ENDELLIPSIS;
		else if (styles.gettok(i, " ") == "pathellipsis")
			*Styles |= SS_PATHELLIPSIS;
		else if (styles.gettok(i, " ") == "transparent")
			*ExStyles |= WS_EX_TRANSPARENT;

		i++;
	}

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

void DcxText::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

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

void DcxText::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

	//xdid -t [NAME] [ID] [SWITCH]
	if (flags.switch_flags[19]) {
		TString text = input.gettok(4, -1, " ");
		text.trim();
		SetWindowText(this->m_Hwnd, text.to_chr());

		// redraw if transparent
		if (this->isExStyle(WS_EX_TRANSPARENT)) {
			RECT r;
			POINT pt;
			int w, h;

			GetWindowRect(this->m_Hwnd, &r);
			pt.x = r.left;
			pt.y = r.top;
			w = r.right - r.left;
			h = r.bottom - r.top;
			ScreenToClient(GetParent(this->m_Hwnd), &pt);
			r.left = pt.x;
			r.top = pt.y;
			r.right = r.left + w;
			r.bottom = r.top + h;
			InvalidateRect(GetParent(this->m_Hwnd), &r, TRUE);
			this->redrawWindow();
		}
	}
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxText::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

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

		case WM_ERASEBKGND:
		{
			if (this->isExStyle(WS_EX_TRANSPARENT)) {
				bParsed = TRUE;
				return TRUE;
			}
		}

		case WM_PAINT:
		{
			if (!this->isExStyle(WS_EX_TRANSPARENT))
				break;

			bParsed = TRUE;

			PAINTSTRUCT ps;
			RECT r;
			HDC hdc = BeginPaint(this->m_Hwnd, &ps);

			int nText = GetWindowTextLength(this->m_Hwnd);
			char *text = new char[nText +1];
			GetWindowText(this->m_Hwnd, text, nText +1);

			GetClientRect(this->m_Hwnd, &r);

			SelectObject(hdc, this->m_hFont);
			SetTextColor(hdc, this->m_clrText);
			//SelectObject(hdc, GetStockBrush(HOLLOW_BRUSH));
			SetBkMode(hdc, TRANSPARENT);
			//Rectangle(hdc, r.left, r.top, r.right, r.bottom);
			//DrawTextEx(hdc, "hello world", 0, &r, 0, 0);

			DrawText(hdc, text, nText, &r, DT_VCENTER | DT_LEFT);

			EndPaint(this->m_Hwnd, &ps);
			return TRUE;
		}

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