/*!
 * \file dcxstatusbar.cpp
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

#include "dcxstatusbar.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxStatusBar::DcxStatusBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) : DcxControl( ID, p_Dialog ) {

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_STATUSBARCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_hImageList = NULL;

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

DcxStatusBar::DcxStatusBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    ExStyles, 
    DCX_STATUSBARCLASS, 
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

  this->m_hImageList = NULL;

  this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxStatusBar::~DcxStatusBar( ) {

  this->cleanPartIcons( );
  ImageList_Destroy( this->getImageList( ) );

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "grip" ) 
      *Styles |= SBARS_SIZEGRIP;
    else if ( styles.gettok( i , " " ) == "tooltips" ) 
      *Styles |= SBARS_TOOLTIPS;
    else if ( styles.gettok( i , " " ) == "nodivider" ) 
      *Styles |= CCS_NODIVIDER;
    else if ( styles.gettok( i , " " ) == "top" ) 
      *Styles |= CCS_TOP;
    else if ( styles.gettok( i , " " ) == "noresize" ) 
      *Styles |= CCS_NORESIZE;
    else if ( styles.gettok( i , " " ) == "noparentalign" ) 
      *Styles |= CCS_NOPARENTALIGN ;

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

void DcxStatusBar::parseInfoRequest( TString & input, char * szReturnValue ) {

	int numtok = input.numtok( " " );

	// [NAME] [ID] [PROP] [N]
	if ( input.gettok( 3, " " ) == "text" && numtok > 3 ) {

		int iPart = input.gettok( 4, " " ).to_int( ), nParts = this->getParts( 256, 0 );

		if ( iPart > -1 && iPart < nParts ) {

			this->getText( iPart, szReturnValue );
			return;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( input.gettok( 3, " " ) == "parts" ) {

		INT parts[256];
		int nParts = this->getParts( 256, 0 );

		this->getParts( 256, parts );

		int i = 0;
		szReturnValue[0] = 0;
		char d[10];

		while ( i < nParts ) {

			wsprintf( d, "%d", parts[i] );

			if ( i == 0 ) {
				lstrcat( szReturnValue, d );
			}
			else {
				lstrcat( szReturnValue, " " );
				lstrcat( szReturnValue, d );
			}

			i++;
		}
		return;
	}
	// [NAME] [ID] [PROP] [N]
	else if ( input.gettok( 3, " " ) == "tooltip" && numtok > 3 ) {

		int iPart = input.gettok( 3, " " ).to_int( ), nParts = this->getParts( 256, 0 );

		if ( iPart > -1 && iPart < nParts ) {

			this->getTipText( iPart, 900, szReturnValue );
			return;
		}
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

void DcxStatusBar::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -k [NAME] [ID] [SWITCH] [COLOR]
  if (flags.switch_flags[10] && numtok > 3) {
    this->setBkColor((COLORREF) input.gettok(4, " ").to_num());
  }
  // xdid -l [NAME] [ID] [SWITCH] [POS [POS POS ...]]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    int nParts = numtok - 3;
    INT parts[256];

    int i = 0;
    while ( i < nParts ) {

      parts[i] = input.gettok( i+4, " " ).to_int( );
      i++;
    }
    this->setParts( nParts, parts );
  }
  // xdid -t [NAME] [ID] [SWITCH] N [+FLAGS] [#ICON] [Cell Text][TAB]Tooltip Text
  else if ( flags.switch_flags[19] && numtok > 5 ) {

    int nPos = input.gettok( 4, " " ).to_int( ) - 1;
    TString flags = input.gettok( 5, " " );
    int icon = input.gettok( 6, " " ).to_int( ) - 1;

    TString itemtext;

    if ( input.gettok( 1, "\t" ).numtok( " " ) > 6 ) {
      
      itemtext = input.gettok( 1, "\t" ).gettok( 7, -1, " " );
      itemtext.trim( );
    }

    TString tooltip;

    if ( input.numtok( "\t" ) > 1 ) {
      
      tooltip = input.gettok( 2, "\t" );
      tooltip.trim( );
    }

    DestroyIcon( (HICON) this->getIcon( nPos ) );
    if ( icon != -1 )
      this->setIcon( nPos, ImageList_GetIcon( this->getImageList( ), icon, ILD_TRANSPARENT ) );
    else
      this->setIcon( nPos, NULL );

    this->setText( nPos, this->parseItemFlags( flags ), itemtext.to_chr( ) );
    this->setTipText( nPos, tooltip.to_chr( ) );

  }
  // xdid -v [NAME] [ID] [SWITCH] [N] (TEXT)
  else if ( flags.switch_flags[21] && numtok > 3 ) {

    int nPos = input.gettok( 4, " " ).to_int( ) - 1;

    if ( nPos > -1 && nPos < this->getParts( 256, 0 ) ) {

      TString itemtext = "";
      if ( numtok > 4 )
        itemtext = input.gettok( 5, -1, " " );

      char text[900];
      this->setText( nPos, HIWORD( this->getText( nPos, text ) ), itemtext.to_chr( ) );
    }
  }
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 5) {
		HIMAGELIST himl;
		HICON icon;
		TString flags = input.gettok(4, " ");
		int index = input.gettok(5, " ").to_int();
		TString filename = input.gettok(6, -1, " ");

		if ((himl = this->getImageList()) == NULL) {
			himl = this->createImageList();

			if (himl)
				this->setImageList(himl);
		}

		ExtractIconEx(filename.to_chr(), index, NULL, &icon, 1);

		if (flags.find('g', 0))
			icon = CreateGrayscaleIcon(icon);

		ImageList_AddIcon(himl, icon);
		DestroyIcon(icon);
	}
  // xdid -y [NAME] [ID] [SWITCH] [+FLAGS]
  else if ( flags.switch_flags[24] ) {

    ImageList_Destroy( this->getImageList( ) );
		this->setImageList(NULL);
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

HIMAGELIST DcxStatusBar::getImageList( ) {

  return this->m_hImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::setImageList( HIMAGELIST himl ) {

  this->m_hImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxStatusBar::createImageList( ) {

  return ImageList_Create( 16, 16, ILC_COLOR32|ILC_MASK, 1, 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxStatusBar::parseItemFlags( TString & flags ) {

	INT i = 1, len = flags.len( ), iFlags = 0;

	// no +sign, missing params
	if ( flags[0] != '+' ) 
		return iFlags;

	while ( i < len ) {
		switch(flags[i])
		{
		case 'n':
			iFlags |= SBT_NOBORDERS;
			break;
		case 'p':
			iFlags |= SBT_POPOUT;
			break;
		}

		//if ( flags[i] == 'n' )
		//	iFlags |= SBT_NOBORDERS;
		//else if ( flags[i] == 'p' )
		//	iFlags |= SBT_POPOUT;

		++i;
	}
	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxStatusBar::cleanPartIcons( ) {

  int n = 0;
  while ( n < 256 ) {

    DestroyIcon( (HICON) this->getIcon( n ) );
    n++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setParts( int nParts, LPINT aWidths ) {
  return SendMessage( this->m_Hwnd, SB_SETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getParts( int nParts, LPINT aWidths ) {
  return SendMessage( this->m_Hwnd, SB_GETPARTS, (WPARAM) nParts, (LPARAM) aWidths );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setBkColor( COLORREF clrBk ) {
  return SendMessage( this->m_Hwnd, SB_SETBKCOLOR, (WPARAM) 0, (LPARAM) clrBk );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setText( int iPart, int Style, LPSTR lpstr ) {
  return SendMessage( this->m_Hwnd, SB_SETTEXT, (WPARAM) iPart | Style, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getText( int iPart, LPSTR lpstr ) {
  return SendMessage( this->m_Hwnd, SB_GETTEXT, (WPARAM) iPart, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setTipText( int iPart, LPSTR lpstr ) {
  return SendMessage( this->m_Hwnd, SB_SETTIPTEXT, (WPARAM) iPart, (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getTipText( int iPart, int nSize, LPSTR lpstr ) {
  return SendMessage( this->m_Hwnd, SB_GETTIPTEXT, (WPARAM) MAKEWPARAM (iPart, nSize), (LPARAM) lpstr );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getRect( int iPart, LPRECT lprc ) {
  return SendMessage( this->m_Hwnd, SB_GETRECT, (WPARAM) iPart, (LPARAM) lprc );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::setIcon( int iPart, HICON hIcon ) {
  return SendMessage( this->m_Hwnd, SB_SETICON, (WPARAM) iPart, (LPARAM) hIcon );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxStatusBar::getIcon( int iPart ) {
  return SendMessage( this->m_Hwnd, SB_GETICON, (WPARAM) iPart, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

int DcxStatusBar::hitTest( POINT & pt ) {

  RECT rc;
  int n = 0;
  //int tx = 0;
  int nParts = this->getParts( 256, 0 );

  while ( n < nParts ) {

    this->getRect( n, &rc );
    if ( PtInRect( &rc, pt ) )
      return n;

    n++;
  }
  return -1;
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxStatusBar::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
    case WM_NOTIFY:
      {
        LPNMHDR hdr = (LPNMHDR) lParam;

        if (!hdr)
          break;

        switch( hdr->code ) {

          case NM_CLICK:
            {
              POINT pt;
              GetCursorPos( &pt );
              ScreenToClient( this->m_Hwnd, &pt );
              int cell = this->hitTest( pt );

              if ( cell != -1 )
                this->callAliasEx( NULL, "%s,%d,%d", "sclick", this->getUserID( ), cell + 1 );

              bParsed = TRUE;
            }
            break;

          case NM_RCLICK:
            {
              POINT pt;
              GetCursorPos( &pt );
              ScreenToClient( this->m_Hwnd, &pt );
              int cell = this->hitTest( pt );

              if ( cell != -1 )
                this->callAliasEx( NULL, "%s,%d,%d", "rclick", this->getUserID( ), cell + 1 );

              bParsed = TRUE;
            }
            break;

          case NM_DBLCLK: 
            {
              POINT pt;
              GetCursorPos( &pt );
              ScreenToClient( this->m_Hwnd, &pt );
              int cell = this->hitTest( pt );

              if ( cell != -1 )
                this->callAliasEx( NULL, "%s,%d,%d", "dclick", this->getUserID( ), cell + 1 );

              bParsed = TRUE;
            }
            break;

        } // switch
      }
      break;
	}
	return 0L;
}

LRESULT DcxStatusBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
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