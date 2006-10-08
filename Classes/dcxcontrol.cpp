/*!
 * \file dcxcontrol.cpp
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

#include "../defines.h"

#include "dcxcontrol.h"
#include "dcxdialog.h"

#include "dcxprogressbar.h"
#include "dcxtrackbar.h"
#include "dcxcolorcombo.h"
#include "dcxcomboex.h"
#include "dcxstatusbar.h"
#include "dcxtreeview.h"
#include "dcxtoolbar.h"
#include "dcxlistview.h"
#include "dcxrebar.h"
#include "dcxbutton.h"
#include "dcxrichedit.h"
#include "dcxupdown.h"
#include "dcxipaddress.h"
#include "dcxwebctrl.h"
#include "dcxcalendar.h"
#include "dcxpager.h"

#include "dcxdivider.h"
#include "dcxpanel.h"
#include "dcxtab.h"

#include "mirc/dcxline.h"
#include "mirc/dcxbox.h"
#include "mirc/dcxradio.h"
#include "mirc/dcxcheck.h"
#include "mirc/dcxtext.h"
#include "mirc/dcxedit.h"
#include "mirc/dcxscroll.h"
#include "mirc/dcxlist.h"
#include "mirc/dcxlink.h"
#include "mirc/dcximage.h"

#include "dcxmwindow.h"
#include "dcxmdialog.h"

extern mIRCDLL mIRCLink;

/*!
 * \brief Constructor
 *
 * \param mID Control ID
 * \param p_Dialog Parent DcxDialog Object
 */

DcxControl::DcxControl( const UINT mID, DcxDialog * p_Dialog ) : DcxWindow( mID ), m_pParentDialog( p_Dialog ) {

  this->m_hFont = NULL;
  this->m_tsMark = "";
  this->m_clrText = (COLORREF)-1;
  this->m_clrBackText = (COLORREF)-1;
  this->m_hBackBrush = NULL;
  this->m_iRefCount = 0;

  this->m_hCursor = NULL;
  this->m_bCursorFromFile = FALSE;
	this->m_dEventMask = p_Dialog->getEventMask();
}

/*!
 * \brief Destructor
 *
 * Control Interface Destructor.
 *
 * Frees global control resources.
 */

