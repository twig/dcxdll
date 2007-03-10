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
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxLink::DcxLink( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles ) 
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
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_hIcon = NULL;
	this->m_aColors[0] = RGB( 0, 0, 255 );
	this->m_aColors[1] = RGB( 255, 0, 0 );
	this->m_aColors[2] = RGB( 0, 0, 255 );
	this->m_aColors[3] = RGB( 128, 128, 128 );
	this->m_bHover = FALSE;
	this->m_bTracking = FALSE;
	this->m_bVisited = FALSE;

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips")) {
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
	unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= SS_NOTIFY;

  
  while ( i <= numtok ) {

		if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i ) == "shadow" ))
			this->m_bShadowText = true;
		else if (( styles.gettok( i ) == "noformat" ))
			this->m_bCtrlCodeText = false;

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

void DcxLink::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3 ) == "text" ) {

    GetWindowText( this->m_Hwnd, szReturnValue, 900 );
    return;
  }
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;
  
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
  this->parseSwitchFlags( input.gettok( 3 ), &flags );

  int numtok = input.numtok( );
  
  // xdid -l [NAME] [ID] [SWITCH] [N] [COLOR]
  if ( flags.switch_flags[11] && numtok > 4 ) {

    int nColor = input.gettok( 4 ).to_int( ) - 1;

    if ( nColor > -1 && nColor < 4 )
      this->m_aColors[nColor] = (COLORREF)input.gettok( 5 ).to_num( );
  }
  // xdid -q [NAME] [ID] [SWITCH] [COLOR1] ... [COLOR4]
  else if ( flags.switch_flags[16] && numtok > 3 ) {

    int i = 0, len = input.gettok( 4, -1 ).numtok( );
    while ( i < len && i < 4 ) {

      this->m_aColors[i] = (COLORREF)input.gettok( 4 + i ).to_num( );

      i++;
    }
  }
  //xdid -t [NAME] [ID] [SWITCH] (TEXT)
  else if ( flags.switch_flags[19] ) {

		TString text(input.gettok( 4, -1 ));
    //text.trim( );
    SetWindowText( this->m_Hwnd, text.to_chr( ) );
    this->redrawWindow( );
  }
	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 5) {
		TString flag(input.gettok( 4 ));
		int index = input.gettok( 5 ).to_int();
		TString filename(input.gettok(6, -1));

		if (this->m_hIcon != NULL)
			DestroyIcon(this->m_hIcon);

		this->m_hIcon = dcxLoadIcon(index, filename, FALSE, flag);

		//if (flag.find('g', 0))
		//	this->m_hIcon = CreateGrayscaleIcon(this->m_hIcon);

		this->redrawWindow();
	}
  else
    this->parseGlobalCommandRequest( input, flags );
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxLink::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
  switch( uMsg ) {
	    case WM_COMMAND:
      {
        switch ( HIWORD( wParam ) ) {

          case STN_CLICKED:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
	              this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
            }
            break;

          case STN_DBLCLK:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
								this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
            }
            break;
        }
      }
      break;
	}
	return 0L;
}

