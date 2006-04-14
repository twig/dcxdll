/*!
 * \file dcxscroll.cpp
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

#include "dcxscroll.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxScroll::DcxScroll( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0, 
    "SCROLLBAR", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    p_Dialog->getHwnd( ),
    (HMENU) ID,
    GetModuleHandle( NULL ), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  SCROLLINFO si;
  si.cbSize = sizeof( SCROLLINFO );
  si.fMask = SIF_POS | SIF_RANGE;
  si.nPos = 0;
  si.nMin = 0;
  si.nMax = 100;
  SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );

  this->m_nPage = 5;
  this->m_nLine = 1;

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

DcxScroll::DcxScroll( UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog ) 
{

  LONG Styles = 0, ExStyles = 0;
  BOOL bNoTheme = FALSE;
  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

  this->m_Hwnd = CreateWindowEx(	
    0, 
    "SCROLLBAR", 
    NULL,
    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
    mParentHwnd,
    (HMENU) ID,
    GetModuleHandle(NULL), 
    NULL);

  if ( bNoTheme )
    SetWindowTheme( this->m_Hwnd , L" ", L" " );

  SCROLLINFO si;
  si.cbSize = sizeof( SCROLLINFO );
  si.fMask = SIF_POS | SIF_RANGE;
  si.nPos = 0;
  si.nMin = 0;
  si.nMax = 100;
  SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );

  this->m_nPage = 5;
  this->m_nLine = 1;

  this->registreDefaultWindowProc( );
  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxScroll::~DcxScroll( ) {

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxScroll::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );
  
  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "vertical" )
      *Styles |= SBS_VERT;

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

void DcxScroll::parseInfoRequest( TString & input, char * szReturnValue ) {

  int numtok = input.numtok( " " );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3, " " ) == "value" ) {

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_POS;
    GetScrollInfo( this->m_Hwnd, SB_CTL, &si );

    wsprintf( szReturnValue, "%d", si.nPos );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "range" ) {

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_RANGE;
    GetScrollInfo( this->m_Hwnd, SB_CTL, &si );
    wsprintf( szReturnValue, "%d %d", si.nMin, si.nMax );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "line" ) {

    wsprintf( szReturnValue, "%d %d", this->m_nLine );
    return;
  }
  // [NAME] [ID] [PROP]
  else if ( input.gettok( 3, " " ) == "page" ) {

    wsprintf( szReturnValue, "%d", this->m_nPage );
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

void DcxScroll::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( &input.gettok( 3, " " ), &flags );

  int numtok = input.numtok( " " );

  //xdid -l [NAME] [ID] [SWITCH] [N]
  if ( flags.switch_flags[11] && numtok > 3 ) {

    int nLine = atoi( input.gettok( 4, " " ).to_chr( ) );

    if ( nLine > 0 )
      this->m_nLine = nLine;
  }
  //xdid -m [NAME] [ID] [SWITCH] [N]
  else if ( flags.switch_flags[12] && numtok > 3 ) {

    int nPage = atoi( input.gettok( 4, " " ).to_chr( ) );

    if ( nPage > 0 )
      this->m_nPage = nPage;
  }
  //xdid -r [NAME] [ID] [SWITCH] [L] [R]
  else if ( flags.switch_flags[17] && numtok > 4 ) {

    INT L = atoi( input.gettok( 4, " " ).to_chr( ) );
    INT R = atoi( input.gettok( 5, " " ).to_chr( ) );

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_RANGE;
    si.nMin = L;
    si.nMax = R;
    SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );
  }
  //xdid -v [NAME] [ID] [SWITCH] [VALUE]
  else if ( flags.switch_flags[21] && numtok > 3 ) {

    int pos = atoi( input.gettok( 4, " " ).to_chr( ) );

    SCROLLINFO si;
    si.cbSize = sizeof( SCROLLINFO );
    si.fMask = SIF_POS;
    si.nPos = pos;
    SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );
  }
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxScroll::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
        char ret[256];
        this->callAliasEx( ret, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_HSCROLL: 
      {

        SCROLLINFO si;
        si.cbSize = sizeof( SCROLLINFO );
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
        GetScrollInfo( this->m_Hwnd, SB_CTL, &si );

        char ret[256];

        switch( LOWORD( wParam ) ) {

          case SB_TOP:
            {
              si.nPos = si.nMin;
              this->callAliasEx( ret, "%s,%d,%d", "top", this->getUserID( ), si.nPos );
            }
            break;

          case SB_BOTTOM:
            {
              si.nPos = si.nMax;
              this->callAliasEx( ret, "%s,%d,%d", "bottom", this->getUserID( ), si.nPos );
            }
            break;

            //case SB_ENDTRACK:
            //  CallAliasEx( p_Dialog, ret, "%s,%d,%d", "sclick", 
            //               this->getUserID( ), p_DcxTrackBar->getPos( ) );
            //  break;

          case SB_PAGEUP:
            {
              if ( si.nPos - this->m_nPage >= si.nMin )
                si.nPos -= this->m_nPage;
              else
                si.nPos = si.nMin;

              this->callAliasEx( ret, "%s,%d,%d", "pageup", this->getUserID( ), si.nPos );
            }
            break;

          case SB_PAGEDOWN:
            {
              if ( si.nPos + this->m_nPage <= si.nMax )
                si.nPos += this->m_nPage;
              else
                si.nPos = si.nMax;

              this->callAliasEx( ret, "%s,%d,%d", "pagedown", this->getUserID( ), si.nPos );
            }
            break;

          case SB_LINEUP:
            {
              if ( si.nPos - this->m_nLine >= si.nMin )
                si.nPos -= this->m_nLine;
              else
                si.nPos = si.nMin;

              this->callAliasEx( ret, "%s,%d,%d", "lineup", this->getUserID( ), si.nPos );
            }
            break;

          case SB_LINEDOWN:
            {
              if ( si.nPos + this->m_nLine <= si.nMax )
                si.nPos += this->m_nLine;
              else
                si.nPos = si.nMax;

              this->callAliasEx( ret, "%s,%d,%d", "linedown", this->getUserID( ), si.nPos );
            }
            break;

          case SB_THUMBPOSITION:
            {
              this->callAliasEx( ret, "%s,%d,%d", "trackend", this->getUserID( ), si.nPos );
            }
            break;

          case SB_THUMBTRACK:
            {
              si.nPos = si.nTrackPos;
              this->callAliasEx( ret, "%s,%d,%d", "tracking", this->getUserID( ), si.nTrackPos );
            }
            break;
        }
  
        bParsed = TRUE;
        si.fMask = SIF_POS;
        SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );
      }
      break;

    case WM_VSCROLL:
      {
        
        SCROLLINFO si;
        si.cbSize = sizeof( SCROLLINFO );
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
        GetScrollInfo( this->m_Hwnd, SB_CTL, &si );

        char ret[256];

        switch( LOWORD( wParam ) ) {

          case SB_TOP:
            {
              si.nPos = si.nMin;
              this->callAliasEx( ret, "%s,%d,%d", "top", this->getUserID( ), si.nPos );
            }
            break;

          case SB_BOTTOM:
            {
              si.nPos = si.nMax;
              this->callAliasEx( ret, "%s,%d,%d", "bottom", this->getUserID( ), si.nPos );
            }
            break;

            //case SB_ENDTRACK:
            //  CallAliasEx( p_Dialog, ret, "%s,%d,%d", "sclick", 
            //               this->getUserID( ), p_DcxTrackBar->getPos( ) );
            //  break;

          case SB_PAGEUP:
            {
              if ( si.nPos - this->m_nPage >= si.nMin )
                si.nPos -= this->m_nPage;
              else
                si.nPos = si.nMin;

              this->callAliasEx( ret, "%s,%d,%d", "pageup", this->getUserID( ), si.nPos );
            }
            break;

          case SB_PAGEDOWN:
            {
              if ( si.nPos + this->m_nPage <= si.nMax )
                si.nPos += this->m_nPage;
              else
                si.nPos = si.nMax;

              this->callAliasEx( ret, "%s,%d,%d", "pagedown", this->getUserID( ), si.nPos );
            }
            break;

          case SB_LINEUP:
            {
              if ( si.nPos - this->m_nLine >= si.nMin )
                si.nPos -= this->m_nLine;
              else
                si.nPos = si.nMin;

              this->callAliasEx( ret, "%s,%d,%d", "lineup", this->getUserID( ), si.nPos );
            }
            break;

          case SB_LINEDOWN:
            {
              if ( si.nPos + this->m_nLine <= si.nMax )
                si.nPos += this->m_nLine;
              else
                si.nPos = si.nMax;

              this->callAliasEx( ret, "%s,%d,%d", "linedown", this->getUserID( ), si.nPos );
            }
            break;

          case SB_THUMBPOSITION:
            {
              this->callAliasEx( ret, "%s,%d,%d", "trackend", this->getUserID( ), si.nPos );
            }
            break;

          case SB_THUMBTRACK:
            {
              si.nPos = si.nTrackPos;
              this->callAliasEx( ret, "%s,%d,%d", "tracking", this->getUserID( ), si.nTrackPos );
            }
            break;
        }
  
        bParsed = TRUE;
        si.fMask = SIF_POS;
        SetScrollInfo( this->m_Hwnd, SB_CTL, &si, TRUE );
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