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

#include "defines.h"
#include "Dcx.h"

#include "Classes/dcxcontrol.h"
#include "Classes/dcxdialog.h"
#include "Classes/dcxprogressbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxtrackbar.h"
#include "Classes/dcxcomboex.h"
#include "Classes/dcxcolorcombo.h"
#include "Classes/dcxstatusbar.h"
#include "Classes/dcxtreeview.h"
#include "Classes/dcxtoolbar.h"
#include "Classes/dcxlistview.h"
#include "Classes/dcxrebar.h"
#include "Classes/dcxbutton.h"
#include "Classes/dcxrichedit.h"
#include "Classes/dcxupdown.h"
#include "Classes/dcxipaddress.h"
#include "Classes/dcxwebctrl.h"
#include "Classes/dcxcalendar.h"
#include "Classes/dcxdatetime.h"
#include "Classes/dcxpager.h"
#include "Classes/dcxdivider.h"
#include "Classes/dcxpanel.h"
#include "Classes/dcxtab.h"
#include "Classes/mirc/dcxbox.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxline.h"
#include "Classes/mirc/dcxradio.h"
#include "Classes/mirc/dcxcheck.h"
#include "Classes/mirc/dcxtext.h"
#include "Classes/mirc/dcxedit.h"
#include "Classes/mirc/dcxscroll.h"
#include "Classes/mirc/dcxlist.h"
#include "Classes/mirc/dcxlink.h"
#include "Classes/mirc/dcximage.h"
#include "Classes/dcxmwindow.h"
#include "Classes/dcxmdialog.h"
#include "Classes/dcxstacker.h"

#ifdef DCX_USE_DXSDK
#include "Classes/dcxdirectshow.h"
#endif // DCX_USE_DXSDK


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
//, m_tsMark("")
, m_clrText(CLR_INVALID)
, m_clrBackText(CLR_INVALID)
, m_hBackBrush(NULL)
, m_clrBackground(CLR_INVALID)
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
, m_bGradientFill(false)
, m_clrStartGradient(CLR_INVALID)
, m_clrEndGradient(CLR_INVALID)
, m_bGradientVertical(FALSE)
, m_ToolTipHWND(NULL)
, m_colTransparentBg(CLR_INVALID)
{
	this->m_dEventMask = p_Dialog->getEventMask();
//#if DCX_USE_C11
//	if (IDC_map.empty()) {
//		IDC_map[TEXT("appstarting")] = IDC_APPSTARTING;
//		IDC_map[TEXT("arrow")] = IDC_ARROW;
//		IDC_map[TEXT("cross")] = IDC_CROSS;
//		IDC_map[TEXT("hand")] = IDC_HAND;
//		IDC_map[TEXT("help")] = IDC_HELP;
//		IDC_map[TEXT("ibeam")] = IDC_IBEAM;
//		IDC_map[TEXT("no")] = IDC_NO;
//		IDC_map[TEXT("sizeall")] = IDC_SIZEALL;
//		IDC_map[TEXT("sizenesw")] = IDC_SIZENESW;
//		IDC_map[TEXT("sizens")] = IDC_SIZENS;
//		IDC_map[TEXT("sizenwse")] = IDC_SIZENWSE;
//		IDC_map[TEXT("sizewe")] = IDC_SIZEWE;
//		IDC_map[TEXT("uparrow")] = IDC_UPARROW;
//		IDC_map[TEXT("wait")] = IDC_WAIT;
//	}
//#endif
}

/*!
 * \brief Destructor
 *
 * Control Interface Destructor.
 *
 * Frees global control resources.
 */

