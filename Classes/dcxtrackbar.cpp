/*!
 * \file dcxtrackbar.cpp
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

#include "dcxtrackbar.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxTrackBar::DcxTrackBar( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) {

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0,
    DCX_TRACKBARCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
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
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxTrackBar::DcxTrackBar( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles )
: DcxControl( ID, p_Dialog ) {

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0,
    DCX_TRACKBARCLASS,
    NULL,
    WS_CHILD | WS_VISIBLE | Styles, 
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

DcxTrackBar::~DcxTrackBar( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxTrackBar::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  *Styles |= TBS_FIXEDLENGTH;
  unsigned int i = 1, numtok = styles.numtok( " " );

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "autoticks" ) 
      *Styles |= TBS_AUTOTICKS;
    else if ( styles.gettok( i , " " ) == "both" ) 
      *Styles |= TBS_BOTH;
    else if ( styles.gettok( i , " " ) == "top" ) 
      *Styles |= TBS_TOP;
    else if ( styles.gettok( i , " " ) == "bottom" ) 
      *Styles |= TBS_BOTTOM;
    else if ( styles.gettok( i , " " ) == "left" ) 
      *Styles |= TBS_LEFT;
    else if ( styles.gettok( i , " " ) == "right" ) 
      *Styles |= TBS_RIGHT;
    else if ( styles.gettok( i , " " ) == "select" ) 
      *Styles |= TBS_ENABLESELRANGE;
    else if ( styles.gettok( i , " " ) == "vertical" ) 
      *Styles |= TBS_VERT;
    else if ( styles.gettok( i , " " ) == "nothumb" ) 
      *Styles |= TBS_NOTHUMB;
    else if ( styles.gettok( i , " " ) == "noticks" ) 
      *Styles |= TBS_NOTICKS;
    else if ( styles.gettok( i , " " ) == "reversed" ) 
      *Styles |= TBS_REVERSED;
    else if ( styles.gettok( i , " " ) == "downisleft" ) 
      *Styles |= TBS_DOWNISLEFT;
    else if ( styles.gettok( i , " " ) == "tooltips" ) 
      *Styles |= TBS_TOOLTIPS;

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

void DcxTrackBar::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  if ( input.gettok( 3, " " ) == "value" ) {

    wsprintf( szReturnValue, "%d", this->getPos( ) );
    return;
  }
  else if ( input.gettok( 3, " " ) == "range" ) {
    
    wsprintf( szReturnValue, "%d %d", this->getRangeMin( ), this->getRangeMax( ) );
    return;
  }
  else if ( input.gettok( 3, " " ) == "line" ) {
    
    wsprintf( szReturnValue, "%d", this->getLineSize( ) );
    return;
  }
  else if ( input.gettok( 3, " " ) == "page" ) {
    
    wsprintf( szReturnValue, "%d", this->getPageSize( ) );
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
 * \param input [NAME] [SWITCH] [ID] (OPTIONS)
 */

void DcxTrackBar::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  // xdid -c [NAME] [ID] [SWITCH] [VALUE]
  if ( flags.switch_flags[2] && numtok > 3 ) {

    LONG lPosition = atol( input.gettok( 4, " " ).to_chr( ) );
    this->setTic( lPosition );
  }
  // xdid -j [NAME] [ID] [SWITCH] [MIN] [MAX]
  else if ( flags.switch_flags[9] && numtok > 4 ) {

    INT iMin = atol( input.gettok( 4, " " ).to_chr( ) );
    INT iMax = atol( input.gettok( 5, " " ).to_chr( ) );
    this->setSel( iMin, iMax );
  }
  // xdid -l [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[11] && numtok > 3 ) {

    LONG lLineSize = atol( input.gettok( 4, " " ).to_chr( ) );
    this->setLineSize( lLineSize );
  }
  // xdid -m [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[12] && numtok > 3 ) {

    LONG lPageSize = atol( input.gettok( 4, " " ).to_chr( ) );
    this->setPageSize( lPageSize );
  }
  // xdid -n [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[13] && numtok > 3 ) {

    int iTicFreq = atoi( input.gettok( 4, " " ).to_chr( ) );
    this->setTicFreq( iTicFreq );
  }
  // xdid -q [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[16] ) {

    this->clearTics( );
  }
  // xdid -r [NAME] [ID] [SWITCH] [MIN] [MAX]
  else if ( flags.switch_flags[17] && numtok > 4 ) {

    LONG lMinRange = atol( input.gettok( 4, " " ).to_chr( ) );
    LONG lMaxRange = atol( input.gettok( 5, " " ).to_chr( ) );

    this->setRangeMin( lMinRange );
    this->setRangeMax( lMaxRange );
  }
  // xdid -t [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[19] && numtok > 3 ) {

    TString value = input.gettok( 4, " " );

    if ( value == "left" )
      this->setTipSide( TBTS_LEFT );
    else if ( value == "right" )
      this->setTipSide( TBTS_RIGHT );
    else if ( value == "bottom" )
      this->setTipSide( TBTS_BOTTOM );
    else
      this->setTipSide( TBTS_TOP );
  }
  // xdid -u [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[20] && numtok > 3 ) {

    LONG lLength = atol( input.gettok( 4, " " ).to_chr( ) );

    this->setThumbLength( lLength );
  }
  // xdid -v [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[21] && numtok > 3 ) {

    LONG lPosition = atol( input.gettok( 4, " " ).to_chr( ) );

    this->setPos( lPosition );
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