LRESULT DcxLink::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
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
          this->m_bHover = TRUE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_MOUSELEAVE:
      {
        if ( this->m_bTracking ) {
          this->m_bHover = FALSE;
          this->m_bTracking = FALSE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_LBUTTONDOWN:
      {
        if ( this->m_bVisited == FALSE ) {
          this->m_bVisited = TRUE;
          InvalidateRect( this->m_Hwnd, NULL, FALSE );
        }
      }
      break;

    case WM_ENABLE:
      {
        InvalidateRect( this->m_Hwnd, NULL, FALSE );
      }
      break;

		case WM_SETCURSOR:
			{
				if (this->m_hCursor) {
					if (GetCursor() != this->m_hCursor)
						SetCursor( this->m_hCursor );
					bParsed = TRUE;
					return TRUE;
				}
				else if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd ) {
					HCURSOR hCursor = LoadCursor( NULL, IDC_HAND );
					if (GetCursor() != hCursor)
						SetCursor( hCursor );
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
			break;
		}

    case WM_PAINT:
      {

        PAINTSTRUCT ps; 
        HDC hdc; 

        hdc = BeginPaint( this->m_Hwnd, &ps );

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

        RECT rect;
        GetClientRect( this->m_Hwnd, &rect );

				// fill background.
				DcxControl::DrawCtrlBackground(hdc,this,&rect);

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
          this->m_clrText = this->m_aColors[3];
        else if ( this->m_bHover )
          this->m_clrText = this->m_aColors[1];
        else if ( this->m_bVisited )
          this->m_clrText = this->m_aColors[2];
        else
          this->m_clrText = this->m_aColors[0];

        int nText = GetWindowTextLength( this->m_Hwnd );
        char * text = new char[nText+2];
        GetWindowText( this->m_Hwnd, text, nText+1 );

				if (!this->m_bCtrlCodeText) {
					if (this->m_bShadowText) { // could cause problems with pre-XP as this is commctrl v6+
						TString wtext(text);
						dcxDrawShadowText(hdc,wtext.to_wchr(), wtext.len(), &rect,
							DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER, this->m_clrText, 0, 5, 5);
					}
					else {
						SetTextColor( hdc, this->m_clrText );
						DrawText( hdc, text, nText, &rect, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );
					}
				}
				else {
					TString wtext(text);
					mIRC_DrawText(hdc, wtext, &rect, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER, this->m_bShadowText);
				}

        delete [] text;

        SelectObject( hdc, hOldFont );
        DeleteObject( hNewFont );

				this->FinishAlphaBlend(ai);

        EndPaint( this->m_Hwnd, &ps ); 

        bParsed = TRUE;
        return 0L;
      }
      break;

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

    case WM_SETFOCUS:
      {
        this->m_pParentDialog->setFocusControl( this->getUserID( ) );
      }
      break;

    //case WM_CLOSE: 
    //  {

    //    mIRCError( "WM_CLOSE" );
    //  }
    //  break;

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
//static const char HEX2DEC[256] = 
//{
//    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
//    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
//    
//    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    
//    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    
//    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
//    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
//};
//    
//TString DcxLink::UriDecode(const TString & sSrc)
//{
//	// Note from RFC1630:  "Sequences which start with a percent sign
//	// but are not followed by two hexadecimal characters (0-9, A-F) are reserved
//	// for future extension"
//
//	const unsigned char * pSrc = (const unsigned char *)sSrc.to_chr();
//	const int SRC_LEN = sSrc.len();
//	const unsigned char * const SRC_END = pSrc + SRC_LEN;
//	const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%' 
//
//	char * const pStart = new char[SRC_LEN];
//	char * pEnd = pStart;
//
//	while (pSrc < SRC_LAST_DEC)
//	{
//		if (*pSrc == '%')
//		{
//			char dec1, dec2;
//			if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
//				&& -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
//			{
//				*pEnd++ = (dec1 << 4) + dec2;
//				pSrc += 3;
//				continue;
//			}
//		}
//
//		*pEnd++ = *pSrc++;
//	}
//
//	// the last 2- chars
//	while (pSrc < SRC_END)
//		*pEnd++ = *pSrc++;
//
//	TString sResult(pStart, pEnd);
//	delete [] pStart;
//	return sResult;
//}
//
//// Only alphanum is safe.
//static const char SAFE[256] =
//{
//    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
//    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
//    
//    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
//    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
//    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
//    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
//    
//    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    
//    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
//    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
//};
//
//TString DcxLink::UriEncode(const TString & sSrc)
//{
//	const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
//	const unsigned char * pSrc = (const unsigned char *)sSrc.to_chr();
//	const int SRC_LEN = sSrc.len();
//	unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
//	unsigned char * pEnd = pStart;
//	const unsigned char * const SRC_END = pSrc + SRC_LEN;
//
//	for (; pSrc < SRC_END; ++pSrc)
//	{
//		if (SAFE[*pSrc]) 
//			*pEnd++ = *pSrc;
//		else
//		{
//			// escape this char
//			*pEnd++ = '%';
//			*pEnd++ = DEC2HEX[*pSrc >> 4];
//			*pEnd++ = DEC2HEX[*pSrc & 0x0F];
//		}
//	}
//
//	TString sResult((char *)pStart, (char *)pEnd);
//	delete [] pStart;
//	return sResult;
//}