DcxControl::~DcxControl( ) {

	RemoveProp( this->m_Hwnd, TEXT("dcx_cthis") );

	// Reverse to old font
	this->setControlFont( NULL, FALSE );

	// Delete background brush
	if ( this->m_hBackBrush != NULL ) {

		DeleteBrush( this->m_hBackBrush );
		this->m_hBackBrush = NULL;
	}
	// Delete border brush
	if ( this->m_hBorderBrush != NULL ) {

		DeleteBrush( this->m_hBorderBrush );
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

void DcxControl::parseGeneralControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	//const UINT numtok = styles.numtok( );

	*Styles |= WS_CLIPCHILDREN | WS_VISIBLE;

	//styles.getfirsttok( 0 );

	//for (UINT i = 1; i <= numtok; i++ )
	//{
	//	const TString tsStyle(styles.getnexttok( )); // tok i

	//	if ( tsStyle == TEXT("notheme") )
	//		*bNoTheme = TRUE;
	//	else if ( tsStyle == TEXT("tabstop") )
	//		*Styles |= WS_TABSTOP;
	//	else if ( tsStyle == TEXT("group") )
	//		*Styles |= WS_GROUP;
	//	else if ( tsStyle == TEXT("disabled") )
	//		*Styles |= WS_DISABLED;
	//	else if ( tsStyle == TEXT("transparent") )
	//		*ExStyles |= WS_EX_TRANSPARENT;
	//	else if ( tsStyle == TEXT("hidden") )
	//		*Styles &= ~WS_VISIBLE;
	//	else if ( tsStyle == TEXT("alpha") )
	//		this->m_bAlphaBlend = true;
	//	else if ( tsStyle == TEXT("shadow") )
	//		this->m_bShadowText = true;
	//	else if ( tsStyle == TEXT("noformat") )
	//		this->m_bCtrlCodeText = false;
	//	else if ( tsStyle == TEXT("hgradient") )
	//		this->m_bGradientFill = true;
	//	else if ( tsStyle == TEXT("vgradient") ) {
	//		this->m_bGradientFill = true;
	//		this->m_bGradientVertical = TRUE;
	//	}
	//}
	for (TString tsStyle(styles.getfirsttok(1)); !tsStyle.empty(); tsStyle = styles.getnexttok())
	{
		if ( tsStyle == TEXT("notheme") )
			*bNoTheme = TRUE;
		else if ( tsStyle == TEXT("tabstop") )
			*Styles |= WS_TABSTOP;
		else if ( tsStyle == TEXT("group") )
			*Styles |= WS_GROUP;
		else if ( tsStyle == TEXT("disabled") )
			*Styles |= WS_DISABLED;
		else if ( tsStyle == TEXT("transparent") )
			*ExStyles |= WS_EX_TRANSPARENT;
		else if ( tsStyle == TEXT("hidden") )
			*Styles &= ~WS_VISIBLE;
		else if ( tsStyle == TEXT("alpha") )
			this->m_bAlphaBlend = true;
		else if ( tsStyle == TEXT("shadow") )
			this->m_bShadowText = true;
		else if ( tsStyle == TEXT("noformat") )
			this->m_bCtrlCodeText = false;
		else if ( tsStyle == TEXT("hgradient") )
			this->m_bGradientFill = true;
		else if ( tsStyle == TEXT("vgradient") ) {
			this->m_bGradientFill = true;
			this->m_bGradientVertical = TRUE;
		}
	}
}

/*!
 * \brief blah
 *
 * blah
 */

const UINT DcxControl::getUserID( ) const {

	return this->getID( ) - mIRC_ID_OFFSET;
}

/*!
 * \brief blah
 *
 * NB: Possible buffer overwrite condition when returned data is longer than allocated szReturn
 */

bool DcxControl::evalAliasEx( TCHAR * szReturn, const int maxlen, const TCHAR * szFormat, ... ) {

	TString parms;
	va_list args;

	va_start( args, szFormat );
	parms.tvprintf(szFormat, &args );
	va_end( args );

	return this->m_pParentDialog->evalAlias(szReturn, maxlen, parms.to_chr());
}

bool DcxControl::execAliasEx( const TCHAR * szFormat, ... ) {

	TString parms;
	va_list args;

	va_start( args, szFormat );
	parms.tvprintf(szFormat, &args);
	va_end( args );

	return this->m_pParentDialog->execAlias(parms.to_chr());
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseGlobalCommandRequest( const TString & input, const XSwitchFlags & flags ) {
	const UINT  numtok = input.numtok( );

	// xdid -f [NAME] [ID] [SWITCH] [+FLAGS] [CHARSET] [SIZE] [FONTNAME]
	if ( flags[TEXT('f')] && numtok > 3 ) {
		LOGFONT lf;

		if (ParseCommandToLogfont(input.gettok(4, -1), &lf)) {
			HFONT hFont = CreateFontIndirect(&lf);
			this->setControlFont(hFont, FALSE);
		}

		this->redrawWindow( );
	}
	// xdid -p [NAME] [ID] [SWITCH] [X] [Y] [W] [H]
	else if ( flags[TEXT('p')] && numtok > 6 ) {

		const int x = input.getfirsttok( 4 ).to_int( );
		const int y = input.getnexttok( ).to_int( );	// tok 5
		const int w = input.getnexttok( ).to_int( );	// tok 6
		const int h = input.getnexttok( ).to_int( );	// tok 7

		MoveWindow( this->m_Hwnd, x, y, w, h, TRUE );
		//this->InvalidateParentRect( this->m_Hwnd);
		//InvalidateRect( GetParent( this->m_Hwnd ), NULL, TRUE );
		//this->redrawWindow( );
		//this->m_pParentDialog->redrawBufferedWindow();
		//SendMessage( this->m_Hwnd, WM_NCPAINT, (WPARAM) 1, (LPARAM) 0 );
	}
	// xdid -x [NAME] [ID] [SWITCH] [+FLAGS]
	else if ( flags[TEXT('x')] && numtok > 3 ) {

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
	else if ( flags[TEXT('C')] && numtok > 4 ) {
		const UINT iFlags = this->parseColorFlags( input.getfirsttok( 4 ) );
		const COLORREF clrColor = (COLORREF)input.getnexttok( ).to_num( );	// tok 5

		if ( iFlags & DCC_BKGCOLOR ) {
			if ( this->m_hBackBrush != NULL ) {
				DeleteBrush( this->m_hBackBrush );
				this->m_hBackBrush = NULL;
				this->m_clrBackground = CLR_INVALID;
			}

			if ( clrColor != CLR_INVALID ) {
				this->m_hBackBrush = CreateSolidBrush( clrColor );
				this->m_clrBackground = clrColor;
			}
		}

		if ( iFlags & DCC_TEXTCOLOR )
			this->m_clrText = clrColor;

		if ( iFlags & DCC_TEXTBKGCOLOR )
			this->m_clrBackText = clrColor;

		if ( iFlags & DCC_BORDERCOLOR ) {
			if ( this->m_hBorderBrush != NULL ) {
				DeleteBrush( this->m_hBorderBrush );
				this->m_hBorderBrush = NULL;
			}

			if ( clrColor != CLR_INVALID )
				this->m_hBorderBrush = CreateSolidBrush( clrColor );
		}

		if ( iFlags & DCC_GRADSTARTCOLOR )
			this->m_clrStartGradient = clrColor;

		if ( iFlags & DCC_GRADENDCOLOR )
			this->m_clrEndGradient = clrColor;

		// force a control redraw
		this->redrawWindow( );
	}
	// xdid -F [NAME] [ID] [SWITCH]
	else if (flags[TEXT('F')]) {
		SetFocus(this->m_Hwnd);
	}
	// xdid -J [NAME] [ID] [SWITCH] [+FLAGS] [CURSOR|FILENAME]
	else if ( flags[TEXT('J')] && numtok > 4 ) {
		const UINT iFlags = this->parseCursorFlags( input.getfirsttok( 4 ) );
		HCURSOR hCursor = NULL;

		if ( this->m_bCursorFromFile )
			hCursor = this->m_hCursor;
		this->m_hCursor = NULL;
		this->m_bCursorFromFile = FALSE;
		if ( iFlags & DCCS_FROMRESSOURCE )
			this->m_hCursor = LoadCursor( NULL, this->parseCursorType( input.getnexttok( ) ) );	// tok 5
		else if ( iFlags & DCCS_FROMFILE ) {
			TString filename(input.getlasttoks());	// tok 5, -1
			if (IsFile(filename)) {
				this->m_hCursor = (HCURSOR)LoadImage(NULL, filename.to_chr(), IMAGE_CURSOR, 0,0, LR_DEFAULTSIZE|LR_LOADFROMFILE );
				this->m_bCursorFromFile = TRUE;
			}
			else
				this->showErrorEx(NULL,TEXT("-J"), TEXT("Unable to Access File: %s"), filename.to_chr());
		}
		if (this->m_hCursor == NULL)
			this->showError(NULL, TEXT("-J"), TEXT("Unable to Load Cursor"));
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
	else if (flags[TEXT('M')]) {
		TString info;

		if (numtok > 3)
			info = input.gettok(4, -1).trim();

		this->m_tsMark = info;
	}
	// xdid -Z [NAME] [ID] [SWITCH] [%]
	else if ( flags[TEXT('Z')] && numtok > 3 ) {

		const UINT perc = (UINT)(input.getfirsttok(4).to_int() & 0xFF);

		if (perc > 100) {
			this->showErrorEx(NULL, TEXT("-Z"), TEXT("Invalid Percentage: %u"), perc);
			return;
		}
		int min, max;
		GetScrollRange( this->m_Hwnd, SB_VERT, &min, &max );

		//scrollbar is defined and has visible range
		if ( min != 0 || max != 0 ) {

			const int pos = dcx_round( (float) ( max - min ) * (float) perc / (float) 100.0 );

			SCROLLINFO si;
			ZeroMemory( &si, sizeof ( SCROLLINFO ) );
			si.cbSize = sizeof( SCROLLINFO );
			GetScrollInfo( this->m_Hwnd, SB_VERT, &si );
			si.nPos = pos;
			SetScrollInfo( this->m_Hwnd, SB_VERT, &si, TRUE );
			SendMessage( this->m_Hwnd, WM_VSCROLL, MAKEWPARAM( SB_THUMBPOSITION, si.nPos ), NULL );
		}
	}

	// xdid -b [NAME] [ID]
	else if ( flags[TEXT('b')] ) {
		EnableWindow( this->m_Hwnd, FALSE );
	}
	// xdid -e [NAME] [ID]
	else if ( flags[TEXT('e')] ) {
		EnableWindow( this->m_Hwnd, TRUE );
	}
	// xdid -h [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if (flags[TEXT('h')]) {
		if (numtok > 4)
			AnimateWindow(this->m_Hwnd,
				input.gettok(5).to_int(),
				(AW_HIDE | DcxDialog::getAnimateStyles(input.gettok(4))) & ~AW_ACTIVATE);
		else
			ShowWindow(this->m_Hwnd, SW_HIDE);

		RECT rc;
		GetClientRect(this->m_pParentDialog->getHwnd(), &rc);
		if (this->m_pParentDialog->updateLayout(rc))
			this->m_pParentDialog->redrawWindow(); // why do we need the redraw?
	}
	// xdid -s [NAME] [ID] [SWITCH] (+FLAGS) (DURATION)
	else if ( flags[TEXT('s')] ) {
		if (numtok > 4) {
			AnimateWindow(this->m_Hwnd,
				input.gettok(5).to_int(),
				(AW_ACTIVATE | DcxDialog::getAnimateStyles(input.gettok( 4 ))) & ~AW_HIDE);
		}
		else
			ShowWindow(this->m_Hwnd, SW_SHOW);

		RECT rc;
		GetClientRect(this->m_pParentDialog->getHwnd(), &rc);
		if (this->m_pParentDialog->updateLayout(rc))
			this->m_pParentDialog->redrawWindow(); // why do we need the redraw?
	}
	// xdid -U [NAME] [ID]
	else if (flags[TEXT('U')]) {
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
	else if (flags[TEXT('T')] && numtok > 2) {
		this->m_tsToolTip = (numtok > 3 ? input.gettok(4, -1).trim() : TEXT(""));
	}
	// xdid -R [NAME] [ID] [SWITCH] [FLAG] [ARGS]
	else if (flags[TEXT('R')] && numtok > 3) {
		const XSwitchFlags xflags(input.getfirsttok( 4 ));

		if (!xflags[TEXT('+')]) {
			this->showError(NULL, TEXT("-R"), TEXT("Invalid Flag"));
			return;
		}

		RECT rc;
		HRGN m_Region = NULL;
		int RegionMode = 0;
		bool noRegion = false;
		GetWindowRect(this->m_Hwnd,&rc);

		if (xflags[TEXT('o')])
			RegionMode = RGN_OR;
		else if (xflags[TEXT('a')])
			RegionMode = RGN_AND;
		else if (xflags[TEXT('i')])
			RegionMode = RGN_DIFF;
		else if (xflags[TEXT('x')])
			RegionMode = RGN_XOR;

		if (xflags[TEXT('f')]) // image file - [COLOR] [FILE]
		{
			if (numtok < 6) {
				this->showError(NULL, TEXT("-R +f"), TEXT("Invalid Arguments"));
				return;
			}

			const COLORREF tCol = (COLORREF)input.getnexttok( ).to_num();	// tok 5
			TString filename(input.getlasttoks());							// tok 6, -1
			HBITMAP bitmapRgn = dcxLoadBitmap(NULL,filename);

			if (bitmapRgn != NULL) {
				if (xflags[TEXT('R')]) // now resize image to match control.
					bitmapRgn = DcxControl::resizeBitmap(bitmapRgn, &rc);
				m_Region = BitmapRegion(bitmapRgn, tCol, ((tCol != -1) ? TRUE : FALSE));

				DeleteBitmap(bitmapRgn);
			}
			else
				this->showError(NULL,TEXT("-R +f"), TEXT("Unable To Load Image file."));
		}
		else if (xflags[TEXT('r')]) // rounded rect - radius args (optional)
		{
			int radius;

			if (numtok > 4)
				radius = input.getnexttok( ).to_int();	// tok 5
			else
				radius = 20;

			m_Region = CreateRoundRectRgn(0,0,rc.right - rc.left,rc.bottom - rc.top, radius, radius);
		}
		else if (xflags[TEXT('c')]) // circle - radius arg (optional)
		{
			if (numtok > 4) {
				int radius = input.getnexttok( ).to_int();	// tok 5
				if (radius < 1)
					radius = 100; // handle cases where arg isnt a number or is a negative.
				const int cx = ((rc.right - rc.left)/2);
				const int cy = ((rc.bottom - rc.top)/2);
				m_Region = CreateEllipticRgn(cx-radius,cy-radius,cx+radius,cy+radius);
			}
			else
				m_Region = CreateEllipticRgn(0,0,rc.right - rc.left,rc.bottom - rc.top);
		}
		else if (xflags[TEXT('p')]) // polygon
		{
			// u need at least 3 points for a shape
			if (numtok < 7) {
				this->showError(NULL, TEXT("-R +p"), TEXT("Invalid Arguments"));
				return;
			}

			const TString strPoints(input.getlasttoks());	// tok 5, -1
			TString strPoint;
			const int tPoints = strPoints.numtok( );

			if (tPoints < 3) {
				this->showError(NULL, TEXT("-R +p"), TEXT("Invalid Points: At least 3 points required."));
				return;
			}

			POINT *pnts = new POINT[tPoints];

			strPoints.getfirsttok( 0 );

			for (int cnt = 1; cnt <= tPoints; cnt++)
			{
				strPoint = strPoints.getnexttok( );	// tok cnt
				pnts[cnt-1].x = (LONG)strPoint.getfirsttok(1, TSCOMMA).to_num();
				pnts[cnt-1].y = (LONG)strPoint.getnexttok( TSCOMMA ).to_num();	// tok 2
			}

			m_Region = CreatePolygonRgn(pnts,tPoints,WINDING);

			delete [] pnts;
		}
		// Doesn't work with child controls :/ (yet)
		//else if (flag.find(TEXT('s'),0)) // shadow - <colour> <sharpness> <darkness> <size> <xoffset> <yoffset>
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
		//		DCXError(TEXT("xdid -R +s"), TEXT("Invalid Args"));
		//		return;
		//	}
		//}
		else if (xflags[TEXT('b')]) { // alpha [1|0] [level]
			noRegion = true;
			if (numtok != 6) {
				this->showError(NULL, TEXT("-R +b"), TEXT("Invalid Args"));
				return;
			}
			if (input.getnexttok( ).to_int() > 0)	// tok 5
				this->m_bAlphaBlend = true;
			else
				this->m_bAlphaBlend = false;

			const BYTE alpha = (BYTE)(input.getnexttok( ).to_int() & 0xFF);	// tok 6

			if (alpha == 255)
				this->m_bAlphaBlend = false;
			this->m_iAlphaLevel = alpha;
		}
		else if (xflags[TEXT('n')]) { // none, no args
			noRegion = true;
			SetWindowRgn(this->m_Hwnd,NULL,TRUE);
		}
		else
			this->showError(NULL, TEXT("-R"), TEXT("Invalid Flag"));

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
				this->showError(NULL, TEXT("-R"), TEXT("Unable to create region."));
		}
		this->redrawWindow();
	}
	// invalid command
	else
		this->showError(NULL, input.gettok( 3 ).to_chr(), TEXT("Invalid Command"));
}

HBITMAP DcxControl::resizeBitmap(HBITMAP srcBM, const LPRECT rc)
{
	// set result to original bitmap incase resize fails at some point.
	HBITMAP hRes = srcBM;

	if (srcBM == NULL)
		return NULL;

	// create temp hdc to render from
	HDC srcDC = CreateCompatibleDC(NULL);
	if (srcDC != NULL) {
		// select bitmap into new hdc
		HBITMAP oldSrcBm = SelectBitmap(srcDC, srcBM);
		// create temp dest hdc to render into.
		HDC destDC = CreateCompatibleDC(srcDC);
		if (destDC != NULL) {
			int w = (rc->right - rc->left), h = (rc->bottom - rc->top);
			// create dest bitmap.
			HBITMAP newBM = CreateCompatibleBitmap(srcDC,w,h);
			if (newBM != NULL) {
				// select dest bitmap into dest hdc
				HBITMAP oldDestBm = SelectBitmap(destDC, newBM);
				// get source bitmap info.
				BITMAP bm;
				GetObject(srcBM,sizeof(bm),&bm);
				// resize bitmap, render into dest hdc/bitmap
				if (StretchBlt(destDC, 0, 0, w, h, srcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY)) {
					GdiFlush();
					// set the return bitmap if resize worked.
					hRes = newBM;
					// set dest hdc back to orig bitmap.
					SelectBitmap(destDC, oldDestBm);
				}
				else
					DeleteBitmap(SelectObject(destDC, oldDestBm)); // resize failed, delete dest bitmap.
			}
			// delete dest hdc
			DeleteDC(destDC);
		}
		// set source hdc back to orig bitmap.
		SelectBitmap(srcDC, oldSrcBm);
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

const UINT DcxControl::parseColorFlags( const TString & flags ) {

	const XSwitchFlags xflags(flags);
	UINT iFlags = 0;

	// no +sign, missing params
	if ( !xflags[TEXT('+')] )
		return iFlags;

	if ( xflags[TEXT('b')] )
		iFlags |= DCC_BKGCOLOR;
	if ( xflags[TEXT('k')] )
		iFlags |= DCC_TEXTBKGCOLOR;
	if ( xflags[TEXT('t')] )
		iFlags |= DCC_TEXTCOLOR;
	if ( xflags[TEXT('r')] )
		iFlags |= DCC_BORDERCOLOR;
	if ( xflags[TEXT('g')] )
		iFlags |= DCC_GRADSTARTCOLOR;
	if ( xflags[TEXT('G')] )
		iFlags |= DCC_GRADENDCOLOR;

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::parseBorderStyles( const TString & flags, LONG * Styles, LONG * ExStyles ) {

	const XSwitchFlags xflags(flags);

	// no +sign, missing params
	if ( !xflags[TEXT('+')] ) 
		return;

	if ( xflags[TEXT('b')] )
		*Styles |= WS_BORDER;
	if ( xflags[TEXT('c')] )
		*ExStyles |= WS_EX_CLIENTEDGE;
	if ( xflags[TEXT('d')] )
		*Styles |= WS_DLGFRAME ;
	if ( xflags[TEXT('f')] )
		*ExStyles |= WS_EX_DLGMODALFRAME;
	if ( xflags[TEXT('s')] )
		*ExStyles |= WS_EX_STATICEDGE;
	if ( xflags[TEXT('w')] )
		*ExStyles |= WS_EX_WINDOWEDGE;

}

/*!
 * \brief blah
 *
 * blah
 */

BOOL DcxControl::parseGlobalInfoRequest( const TString & input, TCHAR * szReturnValue ) const
{
	const TString prop(input.getfirsttok( 3 ));

	if ( prop == TEXT("hwnd") ) {
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), (DWORD)this->m_Hwnd );	// can't use %p as this gives a hex result.
		return TRUE;
	}
	else if ( prop == TEXT("visible") ) {
		dcx_ConRetState(IsWindowVisible(this->m_Hwnd), szReturnValue);
		//TCHAR *p;
		//if ( IsWindowVisible( this->m_Hwnd ) )
		//	p = lstrcpyn( szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH );
		//else
		//	p = lstrcpyn( szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH );

		//if (p != NULL)
		//	return TRUE;
	}
	else if ( prop == TEXT("enabled") ) {
		dcx_ConRetState(IsWindowEnabled(this->m_Hwnd), szReturnValue);
		//TCHAR *p;
		//if ( IsWindowEnabled( this->m_Hwnd ) )
		//	p = lstrcpyn( szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH );
		//else
		//	p = lstrcpyn( szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH );

		//if (p != NULL)
		//	return TRUE;    
	}
	else if ( prop == TEXT("pos") ) {
		const RECT rc = getWindowPosition();
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
		return TRUE;
	}
	else if ( prop == TEXT("dpos") ) {
		RECT rc;
		GetWindowRect( this->m_Hwnd, &rc );
		MapWindowPoints(NULL, this->m_pParentDialog->getHwnd( ), (LPPOINT)&rc, 2);

		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d %d %d"), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
		return TRUE;
	}
	else if ( prop == TEXT("mark") ) {
		if (lstrcpyn( szReturnValue, this->m_tsMark.to_chr( ), MIRC_BUFFER_SIZE_CCH ) != NULL)
			return TRUE;
	}
	else if ( prop == TEXT("mouse") ) {
		POINT pt;
		GetCursorPos( &pt );
		MapWindowPoints(NULL, this->m_Hwnd, &pt, 1);
		wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%d %d"), pt.x, pt.y );
		return TRUE;
	}
	else if ( prop == TEXT("pid") ) {
		TCHAR classname[257];
		GetClassName( GetParent( this->m_Hwnd ), classname, 256 );

		if (lstrcmp(classname, TEXT("#32770")) == 0) {
			szReturnValue[0] = TEXT('0');
			szReturnValue[1] = TEXT('\0');
			//lstrcpyn(szReturnValue, TEXT("0"), MIRC_BUFFER_SIZE_CCH);
		}
		else
			wnsprintf( szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"),  this->m_pParentDialog->getControlByHWND( GetParent( this->m_Hwnd ) )->getUserID( ) );

		return TRUE;
	}
	else if ( prop == TEXT("type") ) {

		if (lstrcpyn( szReturnValue, this->getType( ).to_chr( ), MIRC_BUFFER_SIZE_CCH ) != NULL)
			return TRUE;
	}
	else if ( prop == TEXT("styles") ) {
		if (lstrcpyn( szReturnValue, this->getStyles( ).to_chr( ), MIRC_BUFFER_SIZE_CCH ) != NULL)
			return TRUE;
	}
	else if ( prop == TEXT("font")) {
		HFONT hFontControl = this->m_hFont;

//		if (!hFontControl)
//			hFontControl = (HFONT) this->getFont();
		if (!hFontControl)
			hFontControl = (HFONT) GetStockObject(DEFAULT_GUI_FONT);

		if (hFontControl) {
			LOGFONT lfCurrent;

			ZeroMemory(&lfCurrent, sizeof(LOGFONT));
			GetObject(hFontControl, sizeof(LOGFONT), &lfCurrent);

			if (lstrcpyn(szReturnValue, ParseLogfontToCommand(&lfCurrent).to_chr(), MIRC_BUFFER_SIZE_CCH) != NULL)
				return TRUE;
		}
	}
	// [NAME] [ID] [PROP]
	else if ( prop == TEXT("tooltipbgcolor")) {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND,TTM_GETTIPBKCOLOR, NULL, NULL);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("tooltiptextcolor")) {
		DWORD cref = 0;

		if (this->m_ToolTipHWND != NULL)
			cref = (DWORD) SendMessage(this->m_ToolTipHWND, TTM_GETTIPTEXTCOLOR, NULL, NULL);

		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%lu"), cref);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("alpha")) {
		dcx_ConRetState(this->m_bAlphaBlend, szReturnValue);
		//TCHAR *p;
		//if (this->m_bAlphaBlend)
		//	p = lstrcpyn(szReturnValue, TEXT("$true"), MIRC_BUFFER_SIZE_CCH);
		//else
		//	p = lstrcpyn(szReturnValue, TEXT("$false"), MIRC_BUFFER_SIZE_CCH);

		//if (p != NULL)
		//	return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("textcolor")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_clrText);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("textbgcolor")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_clrBackText);
		return TRUE;
	}
	// [NAME] [ID] [PROP]
	else if (prop == TEXT("bgcolor")) {
		wnsprintf(szReturnValue, MIRC_BUFFER_SIZE_CCH, TEXT("%u"), this->m_clrBackground);
		return TRUE;
	}
	else
		this->showError(prop.to_chr(),NULL,TEXT("Invalid property or number of arguments"));

	return FALSE;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::registreDefaultWindowProc( ) {
	//this->removeStyle( WS_BORDER|WS_DLGFRAME );
	//this->removeExStyle( WS_EX_CLIENTEDGE|WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE );

	this->m_DefaultWindowProc = SubclassWindow( this->m_Hwnd, DcxControl::WindowProc );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxControl::unregistreDefaultWindowProc( ) {
	if (this->m_DefaultWindowProc != NULL) // implies this has alrdy been called.
		SubclassWindow( this->m_Hwnd, this->m_DefaultWindowProc );
	this->m_DefaultWindowProc = NULL;
}

/*!
 * \brief blah
 *
 * blah
 */

LRESULT CALLBACK DcxControl::WindowProc(HWND mHwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//DcxControl *pthis = (DcxControl*) GetProp(mHwnd, TEXT("dcx_cthis"));
	DcxControl *pthis = static_cast<DcxControl*>(GetProp(mHwnd, TEXT("dcx_cthis")));

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

	const bool fBlocked = (InSendMessageEx(NULL) & (ISMEX_REPLIED|ISMEX_SEND)) == ISMEX_SEND;

	if (!fBlocked) {
		// If Message is blocking just call old win proc
		BOOL bParsed = FALSE;

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->incRef( );

		LRESULT lrRes = pthis->PostMessage(uMsg, wParam, lParam, bParsed);

		if ((uMsg != WM_DESTROY) && (uMsg != WM_NCDESTROY))
			pthis->decRef();

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

DcxControl * DcxControl::controlFactory( DcxDialog * p_Dialog, const UINT mID, const TString & tsInput, UINT offset, const UINT64 mask, HWND hParent ) {

	//const TString type(tsInput.gettok( offset++ ));

	//RECT rc;
	//rc.left = (LONG)tsInput.gettok( offset++ ).to_num( );
	//rc.top = (LONG)tsInput.gettok( offset++ ).to_num( );
	//rc.right = rc.left + (LONG)tsInput.gettok( offset++ ).to_num( );
	//rc.bottom = rc.top + (LONG)tsInput.gettok( offset ).to_num( );
	const TString type(tsInput.getfirsttok( offset ));

	RECT rc;
	rc.left = (LONG)tsInput.getnexttok( ).to_num( );
	rc.top = (LONG)tsInput.getnexttok( ).to_num( );
	rc.right = rc.left + (LONG)tsInput.getnexttok( ).to_num( );
	rc.bottom = rc.top + (LONG)tsInput.getnexttok( ).to_num( );
	offset += 4;

	TString styles;
	if ( tsInput.numtok( ) > offset )
		styles = tsInput.gettok( offset +1, -1);

	if (hParent == NULL)
		hParent = p_Dialog->getHwnd();

	if (( type == TEXT("pbar") ) && (mask & CTLF_ALLOW_PBAR))
		return new DcxProgressBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("trackbar") ) && (mask & CTLF_ALLOW_TRACKBAR))
		return new DcxTrackBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("comboex") ) && (mask & CTLF_ALLOW_COMBOEX))
		return new DcxComboEx( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("colorcombo") ) && (mask & CTLF_ALLOW_COLORCOMBO))
		return new DcxColorCombo( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("statusbar") ) && (mask & CTLF_ALLOW_STATUSBAR))
		return new DcxStatusBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("toolbar") ) && (mask & CTLF_ALLOW_TOOLBAR))
		return new DcxToolBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("treeview") ) && (mask & CTLF_ALLOW_TREEVIEW))
		return new DcxTreeView( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("listview") ) && (mask & CTLF_ALLOW_LISTVIEW))
		return new DcxListView( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("rebar") ) && (mask & CTLF_ALLOW_REBAR))
		return new DcxReBar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("button") ) && (mask & CTLF_ALLOW_BUTTON))
		return new DcxButton( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("richedit") ) && (mask & CTLF_ALLOW_RICHEDIT))
		return new DcxRichEdit( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("updown") ) && (mask & CTLF_ALLOW_UPDOWN))
		return new DcxUpDown( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("ipaddress") ) && (mask & CTLF_ALLOW_IPADDRESS))
		return new DcxIpAddress( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("webctrl") ) && (mask & CTLF_ALLOW_WEBCTRL))
		return new DcxWebControl( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("calendar") ) && (mask & CTLF_ALLOW_CALANDER))
		return new DcxCalendar( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("datetime") ) && (mask & CTLF_ALLOW_CALANDER))
		return new DcxDateTime(mID, p_Dialog, hParent, &rc, styles);
	else if (( type == TEXT("divider") ) && (mask & CTLF_ALLOW_DIVIDER))
		return new DcxDivider( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("panel") ) && (mask & CTLF_ALLOW_PANEL))
		return new DcxPanel( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("tab") ) && (mask & CTLF_ALLOW_TAB))
		return new DcxTab( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("line") ) && (mask & CTLF_ALLOW_LINE))
		return new DcxLine( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("box") ) && (mask & CTLF_ALLOW_BOX))
		return new DcxBox( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("radio") ) && (mask & CTLF_ALLOW_RADIO))
		return new DcxRadio( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("check") ) && (mask & CTLF_ALLOW_CHECK))
		return new DcxCheck( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("text") ) && (mask & CTLF_ALLOW_TEXT))
		return new DcxText( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("edit") ) && (mask & CTLF_ALLOW_EDIT))
		return new DcxEdit( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("scroll") ) && (mask & CTLF_ALLOW_SCROLL))
		return new DcxScroll( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("list") ) && (mask & CTLF_ALLOW_LIST))
		return new DcxList( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("link") ) && (mask & CTLF_ALLOW_LINK))
		return new DcxLink( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("image") ) && (mask & CTLF_ALLOW_IMAGE))
		return new DcxImage( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("pager") ) && (mask & CTLF_ALLOW_PAGER))
		return new DcxPager( mID, p_Dialog, hParent, &rc, styles );
	else if (( type == TEXT("stacker") ) && (mask & CTLF_ALLOW_STACKER))
		return new DcxStacker( mID, p_Dialog, hParent, &rc, styles );
	//else if (( type == TEXT("mci") ) && (mask & CTLF_ALLOW_DIRECTSHOW))
	//	return new DcxMci( mID, p_Dialog, hParent, &rc, styles );

