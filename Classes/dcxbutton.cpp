/*!
 * \file dcxbutton.cpp
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
#include "Classes/dcxbutton.h"
#include "Classes/dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxButton::DcxButton( const UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, const TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_bBitmapText(FALSE)
, m_bHasIcons(FALSE)
, m_bHover(FALSE)
, m_bTouched(FALSE)
, m_bSelected(FALSE)
, m_bTracking(FALSE)
, m_iIconSize(16)
, m_ImageList(NULL)
{
	LONG Styles = 0, ExStyles = 0;
	BOOL bNoTheme = FALSE;
	this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );

	this->m_Hwnd = CreateWindowEx(	
		ExStyles,
		DCX_BUTTONCLASS,
		NULL,
		WS_CHILD | BS_PUSHBUTTON | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw TEXT("Unable To Create Window");

	if ( bNoTheme )
		Dcx::UXModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_bNoTheme = (bNoTheme ? true : false);

	ZeroMemory( &this->m_aBitmaps, 4*sizeof(HBITMAP) );
	ZeroMemory( &this->m_aColors, 4*sizeof(COLORREF) );
	ZeroMemory( &this->m_aTransp, 4*sizeof(COLORREF) );

	this->m_aColors[0] = GetSysColor(COLOR_BTNTEXT); // normal
	this->m_aColors[1] = GetSysColor(COLOR_BTNTEXT); // hover
	this->m_aColors[2] = GetSysColor(COLOR_BTNTEXT); // pushed
	this->m_aColors[3] = GetSysColor(COLOR_GRAYTEXT); // disabled

	this->setControlFont( GetStockFont( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, TEXT("dcx_cthis"), (HANDLE) this );

	if (styles.istok(TEXT("tooltips"))) {
		if (IsWindow(p_Dialog->getToolTip())) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
		else
			this->showError(NULL,TEXT("-c"),TEXT("Unable to Initialize Tooltips"));
	}

	// fix to allow pressing enter to work.
	if (Styles & BS_DEFPUSHBUTTON)
		SetFocus(this->m_Hwnd);
}

/*!
 * \brief blah
 *
 * blah
 */

