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
#include "dcxdatetime.h"
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

#include "dcxstacker.h"

#ifdef DCX_USE_DXSDK
#include "dcxdirectshow.h"
#endif // DCX_USE_DXSDK

extern mIRCDLL mIRCLink;

/*!
 * \brief Constructor
 *
 * \param mID Control ID
 * \param p_Dialog Parent DcxDialog Object
 */

DcxControl::DcxControl( const UINT mID, DcxDialog * p_Dialog )
: DcxWindow( mID )
, m_pParentDialog( p_Dialog )
, m_hFont(NULL)
, m_tsMark("")
, m_clrText(CLR_NONE)
, m_clrBackText(CLR_NONE)
, m_hBackBrush(NULL)
, m_clrBackground(CLR_NONE)
, m_iRefCount(0)
, m_hCursor(NULL)
, m_bCursorFromFile(FALSE)
, m_bAlphaBlend(false)
//, m_pParentCtrl(NULL)
, m_pParentHWND(NULL)
, m_bitmapBg(NULL)
, m_bShadowText(false)
, m_bCtrlCodeText(true)
, m_bInPrint(false)
, m_hBorderBrush(NULL)
, m_iAlphaLevel(0x7f)
, m_bNoTheme(false)
{
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
  // Delete border brush
  if ( this->m_hBorderBrush != NULL ) {

      DeleteObject( this->m_hBorderBrush );
      this->m_hBorderBrush = NULL;
  }

  if ( this->m_bCursorFromFile && this->m_hCursor != NULL ) {

    //DeleteObject( this->m_hCursor );
    DestroyCursor( this->m_hCursor );
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

  unsigned int i = 1, numtok = styles.numtok( );

	*Styles |= WS_CLIPCHILDREN | WS_VISIBLE;

  while ( i <= numtok ) {

    if ( styles.gettok( i ) == "notheme" )
      *bNoTheme = TRUE;
    else if ( styles.gettok( i ) == "tabstop" )
      *Styles |= WS_TABSTOP;
    else if ( styles.gettok( i ) == "group" )
      *Styles |= WS_GROUP;
    else if ( styles.gettok( i ) == "disabled" )
      *Styles |= WS_DISABLED;
    else if ( styles.gettok( i ) == "transparent" )
      *ExStyles |= WS_EX_TRANSPARENT;
    else if ( styles.gettok( i ) == "hidden" )
      *Styles &= ~WS_VISIBLE;

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
	int numtok = input.numtok( );

	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	if ( flags.switch_flags[5] && numtok > 3 ) {
		LOGFONT lf;

		if (ParseCommandToLogfont(input.gettok(4, -1), &lf)) {
			HFONT hFont = CreateFontIndirect(&lf);
			this->setControlFont(hFont, FALSE);
		}

		this->redrawWindow( );
	}
	// xdid -p [NAME] [ID] [SWITCH] [X] [Y] [W] [H]
	else if ( flags.switch_flags[15] && numtok > 6 ) {

		int x = input.gettok( 4 ).to_int( );
		int y = input.gettok( 5 ).to_int( );
		int w = input.gettok( 6 ).to_int( );
		int h = input.gettok( 7 ).to_int( );

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
		this->parseBorderStyles( input.gettok( 4 ), &Styles, &ExStyles );

		this->addStyle( Styles );
		this->addExStyle( ExStyles );

		SetWindowPos( this->m_Hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED );
		InvalidateRect( this->m_Hwnd, NULL, TRUE );
		SendMessage( this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -C [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	else if ( flags.switch_cap_flags[2] && numtok > 4 ) {
		UINT iFlags = this->parseColorFlags( input.gettok( 4 ) );
		COLORREF clrColor = (COLORREF)input.gettok( 5 ).to_num( );

		if ( iFlags & DCC_BKGCOLOR ) {
			if ( this->m_hBackBrush != NULL ) {
				DeleteObject( this->m_hBackBrush );
				this->m_hBackBrush = NULL;
			}

			if ( clrColor != -1 ) {
				this->m_hBackBrush = CreateSolidBrush( clrColor );
				this->m_clrBackground = clrColor;
			}
			else
				this->m_clrBackground = CLR_NONE;
		}

		if ( iFlags & DCC_TEXTCOLOR )
			this->m_clrText = clrColor;

		if ( iFlags & DCC_TEXTBKGCOLOR )
			this->m_clrBackText = clrColor;

		if ( iFlags & DCC_BORDERCOLOR ) {
			if ( this->m_hBorderBrush != NULL ) {
				DeleteObject( this->m_hBorderBrush );
				this->m_hBorderBrush = NULL;
			}

			if ( clrColor != -1 )
				this->m_hBorderBrush = CreateSolidBrush( clrColor );
		}

		// force a control redraw
		this->redrawWindow( );
	}
	// xdid -F [NAME] [ID] [SWITCH]
	else if (flags.switch_cap_flags[5]) {
		SetFocus(this->m_Hwnd);
	}
	// xdid -J [NAME] [ID] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if ( flags.switch_cap_flags[9] && numtok > 4 ) {
		UINT iFlags = this->parseCursorFlags( input.gettok( 4 ) );
		HCURSOR hCursor = NULL;
		if ( this->m_bCursorFromFile )
			hCursor = this->m_hCursor;
		this->m_hCursor = NULL;
		this->m_bCursorFromFile = FALSE;
		if ( iFlags & DCCS_FROMRESSOURCE )
			this->m_hCursor = LoadCursor( NULL, this->parseCursorType( input.gettok( 5 ) ) );
		else if ( iFlags & DCCS_FROMFILE ) {
			TString filename(input.gettok( 5, -1 ));
			if (IsFile(filename)) {
				this->m_hCursor = (HCURSOR)LoadImage(NULL, filename.to_chr(), IMAGE_CURSOR, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE );
				this->m_bCursorFromFile = TRUE;
			}
			else
				this->showErrorEx(NULL,"-J", "Unable to Access File: %s", filename.to_chr());
		}
		if (this->m_hCursor == NULL)
			this->showError(NULL, "-J", "Unable to Load Cursor");
		if (hCursor != NULL) {
			if (GetCursor() == hCursor) {
				if (this->m_hCursor != NULL)
					SetCursor(this->m_hCursor);
				else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
			DestroyCursor( hCursor );
		}
	}
	// xdid -M [NAME] [ID] [SWITCH] [MARK INFO]
	else if ( flags.switch_cap_flags[12] ) {

		TString info;
		if ( numtok > 3 ) {

			info = input.gettok( 4, -1 );
			info.trim( );
		}
		this->m_tsMark = info;
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if ( flags.switch_cap_flags[25] && numtok > 3 ) {

		int perc = input.gettok( 4 ).to_int( );

		if ( perc >= 0 || perc <= 100 ) {

			int min, max;
			GetScrollRange( this->m_Hwnd, SB_VERT, &min, &max );

			//scrollbar is defined and has visible range
			if ( min != 0 || max != 0 ) {

				int pos = round( (float) ( max - min ) * (float) perc / (float) 100.0 );

				SCROLLINFO si;
				ZeroMemory( &si, sizeof ( SCROLLINFO ) );
				si.cbSize = sizeof( SCROLLINFO );
				GetScrollInfo( this->m_Hwnd, SB_VERT, &si );
				si.nPos = pos;
				SetScrollInfo( this->m_Hwnd, SB_VERT, &si, TRUE );
				SendMessage( this->m_Hwnd, WM_VSCROLL, MAKEWPARAM( SB_THUMBPOSITION, si.nPos ), NULL );
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
	// xdid -h [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if (flags.switch_flags[7]) {
		if (AnimateWindowUx == NULL)
			this->showError(NULL, "-h", "Unsupported By Current OS");
		else {
			if (numtok > 4)
				AnimateWindowUx(this->m_Hwnd,
					input.gettok(5).to_int(),
					(AW_HIDE | DcxDialog::getAnimateStyles(input.gettok(4))) & ~AW_ACTIVATE);
			else
				ShowWindow(this->m_Hwnd, SW_HIDE);

			RECT rc;
			GetClientRect(this->m_pParentDialog->getHwnd(), &rc);
			if (this->m_pParentDialog->updateLayout(rc))
				this->m_pParentDialog->redrawWindow(); // why do we need the redraw?
		}
	}
	// xdid -s [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if ( flags.switch_flags[18] ) {
		if (AnimateWindowUx == NULL)
			this->showError(NULL, "-s", "Unsupported By Current OS");
		else {
			if (numtok > 4) {
				AnimateWindowUx(this->m_Hwnd,
					input.gettok(5).to_int(),
					(AW_ACTIVATE | DcxDialog::getAnimateStyles(input.gettok(4))) & ~AW_HIDE);
			}
			else
				ShowWindow(this->m_Hwnd, SW_SHOW);

			RECT rc;
			GetClientRect(this->m_pParentDialog->getHwnd(), &rc);
			if (this->m_pParentDialog->updateLayout(rc))
				this->m_pParentDialog->redrawWindow(); // why do we need the redraw?
		}
	}
	// xdid -U [NAME] [ID]
	else if (flags.switch_cap_flags[20]) {
		// Box Double click Bug: the GetNextDlgtabItem() function never returns & seems to just loop forever.
		// from functions doc:
		//	If the search for the next control with the WS_TABSTOP
		//	style encounters a window with the WS_EX_CONTROLPARENT style,
		//	the system recursively searches the window's children.
		//
		//HWND hNextCtrl = GetNextDlgTabItem(this->m_pParentDialog->getHwnd(), this->m_Hwnd, FALSE);
        //
		//if (hNextCtrl && (hNextCtrl != this->m_Hwnd))
		//	SendMessage(this->m_pParentDialog->getHwnd(), WM_NEXTDLGCTL, (WPARAM) hNextCtrl, TRUE);
		////::PostMessage(this->m_pParentDialog->getHwnd(), WM_NEXTDLGCTL, NULL, FALSE);
		//else
		//	SetFocus(NULL);

		// Just remove focus from all controls
		SetFocus(NULL);
	}
	// xdid -T [NAME] [ID] [SWITCH] (ToolTipText)
  else if (flags.switch_cap_flags[19] && numtok > 2) {
		this->m_tsToolTip = (numtok > 3 ? input.gettok(4, -1) : "");
		this->m_tsToolTip.trim();
  }
	// xdid -R [NAME] [ID] [SWITCH] [FLAG] [ARGS]
	else if (flags.switch_cap_flags[17] && numtok > 3) {
		TString flag(input.gettok( 4 ));

		if ((flag.len() < 2) || (flag[0] != '+')) {
			this->showError(NULL, "-R", "Invalid Flag");
			return;
		}

		RECT rc;
		HRGN m_Region = NULL;
		int RegionMode = 0;
		bool noRegion = false;
		GetWindowRect(this->m_Hwnd,&rc);

		if (flag.find('o',0))
			RegionMode = RGN_OR;
		else if (flag.find('a',0))
			RegionMode = RGN_AND;
		else if (flag.find('i',0))
			RegionMode = RGN_DIFF;
		else if (flag.find('x',0))
			RegionMode = RGN_XOR;

		if (flag.find('f',0)) // image file - [COLOR] [FILE]
		{
			if (numtok < 6) {
				this->showError(NULL, "-R +f", "Invalid Arguments");
				return;
			}

			COLORREF tCol = (COLORREF)input.gettok( 5 ).to_num();
			HBITMAP m_bitmapBg = dcxLoadBitmap(NULL,input.gettok(6,-1));

			if (m_bitmapBg != NULL) {
				if (flag.find('R',0)) // now resize image to match control.
					m_bitmapBg = DcxControl::resizeBitmap(m_bitmapBg, &rc);
				m_Region = BitmapRegion(m_bitmapBg,tCol,((tCol != -1) ? TRUE : FALSE));

				DeleteBitmap(m_bitmapBg);
			}
			else
				this->showError(NULL,"-R +f", "Unable To Load Image file.");
		}
		else if (flag.find('r',0)) // rounded rect - radius args (optional)
		{
			int radius;

			if (numtok > 4)
				radius = input.gettok( 5 ).to_int();
			else
				radius = 20;

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (flag.find('c',0)) // circle - radius arg (optional)
		{
			if (numtok > 4) {
				int radius = input.gettok( 5 ).to_int();
				if (radius < 1) radius = 100; // handle cases where arg isnt a number or is a negative.
				int cx = ((rc.right - rc.left)/2);
				int cy = ((rc.bottom - rc.top)/2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (flag.find('p',0)) // polygon
		{
			// u need at least 3 points for a shape
			if (numtok < 7) {
				this->showError(NULL, "-R +p", "Invalid Arguments");
				return;
			}

			TString strPoints(input.gettok(5, -1));
			TString strPoint;
			int tPoints = strPoints.numtok( );

			if (tPoints < 1) {
				this->showError(NULL, "-R +p", "Invalid Points");
				return;
			}

			int cnt = 1;
			POINT *pnts = new POINT[tPoints];

			while (cnt <= tPoints) {
				strPoint = strPoints.gettok( cnt );
				pnts[cnt-1].x = (LONG)strPoint.gettok(1, TSCOMMA).to_num();
				pnts[cnt-1].y = (LONG)strPoint.gettok(2, TSCOMMA).to_num();
				cnt++;
			}

			m_Region = CreatePolygonRgn(pnts,tPoints,WINDING);

			delete [] pnts;
		}
		// Doesn't work with child controls :/ (yet)
		//else if (flag.find('s',0)) // shadow - <colour> <sharpness> <darkness> <size> <xoffset> <yoffset>
		//{
		//	if (numtok == 10) {
		//		noRegion = true;
		//		COLORREF s_clr = (COLORREF)input.gettok(5).to_num();
		//		int s_sharp = input.gettok(6).to_int();
		//		int s_dark = input.gettok(7).to_int();
		//		int s_size = input.gettok(8).to_int();
		//		int s_x = input.gettok(9).to_int();
		//		int s_y = input.gettok(10).to_int();
		//		if (!CWndShadow::isShadowed(this->m_Hwnd))
		//			this->m_oShadow.Create(this->m_Hwnd);
		//		this->m_oShadow.SetColor(s_clr);
		//		this->m_oShadow.SetSharpness(s_sharp);
		//		this->m_oShadow.SetDarkness(s_dark);
		//		this->m_oShadow.SetSize(s_size);
		//		this->m_oShadow.SetPosition(s_x,s_y);
		//	}
		//	else {
		//		DCXError("xdid -R +s", "Invalid Args");
		//		return;
		//	}
		//}
		else if (flag.find('b',0)) { // alpha [1|0] [level]
			noRegion = true;
			if (numtok != 6) {
				this->showError(NULL, "-R +b", "Invalid Args");
				return;
			}
			if (input.gettok( 5 ).to_int() > 0)
				this->m_bAlphaBlend = true;
			else
				this->m_bAlphaBlend = false;

			int alpha = input.gettok( 6 ).to_int();

			if (alpha > 255 || alpha < 0)
				alpha = 255;

			if (alpha == 255)
				this->m_bAlphaBlend = false;
			this->m_iAlphaLevel = alpha;
		}
		else if (flag.find('n',0)) { // none, no args
			noRegion = true;
			SetWindowRgn(this->m_Hwnd,NULL,TRUE);
		}
		else
			this->showError(NULL, "-R", "Invalid Flag");

		if (!noRegion) {
			if (m_Region != NULL) {
				if (RegionMode != 0) {
					HRGN wrgn = CreateRectRgn(0,0,0,0);
					if (wrgn != NULL) {
						if (GetWindowRgn(this->m_Hwnd,wrgn) != ERROR)
							CombineRgn(m_Region,m_Region,wrgn,RegionMode);
						DeleteObject(wrgn);
					}
				}
				SetWindowRgn(this->m_Hwnd,m_Region,TRUE);
			}
			else
				this->showError(NULL, "-R", "Unable to create region.");
		}
		this->redrawWindow();
	}
	// invalid command
	else
		this->showError(NULL, input.gettok( 3 ).to_chr(), "Invalid Command");
}

HBITMAP DcxControl::resizeBitmap(HBITMAP srcBM, const LPRECT rc)
{
	// set result to original bitmap incase resize fails at some point.
	HBITMAP hRes = srcBM;
	// create temp hdc to render from
	HDC srcDC = CreateCompatibleDC(NULL);
	if (srcDC != NULL) {
		// select bitmap into new hdc
		HBITMAP oldSrcBm = (HBITMAP)SelectObject(srcDC, srcBM);
		// create temp dest hdc to render into.
		HDC destDC = CreateCompatibleDC(srcDC);
		if (destDC != NULL) {
			int w = (rc->right - rc->left), h = (rc->bottom - rc->top);
			// create dest bitmap.
			HBITMAP newBM = CreateCompatibleBitmap(srcDC,w,h);
			if (newBM != NULL) {
				// select dest bitmap into dest hdc
				HBITMAP oldDestBm = (HBITMAP)SelectObject(destDC, newBM);
				// get source bitmap info.
				BITMAP bm;
				GetObject(srcBM,sizeof(bm),&bm);
				// resize bitmap, render into dest hdc/bitmap
				if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY)) {
					GdiFlush();
					// set the return bitmap if resize worked.
					hRes = newBM;
					// set dest hdc back to orig bitmap.
					SelectObject(destDC, oldDestBm);
				}
				else
					DeleteBitmap(SelectObject(destDC, oldDestBm)); // resize failed, delete dest bitmap.
			}
			// delete dest hdc
			DeleteDC(destDC);
		}
		// set source hdc back to orig bitmap.
		SelectObject(srcDC, oldSrcBm);
		// delete source hdc
		DeleteDC(srcDC);
	}
	// if result != original bitmap, delete original bitmap.
	if (hRes != srcBM)
		DeleteBitmap(srcBM);
	return hRes;
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
		else if ( flags[i] == 'r' )
			iFlags |= DCC_BORDERCOLOR;

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

	TString prop(input.gettok( 3 ));

	if ( prop == "hwnd" ) {
		wsprintf( szReturnValue, "%d", this->m_Hwnd );
		return TRUE;
	}
	else if ( prop == "visible" ) {
		if ( IsWindowVisible( this->m_Hwnd ) )
			lstrcpy( szReturnValue, "$true" );
		else
			lstrcpy( szReturnValue, "$false" );
		return TRUE;
	}
	else if ( prop == "enabled" ) {
		if ( IsWindowEnabled( this->m_Hwnd ) )
			lstrcpy( szReturnValue, "$true" );
		else
			lstrcpy( szReturnValue, "$false" );
		return TRUE;    
	}
	else if ( prop == "pos" ) {
		RECT rc;
		GetWindowRect( this->m_Hwnd, &rc );
		MapWindowPoints(NULL, GetParent( this->m_Hwnd ), (LPPOINT)&rc, 2);

		wsprintf( szReturnValue, "%d %d %d %d", rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
		return TRUE;
	}
	else if ( prop == "dpos" ) {
		RECT rc;
		GetWindowRect( this->m_Hwnd, &rc );
		MapWindowPoints(NULL, this->m_pParentDialog->getHwnd( ), (LPPOINT)&rc, 2);

		wsprintf( szReturnValue, "%d %d %d %d", rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
		return TRUE;
	}
	else if ( prop == "mark" ) {
		lstrcpyn( szReturnValue, this->m_tsMark.to_chr( ), 900 );
		return TRUE;
	}
	else if ( prop == "mouse" ) {
		POINT pt;
		GetCursorPos( &pt );
		MapWindowPoints(NULL, this->m_Hwnd, &pt, 1);
		wsprintf( szReturnValue, "%d %d", pt.x, pt.y );
		return TRUE;
	}
	else if ( prop == "pid" ) {
		char classname[257];
		GetClassName( GetParent( this->m_Hwnd ), classname, 256 );

		if ( lstrcmp( classname, "#32770" ) == 0 )
			lstrcpy( szReturnValue, "0" );
		else
			wsprintf( szReturnValue, "%d",  this->m_pParentDialog->getControlByHWND( GetParent( this->m_Hwnd ) )->getUserID( ) );

		return TRUE;
	}
	else if ( prop == "type" ) {

		lstrcpyn( szReturnValue, this->getType( ).to_chr( ), 900 );
		return TRUE;
	}
	else if ( prop == "font") {
		HFONT hFontControl = this->m_hFont;

//		if (!hFontControl)
//			hFontControl = (HFONT) this->getFont();
		if (!hFontControl)
			hFontControl = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

		if (hFontControl) {
			LOGFONT lfCurrent;

			ZeroMemory(&lfCurrent, sizeof(LOGFONT));
			GetObject(hFontControl, sizeof(LOGFONT), &lfCurrent);

			//TString str(ParseLogfontToCommand(&lfCurrent));
			//wsprintf(szReturnValue, "%s", str.to_chr());
			lstrcpyn(szReturnValue, ParseLogfontToCommand(&lfCurrent).to_chr(), 900);
			return TRUE;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == "tooltipbgcolor") {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND,TTM_GETTIPBKCOLOR, NULL, NULL);

		wsprintf(szReturnValue, "%ld", cref);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == "tooltiptextcolor") {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL);

		wsprintf(szReturnValue, "%ld", cref);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == "alpha") {
		if (this->m_bAlphaBlend)
			lstrcpy(szReturnValue, "$true");
		else
			lstrcpy(szReturnValue, "$false");
	}
	// [NAME] [ID] [PROP]
	else if (prop == "textcolor") {
		wsprintf(szReturnValue, "%d", this->m_clrText);
	}
	// [NAME] [ID] [PROP]
	else if (prop == "textbgcolor") {
		wsprintf(szReturnValue, "%d", this->m_clrBackText);
	}
	// [NAME] [ID] [PROP]
	else if (prop == "bgcolor") {
		wsprintf(szReturnValue, "%d", this->m_clrBackground);
	}
	else
		this->showError(prop.to_chr(),NULL,"Invalid property or number of arguments");

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::registreDefaultWindowProc( ) {
  this->m_DefaultWindowProc = (WNDPROC) SetWindowLongPtr( this->m_Hwnd, GWLP_WNDPROC, (LONG_PTR) DcxControl::WindowProc );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::unregistreDefaultWindowProc( ) {
	if (this->m_DefaultWindowProc != NULL) // implies this has alrdy been called.
		SetWindowLongPtr( this->m_Hwnd, GWLP_WNDPROC, (LONG_PTR) this->m_DefaultWindowProc );
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

	if (uMsg == WM_PAINT && pthis->m_pParentDialog->IsVistaStyle()) {
		ValidateRect(mHwnd, NULL);
		RECT rcUpdate;
		GetWindowRect(mHwnd, &rcUpdate);
		//RECT rcParent;
		//GetClientRect(GetParent(mHwnd), &rcParent);
		//MapWindowRect(GetParent(mHwnd), NULL, &rcParent);
		//rcUpdate.left = max(rcUpdate.left, rcParent.left);
		//rcUpdate.top = max(rcUpdate.top, rcParent.top);
		//rcUpdate.right = min(rcUpdate.right, rcParent.right);
		//rcUpdate.bottom = min(rcUpdate.bottom, rcParent.bottom);
		pthis->m_pParentDialog->UpdateVistaStyle(&rcUpdate);
		return 0L;
	}

	bool fBlocked = false;
	if (InSendMessageExUx != NULL)
		fBlocked = (InSendMessageExUx(NULL) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND;

	LRESULT lrRes = 0L;

	if (!fBlocked) {
		// If Message is blocking just call old win proc
		BOOL bParsed = FALSE;

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->incRef( );

		lrRes = pthis->PostMessage(uMsg, wParam, lParam, bParsed);

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->decRef();

		if (bParsed)
			return lrRes;
	}

	if (pthis->m_DefaultWindowProc != NULL)
		lrRes = CallWindowProc(pthis->m_DefaultWindowProc, mHwnd, uMsg, wParam, lParam);
	else
		lrRes = DefWindowProc(mHwnd, uMsg, wParam, lParam);

	return lrRes;
}

/*!
 * \brief blah
 *
 * blah
 *
 * Input [NAME] [SWITCH] [ID] [CONTROL] [X] [Y] [W] [H] (OPTIONS)
 */

DcxControl * DcxControl::controlFactory( DcxDialog * p_Dialog, const UINT mID, const TString & tsInput, int offset, const UINT64 mask, HWND hParent ) {

	TString type(tsInput.gettok( offset++ ));

	RECT rc;
	rc.left = (LONG)tsInput.gettok( offset++ ).to_num( );
	rc.top = (LONG)tsInput.gettok( offset++ ).to_num( );
	rc.right = rc.left + (LONG)tsInput.gettok( offset++ ).to_num( );
	rc.bottom = rc.top + (LONG)tsInput.gettok( offset ).to_num( );

	TString styles;
	if ( tsInput.numtok( ) > offset )
		styles = tsInput.gettok( offset +1, -1);

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
	else if (( type == "datetime" ) && (mask & CTLF_ALLOW_CALANDER))
		return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
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
	else if (( type == "stacker" ) && (mask & CTLF_ALLOW_STACKER))
		return new DcxStacker( mID, p_Dialog, hParent, &rc, styles );
	//else if (( type == "mci" ) && (mask & CTLF_ALLOW_DIRECTSHOW))
	//	return new DcxMci( mID, p_Dialog, hParent, &rc, styles );

#ifdef DCX_USE_DXSDK

	else if (( type == "directshow" ) && (mask & CTLF_ALLOW_DIRECTSHOW))
		return new DcxDirectshow( mID, p_Dialog, hParent, &rc, styles );

#endif // DCX_USE_DXSDK

	else if (( type == "window" ) && (mask & CTLF_ALLOW_DOCK)) {
		if ( tsInput.numtok( ) > offset ) {
			HWND winHwnd = (HWND)tsInput.gettok( offset +1 ).to_num();
			if (!IsWindow(winHwnd)) {
				char windowHwnd[30];

				mIRCevalEX(windowHwnd, 30, "$window(%s).hwnd", tsInput.gettok( offset +1 ).to_chr( ) );

				winHwnd = (HWND) atoi( windowHwnd );
			}

			if (IsWindow(winHwnd)) {
				if (p_Dialog->getControlByHWND(winHwnd) == NULL)
					return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
			}
			else
				mIRCDebug("D_ERROR: Docking (No such window %s)", tsInput.gettok( offset +1 ).to_chr());
		}
	}
	else if ((type == "dialog") && (mask & CTLF_ALLOW_DOCK)) {
		if (tsInput.numtok( ) > offset) {
			HWND winHwnd = GetHwndFromString(tsInput.gettok( offset +1 ));

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
				mIRCDebug("D_ERROR: Docking (No such dialog %s)", tsInput.gettok( offset +1 ).to_chr());
		}
	}

	return NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT DcxControl::setFont( const HFONT hFont, const BOOL fRedraw ) {
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

LRESULT DcxControl::setRedraw( const BOOL fView ) {
  return SendMessage( this->m_Hwnd, WM_SETREDRAW, (WPARAM) fView, (LPARAM) 0 );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::setControlFont( const HFONT hFont, const BOOL fRedraw ) {

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

void DcxControl::updateParentCtrl(void)
{
	// find the host control, if any.
	this->m_pParentHWND = GetParent(this->m_Hwnd);
}

void DcxControl::DrawCtrlBackground(const HDC hdc, const DcxControl *p_this, const LPRECT rwnd)
{
	// fill background.
	if (!p_this->isExStyle(WS_EX_TRANSPARENT)) {
		HBRUSH hBrush = p_this->getBackClrBrush();
		if (hBrush == NULL)
			hBrush = GetSysColorBrush(COLOR_3DFACE);
		if ( hBrush != NULL )
			FillRect( hdc, rwnd, hBrush );
	}
}

void DcxControl::DrawControl(HDC hDC, HWND hwnd)
{
	// if window matches this one, don't draw (loop condition)
	if (hwnd == this->m_Hwnd)
		return;

	// if window isn't visible, don't draw.
	if (!IsWindowVisible(hwnd))
		return;

	// if window is within a background paint of it's own, don't draw. (loop condition)
	DcxControl *p_ctrl = this->m_pParentDialog->getControlByHWND(hwnd);
	if (p_ctrl != NULL && p_ctrl->m_bInPrint)
		return;

	RECT rc;
	GetWindowRect(hwnd, &rc);
	MapWindowRect(NULL,GetParent(hwnd),&rc);

	// if window isn't within the client area of the control who's background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;

	HDC hdcMemory = ::CreateCompatibleDC(hDC);

	if (hdcMemory != NULL) {
		int w = (rc.right - rc.left), h = (rc.bottom - rc.top);
		HBITMAP hBitmap = ::CreateCompatibleBitmap( hDC, w, h);

		if (hBitmap != NULL) {
			HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hBitmap);

			::SendMessage( hwnd, WM_ERASEBKGND, (WPARAM)hdcMemory,1L); // HACK: using 1L instead of NULL as a workaround for stacker.
			::SendMessage( hwnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/);

			BitBlt( hDC, rc.left, rc.top, w, h, hdcMemory, 0, 0, SRCCOPY);

			::DeleteObject(::SelectObject( hdcMemory, hbmpOld));
		}
		::DeleteDC(hdcMemory);
	}
}
void DcxControl::DrawParentsBackground(const HDC hdc, const LPRECT rcBounds, const HWND dHwnd)
{
	// fill in parent bg
	RECT rcClient;
	HWND hwnd = this->m_Hwnd;

	if (dHwnd != NULL)
		hwnd = dHwnd;

	if (rcBounds == NULL)
		GetClientRect( hwnd, &rcClient ); // get controls client area
	else
		rcClient = *rcBounds;

	// if themes are active use them.
//	if (dcxIsThemeActive() && !this->m_bNoTheme) {
//#ifdef DCX_USE_WINSDK
//		if (DrawThemeParentBackgroundExUx != NULL)
//			DrawThemeParentBackgroundExUx(hwnd, hdc, 0, &rcClient); // Vista only, does basicly the same as below.
//		else
//#endif
//			DrawThemeParentBackgroundUx(hwnd, hdc, &rcClient); // XP+
//		return;
//	}
	/*
		The following code draws the parents background & client area,
		followed by all child controls covered by this one.
	*/
	this->updateParentCtrl(); // find the host control, if any.
	//If in Vista mode
	if (this->m_pParentDialog->IsVistaStyle())
	{
		// Check if the hdc to render too is the main hdc, if so bkg is already drawn so just return
		if (hdc == this->m_pParentDialog->GetVistaHDC())
			return;
		// Check if parent is dialog.
		if (this->m_pParentHWND == this->m_pParentDialog->getHwnd())
		{
			// When in vista mode dialog has already been drawn
			// So just grab image from windows DC.
			HDC hdcParent = this->m_pParentDialog->GetVistaHDC();
			RECT rcWin = rcClient;
			this->m_pParentDialog->MapVistaRect(hwnd, &rcWin);
			BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
				hdcParent, rcWin.left, rcWin.top, SRCCOPY);
			return;
		}
	}
	if (this->m_pParentDialog->isExStyle(WS_EX_COMPOSITED))
	{
		// When in composited mode underling controls have already been drawn
		// So just grab image from windows DC.
		HDC hdcParent = GetDC(this->m_pParentHWND);
		RECT rcWin = rcClient;
		MapWindowRect(hwnd,this->m_pParentHWND, &rcWin);
		BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
			hdcParent, rcWin.left, rcWin.top, SRCCOPY);
		ReleaseDC(this->m_pParentHWND, hdcParent);
		return;
	}
	//// make a new HDC for background rendering
	//HDC hdcbkg = CreateCompatibleDC( hdc );
	//if (hdcbkg != NULL) {
	//	//RECT rcParentWin, rcParentClient, rcWin;
	//	RECT rcParentWin, rcWin;
	//	// get parent windows client area.
	//	GetClientRect(this->m_pParentHWND,&rcParentWin);
	//	// make a bitmap for rendering to.
	//	HBITMAP memBM = CreateCompatibleBitmap ( hdc, rcParentWin.right - rcParentWin.left, rcParentWin.bottom - rcParentWin.top );
	//	if (memBM != NULL) {
	//		// get this controls x & y pos within its parent.
	//		rcWin = rcClient;
	//		MapWindowPoints(hwnd,this->m_pParentHWND, (LPPOINT)&rcWin, 2);
	//		// associate bitmap with HDC
	//		HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, memBM );
	//		HRGN clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
	//		if (clipRgn != NULL) {
	//			SelectClipRgn(hdcbkg, clipRgn);
	//			DeleteObject(clipRgn);
	//		}
	//		// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
	//		this->m_bInPrint = true; // this helps prevent long drawing loops
	//		// fill in the parents image
	//		::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)hdcbkg,1L); // HACK: using 1L instead of NULL as a workaround for stacker.
	//		::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)hdcbkg,PRF_CLIENT);
	//		// now draw all child controls within area of this control.
	//		// NB: AVOID EnumChildWindows()
	//		HWND child = GetWindow(this->m_Hwnd, GW_HWNDPREV);
	//		while (child != NULL) {
	//			this->DrawControl(hdcbkg, child);
	//			child = GetWindow(child, GW_HWNDPREV);
	//		}
	//		this->m_bInPrint = false;
	//		// draw background to main hdc
	//		BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
	//			hdcbkg, rcWin.left, rcWin.top, SRCCOPY);
	//		DeleteObject(SelectObject( hdcbkg, oldBM ));
	//	}
	//	DeleteDC( hdcbkg );
	//}
	// make a new HDC for background rendering
	RECT rcParentWin, rcWin;
	GetClientRect(this->m_pParentHWND,&rcParentWin);

	HDC *hdcbkg = CreateHDCBuffer(hdc, &rcParentWin);
	if (hdcbkg != NULL) {
		// get this controls x & y pos within its parent.
		rcWin = rcClient;
		MapWindowRect(hwnd,this->m_pParentHWND, &rcWin);
		HRGN clipRgn = CreateRectRgnIndirect(&rcWin); // clip parents drawing to this controls rect.
		if (clipRgn != NULL) {
			SelectClipRgn(*hdcbkg, clipRgn);
			DeleteObject(clipRgn);
		}
		// Sending WM_ERASEBKGND followed by WM_PRINTCLIENT emulates the method used by DrawThemeParentBackgroundEx() on vista.
		this->m_bInPrint = true; // this helps prevent long drawing loops
		// fill in the parents image
		::SendMessage(this->m_pParentHWND, WM_ERASEBKGND, (WPARAM)*hdcbkg,1L); // HACK: using 1L instead of NULL as a workaround for stacker.
		::SendMessage(this->m_pParentHWND, WM_PRINTCLIENT, (WPARAM)*hdcbkg,PRF_CLIENT);
		// now draw all child controls within area of this control.
		// NB: AVOID EnumChildWindows()
		HWND child = GetWindow(this->m_Hwnd, GW_HWNDPREV);
		while (child != NULL) {
			this->DrawControl(*hdcbkg, child);
			child = GetWindow(child, GW_HWNDPREV);
		}
		this->m_bInPrint = false;
		// draw background to main hdc
		BitBlt( hdc, rcClient.left, rcClient.top, (rcClient.right - rcClient.left), (rcClient.bottom - rcClient.top),
			*hdcbkg, rcWin.left, rcWin.top, SRCCOPY);

		DeleteHDCBuffer(hdcbkg);
	}
}
LPALPHAINFO DcxControl::SetupAlphaBlend(HDC *hdc, const bool DoubleBuffer)
{
	if ((hdc == NULL) || (*hdc == NULL) || (!this->m_bAlphaBlend && !DoubleBuffer))
		return NULL;

	LPALPHAINFO ai = new ALPHAINFO;
	ZeroMemory(ai,sizeof(ALPHAINFO));
	/*
		1: draw parents bg to temp hdc
		2: copy bg to temp hdcbkg for later alpha
		3: draw button to temp hdc, over parents bg
		4: alpha blend temp hdc to hdc
	*/
#ifdef DCX_USE_WINSDK
	if (BeginBufferedPaintUx && EndBufferedPaintUx) {
		BP_PAINTPARAMS paintParams = {0};
		paintParams.cbSize = sizeof(paintParams);
		BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)this->m_iAlphaLevel, 0 }; // 0x7f half of 0xff = 50% transparency
		if (this->m_bAlphaBlend)
			paintParams.pBlendFunction = &bf;

		GetClientRect(this->m_Hwnd,&ai->ai_rcClient);
		ai->ai_Buffer = BeginBufferedPaintUx(*hdc, &ai->ai_rcClient, BPBF_COMPATIBLEBITMAP, &paintParams, &ai->ai_hdc);
		if (ai->ai_Buffer != NULL) {
			*hdc = ai->ai_hdc;
			return ai;
		}
	}
	// if vista method failed, fall through to our own method.
#endif
	//{
	//	// create a new HDC for alpha blending.
	//	ai->ai_hdc = CreateCompatibleDC( *hdc );
	//	ai->ai_bkg = NULL;
	//	if (ai->ai_hdc != NULL) {
	//		//GetClientRect(this->m_Hwnd,&ai->ai_rcWin);
	//		GetWindowRect(this->m_Hwnd,&ai->ai_rcWin);
	//		// create a bitmap to render to
	//		ai->ai_bitmap = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//		if (ai->ai_bitmap != NULL) {
	//			GetClientRect(this->m_Hwnd,&ai->ai_rcClient);
	//			// associate bitmap with hdc
	//			ai->ai_oldBM = (HBITMAP)SelectObject ( ai->ai_hdc, ai->ai_bitmap );
	//			// fill in parent bg
	//			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
	//			// If alpha blending, make a background bitmap & fill it.
	//			if (this->m_bAlphaBlend) {
	//				// avoid doing the whole background rendering again by simply copying the one we just did.
	//				HDC hdcbkg = CreateCompatibleDC( *hdc );
	//				if (hdcbkg != NULL) {
	//					ai->ai_bkg = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
	//					if (ai->ai_bkg != NULL) {
	//						HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );

	//						BitBlt( hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

	//						SelectObject(hdcbkg, oldBM);
	//					}
	//					DeleteDC(hdcbkg);
	//				}
	//			}
	//			ai->ai_Oldhdc = *hdc;
	//			*hdc = ai->ai_hdc;
	//		}
	//	}
	//}
	{
		// get window rect
		GetWindowRect(this->m_Hwnd,&ai->ai_rcWin);
		// create a new HDC Buffer for alpha blending.
		ai->ai_hdcBuffer = CreateHDCBuffer( *hdc, &ai->ai_rcWin );
		ai->ai_bkg = NULL;
		if (ai->ai_hdcBuffer != NULL) {
			// assign hdc for easy refrence & compat with previous code.
			ai->ai_hdc = *ai->ai_hdcBuffer;
			// get windows client rect
			GetClientRect(this->m_Hwnd,&ai->ai_rcClient);
			// fill in parent bg
			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
			// If alpha blending, make a background bitmap & fill it.
			if (this->m_bAlphaBlend) {
				// avoid doing the whole background rendering again by simply copying the one we just did.
				HDC hdcbkg = CreateCompatibleDC( *hdc );
				if (hdcbkg != NULL) {
					ai->ai_bkg = CreateCompatibleBitmap ( *hdc, ai->ai_rcWin.right - ai->ai_rcWin.left, ai->ai_rcWin.bottom - ai->ai_rcWin.top );
					if (ai->ai_bkg != NULL) {
						HBITMAP oldBM = SelectBitmap( hdcbkg, ai->ai_bkg );

						BitBlt( hdcbkg, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

						SelectBitmap(hdcbkg, oldBM);
					}
					DeleteDC(hdcbkg);
				}
			}
			ai->ai_Oldhdc = *hdc;
			*hdc = ai->ai_hdc;
		}
	}
	return ai;
}
void DcxControl::FinishAlphaBlend(LPALPHAINFO ai)
{
	if (ai == NULL)
		return;

#ifdef DCX_USE_WINSDK
	if (EndBufferedPaintUx && ai->ai_Buffer != NULL) {
		EndBufferedPaintUx(ai->ai_Buffer, TRUE);
		return;
	}
#endif
	//// if we can't do Vista method, try do our own
	//if (ai->ai_hdc != NULL) {
	//	if (ai->ai_bitmap != NULL) {
	//		int w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top);
	//		if (this->m_bAlphaBlend) {
	//			if (ai->ai_bkg != NULL) {
	//				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
	//				HDC hdcbkg = CreateCompatibleDC( ai->ai_Oldhdc);
	//				if (hdcbkg != NULL) {
	//					// associate bitmap with hdc
	//					HBITMAP oldBM = (HBITMAP)SelectObject ( hdcbkg, ai->ai_bkg );
	//					// alpha blend finished button with parents background
	//					BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)this->m_iAlphaLevel, 0 }; // 0x7f half of 0xff = 50% transparency
	//					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h,bf);
	//					// draw final image to windows hdc.
	//					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

	//					SelectObject( hdcbkg, oldBM);
	//					DeleteDC(hdcbkg);
	//				}
	//				DeleteBitmap(ai->ai_bkg);
	//			}
	//		}
	//		else
	//			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

	//		SelectObject (ai->ai_hdc, ai->ai_oldBM );
	//		DeleteObject(ai->ai_bitmap);
	//	}
	//	DeleteDC( ai->ai_hdc );
	//}
	//delete ai;
	// if we can't do Vista method, try do our own
	if (ai->ai_hdcBuffer != NULL) {
		int w = (ai->ai_rcClient.right - ai->ai_rcClient.left), h = (ai->ai_rcClient.bottom - ai->ai_rcClient.top);
		if (this->m_bAlphaBlend) {
			if (ai->ai_bkg != NULL) {
				// create a new HDC for alpha blending. (doing things this way avoids any flicker)
				HDC hdcbkg = CreateCompatibleDC( ai->ai_Oldhdc);
				if (hdcbkg != NULL) {
					// associate bitmap with hdc
					HBITMAP oldBM = SelectBitmap( hdcbkg, ai->ai_bkg );
					// alpha blend finished button with parents background
					BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)this->m_iAlphaLevel, 0 }; // 0x7f half of 0xff = 50% transparency
					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h,bf);
					// draw final image to windows hdc.
					BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,hdcbkg,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

					SelectBitmap( hdcbkg, oldBM);
					DeleteDC(hdcbkg);
				}
				DeleteBitmap(ai->ai_bkg);
			}
		}
		else
			BitBlt(ai->ai_Oldhdc,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc,ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);

		DeleteHDCBuffer(ai->ai_hdcBuffer);
	}
	delete ai;
}
void DcxControl::showError(const char *prop, const char *cmd, const char *err)
{
	if (this->m_pParentDialog->IsVerbose()) {
		TString res;
		if (prop != NULL)
			res.sprintf("D_IERROR %s(%s, %d).%s: %s", this->getType().to_chr(), this->m_pParentDialog->getName().to_chr(), this->getUserID(), prop, err);
		else
			res.sprintf("D_CERROR (%s) xdid %s %s %d: %s", this->getType().to_chr(), cmd, this->m_pParentDialog->getName().to_chr(), this->getUserID(), err);
		mIRCError(res.to_chr());
	}

	if (this->m_pParentDialog->getAliasName().len() > 0)
		this->callAliasEx(NULL, "error,%d,%s,%s,%s,%s", this->getUserID(), this->getType().to_chr(), (prop != NULL ? prop : "none"), (cmd != NULL ? cmd : "none"), err);
}
void DcxControl::showErrorEx(const char *prop, const char *cmd, const char *fmt, ...)
{
	va_list args;
	va_start( args, fmt );

	int cnt = _vscprintf(fmt, args);
	char *txt = new char[cnt +1];
	vsprintf(txt, fmt, args );
	this->showError(prop, cmd, txt);
	delete [] txt;

	va_end( args );
}

LRESULT DcxControl::CommonMessage( const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_HELP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
					this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
				bParsed = TRUE;
				lRes = TRUE;
			}
			break;

		case WM_SETCURSOR:
			{
				if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd && this->m_hCursor != NULL ) {
					if (GetCursor() != this->m_hCursor)
						SetCursor( this->m_hCursor );
					bParsed = TRUE;
					return TRUE;
				}
			}
			break;

		//case WM_SETCURSOR:
		//	{
		//		if ( LOWORD( lParam ) == HTCLIENT && (HWND) wParam == this->m_Hwnd ) {
		//			if (this->m_hCursor != NULL) {
		//				if (GetCursor() != this->m_hCursor)
		//					SetCursor( this->m_hCursor );
		//			}
		//			else
		//				SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		//			bParsed = TRUE;
		//			return TRUE;
		//		}
		//	}
		//	break;

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

		case WM_CTLCOLORDLG:
			{
				bParsed = TRUE;
				return (INT_PTR) this->getBackClrBrush( );
			}
			break;

		case WM_CTLCOLORBTN:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
			{

				DcxControl * p_Control = this->m_pParentDialog->getControlByHWND( (HWND) lParam );

				if ( p_Control != NULL ) {

					COLORREF clrText = p_Control->getTextColor( );
					COLORREF clrBackText = p_Control->getBackColor( );
					HBRUSH hBackBrush = p_Control->getBackClrBrush( );

					bParsed = TRUE;
					lRes = CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, wParam, lParam);

					if ( clrText != -1 )
						SetTextColor( (HDC) wParam, clrText );

					if ( clrBackText != -1 )
						SetBkColor( (HDC) wParam, clrBackText );

					if (p_Control->isExStyle(WS_EX_TRANSPARENT)) {
						// when transparent set as no bkg brush & default transparent drawing.
						SetBkMode((HDC) wParam, TRANSPARENT);
						hBackBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
					}

					if ( hBackBrush != NULL )
						lRes = (LRESULT) hBackBrush;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "lbdown", this->getUserID( ) );
			}
			break;

		case WM_LBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					this->callAliasEx( NULL, "%s,%d", "lbup", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "sclick", this->getUserID( ) );
				}
			}
			break;

		case WM_LBUTTONDBLCLK:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
					this->callAliasEx( NULL, "%s,%d", "lbdblclk", this->getUserID( ) );
				}
			}
			break;

		case WM_RBUTTONDOWN:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "rbdown", this->getUserID( ) );
			}
			break;

		case WM_RBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "rbup", this->getUserID( ) );
			}
			break;

		case WM_RBUTTONDBLCLK:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "rdclick", this->getUserID( ) );
			}
			break;

		case WM_CONTEXTMENU:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID( ) );
			}
			break;
		case WM_DROPFILES:
		{
			HDROP files = (HDROP) wParam;
			char filename[500];
			int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

			if (count) {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_DRAG) {
					char ret[20];

					this->callAliasEx(ret, "%s,%d,%d", "dragbegin", this->getUserID(), count);

					// cancel drag drop event
					if (lstrcmpi(ret, "cancel") == 0) {
						DragFinish(files);
						return 0L;
					}

					// for each file, send callback message
					for (int i = 0; i < count; i++) {
						if (DragQueryFile(files, i, filename, 500))
							this->callAliasEx(NULL, "%s,%d,%s", "dragfile", this->getUserID(), filename);
					}

					this->callAliasEx(NULL, "%s,%d", "dragfinish", this->getUserID());
				}
			}

			DragFinish(files);
			break;
		}
		case WM_NOTIFY:
			{
				LPNMHDR hdr = (LPNMHDR) lParam;

				if (!hdr)
					break;

				switch( hdr->code ) {
					case TTN_GETDISPINFO:
						{
							if (this->m_tsToolTip.len() > 0) {
								LPNMTTDISPINFO di = (LPNMTTDISPINFO)lParam;
								di->lpszText = this->m_tsToolTip.to_chr();
								di->hinst = NULL;
								bParsed = TRUE;
							}
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
			// Default WM_PRINTCLIENT method that handles alpha for us.
			// This Message is required for AnimateWindow() to work (also used by new transparency/alpha code)
		case WM_PRINTCLIENT:
			{
				HDC hdc = (HDC)wParam;

				bParsed = TRUE;

				// Setup alpha blend if any.
				LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

				lRes = CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );

				this->FinishAlphaBlend(ai);
			}
			break;
		case WM_PRINT:
			{
				if (this->m_bInPrint) // avoid a drawing loop.
					bParsed = TRUE;
			}
			break;

		// redraw the control if the theme has changed
		case WM_THEMECHANGED:
			this->redrawWindow();
			break;

		case WM_CLOSE:
			{
				if (GetKeyState(VK_ESCAPE) != 0) // don't allow the window to close if escape is pressed. Needs looking into for a better method.
					bParsed = TRUE;
			}
			break;

		//case WM_WINDOWPOSCHANGING:
		//{
		//	if (this->m_pParentDialog->getEventMask() & DCX_EVENT_MOVE) {
		//		WINDOWPOS *wp = (WINDOWPOS *) lParam;

		//		// break if nomove & nosize specified, since thats all we care about.
		//		if ((wp->flags & SWP_NOMOVE) && (wp->flags & SWP_NOSIZE))
		//			break;

		//		char ret[256];

		//		this->callAliasEx(ret, "changing,%d,%d,%d,%d,%d,%d", this->getUserID(), (wp->flags & 3),wp->x, wp->y, wp->cx, wp->cy);

		//		if (wp != NULL) {
		//			if (lstrcmp("nosize", ret) == 0)
		//				wp->flags |= SWP_NOSIZE;
		//			else if (lstrcmp("nomove", ret) == 0)
		//				wp->flags |= SWP_NOMOVE;
		//			else if (lstrcmp("nochange", ret) == 0)
		//				wp->flags |= SWP_NOSIZE | SWP_NOMOVE;
		//		}
		//	}
		//	break;
		//}
	}
	return lRes;
}
// Invalidate controls area in parent.
void DcxControl::InvalidateParentRect(HWND hwnd)
{
	RECT rc;
	HWND parent = GetParent(hwnd);
	GetWindowRect(hwnd, &rc);
	MapWindowRect(NULL,parent, &rc);
	InvalidateRect(parent, &rc, TRUE);
}