#ifdef DCX_USE_DXSDK

	else if (( type == TEXT("directshow") ) && (mask & CTLF_ALLOW_DIRECTSHOW))
		return new DcxDirectshow( mID, p_Dialog, hParent, &rc, styles );

#endif // DCX_USE_DXSDK

	else if (( type == TEXT("window") ) && (mask & CTLF_ALLOW_DOCK)) {
		if ( tsInput.numtok( ) > offset ) {
			const TString tsWin(tsInput.gettok(offset + 1));

			if (tsWin.len() < 2) {
				// this helps stop '@' being passed as $window(@).hwnd == $window(-2).hwnd & usually causes a crash.
				TString tsErr(TEXT("No such window: "));
				tsErr += tsWin;
				throw tsErr.to_chr();
				//Dcx::errorex(TEXT("ControlFactory"), TEXT("Docking (No such window %s)"), tsWin.to_chr());
				//throw TEXT("No such window");
			}

			HWND winHwnd = (HWND)tsWin.to_num();
			if (!IsWindow(winHwnd)) {
				TCHAR windowHwnd[30];

				mIRCLinker::evalex(windowHwnd, 30, TEXT("$window(%s).hwnd"), tsWin.to_chr( ) );

				winHwnd = (HWND) dcx_atoi( windowHwnd );
			}

			if (IsWindow(winHwnd)) {
				if (p_Dialog->getControlByHWND(winHwnd) == NULL)
					return new DcxMWindow(winHwnd, hParent, mID, p_Dialog, &rc, styles);
			}
			else {
				TString tsErr(TEXT("No such window: "));
				tsErr += tsWin;
				throw tsErr.to_chr();
				//Dcx::errorex(TEXT("ControlFactory"), TEXT("Docking (No such window %s)"), tsWin.to_chr());
				//throw TEXT("No such window");
			}
		}
	}
	else if ((type == TEXT("dialog")) && (mask & CTLF_ALLOW_DOCK)) {
		if (tsInput.numtok( ) > offset) {
			HWND winHwnd = GetHwndFromString(tsInput.gettok( offset +1 ));

			if (IsWindow(winHwnd)) {
				if (p_Dialog->getControlByHWND(winHwnd) == NULL) {
					DcxControl* newDialog = new DcxMDialog(winHwnd, hParent, mID, p_Dialog, &rc, styles);
					DcxDialog* dlg = Dcx::Dialogs.getDialogByHandle(winHwnd);

					// if its a dcx marked dialog, mark the parent name
					if (dlg != NULL)
						dlg->setParentName(p_Dialog->getName());

					return newDialog;
				}
			}
			else {
				TString tsErr(TEXT("No such dialog: "));
				tsErr += tsInput.gettok(offset + 1);
				throw tsErr.to_chr();
				//Dcx::errorex(TEXT("ControlFactory"),TEXT("Docking (No such dialog %s)"), tsInput.gettok( offset +1 ).to_chr());
				//throw TEXT("No such dialog");
			}
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

HFONT DcxControl::getFont( ) const {
  return GetWindowFont( this->m_Hwnd );
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
      DeleteFont( hControlFont );
      this->m_hFont = NULL;
    }
    else if ( this->m_hFont != NULL ) {
      DeleteFont( this->m_hFont );
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

const HBRUSH &DcxControl::getBackClrBrush( ) const {

  return this->m_hBackBrush;
}

/*!
 * \brief blah
 *
 * blah
 */

const COLORREF &DcxControl::getBackColor( ) const {
	return this->m_clrBackText;
}

/*!
 * \brief blah
 *
 * blah
 */

const COLORREF &DcxControl::getTextColor( ) const {
	return this->m_clrText;
}

const RECT DcxControl::getWindowPosition(void) const {
	RECT rc;
	GetWindowRect( this->m_Hwnd, &rc );
	MapWindowRect(NULL, GetParent( this->m_Hwnd ), &rc);
	return rc;
}

void DcxControl::updateParentCtrl(void)
{
	// find the host control, if any.
	this->m_pParentHWND = GetParent(this->m_Hwnd);
}

void DcxControl::DrawCtrlBackground(const HDC hdc, const DcxControl *p_this, const LPRECT rwnd, HTHEME hTheme, const int iPartId, const int iStateId)
{
	// fill background.
	if (!p_this->isExStyle(WS_EX_TRANSPARENT)) {
		RECT rc;
		if (rwnd == NULL)
			GetClientRect(p_this->getHwnd(), &rc);
		else
			CopyRect(&rc, rwnd);
		if (!IsWindowEnabled(p_this->m_Hwnd)) {// use disabled colouring when windows disabled.
			if (hTheme && !p_this->m_bNoTheme && Dcx::UXModule.dcxIsThemeActive()) {
				if (Dcx::UXModule.dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
					Dcx::UXModule.dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
				Dcx::UXModule.dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, NULL);
			}
			else
				FillRect( hdc, &rc, GetSysColorBrush(COLOR_3DFACE) );
		}
		else if (p_this->m_bGradientFill) {
			COLORREF clrStart = p_this->m_clrStartGradient;
			COLORREF clrEnd = p_this->m_clrEndGradient;

			if (clrStart == CLR_INVALID)
				clrStart = GetSysColor(COLOR_3DFACE);
			if (clrEnd == CLR_INVALID)
				clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);

			XPopupMenuItem::DrawGradient( hdc, &rc, clrStart, clrEnd, p_this->m_bGradientVertical);
		}
		else {
			HBRUSH hBrush = p_this->getBackClrBrush();
			if (hBrush == NULL) {
				if (hTheme && !p_this->m_bNoTheme && Dcx::UXModule.dcxIsThemeActive()) {
					if (Dcx::UXModule.dcxIsThemeBackgroundPartiallyTransparent(hTheme, iPartId, iStateId))
						Dcx::UXModule.dcxDrawThemeParentBackground(p_this->m_Hwnd, hdc, &rc);
					Dcx::UXModule.dcxDrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rc, NULL);
					return;
				}
				else
					hBrush = GetSysColorBrush(COLOR_3DFACE);
			}
			if ( hBrush != NULL )
				FillRect( hdc, &rc, hBrush );
		}
	}
}

void DcxControl::DrawControl(HDC hDC, HWND hwnd)
{
	// if window matches this one, don't draw (loop condition)
	if (hwnd == this->m_Hwnd)
		return;

	// if window isnTEXT('t visible, don')t draw.
	if (!IsWindowVisible(hwnd))
		return;

	// if window is within a background paint of itTEXT('s own, don')t draw. (loop condition)
	DcxControl *p_ctrl = this->m_pParentDialog->getControlByHWND(hwnd);
	if (p_ctrl != NULL && p_ctrl->m_bInPrint)
		return;

	RECT rc;
	GetWindowRect(hwnd, &rc);
	MapWindowRect(NULL,GetParent(hwnd),&rc);

	// if window isnTEXT('t within the client area of the control who')s background we are drawing, don't draw.
	if (!RectVisible(hDC, &rc))
		return;

	HDC hdcMemory = ::CreateCompatibleDC(hDC);

	if (hdcMemory != NULL) {
		int w = (rc.right - rc.left), h = (rc.bottom - rc.top);
		HBITMAP hBitmap = ::CreateCompatibleBitmap( hDC, w, h);

		if (hBitmap != NULL) {
			HBITMAP hbmpOld = SelectBitmap( hdcMemory, hBitmap);

			::SendMessage( hwnd, WM_ERASEBKGND, (WPARAM)hdcMemory,1L); // HACK: using 1L instead of NULL as a workaround for stacker.
			::SendMessage( hwnd, WM_PRINT, (WPARAM)hdcMemory, (LPARAM)PRF_NONCLIENT | PRF_CLIENT | PRF_CHILDREN | PRF_CHECKVISIBLE /*| PRF_ERASEBKGND*/);

			BitBlt( hDC, rc.left, rc.top, w, h, hdcMemory, 0, 0, SRCCOPY);

			DeleteBitmap(SelectBitmap( hdcMemory, hbmpOld));
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
		//::SendMessage(this->m_pParentHWND, WM_PRINT, (WPARAM)*hdcbkg,PRF_CLIENT|PRF_ERASEBKGND);
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

	LPALPHAINFO ai;

	try {
		ai = new ALPHAINFO;
	}
	catch (std::bad_alloc)
	{
		return NULL;
	}

	ZeroMemory(ai,sizeof(ALPHAINFO));
	/*
		1: draw parents bg to temp hdc
		2: copy bg to temp hdcbkg for later alpha
		3: draw button to temp hdc, over parents bg
		4: alpha blend temp hdc to hdc
	*/
#ifdef DCX_USE_WINSDK
	if (Dcx::UXModule.IsBufferedPaintSupported()) {
		BP_PAINTPARAMS paintParams = {0};
		paintParams.cbSize = sizeof(paintParams);
		paintParams.dwFlags = BPPF_ERASE;
		//paintParams.dwFlags = BPPF_NONCLIENT;
		ai->ai_bf.AlphaFormat = AC_SRC_OVER;
		ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
		if (this->m_bAlphaBlend)
			paintParams.pBlendFunction = &ai->ai_bf;

		GetClientRect(this->m_Hwnd,&ai->ai_rcClient);
		GetWindowRect(this->m_Hwnd,&ai->ai_rcWin);
		ai->ai_Buffer = Dcx::UXModule.dcxBeginBufferedPaint(*hdc, &ai->ai_rcClient, BPBF_COMPATIBLEBITMAP, &paintParams, &ai->ai_hdc);
		if (ai->ai_Buffer != NULL) {
			//Dcx::XPPlusModule.dcxDrawThemeParentBackground(this->m_Hwnd, ai->ai_hdc, &ai->ai_rcClient);
			this->DrawParentsBackground(ai->ai_hdc, &ai->ai_rcClient);
			BitBlt( *hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, ai->ai_rcClient.right - ai->ai_rcClient.left, ai->ai_rcClient.bottom - ai->ai_rcClient.top, ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, SRCCOPY);
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
	if (ai->ai_Buffer != NULL) {
		Dcx::UXModule.dcxEndBufferedPaint(ai->ai_Buffer, TRUE);
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
					ai->ai_bf.AlphaFormat = AC_SRC_OVER;
					ai->ai_bf.SourceConstantAlpha = this->m_iAlphaLevel; // 0x7f half of 0xff = 50% transparency
					AlphaBlend(hdcbkg,ai->ai_rcClient.left,ai->ai_rcClient.top,w,h,ai->ai_hdc, ai->ai_rcClient.left, ai->ai_rcClient.top, w, h, ai->ai_bf);
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
void DcxControl::showError(const TCHAR *prop, const TCHAR *cmd, const TCHAR *err) const
{
	if (this->m_pParentDialog->IsVerbose()) {
		TString res;
		if (prop != NULL)
			res.tsprintf(TEXT("D_IERROR %s(%s, %d).%s: %s"), this->getType().to_chr(), this->m_pParentDialog->getName().to_chr(), this->getUserID(), prop, err);
		else
			res.tsprintf(TEXT("D_CERROR (%s) xdid %s %s %d: %s"), this->getType().to_chr(), cmd, this->m_pParentDialog->getName().to_chr(), this->getUserID(), err);		
		Dcx::error(cmd, res.to_chr());
	}

	if (!this->m_pParentDialog->getAliasName().empty())
		const_cast<DcxControl *>(this)->execAliasEx(TEXT("error,%d,%s,%s,%s,%s"), this->getUserID(), this->getType().to_chr(), (prop != NULL ? prop : TEXT("none")), (cmd != NULL ? cmd : TEXT("none")), err);
}
void DcxControl::showErrorEx(const TCHAR *prop, const TCHAR *cmd, const TCHAR *fmt, ...) const
{
	va_list args;
	TString txt;

	va_start( args, fmt );
	txt.tvprintf(fmt, &args );
	va_end( args );

	this->showError(prop, cmd, txt.to_chr());
}

LRESULT DcxControl::CommonMessage( const UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed )
{
	LRESULT lRes = 0L;
	switch( uMsg ) {

		case WM_HELP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
					this->execAliasEx(TEXT("%s,%d"), TEXT("help"), this->getUserID( ) );
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

					const COLORREF clrText = p_Control->getTextColor( );
					const COLORREF clrBackText = p_Control->getBackColor( );
					HBRUSH hBackBrush = p_Control->getBackClrBrush( );

					bParsed = TRUE;
					lRes = CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, wParam, lParam);

					if ( clrText != CLR_INVALID )
						SetTextColor( (HDC) wParam, clrText );

					if ( clrBackText != CLR_INVALID )
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
				{
					this->execAliasEx(TEXT("%s,%d"), TEXT("lbdown"), this->getUserID( ) );
				}
			}
			break;

		case WM_LBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				{
					this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), this->getUserID( ) );
					this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID( ) );
				}
			}
			break;

		case WM_LBUTTONDBLCLK:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK) {
					this->execAliasEx(TEXT("%s,%d"), TEXT("dclick"), this->getUserID( ) );
					this->execAliasEx(TEXT("%s,%d"), TEXT("lbdblclk"), this->getUserID( ) );
				}
			}
			break;

		case WM_RBUTTONDOWN:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx(TEXT("%s,%d"), TEXT("rbdown"), this->getUserID( ) );
			}
			break;

		case WM_RBUTTONUP:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx(TEXT("%s,%d"), TEXT("rbup"), this->getUserID( ) );
			}
			break;

		case WM_RBUTTONDBLCLK:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx(TEXT("%s,%d"), TEXT("rdclick"), this->getUserID( ) );
			}
			break;

		case WM_CONTEXTMENU:
			{
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->execAliasEx(TEXT("%s,%d"), TEXT("rclick"), this->getUserID( ) );
				bParsed = TRUE; // stops event being passed down to parent controls
			}
			break;
		case WM_DROPFILES:
		{
			HDROP files = (HDROP) wParam;
			TCHAR filename[500];
			const int count = DragQueryFile(files, 0xFFFFFFFF,  filename, 500);

			if (count) {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_DRAG) {
					TCHAR ret[20];

					this->evalAliasEx(ret, 255, TEXT("%s,%d,%d"), TEXT("dragbegin"), this->getUserID(), count);

					// cancel drag drop event
					if (lstrcmpi(ret, TEXT("cancel")) == 0) {
						DragFinish(files);
						return 0L;
					}

					// for each file, send callback message
					for (int i = 0; i < count; i++) {
						if (DragQueryFile(files, i, filename, 500))
							this->execAliasEx(TEXT("%s,%d,%s"), TEXT("dragfile"), this->getUserID(), filename);
					}

					this->execAliasEx(TEXT("%s,%d"), TEXT("dragfinish"), this->getUserID());
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
							if (!this->m_tsToolTip.empty()) {
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
							this->execAliasEx(TEXT("%s,%d"), TEXT("tooltiplink"), this->getUserID( ) );
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
		case WM_KEYDOWN:
		{
						   if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT) {
							   if (wParam == VK_RETURN)
								  this->execAliasEx(TEXT("%s,%d"), TEXT("return"), this->getUserID());

							   //if ((this->m_bIgnoreRepeat) && (lParam & 0x40000000)) // ignore repeats
							   // break;

							   this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keydown"), this->getUserID(), wParam);
						   }
						   break;
		}
		case WM_KEYUP:
		{
						 if (this->m_pParentDialog->getEventMask() & DCX_EVENT_EDIT)
							 this->execAliasEx(TEXT("%s,%d,%d"), TEXT("keyup"), this->getUserID(), wParam);
						 break;
		}

		//case WM_WINDOWPOSCHANGING:
		//{
		//	if (this->m_pParentDialog->getEventMask() & DCX_EVENT_MOVE) {
		//		WINDOWPOS *wp = (WINDOWPOS *) lParam;
//
		//		// break if nomove & nosize specified, since thats all we care about.
		//		if ((wp->flags & SWP_NOMOVE) && (wp->flags & SWP_NOSIZE))
		//			break;
//
		//		TCHAR ret[256];
//
		//		this->callAliasEx(ret, "changing,%d,%d,%d,%d,%d,%d", this->getUserID(), (wp->flags & 3),wp->x, wp->y, wp->cx, wp->cy);
//
		//		if (wp != NULL) {
		//			if (lstrcmp(TEXT("nosize"), ret) == 0)
		//				wp->flags |= SWP_NOSIZE;
		//			else if (lstrcmp(TEXT("nomove"), ret) == 0)
		//				wp->flags |= SWP_NOMOVE;
		//			else if (lstrcmp(TEXT("nochange"), ret) == 0)
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

void DcxControl::calcTextRect(HDC hdc, const TString &txt, LPRECT rc, const UINT style)
{
	TString t(txt);
	if (this->m_bCtrlCodeText)
		t.strip();
	if (this->m_bShadowText)
		dcxDrawShadowText(hdc, t.to_chr(), t.len(), rc, style | DT_CALCRECT, this->m_clrText, 0,5,5);
	else
		DrawText(hdc, t.to_chr(), t.len(), rc, style | DT_CALCRECT);
}

void DcxControl::ctrlDrawText(HDC hdc, const TString &txt, const LPRECT rc, const UINT style)
{
	if (!this->m_bCtrlCodeText) {
		const int oldBkgMode = SetBkMode(hdc, TRANSPARENT);
		if (this->m_bShadowText)
			dcxDrawShadowText(hdc, txt.to_chr(), txt.len(), rc, style, this->m_clrText, 0, 5, 5);
		else
			DrawText(hdc, txt.to_chr(), txt.len(), rc, style);
		SetBkMode(hdc, oldBkgMode);
	}
	else
		mIRC_DrawText(hdc, txt, rc, style, this->m_bShadowText);
}

TString DcxControl::getStyles(void) const
{
	TString result;
	const DWORD exStyles = GetWindowExStyle(this->m_Hwnd);
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);

	if ( Dcx::UXModule.dcxGetWindowTheme(this->m_Hwnd) == NULL )
		result = TEXT("notheme");
	if ( Styles & WS_TABSTOP ) 
		result.addtok(TEXT("tabstop"));
	if ( Styles & WS_GROUP ) 
		result.addtok(TEXT("group"));
	if ( Styles & WS_DISABLED ) 
		result.addtok(TEXT("disabled"));
	if ( exStyles & WS_EX_TRANSPARENT )
		result.addtok(TEXT("transparent"));
	if ( ~Styles & WS_VISIBLE )
		result.addtok(TEXT("hidden"));
	if ( this->m_bAlphaBlend )
		result.addtok(TEXT("alpha"));
	if ( this->m_bShadowText )
		result.addtok(TEXT("shadow"));
	if ( !this->m_bCtrlCodeText ) 
		result.addtok(TEXT("noformat"));
	if ( this->m_bGradientFill ) {
		if ( this->m_bGradientVertical )
			result.addtok(TEXT("vgradient"));
		else 
			result.addtok(TEXT("hgradient"));
	}
	result.addtok(TEXT("utf8"));
	return result;
}

TString DcxControl::getBorderStyles(void) const
{
	TString bstyles;
	const DWORD exStyles = GetWindowExStyle(this->m_Hwnd);
	const DWORD Styles = GetWindowStyle(this->m_Hwnd);

	if (Styles & WS_BORDER)
		bstyles += TEXT('b');
	if (exStyles & WS_EX_CLIENTEDGE)
		bstyles += TEXT('c');
	if (Styles & WS_DLGFRAME)
		bstyles += TEXT('d');
	if (exStyles & WS_EX_DLGMODALFRAME)
		bstyles += TEXT('f');
	if (exStyles & WS_EX_STATICEDGE)
		bstyles += TEXT('s');
	if (exStyles & WS_EX_WINDOWEDGE)
		bstyles += TEXT('w');
	return bstyles;
}

void DcxControl::toXml(TiXmlElement * xml) const
{
	const TString styles(getStyles());

	xml->SetAttribute("id", getUserID());
	xml->SetAttribute("type", getType().c_str());
	if (!styles.empty())
		xml->SetAttribute("styles", styles.c_str());
}

TiXmlElement * DcxControl::toXml(void) const
{
	TiXmlElement * result = new TiXmlElement("control");
	this->toXml(result);
	return result;
}