DcxButton::~DcxButton( ) {

	ImageList_Destroy( this->getImageList( ) );

	for ( int i = 0; i < 4; i++ ) {
		if ( this->m_aBitmaps[i] != NULL )
			DeleteBitmap( this->m_aBitmaps[i] );
	}

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::parseControlStyles( const TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme )
{
	const UINT numtok = styles.numtok( );
	*Styles |= BS_NOTIFY;

	for (UINT i = 1; i <= numtok; i++ )
	{
		if ( styles.gettok( i ) == TEXT("bitmap") )
			*Styles |= BS_BITMAP;
		else if ( styles.gettok( i ) == TEXT("default") )
			*Styles |= BS_DEFPUSHBUTTON;
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

void DcxButton::parseInfoRequest( const TString & input, PTCHAR szReturnValue ) const
{
	// [NAME] [ID] [PROP]
	if ( input.gettok( 3 ) == TEXT("text") ) {

		lstrcpyn( szReturnValue, this->m_tsCaption.to_chr( ), MIRC_BUFFER_SIZE_CCH );
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

void DcxButton::parseCommandRequest( const TString & input ) {
	const XSwitchFlags flags(input.gettok(3));
	const int numtok = input.numtok();

	// xdid -c [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	if (flags[TEXT('c')] && numtok > 4) {
		UINT iColorStyles = this->parseColorFlags(input.gettok( 4 ));
		COLORREF clrColor = (COLORREF)input.gettok( 5 ).to_num();

		if (iColorStyles & BTNCS_NORMAL)
			this->m_aColors[0] = clrColor;
		if (iColorStyles & BTNCS_HOVER)
			this->m_aColors[1] = clrColor;
		if (iColorStyles & BTNCS_SELECTED)
			this->m_aColors[2] = clrColor;
		if (iColorStyles & BTNCS_DISABLED)
			this->m_aColors[3] = clrColor;

		this->redrawWindow();
	}
	// xdid -k [NAME] [ID] [SWITCH] [+FLAGS] [COLOR] [FILENAME]
	else if (flags[TEXT('k')] && (numtok > 5) && (this->isStyle(BS_BITMAP) || this->isStyle(BS_OWNERDRAW))) {
		UINT iColorStyles = this->parseColorFlags(input.gettok( 4 ));
		COLORREF clrColor = (COLORREF)input.gettok( 5 ).to_num();

		TString filename(input.gettok(6, -1).trim());

		if (iColorStyles & BTNCS_NORMAL) {
			this->m_aBitmaps[0] = dcxLoadBitmap(this->m_aBitmaps[0], filename);
			this->m_aTransp[0] = clrColor;
		}
		if (iColorStyles & BTNCS_HOVER) {
			this->m_aBitmaps[1] = dcxLoadBitmap(this->m_aBitmaps[1], filename);
			this->m_aTransp[1] = clrColor;
		}
		if (iColorStyles & BTNCS_SELECTED) {
			this->m_aBitmaps[2] = dcxLoadBitmap(this->m_aBitmaps[2], filename);
			this->m_aTransp[2] = clrColor;
		}
		if (iColorStyles & BTNCS_DISABLED) {
			this->m_aBitmaps[3] = dcxLoadBitmap(this->m_aBitmaps[3], filename);
			this->m_aTransp[3] = clrColor;
		}

		this->redrawWindow( );
	}
	// xdid -l [NAME] [ID] [SWITCH] [SIZE]
	else if (flags[TEXT('l')] && numtok > 3) {
		int size = input.gettok( 4 ).to_int();

		if (size == 32 || size == 24)
			this->m_iIconSize = size;
		else
			this->m_iIconSize = 16;

		if (this->getImageList() != NULL) {
			ImageList_Destroy(this->getImageList());
			this->setImageList(NULL);
			this->m_bHasIcons = FALSE;
		}
	}
	// xdid -t [NAME] [ID] [SWITCH] ItemText
	else if ( flags[TEXT('t')] && numtok > 2 ) {
		this->m_tsCaption = (numtok > 3 ? input.gettok(4, -1).trim() : TEXT(""));
		this->redrawWindow();
	}
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags[TEXT('w')] && numtok > 5) {
		HIMAGELIST himl;
		HICON icon = NULL;
		int index = input.gettok( 5 ).to_int();
		TString tflags(input.gettok( 4 ));
		UINT flag = parseColorFlags(tflags);
		TString filename(input.gettok(6, -1));

		// load the icon
		if (this->m_iIconSize > 16)
			icon = dcxLoadIcon(index, filename, true, tflags);
		else
			icon = dcxLoadIcon(index, filename, false, tflags);

		// convert to greyscale
		//if (flag & BTNIS_GREY)
		//	icon = CreateGrayscaleIcon(icon);

		// prepare the image list
		if ((himl = this->getImageList()) == NULL) {
			himl = this->createImageList();

			if (himl) {
				this->setImageList(himl);

				ImageList_AddIcon(himl, icon);
				ImageList_AddIcon(himl, icon);
				ImageList_AddIcon(himl, icon);
				ImageList_AddIcon(himl, icon);

				this->m_bHasIcons = TRUE;
			}
		}
		else {
			if (flag & BTNCS_DISABLED)
				ImageList_ReplaceIcon(himl, 3, icon);
			if (flag & BTNCS_SELECTED)
				ImageList_ReplaceIcon(himl, 2, icon);
			if (flag & BTNCS_HOVER)
				ImageList_ReplaceIcon(himl, 1, icon);
			if (flag & BTNCS_NORMAL)
				ImageList_ReplaceIcon(himl, 0, icon);
		}
		DestroyIcon(icon);

		this->redrawWindow();
	}
	// xdid -m [NAME] [ID] [SWITCH] [ENABLED]
	else if (flags[TEXT('m')] && numtok > 3) {
		int b = input.gettok( 4 ).to_int();

		this->m_bBitmapText = (b ? TRUE : FALSE);
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

UINT DcxButton::parseColorFlags(const TString & flags) {
	INT i = 1, len = (INT)flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != TEXT('+')) 
		return iFlags;

	while (i < len) {
		if (flags[i] == TEXT('d'))
			iFlags |= BTNCS_DISABLED;
		else if (flags[i] == TEXT('h'))
			iFlags |= BTNCS_HOVER;
		else if (flags[i] == TEXT('n'))
			iFlags |= BTNCS_NORMAL;
		else if (flags[i] == TEXT('s'))
			iFlags |= BTNCS_SELECTED;
		else if (flags[i] == TEXT('g'))
			iFlags |= BTNIS_GREY;
		else if (flags[i] == TEXT('a'))
			iFlags |= BTNIS_ASSOC;

		++i;
	}

	return iFlags;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxButton::getImageList(  ) const {

	return this->m_ImageList;
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::setImageList( const HIMAGELIST himl ) {

	this->m_ImageList = himl;
}

/*!
 * \brief blah
 *
 * blah
 */

HIMAGELIST DcxButton::createImageList() {
	return ImageList_Create(this->m_iIconSize, this->m_iIconSize, ILC_COLOR32 | ILC_MASK, 1, 0);
}

TString DcxButton::getStyles(void) const
{
	TString styles(__super::getStyles());
	DWORD Styles;
	Styles = GetWindowStyle(this->m_Hwnd);
	if (Styles & BS_BITMAP)
		styles.addtok(TEXT("bitmap"));
	if (Styles & BS_DEFPUSHBUTTON) 
		styles.addtok(TEXT("default"));
	return styles;
}
	
void DcxButton::toXml(TiXmlElement * xml) const
{
	__super::toXml(xml);
	xml->SetAttribute("caption", this->m_tsCaption.c_str());
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxButton::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	switch( uMsg )
	{
		case WM_COMMAND:
		{
			switch ( HIWORD( wParam ) )
			{
			case BN_CLICKED:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx(TEXT("%s,%d"), TEXT("sclick"), this->getUserID( ) );
				}
				break;
			case BN_DBLCLK:
				{
					if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
						this->execAliasEx(TEXT("%s,%d"), TEXT("dclick"), this->getUserID( ) );
				}
				break;
			}
		}
		break;
		//case WM_NOTIFY:
		//	{
		//		LPNMHDR pnmh = (LPNMHDR)lParam;
		//		switch (pnmh->code) {
		//			case BCN_HOTITEMCHANGE:
		//				{
		//					LPNMBCHOTITEM item = (LPNMBCHOTITEM)lParam;
		//					if (item->dwFlags & HICF_ENTERING)
		//						mIRCError(TEXT("Entering Button"));
		//					else
		//						mIRCError(TEXT("Leaving Button"));
		//				}
		//				break;
		//			default:
		//				break;
		//		}
		//	}
		//	break;
	}
	return 0L;
}

LRESULT DcxButton::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

	case WM_MOUSEMOVE:
		{
			this->m_pParentDialog->setMouseControl( this->getUserID( ) );

			if ( this->m_bTracking == FALSE ) {
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.hwndTrack = this->m_Hwnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER;
				tme.dwHoverTime = HOVER_DEFAULT; //1;
				this->m_bTracking = (BOOL) _TrackMouseEvent( &tme );		
			}

			if (this->m_bTouched == FALSE) {
				this->m_bTouched = TRUE;
				InvalidateRect( this->m_Hwnd, NULL, FALSE );
			}
		}
		break;

	case WM_MOUSEHOVER:
		{
			if ( this->m_bHover == FALSE && this->m_bTracking ) {
				this->m_bHover = TRUE;
			}
		}
		break;

	case WM_MOUSELEAVE:
		{
			if ( this->m_bTracking ) {
				this->m_bHover = FALSE;
				this->m_bTracking = FALSE;
				this->m_bTouched = FALSE;
				this->m_bSelected = FALSE;
				InvalidateRect( this->m_Hwnd, NULL, FALSE );
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
			if ( this->m_bSelected == FALSE ) {
				this->m_bSelected = TRUE;
				InvalidateRect( this->m_Hwnd, NULL, FALSE );
			}
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx(TEXT("%s,%d"), TEXT("lbdown"), this->getUserID());
		}
		break;

	case WM_LBUTTONUP:
		{
			this->m_bSelected = FALSE;
			if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				this->execAliasEx(TEXT("%s,%d"), TEXT("lbup"), this->getUserID());
		}
		break;

	case WM_ENABLE:
		{
			InvalidateRect( this->m_Hwnd, NULL, TRUE );
		}
		break;

	case WM_UPDATEUISTATE:
		{
			InvalidateRect( this->m_Hwnd, NULL, TRUE );
		}
		break;

	case WM_ERASEBKGND:
		{
			bParsed = TRUE;
			return TRUE;
		}
		break;
	case WM_PRINTCLIENT:
		{
			this->DrawClientArea((HDC)wParam, uMsg, lParam);
			bParsed = TRUE;
		}
		break;
	case WM_PAINT:
		{
			bParsed = TRUE;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( this->m_Hwnd, &ps );

			this->DrawClientArea(hdc, uMsg, lParam);

			EndPaint( this->m_Hwnd, &ps );
		}
		break;

	case WM_DESTROY:
		{
			delete this;
			bParsed = TRUE;
		}
		break;

	default:
		return this->CommonMessage( uMsg, wParam, lParam, bParsed);
		break;
  }

  return 0L;
}

void DcxButton::DrawClientArea(HDC hdc, const UINT uMsg, LPARAM lParam)
{
	BOOL isBitmap = this->isStyle(BS_BITMAP);
	int nState; // get buttons state.
	RECT rcClient;

	// get controls client area
	GetClientRect( this->m_Hwnd, &rcClient );
	// get controls width & height.
	int w = (rcClient.right - rcClient.left), h = (rcClient.bottom - rcClient.top);

	if ( IsWindowEnabled( this->m_Hwnd ) == FALSE )
		nState = 3;
	else if ( this->m_bSelected )
		nState = 2;
	else if ( this->m_bTouched )
		nState = 1;
	else
		nState = 0;

	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

	HTHEME hTheme = NULL;
	int iStateId = 0;
	if (!this->m_bNoTheme && Dcx::UXModule.dcxIsThemeActive()) {
		hTheme = Dcx::UXModule.dcxOpenThemeData(this->m_Hwnd, L"BUTTON");

		// this allows the theme buttons to have a transparent background like the normal ones
		switch (nState)
		{
		case 1:
			iStateId = PBS_HOT;
			break;
		case 2:
			iStateId = PBS_PRESSED;
			break;
		case 3:
			iStateId = PBS_DISABLED;
			break;
		default:
			iStateId = PBS_NORMAL;
			break;
		}
	}

	// fill background.
	if (this->isExStyle(WS_EX_TRANSPARENT))
	{ // fill with parent image
		if (!this->m_bAlphaBlend)
			this->DrawParentsBackground(hdc,&rcClient);
	}
	else // normal bkg
		DcxControl::DrawCtrlBackground(hdc,this,&rcClient, hTheme, BP_PUSHBUTTON, iStateId);

	// Bitmapped button
	if (isBitmap) {
		// create a new HDC for background rendering
		HDC hdcbmp = CreateCompatibleDC( hdc );
		if (hdcbmp != NULL) {
			BITMAP bmp;

			// get bitmaps info.
			GetObject( this->m_aBitmaps[nState], sizeof(BITMAP), &bmp );
			// associate bitmap with HDC
			HBITMAP oldbm = SelectBitmap( hdcbmp, this->m_aBitmaps[nState] );
			TransparentBlt( hdc, rcClient.left, rcClient.top, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_aTransp[nState] );
			SelectBitmap( hdcbmp, oldbm ); // got to put the old bm back.
			DeleteDC( hdcbmp );
		}
	}

	// Regular button
	if ((!isBitmap) || (this->m_bBitmapText)) {          
		// draw default window bg
		if (!isBitmap) {
			if (hTheme != NULL) {
				// This method causes the button to valis when the dialog is resized using the redrawBuffered() function.
				//HRGN hRgn = NULL;
				//if (GetThemeBackgroundRegionUx(hTheme, hdc, BP_PUSHBUTTON, iStateId, &rcClient, &hRgn) == S_OK)
				//	SelectClipRgn(hdc, hRgn);

				//CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);
				//if (hRgn != NULL)
				//	DeleteRgn(hRgn);
				// This method causes the theme bkg to re-appear during resize, but button is otherwise drawn correctly.
				if (uMsg != WM_PRINTCLIENT) {
					HRGN hRgn = NULL;
					if (Dcx::UXModule.dcxGetThemeBackgroundRegion(hTheme, hdc, BP_PUSHBUTTON, iStateId, &rcClient, &hRgn) == S_OK)
					{
						HRGN hZeroRgn = CreateRectRgn(0,0,0,0);
						if ((hRgn != NULL) && (!EqualRgn(hRgn, hZeroRgn)))
							SelectClipRgn(hdc, hRgn);
						DeleteRgn(hZeroRgn);
						DeleteRgn(hRgn);
					}
				}
				CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);
				// This method causes the button to valis when the dialog is resized using the redrawBuffered() function.
				//HDC *buf = CreateHDCBuffer(hdc, &rcClient);
				//if (buf != NULL) {
				//	COLORREF bkgClr = RGB(0,0,0);
				//	GetThemeColorUx(hTheme, BP_PUSHBUTTON, iStateId, TMT_FILLCOLOR, &bkgClr);
				//	//bkgClr = GetPixel(*buf, 0,0);
				//	CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)*buf, lParam);
				//	TransparentBlt( hdc, rcClient.left, rcClient.top, w, h, *buf, 0, 0, w, h, bkgClr );
				//	DeleteHDCBuffer(buf);
				//}
				//else
				//	CallWindowProc(this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM)hdc, lParam);
			}
			else {
				RECT rc = rcClient;
				UINT iState = DFCS_BUTTONPUSH|DFCS_ADJUSTRECT;
				switch (nState)
				{
				case 1:
					iState |= DFCS_HOT;
					break;
				case 2:
					iState |= DFCS_PUSHED;
					break;
				case 3:
					iState |= DFCS_INACTIVE;
					break;
				default:
					break;
				}
				DrawFrameControl(hdc, &rc, DFC_BUTTON, iState);
				if (this->m_bGradientFill) {
					COLORREF clrStart = this->m_clrStartGradient;
					COLORREF clrEnd = this->m_clrEndGradient;

					if (clrStart == CLR_INVALID)
						clrStart = GetSysColor(COLOR_3DFACE);
					if (clrEnd == CLR_INVALID)
						clrEnd = GetSysColor(COLOR_GRADIENTACTIVECAPTION);

					XPopupMenuItem::DrawGradient( hdc, &rc, clrStart, clrEnd, this->m_bGradientVertical);
				}
			}
		}

		//HFONT hFontOld = SelectFont( hdc, this->m_hFont );

		//RECT rcTxt;
		//SetRectEmpty( &rcTxt );

		//SetBkMode( hdc, TRANSPARENT );

		//HIMAGELIST himl = this->getImageList( );

		//SetTextColor(hdc, this->m_aColors[nState]);

		////if ( this->m_tsCaption.len( ) > 0 )
		////	DrawTextW(hdc, this->m_tsCaption.to_chr(), -1, &rcTxt, DT_CALCRECT | DT_SINGLELINE);
		//if ( this->m_tsCaption.len( ) > 0 )
		//	this->calcTextRect(hdc, this->m_tsCaption, &rcTxt, /*DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP |*/ DT_SINGLELINE);

		//int iCenter = w / 2;
		//int iVCenter = h / 2;
		//int iTextW = ( rcTxt.right - rcTxt.left );
		//int iTextH = ( rcTxt.bottom - rcTxt.top );

		//// If there is an icon
		//if (himl != NULL && this->m_bHasIcons) {
		//	int iIconLeft = iCenter - (this->m_iIconSize + ICON_XPAD + iTextW) / 2;
		//	int iIconTop = iVCenter - this->m_iIconSize / 2;

		//	if (iIconLeft < BUTTON_XPAD)
		//		iIconLeft = BUTTON_XPAD;

		//	if (iIconTop < BUTTON_YPAD)
		//		iIconTop = BUTTON_YPAD;

		//	rcTxt.left = iIconLeft + this->m_iIconSize + ICON_XPAD;

		//	if (nState == 3) // disabled
		//		ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT | ILD_BLEND50);
		//	else
		//		ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT);
		//}
		//else {
		//	rcTxt.left = iCenter - iTextW / 2;
		//	if ( rcTxt.left < BUTTON_XPAD )
		//		rcTxt.left = BUTTON_XPAD;
		//}

		//if ( iTextW > 0 ) {
		//	rcTxt.top = iVCenter - iTextH / 2;

		//	if ( rcTxt.top < BUTTON_YPAD )
		//		rcTxt.top = BUTTON_YPAD;

		//	rcTxt.right = rcClient.right - BUTTON_XPAD;
		//	rcTxt.bottom = rcClient.bottom - BUTTON_YPAD;
		//	//this->ctrlDrawText(hdc,this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE);
		//	if (!this->m_bCtrlCodeText) {
		//		if (!this->m_bSelected && this->m_bShadowText)
		//			dcxDrawShadowText(hdc,this->m_tsCaption.to_chr(), (UINT)this->m_tsCaption.len(),&rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, this->m_aColors[nState], 0, 5, 5);
		//		else
		//			DrawTextW( hdc, this->m_tsCaption.to_chr(), (int)this->m_tsCaption.len( ), &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
		//	}
		//	else
		//		mIRC_DrawText(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, ((!this->m_bSelected && this->m_bShadowText) ? true : false));
		//}
		HFONT hFontOld = SelectFont( hdc, this->m_hFont );

		int oldbkg = SetBkMode( hdc, TRANSPARENT );

		HIMAGELIST himl = this->getImageList( );

		RECT rcTxt;

		rcTxt.top = BUTTON_YPAD;
		rcTxt.right = (rcClient.right - BUTTON_XPAD);
		rcTxt.bottom = (rcClient.bottom - BUTTON_YPAD);
		rcTxt.left = BUTTON_XPAD;

		if ( this->m_tsCaption.len( ) > 0 ) {
			//this->calcTextRect(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE);
			TString t(this->m_tsCaption);
			if (this->m_bCtrlCodeText)
				t.strip();
			if (!this->m_bSelected && this->m_bShadowText)
				dcxDrawShadowText(hdc, t.to_chr(), t.len(), &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_CALCRECT, this->m_aColors[nState], 0,5,5);
			else
				DrawText(hdc, t.to_chr(), t.len(), &rcTxt, DT_WORD_ELLIPSIS | DT_SINGLELINE | DT_CALCRECT);
		}

		int iCenter = w / 2;
		int iVCenter = h / 2;
		int iTextW = ( rcTxt.right - rcTxt.left );
		int iTextH = ( rcTxt.bottom - rcTxt.top );

		// If there is an icon
		if (himl != NULL && this->m_bHasIcons) {
			int iIconLeft = (iCenter - ((this->m_iIconSize + ICON_XPAD + iTextW) / 2));
			int iIconTop = (iVCenter - (this->m_iIconSize / 2));

			if (iIconLeft < BUTTON_XPAD)
				iIconLeft = BUTTON_XPAD;

			if (iIconTop < BUTTON_YPAD)
				iIconTop = BUTTON_YPAD;

			rcTxt.left = iIconLeft + this->m_iIconSize + ICON_XPAD;

			if (nState == 3) // disabled
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT | ILD_BLEND50);
			else
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT);
		}
		else {
			rcTxt.left = (iCenter - (iTextW / 2));
			if ( rcTxt.left < BUTTON_XPAD )
				rcTxt.left = BUTTON_XPAD;
		}

		if ( iTextW > 0 ) {
			rcTxt.top = iVCenter - iTextH / 2;

			if ( rcTxt.top < BUTTON_YPAD )
				rcTxt.top = BUTTON_YPAD;

			rcTxt.right = (rcClient.right - BUTTON_XPAD);
			rcTxt.bottom = (rcClient.bottom - BUTTON_YPAD);

			COLORREF oldClr = SetTextColor(hdc, this->m_aColors[nState]);

			//this->ctrlDrawText(hdc,this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE);
			if (!this->m_bCtrlCodeText) {
				if (!this->m_bSelected && this->m_bShadowText)
					dcxDrawShadowText(hdc,this->m_tsCaption.to_chr(), (UINT)this->m_tsCaption.len(),&rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, this->m_aColors[nState], 0, 5, 5);
				else
					DrawTextW( hdc, this->m_tsCaption.to_chr(), (int)this->m_tsCaption.len( ), &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
			}
			else
				mIRC_DrawText(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, ((!this->m_bSelected && this->m_bShadowText) ? true : false));

			SetTextColor(hdc, oldClr);
		}

		SetBkMode( hdc, oldbkg );

		SelectFont( hdc, hFontOld );
	}

	if (hTheme != NULL)
		Dcx::UXModule.dcxCloseThemeData(hTheme);

	this->FinishAlphaBlend(ai);
}