DcxControl::~DcxControl( ) {

  RemoveProp( this->m_Hwnd, "dcx_cthis" );

  // Reverse to old font
  this->setControlFont( NULL, FALSE );

  // Delete background brush
  if ( this->m_hBackBrush != NULL ) {

      DeleteObject( this->m_hBackBrush );
      this->m_hBackBrush = NULL;
  }

  if ( this->m_bCursorFromFile && this->m_hCursor != NULL ) {

    DeleteObject( this->m_hCursor );
    this->m_hCursor = NULL;
  }

  this->m_pParentDialog->deleteControl( this );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseGeneralControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( " " );

	//*ExStyles |= WS_EX_NOPARENTNOTIFY;

  while ( i <= numtok ) {

    if ( styles.gettok( i , " " ) == "notheme" )
      *bNoTheme = TRUE;
    else if ( styles.gettok( i , " " ) == "tabstop" )
      *Styles |= WS_TABSTOP;
    else if ( styles.gettok( i , " " ) == "group" )
      *Styles |= WS_GROUP;
    else if ( styles.gettok( i , " " ) == "disabled" )
      *Styles |= WS_DISABLED;

    i++;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxControl::getUserID( ) const {

  return this->getID( ) - mIRC_ID_OFFSET;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxControl::callAliasEx( char * szReturn, const char * szFormat, ... ) {

  va_list args;
  va_start( args, szFormat );
  char parms[2048];

  vsprintf( parms, szFormat, args );
  wsprintf( mIRCLink.m_pData, "$%s(%s,%s)", 
    this->m_pParentDialog->getAliasName( ).to_chr( ), 
    this->m_pParentDialog->getName( ).to_chr( ),
    parms );

	this->m_pParentDialog->incRef();
	SendMessage( mIRCLink.m_mIRCHWND, WM_USER + 201, 0, mIRCLink.m_map_cnt );

	if ( szReturn )
    lstrcpy( szReturn, mIRCLink.m_pData );

	this->m_pParentDialog->decRef();
  va_end( args );

  if ( !lstrcmp( mIRCLink.m_pData, "$false" ) )
    return FALSE;

  return TRUE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseGlobalCommandRequest( const TString & input, XSwitchFlags & flags ) {
	int numtok = input.numtok( " " );

	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	if ( flags.switch_flags[5] && numtok > 3 ) {
		LOGFONT lf;

		if (ParseCommandToLogfont(input.gettok(4, -1, " "), &lf)) {
			HFONT hFont = CreateFontIndirect(&lf);
			this->setControlFont(hFont, FALSE);
		}

		this->redrawWindow( );
	}
	// xdid -p [NAME] [ID] [SWITCH] [X] [Y] [W] [H]
	else if ( flags.switch_flags[15] && numtok > 6 ) {

		int x = input.gettok( 4, " " ).to_int( );
		int y = input.gettok( 5, " " ).to_int( );
		int w = input.gettok( 6, " " ).to_int( );
		int h = input.gettok( 7, " " ).to_int( );

		MoveWindow( this->m_Hwnd, x, y, w, h, FALSE );
		InvalidateRect( GetParent( this->m_Hwnd ), NULL, TRUE );
		this->redrawWindow( );
		SendMessage( this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -x [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags.switch_flags[23] && numtok > 3 ) {

		this->removeStyle( WS_BORDER|WS_DLGFRAME );
		this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

		LONG Styles = 0, ExStyles = 0;
		this->parseBorderStyles( input.gettok( 4, " " ), &Styles, &ExStyles );

		this->addStyle( Styles );
		this->addExStyle( ExStyles );

		SetWindowPos( this->m_Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
		InvalidateRect( this->m_Hwnd, NULL, TRUE );
		SendMessage( this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags.switch_cap_flags[2] && numtok > 4 ) {
		UINT iFlags = this->parseColorFlags( input.gettok( 4, " " ) );
		COLORREF clrColor = (COLORREF)input.gettok( 5, " " ).to_num( );

		if ( iFlags & DCC_BKGCOLOR ) {
			if ( this->m_hBackBrush != NULL ) {
				DeleteObject( this->m_hBackBrush );
				this->m_hBackBrush = NULL;
			}

			if ( clrColor != -1 )
				this->m_hBackBrush = CreateSolidBrush( clrColor );
		}

		if ( iFlags & DCC_TEXTCOLOR )
			this->m_clrText = clrColor;

		if ( iFlags & DCC_TEXTBKGCOLOR )
			this->m_clrBackText = clrColor;

		// force a control redraw
		this->redrawWindow( );
	}
	// xdid -F [NAME] [ID] [SWITCH]
	else if (flags.switch_cap_flags[5]) {
		SetFocus(this->m_Hwnd);
	}
	// xdid -J [NAME] [ID] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if ( flags.switch_cap_flags[9] && numtok > 4 ) {
		if ( this->m_bCursorFromFile ) {
			DeleteObject( this->m_hCursor );
			this->m_hCursor = NULL;
			this->m_bCursorFromFile = FALSE;
		}
		else
			this->m_hCursor = NULL;

		UINT iFlags = this->parseCursorFlags( input.gettok( 4, " " ) );

		if ( iFlags & DCCS_FROMRESSOURCE )
			this->m_hCursor = LoadCursor( NULL, this->parseCursorType( input.gettok( 5, " " ) ) );
		else if ( iFlags & DCCS_FROMFILE ) {
			this->m_hCursor = LoadCursorFromFile( input.gettok( 5, -1, " " ).to_chr( ) );
			this->m_bCursorFromFile = TRUE;
		}
	}
	// xdid -M [NAME] [ID] [SWITCH] [MARK INFO]
	else if ( flags.switch_cap_flags[12] ) {

		TString info = "";
		if ( numtok > 3 ) {

			info = input.gettok( 4, -1 , " " );
			info.trim( );
		}
		this->m_tsMark = info;
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if ( flags.switch_cap_flags[25] && numtok > 3 ) {

		int perc = input.gettok( 4, " " ).to_int( );

		if ( perc >= 0 || perc <= 100 ) {

			int min, max;
			GetScrollRange( this->m_Hwnd, SB_VERT, &min, &max );

			/*
			char data[500];
			wsprintf( data, "%d %d", min, max );
			mIRCError( data );
			*/

			//switchbar is defined and has visible range
			if ( min != 0 || max != 0 ) {

				int pos = round( (float) ( max - min ) * (float) perc / (float) 100.0 );

				/*
				char data[500];
				wsprintf( data, "%d", pos );
				mIRCError( data );
				*/

				SCROLLINFO si;
				ZeroMemory( &si, sizeof ( SCROLLINFO ) );
				si.cbSize = sizeof( SCROLLINFO );
				GetScrollInfo( this->m_Hwnd, SB_VERT, &si );
				si.nPos = pos;
				SetScrollInfo( this->m_Hwnd, SB_VERT, &si, TRUE );
				SendMessage( this->m_Hwnd, WM_VSCROLL, MAKEWPARAM( SB_THUMBPOSITION, si.nPos ), NULL );

				//SetScrollPos( this->m_Hwnd, SB_VERT, pos, TRUE );
				//InvalidateRect( this->m_Hwnd, NULL, TRUE );
			}
		}
	}

	// xdid -b [NAME] [ID]
	else if ( flags.switch_flags[1] ) {
		EnableWindow( this->m_Hwnd, FALSE );
	}
	// xdid -e [NAME] [ID]
	else if ( flags.switch_flags[4] ) {
		EnableWindow( this->m_Hwnd, TRUE );
	}
	// xdid -h [NAME] [ID]
	else if ( flags.switch_flags[7] ) {
		ShowWindow( this->m_Hwnd, SW_HIDE );
	}
	// xdid -s [NAME] [ID]
	else if ( flags.switch_flags[18] ) {
		ShowWindow( this->m_Hwnd, SW_SHOW );
	}
	// xdid -U [NAME] [ID]
	else if (flags.switch_cap_flags[20]) {
		// Box Double click Bug: the GetNextDlgtabItem() function never returns & seems to just loop forever.
		// from functions doc:
		//	If the search for the next control with the WS_TABSTOP
		//	style encounters a window with the WS_EX_CONTROLPARENT style,
		//	the system recursively searches the window's children.
		//
		HWND hNextCtrl = GetNextDlgTabItem(this->m_pParentDialog->getHwnd(), this->m_Hwnd, FALSE);

		if (hNextCtrl && (hNextCtrl != this->m_Hwnd))
			SendMessage(this->m_pParentDialog->getHwnd(), WM_NEXTDLGCTL, (WPARAM) hNextCtrl, TRUE);
		//::PostMessage(this->m_pParentDialog->getHwnd(), WM_NEXTDLGCTL, NULL, FALSE);
	}
	// xdid -T [NAME] [ID] [SWITCH] (ToolTipText)
  else if (flags.switch_cap_flags[19] && numtok > 2) {
		this->m_tsToolTip = (numtok > 3 ? input.gettok(4, -1, " ") : "");
		this->m_tsToolTip.trim();
  }
	// xdid -R [NAME] [ID] [SWITCH] [FLAG] [ARGS]
	else if (flags.switch_cap_flags[17] && numtok > 3) {
		TString flag = input.gettok(4," ");

		if ((flag.len() < 2) || (flag[0] != '+')) {
			DCXError("/xdid -R","Invalid Flag");
			return;
		}

		RECT rc;
		HRGN m_Region = NULL;
		GetWindowRect(this->m_Hwnd,&rc);

		switch (flag[1])
		{
			// image file - [COLOR] [FILE]
			case 'f': 
			{
				if (numtok < 6) {
					DCXError("/xdid -R","Invalid arguments for +f flag");
					return;
				}

				COLORREF tCol = (COLORREF)input.gettok(5," ").to_num();
				HBITMAP m_bitmapBg = dcxLoadBitmap(NULL,input.gettok(6,-1," "));

				if (m_bitmapBg != NULL) {
					m_Region = BitmapRegion(m_bitmapBg,tCol,TRUE);

					if (m_Region != NULL)
						SetWindowRgn(this->m_Hwnd,m_Region,TRUE);
					DeleteObject(m_bitmapBg);
				}

				break;
			}
			
			case 'r': // rounded rect - radius args (optional)
			{
				int radius;

				if (numtok > 4)
					radius = input.gettok(5, " ").to_int();
				else
					radius = 20;

				m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);

				if (m_Region)
					SetWindowRgn(this->m_Hwnd,m_Region,TRUE);

				break;
			}

			case 'c': // circle - radius arg (optional)
			{
				if (numtok > 4) {
					int radius = input.gettok(5, " ").to_int();
					if (radius < 1) radius = 100; // handle cases where arg isnt a number or is a negative.
					int cx = ((rc.right - rc.left)/2);
					int cy = ((rc.bottom - rc.top)/2);
					m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
				}
				else
					m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);

				if (m_Region)
					SetWindowRgn(this->m_Hwnd,m_Region,TRUE);

				break;
			}
			
			case 'p': // polygon
			{
				// u need at least 3 points for a shape
				if (numtok < 7) {
					DCXError("/xdid -R","Invalid arguments for +p flag");
					return;
				}

				TString strPoints = input.gettok(5, -1, " ");
				TString strPoint;
				int tPoints = strPoints.numtok(" ");

				if (tPoints < 1) {
					DCXError("/xdid -R","Invalid Points");
					return;
				}

				int cnt = 1;
				POINT *pnts = new POINT[tPoints];

				while (cnt <= tPoints) {
					strPoint = strPoints.gettok(cnt," ");
					pnts[cnt-1].x = (LONG)strPoint.gettok(1, ",").to_num();
					pnts[cnt-1].y = (LONG)strPoint.gettok(2, ",").to_num();
					cnt++;
				}

				m_Region = CreatePolygonRgn(pnts,tPoints,WINDING);

				if (m_Region)
					SetWindowRgn(this->m_Hwnd,m_Region,TRUE);

				delete [] pnts;
				break;
			}

			case 'n': // none, no args
			{
				SetWindowRgn(this->m_Hwnd,NULL,TRUE);
				break;
			}

			default:
			{
				DCXError("/xdid -R","Invalid Flag");
				break;
			}
		}
		this->redrawWindow();
	}
	// invalid command
	else {
		TString error;

		if (numtok > 3) {
			error.sprintf("D_ERROR /xdid: Invalid %s command /xdid %s %s %s %s (or invalid arguments) on Control %d",
				this->getType().to_chr(),
				input.gettok(3, " ").to_chr(),
				input.gettok(1, " ").to_chr(),
				input.gettok(2, " ").to_chr(),
				input.gettok(4, -1, " ").to_chr(),
				this->getUserID());
		}
		else {
			error.sprintf("D_ERROR /xdid: Invalid %s command /xdid %s %s %s (or invalid arguments) on Control %d",
				this->getType().to_chr(),
				input.gettok(3, " ").to_chr(),
				input.gettok(1, " ").to_chr(),
				input.gettok(2, " ").to_chr(),
				this->getUserID());
		}

		mIRCError(error.to_chr());
	}
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxControl::parseColorFlags( TString & flags ) {

  INT i = 1, len = flags.len( ), iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'b' )
      iFlags |= DCC_BKGCOLOR;
    else if ( flags[i] == 'k' )
      iFlags |= DCC_TEXTBKGCOLOR;
    else if ( flags[i] == 't' )
      iFlags |= DCC_TEXTCOLOR;
    
    ++i;
  }
  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseBorderStyles( TString & flags, LONG * Styles, LONG * ExStyles ) {

  INT i = 1, len = flags.len( );

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return;

  while ( i < len ) {

    if ( flags[i] == 'b' )
      *Styles |= WS_BORDER;
    else if ( flags[i] == 'c' )
      *ExStyles |= WS_EX_CLIENTEDGE;
    else if ( flags[i] == 'd' )
      *Styles |= WS_DLGFRAME ;
    else if ( flags[i] == 'f' )
      *ExStyles |= WS_EX_DLGMODALFRAME;
    else if ( flags[i] == 's' )
      *ExStyles |= WS_EX_STATICEDGE;
    else if ( flags[i] == 'w' )
      *ExStyles |= WS_EX_WINDOWEDGE;

    ++i;
  }
}

/*!
 * \brief blah
 *
 * blah
 */

UINT DcxControl::parseCursorFlags( TString & flags ) {

  INT i = 1, len = flags.len( );
  UINT iFlags = 0;

  // no +sign, missing params
  if ( flags[0] != '+' ) 
    return iFlags;

  while ( i < len ) {

    if ( flags[i] == 'f' )
      iFlags |= DCCS_FROMFILE;
    else if ( flags[i] == 'r' )
      iFlags |= DCCS_FROMRESSOURCE;

    ++i;
  }

  return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

LPSTR DcxControl::parseCursorType( TString & cursor ) {

  if ( cursor == "appstarting" )
    return IDC_APPSTARTING;
  else if ( cursor == "arrow" )
    return IDC_ARROW;
  else if ( cursor == "cross" )
    return IDC_CROSS;
  else if ( cursor == "hand" )
    return IDC_HAND;
  else if ( cursor == "help" )
    return IDC_HELP;
  else if ( cursor == "ibeam" )
    return IDC_IBEAM;
  else if ( cursor == "no" )
    return IDC_NO;
  else if ( cursor == "sizeall" )
    return IDC_SIZEALL;
  else if ( cursor == "sizenesw" )
    return IDC_SIZENESW;
  else if ( cursor == "sizens" )
    return IDC_SIZENS;
  else if ( cursor == "sizenwse" )
    return IDC_SIZENWSE;
  else if ( cursor == "sizewe" )
    return IDC_SIZEWE;
  else if ( cursor == "uparrow" )
    return IDC_UPARROW;
  else if ( cursor == "wait" )
    return IDC_WAIT;

  return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxControl::parseGlobalInfoRequest( const TString & input, char * szReturnValue ) {

  if ( input.gettok( 3, " " ) == "hwnd" ) {

    wsprintf( szReturnValue, "%d", this->m_Hwnd );
    return TRUE;
  }
  else if ( input.gettok( 3, " " ) == "visible" ) {

    if ( IsWindowVisible( this->m_Hwnd ) )
      lstrcpy( szReturnValue, "$true" );
    else
      lstrcpy( szReturnValue, "$false" );
    return TRUE;       
  }
  else if ( input.gettok( 3, " " ) == "enabled" ) {

    if ( IsWindowEnabled( this->m_Hwnd ) )
      lstrcpy( szReturnValue, "$true" );
    else
      lstrcpy( szReturnValue, "$false" );
    return TRUE;    
  }
  else if ( input.gettok( 3, " " ) == "pos" ) {

    RECT rc;
    GetWindowRect( this->m_Hwnd, &rc );
    POINT pt;
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient( GetParent( this->m_Hwnd ), &pt );

    wsprintf( szReturnValue, "%d %d %d %d", pt.x, pt.y, rc.right-rc.left, rc.bottom-rc.top );
    return TRUE;
  }
  else if ( input.gettok( 3, " " ) == "dpos" ) {

    RECT rc;
    GetWindowRect( this->m_Hwnd, &rc );
    POINT pt;
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient( this->m_pParentDialog->getHwnd( ), &pt );

    wsprintf( szReturnValue, "%d %d %d %d", pt.x, pt.y, rc.right-rc.left, rc.bottom-rc.top );
    return TRUE;
  }
  else if ( input.gettok( 3, " " ) == "mark" ) {

    lstrcpy( szReturnValue, this->m_tsMark.to_chr( ) );
    return TRUE;
  }
  else if ( input.gettok( 3, " " ) == "mouse" ) {

    POINT pt;
    GetCursorPos( &pt );
    ScreenToClient( this->m_Hwnd, &pt );
    wsprintf( szReturnValue, "%d %d", pt.x, pt.y );
    return TRUE;
  }
  else if ( input.gettok( 3, " " ) == "pid" ) {

    char classname[257];
    GetClassName( GetParent( this->m_Hwnd ), classname, 256 );

    if ( lstrcmp( classname, "#32770" ) == 0 )
      lstrcpy( szReturnValue, "0" );
    else
      wsprintf( szReturnValue, "%d",  this->m_pParentDialog->getControlByHWND( GetParent( this->m_Hwnd ) )->getUserID( ) );

    return TRUE;
  }
  else if ( input.gettok( 3, " " ) == "type" ) {

    lstrcpy( szReturnValue, this->getType( ).to_chr( ) );
    return TRUE;
  }
	else if (input.gettok(3, " ") == "font") {
		HFONT hFontControl = this->m_hFont;

//		if (!hFontControl)
//			hFontControl = (HFONT) this->getFont();
		if (!hFontControl)
			hFontControl = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

		if (hFontControl) {
			LOGFONT lfCurrent;

			ZeroMemory(&lfCurrent, sizeof(LOGFONT));
			GetObject(hFontControl, sizeof(LOGFONT), &lfCurrent);
			
			TString str(ParseLogfontToCommand(&lfCurrent));
			wsprintf(szReturnValue, "%s", str.to_chr());
			return TRUE;
		}
  }
	// [NAME] [ID] [PROP]
	else if (input.gettok(3, " ") == "tooltipbgcolor") {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND,TTM_GETTIPBKCOLOR, NULL, NULL);

		wsprintf(szReturnValue, "%ld", cref);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (input.gettok(3, " ") == "tooltiptextcolor") {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL);

		wsprintf(szReturnValue, "%ld", cref);
		return TRUE;
	}
  else
		dcxInfoError("General", input.gettok( 3, " " ).to_chr( ), input.gettok(1, " ").to_chr(), this->getUserID(), "Invalid property or number of arguments");

  return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::registreDefaultWindowProc( ) {
  this->m_DefaultWindowProc = (WNDPROC) SetWindowLong( this->m_Hwnd, GWL_WNDPROC, (LONG) DcxControl::WindowProc );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::unregistreDefaultWindowProc( ) {
	SetWindowLong( this->m_Hwnd, GWL_WNDPROC, (LONG) this->m_DefaultWindowProc );
  this->m_DefaultWindowProc = NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DcxControl::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DcxControl *pthis = (DcxControl*) GetProp(mHwnd, "dcx_cthis");

	// sanity check, see that prop exists.
	if (pthis == NULL)
		return DefWindowProc(mHwnd, uMsg, wParam, lParam);

	bool fBlocked = (InSendMessageEx(NULL) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND;

	if (!fBlocked) {
		// If Message is blocking just call old win proc
		BOOL bParsed = FALSE;

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY)) {
			pthis->incRef( );
		}

		LRESULT lrRes = pthis->PostMessage(uMsg, wParam, lParam, bParsed);

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY)) {
			pthis->decRef();
		}

		if (bParsed)
			return lrRes;
	}

	if (pthis->m_DefaultWindowProc != NULL)
		return CallWindowProc(pthis->m_DefaultWindowProc, mHwnd, uMsg, wParam, lParam);

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

/*!
 * \brief blah
 *
 * blah
 *
 * Input [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
 */

DcxControl * DcxControl::controlFactory( DcxDialog * p_Dialog, const UINT mID, const TString & tsInput, int offset, DWORD mask, HWND hParent ) {

	TString type(tsInput.gettok( offset++, " " ));

	RECT rc;
	rc.left = (LONG)tsInput.gettok( offset++, " " ).to_num( );
	rc.top = (LONG)tsInput.gettok( offset++, " " ).to_num( );
	rc.right = rc.left + (LONG)tsInput.gettok( offset++, " " ).to_num( );
	rc.bottom = rc.top + (LONG)tsInput.gettok( offset, " " ).to_num( );

	TString styles;
	if ( tsInput.numtok( " " ) > offset )
		styles = tsInput.gettok( offset +1, -1, " " );

	if (hParent == NULL)
		hParent = p_Dialog->getHwnd();

	if (( type == "pbar" ) && (mask & CTLF_ALLOW_PBAR))
		return new DcxProgressBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "trackbar" ) && (mask & CTLF_ALLOW_TRACKBAR))
		return new DcxTrackBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "comboex" ) && (mask & CTLF_ALLOW_COMBOEX))
		return new DcxComboEx( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "colorcombo" ) && (mask & CTLF_ALLOW_COLORCOMBO))
		return new DcxColorCombo( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "statusbar" ) && (mask & CTLF_ALLOW_STATUSBAR))
		return new DcxStatusBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "toolbar" ) && (mask & CTLF_ALLOW_TOOLBAR))
		return new DcxToolBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "treeview" ) && (mask & CTLF_ALLOW_TREEVIEW))
		return new DcxTreeView( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "listview" ) && (mask & CTLF_ALLOW_LISTVIEW))
		return new DcxListView( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "rebar" ) && (mask & CTLF_ALLOW_REBAR))
		return new DcxReBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "button" ) && (mask & CTLF_ALLOW_BUTTON))
		return new DcxButton( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "richedit" ) && (mask & CTLF_ALLOW_RICHEDIT))
		return new DcxRichEdit( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "updown" ) && (mask & CTLF_ALLOW_UPDOWN))
		return new DcxUpDown( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "ipaddress" ) && (mask & CTLF_ALLOW_IPADDRESS))
		return new DcxIpAddress( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "webctrl" ) && (mask & CTLF_ALLOW_WEBCTRL))
		return new DcxWebControl( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "calendar" ) && (mask & CTLF_ALLOW_CALANDER))
		return new DcxCalendar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "divider" ) && (mask & CTLF_ALLOW_DIVIDER))
		return new DcxDivider( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "panel" ) && (mask & CTLF_ALLOW_PANEL))
		return new DcxPanel( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "tab" ) && (mask & CTLF_ALLOW_TAB))
		return new DcxTab( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "line" ) && (mask & CTLF_ALLOW_LINE))
		return new DcxLine( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "box" ) && (mask & CTLF_ALLOW_BOX))
		return new DcxBox( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "radio" ) && (mask & CTLF_ALLOW_RADIO))
		return new DcxRadio( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "check" ) && (mask & CTLF_ALLOW_CHECK))
		return new DcxCheck( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "text" ) && (mask & CTLF_ALLOW_TEXT))
		return new DcxText( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "edit" ) && (mask & CTLF_ALLOW_EDIT))
		return new DcxEdit( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "scroll" ) && (mask & CTLF_ALLOW_SCROLL))
		return new DcxScroll( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "list" ) && (mask & CTLF_ALLOW_LIST))
		return new DcxList( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "link" ) && (mask & CTLF_ALLOW_LINK))
		return new DcxLink( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "image" ) && (mask & CTLF_ALLOW_IMAGE))
		return new DcxImage( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "pager" ) && (mask & CTLF_ALLOW_PAGER))
		return new DcxPager( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == "window" ) && (mask & CTLF_ALLOW_DOCK)) {
		if ( tsInput.numtok( " " ) > offset ) {
			char windowHwnd[30];
			TString expression;
			expression.sprintf("$window(%s).hwnd", tsInput.gettok( offset +1, " " ).to_chr( ) );
			mIRCeval( expression.to_chr(), windowHwnd );

			HWND winHwnd = (HWND) atoi( windowHwnd );

			if (IsWindow(winHwnd)) {
				if (p_Dialog->getControlByHWND(winHwnd) == NULL)
					return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
			}
			else
				mIRCDebug("D_ERROR: Docking (No such window %s)", tsInput.gettok(offset +1, " ").to_chr());
		}
	}
	else if ((type == "dialog") && (mask & CTLF_ALLOW_DOCK)) {
		if (tsInput.numtok(" ") > offset) {
			HWND winHwnd = GetHwndFromString(tsInput.gettok(offset +1, " "));

			if (IsWindow(winHwnd)) {
				if (p_Dialog->getControlByHWND(winHwnd) == NULL) {
					DcxControl* newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
					DcxDialog* dlg = dcxDialogs().getDialogByHandle(winHwnd);

					// if its a dcx marked dialog, mark the parent name
					if (dlg != NULL)
						dlg->setParentName(p_Dialog->getName());

					return newDialog;
				}
			}
			else
				mIRCDebug("D_ERROR: Docking (No such dialog %s)", tsInput.gettok(offset +1, " ").to_chr());
		}
	}

	return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setFont( HFONT hFont, BOOL fRedraw ) {
  return SendMessage( this->m_Hwnd, WM_SETFONT, (WPARAM) hFont, (LPARAM) MAKELPARAM(fRedraw, 0) );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::getFont( ) const {
  return SendMessage( this->m_Hwnd, WM_GETFONT, (WPARAM) 0, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setRedraw( BOOL fView ) {
  return SendMessage( this->m_Hwnd, WM_SETREDRAW, (WPARAM) fView, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::setControlFont( HFONT hFont, BOOL fRedraw ) {

  HFONT hControlFont = (HFONT) this->getFont( );

  if ( hControlFont != GetStockObject( DEFAULT_GUI_FONT ) ) {

    if ( hControlFont != NULL ) {
      DeleteObject( hControlFont );
      this->m_hFont = NULL;
    }
    else if ( this->m_hFont != NULL ) {
      DeleteObject( this->m_hFont );
      this->m_hFont = NULL;
    }
  }

  this->setFont( hFont, fRedraw );
  this->m_hFont = hFont;
}

/*!
 * \brief blah
 *
 * blah
 */

HBRUSH DcxControl::getBackClrBrush( ) const {

  return this->m_hBackBrush;
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF DcxControl::getBackColor( ) const {
	return this->m_clrBackText;
}

/*!
 * \brief blah
 *
 * blah
 */

COLORREF DcxControl::getTextColor( ) const {
	return this->m_clrText;
}
