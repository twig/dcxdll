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

#include "dcxbutton.h"
#include "dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxButton::DcxButton( const UINT ID, DcxDialog * p_Dialog, HWND mParentHwnd, RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_bBitmapText(FALSE)
, m_bHasIcons(FALSE)
, m_bHover(FALSE)
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

	if ( bNoTheme )
		dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

	this->m_bNoTheme = (bNoTheme ? true : false);

	ZeroMemory( &this->m_aBitmaps, 4*sizeof(HBITMAP) );
	ZeroMemory( &this->m_aColors, 4*sizeof(COLORREF) );
	ZeroMemory( &this->m_aTransp, 4*sizeof(COLORREF) );

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips")) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

	this->m_aColors[0] = GetSysColor(COLOR_BTNTEXT); // normal
	this->m_aColors[1] = GetSysColor(COLOR_BTNTEXT); // hover
	this->m_aColors[2] = GetSysColor(COLOR_BTNTEXT); // pushed
	this->m_aColors[3] = GetSysColor(COLOR_GRAYTEXT); // disabled

	this->setControlFont( (HFONT) GetStockObject( DEFAULT_GUI_FONT ), FALSE );
	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
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
      DeleteObject( this->m_aBitmaps[i] );
  }

  this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxButton::parseControlStyles( TString & styles, LONG * Styles, LONG * ExStyles, BOOL * bNoTheme ) {

  unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= BS_NOTIFY;

  while ( i <= numtok ) {

    if ( styles.gettok( i ) == "bitmap" )
      *Styles |= BS_BITMAP;
    else if ( styles.gettok( i ) == "default" )
      *Styles |= BS_DEFPUSHBUTTON;
    else if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;
		else if (( styles.gettok( i ) == "shadow" ))
			this->m_bShadowText = true;
		else if (( styles.gettok( i ) == "noformat" ))
			this->m_bCtrlCodeText = false;

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

void DcxButton::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( );

  // [NAME] [ID] [PROP]
  if ( input.gettok( 3 ) == "text" ) {

    lstrcpyn( szReturnValue, this->m_tsCaption.to_chr( ), 900 );
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

void DcxButton::parseCommandRequest( TString & input ) {

  XSwitchFlags flags;
  ZeroMemory( (void*)&flags, sizeof( XSwitchFlags ) );
  this->parseSwitchFlags( input.gettok( 3 ), &flags );
  int numtok = input.numtok( );

	// xdid -c [NAME] [ID] [SWITCH] [+FLAGS] [COLOR]
	if (flags.switch_flags[2] && numtok > 4) {
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
	else if (flags.switch_flags[10] && (numtok > 5) && (this->isStyle(BS_BITMAP) || this->isStyle(BS_OWNERDRAW))) {
		UINT iColorStyles = this->parseColorFlags(input.gettok( 4 ));
		COLORREF clrColor = (COLORREF)input.gettok( 5 ).to_num();

		TString filename(input.gettok(6, -1));
		filename.trim();

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
	else if (flags.switch_flags[11] && numtok > 3) {
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
  else if ( flags.switch_flags[19] && numtok > 2 ) {
		this->m_tsCaption = (numtok > 3 ? input.gettok( 4, -1 ) : "");
    this->m_tsCaption.trim( );
    this->redrawWindow( );
  }
	// xdid -w [NAME] [ID] [SWITCH] [FLAGS] [INDEX] [FILENAME]
	else if (flags.switch_flags[22] && numtok > 5) {
		HIMAGELIST himl;
		HICON icon = NULL;
		int index = input.gettok( 5 ).to_int();
		TString tflags(input.gettok( 4 ));
		UINT flag = parseColorFlags(tflags);
		TString filename(input.gettok(6, -1));

		// load the icon
		if (this->m_iIconSize > 16)
			icon = dcxLoadIcon(index, filename, TRUE, tflags);
		else
			icon = dcxLoadIcon(index, filename, FALSE, tflags);

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
	}
	// xdid -m [NAME] [ID] [SWITCH] [ENABLED]
  else if (flags.switch_flags[12] && numtok > 3) {
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

UINT DcxButton::parseColorFlags(TString & flags) {
	INT i = 1, len = flags.len(), iFlags = 0;

	// no +sign, missing params
	if (flags[0] != '+') 
		return iFlags;

	while (i < len) {
		if (flags[i] == 'd')
			iFlags |= BTNCS_DISABLED;
		else if (flags[i] == 'h')
			iFlags |= BTNCS_HOVER;
		else if (flags[i] == 'n')
			iFlags |= BTNCS_NORMAL;
		else if (flags[i] == 's')
			iFlags |= BTNCS_SELECTED;
		else if (flags[i] == 'g')
			iFlags |= BTNIS_GREY;
		else if (flags[i] == 'a')
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

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxButton::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
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
          case BN_DBLCLK:
            {
							if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
				        this->callAliasEx( NULL, "%s,%d", "dclick", this->getUserID( ) );
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
		//						mIRCError("Entering Button");
		//					else
		//						mIRCError("Leaving Button");
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
          tme.dwHoverTime = 1;
          this->m_bTracking = (BOOL) _TrackMouseEvent( &tme );		
        }
      }
      break;

    case WM_MOUSEHOVER:
      {
        if ( this->m_bHover == FALSE && this->m_bTracking ) {
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
					this->callAliasEx(NULL, "%s,%d", "lbdown", this->getUserID());
			}
			break;

    case WM_LBUTTONUP:
      {
				this->m_bSelected = FALSE;
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx(NULL, "%s,%d", "lbup", this->getUserID());
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
				HDC hdc;

				hdc = BeginPaint( this->m_Hwnd, &ps );

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
	else if ( this->m_bHover )
		nState = 1;
	else
		nState = 0;

	// Setup alpha blend if any.
	LPALPHAINFO ai = this->SetupAlphaBlend(&hdc);

	// fill background.
	if (this->isExStyle(WS_EX_TRANSPARENT))
	{
		if (!this->m_bAlphaBlend)
			this->DrawParentsBackground(hdc,&rcClient);
	}
	else
		DcxControl::DrawCtrlBackground(hdc,this,&rcClient);

	// Bitmapped button
	if (isBitmap) {
		// create a new HDC for background rendering
		HDC hdcbmp = CreateCompatibleDC( hdc );
		if (hdcbmp != NULL) {
			BITMAP bmp;

			// get bitmaps info.
			GetObject( this->m_aBitmaps[nState], sizeof(BITMAP), &bmp );
			// associate bitmap with HDC
			HBITMAP oldbm = (HBITMAP)SelectObject( hdcbmp, this->m_aBitmaps[nState] );
			TransparentBlt( hdc, rcClient.left, rcClient.top, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_aTransp[nState] );
			SelectObject( hdcbmp, oldbm ); // got to put the old bm back.
			DeleteDC( hdcbmp );
		}
	}

	// Regular button
	if ((!isBitmap) || (this->m_bBitmapText)) {          
		// draw default window bg
		if (!isBitmap) {
			if (!this->m_bNoTheme && dcxIsThemeActive()) {
				// this allows the theme buttons to have a transparent background like the normal ones
				HRGN hRgn = NULL;
				int iState;
				switch (nState)
				{
				case 1:
					iState = PBS_HOT;
					break;
				case 2:
					iState = PBS_PRESSED;
					break;
				case 3:
					iState = PBS_DISABLED;
					break;
				default:
					iState = PBS_NORMAL;
					break;
				}
				//HTHEME hTheme = GetWindowThemeUx(this->m_Hwnd);
				HTHEME hTheme = OpenThemeDataUx(this->m_Hwnd, L"BUTTON");
				if (GetThemeBackgroundRegionUx(hTheme, hdc, BP_PUSHBUTTON,iState,&rcClient, &hRgn) == S_OK)
					SelectClipRgn(hdc, hRgn);
				CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, WM_PRINTCLIENT, (WPARAM) hdc, PRF_CLIENT );
				DeleteRgn(hRgn);
				CloseThemeDataUx(hTheme);
			}
			else
				CallWindowProc( this->m_DefaultWindowProc, this->m_Hwnd, uMsg, (WPARAM) hdc, lParam );
		}

		HFONT hFontOld = (HFONT) SelectObject( hdc, this->m_hFont );

		RECT rcTxt;
		SetRectEmpty( &rcTxt );

		SetBkMode( hdc, TRANSPARENT );

		HIMAGELIST himl = this->getImageList( );

		SetTextColor(hdc, this->m_aColors[nState]);

		if ( this->m_tsCaption.len( ) > 0 )
			DrawText( hdc, this->m_tsCaption.to_chr( ), -1, &rcTxt, DT_CALCRECT | DT_SINGLELINE );

		int iCenter = w / 2;
		int iVCenter = h / 2;
		int iTextW = ( rcTxt.right - rcTxt.left );
		int iTextH = ( rcTxt.bottom - rcTxt.top );

		int iIconLeft = 0;
		int iIconTop = 0;

		rcTxt.left = iCenter - iTextW / 2;
		rcTxt.top = iVCenter - iTextH / 2;

		if ( rcTxt.left < BUTTON_XPAD )
			rcTxt.left = BUTTON_XPAD;

		if ( rcTxt.top < BUTTON_YPAD )
			rcTxt.top = BUTTON_YPAD;

		rcTxt.right = rcClient.right - BUTTON_XPAD;
		rcTxt.bottom = rcClient.bottom - BUTTON_YPAD;

		// If there is an icon
		if (himl != NULL && this->m_bHasIcons) {
			iIconLeft = iCenter - (this->m_iIconSize + ICON_XPAD + iTextW) / 2;
			iIconTop = iVCenter - this->m_iIconSize / 2;

			if (iIconLeft < BUTTON_XPAD)
				iIconLeft = BUTTON_XPAD;

			if (iIconTop < BUTTON_YPAD)
				iIconTop = BUTTON_YPAD;

			rcTxt.left = iIconLeft + this->m_iIconSize + ICON_XPAD;

			if (IsWindowEnabled(this->m_Hwnd) == FALSE)
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT | ILD_BLEND50);
			else
				ImageList_Draw(himl, nState, hdc, iIconLeft, iIconTop, ILD_TRANSPARENT);
		}

		if ( this->m_tsCaption.len( ) > 0 ) {
			if (!this->m_bCtrlCodeText) {
				if (!this->m_bSelected && this->m_bShadowText) // could cause problems with pre-XP as this is commctrl v6+
					dcxDrawShadowText(hdc,this->m_tsCaption.to_wchr(), this->m_tsCaption.len(),&rcTxt,
						DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, this->m_aColors[nState], 0, 5, 5);
				else
					DrawText( hdc, this->m_tsCaption.to_chr( ), this->m_tsCaption.len( ), 
						&rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE );
			}
			else
				mIRC_DrawText(hdc, this->m_tsCaption, &rcTxt, DT_WORD_ELLIPSIS | DT_LEFT | DT_TOP | DT_SINGLELINE, ((!this->m_bSelected && this->m_bShadowText) ? true : false));
		}

		SelectObject( hdc, hFontOld );
	}

	this->FinishAlphaBlend(ai);
}