LRESULT DcxTrackBar::setRangeMin( LONG iLowLim ) {
  return SendMessage( this->m_Hwnd, TBM_SETRANGEMIN, (WPARAM) TRUE, (LPARAM) iLowLim );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getRangeMin(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETRANGEMIN, (WPARAM) 0, (LPARAM) 0 );
} 

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setRangeMax( LONG iHighLim ) {
  return SendMessage( this->m_Hwnd, TBM_SETRANGEMAX, (WPARAM) TRUE, (LPARAM) iHighLim );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getRangeMax(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETRANGEMAX, (WPARAM) 0, (LPARAM) 0 );
} 

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setRange( LONG iLowLim, LONG iHighLim ) {
  return SendMessage( this->m_Hwnd, TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG ( iLowLim, iHighLim ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setPos( LONG lPosition ) {
  return SendMessage( this->m_Hwnd, TBM_SETPOS, (WPARAM) TRUE, (LPARAM) lPosition );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getPos(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETPOS, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setTic( LONG lPosition ) {
  return (BOOL) SendMessage( this->m_Hwnd, TBM_SETTIC, (WPARAM) 0, (LPARAM) lPosition );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setTicFreq( LONG wFreq ) {
  return SendMessage( this->m_Hwnd, TBM_SETTICFREQ, (WPARAM) wFreq, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::clearTics( ) {
  return SendMessage( this->m_Hwnd, TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setTipSide( int fLocation ) {
  return SendMessage( this->m_Hwnd, TBM_SETTIPSIDE, (WPARAM) fLocation, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setPageSize( LONG lPageSize ) {
  return SendMessage( this->m_Hwnd, TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) lPageSize );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getPageSize(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETPAGESIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setLineSize( LONG lLineSize ) {
  return SendMessage( this->m_Hwnd, TBM_SETLINESIZE, (WPARAM) 0, (LPARAM) lLineSize );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getLineSize(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETLINESIZE, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setThumbLength( UINT iLength ) {
  return SendMessage( this->m_Hwnd, TBM_SETTHUMBLENGTH, (WPARAM) iLength, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::setSel( LONG iLowLim, LONG iHighLim ) {
  return SendMessage( this->m_Hwnd, TBM_SETSEL, (WPARAM) TRUE, (LPARAM) MAKELONG( iLowLim, iHighLim ) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getSelStart(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETSELSTART, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::getSelEnd(  ) {
  return SendMessage( this->m_Hwnd, TBM_GETSELEND, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxTrackBar::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "help", this->getUserID( ) );
      }
      break;


    case WM_VSCROLL:
    case WM_HSCROLL: 
      {
        char ret[256];

        switch( LOWORD( wParam ) ) {

          case TB_TOP:
            this->callAliasEx( ret, "%s,%d,%d", "top", this->getUserID( ), this->getPos( ) );
            break;

          case TB_BOTTOM:
            this->callAliasEx( ret, "%s,%d,%d", "bottom", this->getUserID( ), this->getPos( ) );
            break;

          case TB_PAGEUP:
            this->callAliasEx( ret, "%s,%d,%d", "pageup", this->getUserID( ), this->getPos( ) );
            break;

          case TB_PAGEDOWN:
            this->callAliasEx( ret, "%s,%d,%d", "pagedown", this->getUserID( ), this->getPos( ) );
            break;

          case TB_LINEUP:
            this->callAliasEx( ret, "%s,%d,%d", "lineup", this->getUserID( ), this->getPos( ) );
            break;

          case TB_LINEDOWN:
            this->callAliasEx( ret, "%s,%d,%d", "linedown", this->getUserID( ), this->getPos( ) );
            break;

          case TB_THUMBPOSITION:
            this->callAliasEx( ret, "%s,%d,%d", "trackend", this->getUserID( ), this->getPos( ) );
            break;

          case SB_THUMBTRACK:
            this->callAliasEx( ret, "%s,%d,%d", "tracking", this->getUserID( ), this->getPos( ) );
            break;
        }
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
			if (this->m_hBackBrush) {
				HDC hdc = (HDC) wParam;
				RECT rect;

				GetClientRect(this->m_Hwnd, &rect);
				FillRect(hdc, &rect, this->m_hBackBrush);
				//ReleaseDC(this->m_Hwnd, hdc);

				bParsed = TRUE;
				return TRUE;
			}

			break;
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