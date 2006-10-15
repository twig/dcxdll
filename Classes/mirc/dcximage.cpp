/*!
 * \file dcximage.cpp
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

#include "dcximage.h"
#include "../dcxdialog.h"

/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

//DcxImage::DcxImage( UINT ID, DcxDialog * p_Dialog, RECT * rc, TString & styles ) 
//: DcxControl( ID, p_Dialog ) 
//, m_bIsIcon(FALSE)
//, m_iIconSize(0)
//{
//
//  LONG Styles = 0, ExStyles = 0;
//  BOOL bNoTheme = FALSE;
//  this->parseControlStyles( styles, &Styles, &ExStyles, &bNoTheme );
//
//  this->m_Hwnd = CreateWindowEx(	
//    ExStyles, 
//    "STATIC", 
//    NULL,
//    WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | Styles, 
//    rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
//    p_Dialog->getHwnd( ),
//    (HMENU) ID,
//    GetModuleHandle( NULL ), 
//    NULL);
//
//  if ( bNoTheme )
//    dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );
//
//  this->m_pImage = NULL;
//	this->m_bResizeImage = false;
//  this->m_hBitmap = NULL;
//  this->m_clrTransColor = -1;
//  this->m_hIcon = NULL;
//
//	if (p_Dialog->getToolTip() != NULL) {
//		if (styles.istok("tooltips"," ")) {
//
//			this->m_ToolTipHWND = p_Dialog->getToolTip();
//
//			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
//		}
//	}
//
//  this->registreDefaultWindowProc( );
//  SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
//}
//
/*!
 * \brief Constructor
 *
 * \param ID Control ID
 * \param p_Dialog Parent DcxDialog Object
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxImage::DcxImage( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_bIsIcon(FALSE)
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

	this->m_pImage = NULL;
	this->m_bResizeImage = true;
	this->m_hBitmap = NULL;
	this->m_clrTransColor = -1;
	this->m_hIcon = NULL;

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips"," ")) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

	this->registreDefaultWindowProc( );
	SetProp( this->m_Hwnd, "dcx_cthis", (HANDLE) this );
}

/*!
 * \brief blah
 *
 * blah
 */

DcxImage::~DcxImage() {
	PreloadData();

	this->unregistreDefaultWindowProc( );
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxImage::parseControlStyles(TString &styles, LONG *Styles, LONG *ExStyles, BOOL *bNoTheme) {
  unsigned int i = 1, numtok = styles.numtok( " " );
	*Styles |= SS_NOTIFY;

	while ( i <= numtok ) {
		if ( styles.gettok( i , " " ) == "transparent" )
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

void DcxImage::parseInfoRequest( TString & input, char * szReturnValue ) {

//  int numtok = input.numtok( " " );

  if ( this->parseGlobalInfoRequest( input, szReturnValue ) ) {

    return;
  }
  
  szReturnValue[0] = 0;
}

// clears existing image and icon data and sets pointers to null
void DcxImage::PreloadData() {
	if (this->m_hBitmap != NULL) {
		DeleteBitmap(this->m_hBitmap);
		this->m_hBitmap = NULL;
	}

	if (this->m_hIcon != NULL) {
		DestroyIcon(this->m_hIcon);
		this->m_hIcon = NULL;
	}

	if (this->m_pImage != NULL) {
		delete this->m_pImage;
		this->m_pImage = NULL;
	}
}

/*!
 * \brief blah
 *
 * blah
 */

void DcxImage::parseCommandRequest(TString & input) {
	XSwitchFlags flags;
	ZeroMemory((void*)&flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok(3, " "), &flags);
	int numtok = input.numtok(" ");

	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [SIZE] [FILENAME]
	if (flags.switch_flags[22] && numtok > 6) {
		TString flags(input.gettok(4, " "));
		int index = input.gettok(5, " ").to_int();
		int size = input.gettok(6, " ").to_int();
		TString filename(input.gettok(7, -1, " "));

		filename.trim();
		PreloadData();

		if (size > 16)
			ExtractIconEx(filename.to_chr(), index, &(this->m_hIcon), NULL, 1);
		else
			ExtractIconEx(filename.to_chr(), index, NULL, &(this->m_hIcon), 1);

		if (flags.find('g', 0))
			this->m_hIcon = CreateGrayscaleIcon(this->m_hIcon);

		this->m_iIconSize = size;
		this->m_bIsIcon = TRUE;

		// resize window to size of icon
		RECT wnd;
		POINT pt;

		GetWindowRect(this->m_Hwnd, &wnd);
		pt.x = wnd.left;
		pt.y = wnd.top;

		ScreenToClient(GetParent(this->m_Hwnd), &pt);
		MoveWindow(this->m_Hwnd, pt.x, pt.y, size, size, TRUE);

		//InvalidateRect(this->m_Hwnd, NULL, TRUE);
		this->redrawWindow();
	}
	//xdid -i [NAME] [ID] [SWITCH] [IMAGE]
	else if (flags.switch_flags[8] && numtok > 3) {
		TString filename(input.gettok(4, -1, " "));

		filename.trim();
		PreloadData();

		//this->m_hBitmap = dcxLoadBitmap(this->m_hBitmap, filename);
		// using this method allows you to render BMP, ICON, GIF, JPEG, Exif, PNG, TIFF, WMF, and EMF (no animation)
		int widelen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename.to_chr(),-1, NULL, 0);
		WCHAR  *wfilename = new WCHAR[widelen+1];
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,filename.to_chr(),-1, wfilename, widelen);
		this->m_pImage = new Image(wfilename);
		delete [] wfilename;
		this->m_bIsIcon = FALSE;
		InvalidateRect(this->m_Hwnd, NULL, TRUE);
	}
	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	else if (flags.switch_flags[10] && numtok > 3) {
		this->m_clrTransColor = (COLORREF)input.gettok(4, " ").to_num();
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [1|0]
	else if (flags.switch_cap_flags[18] && numtok > 3) {
		if (input.gettok(4, " ").to_int() > 0)
			this->m_bResizeImage = true;
		else
			this->m_bResizeImage = false;

		InvalidateRect(this->m_Hwnd, NULL, TRUE);
		//UpdateWindow(this->m_Hwnd);
		//this->redrawWindow();
	}
	else
		this->parseGlobalCommandRequest(input, flags);
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxImage::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
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

LRESULT DcxImage::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

  switch( uMsg ) {

    case WM_HELP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_HELP)
	        this->callAliasEx( NULL, "%s,%d", "help", this->getUserID( ) );
      }
      break;

    case WM_ERASEBKGND:
			{
				if (!this->isExStyle(WS_EX_TRANSPARENT)) {
					HDC hdc = (HDC) wParam;

					RECT rect;
					GetClientRect( this->m_Hwnd, &rect );

					if ( this->m_hBackBrush != NULL )
						FillRect( hdc, &rect, this->m_hBackBrush );
					else
						FillRect( hdc, &rect, GetSysColorBrush( COLOR_3DFACE ));
				}

				bParsed = TRUE;
				return TRUE;
      }
      break;

		case WM_PAINT:
		{
			// default paint method
			if ((this->m_hBitmap == NULL) && (this->m_hIcon == NULL) && (this->m_pImage == NULL))
				break;

			PAINTSTRUCT ps; 
			HDC hdc; 
			RECT rect;

			hdc = BeginPaint(this->m_Hwnd, &ps);
			GetClientRect(this->m_Hwnd, &rect);

			// draw bitmap
			if ((this->m_hBitmap != NULL) && (!this->m_bIsIcon) && (this->m_pImage == NULL)) {
				HDC hdcbmp = CreateCompatibleDC(hdc);
				BITMAP bmp;

				GetObject( this->m_hBitmap, sizeof(BITMAP), &bmp );
				SelectObject( hdcbmp, this->m_hBitmap );

				if (this->m_clrTransColor != -1) {
					TransparentBlt(hdc, rect.left, rect.top, (rect.right - rect.left),
						(rect.bottom - rect.top), hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_clrTransColor);
				}
				else {
					StretchBlt( hdc, rect.left, rect.top, rect.right - rect.left, 
						rect.bottom - rect.top, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
				}

				DeleteDC( hdcbmp );
			}
			// draw icon
			else if ((this->m_hIcon != NULL) && (this->m_bIsIcon)) {
				DrawIconEx(hdc, 0, 0, this->m_hIcon, this->m_iIconSize, this->m_iIconSize, 0, this->m_hBackBrush, DI_NORMAL | DI_COMPAT); 
			}
			else if (this->m_pImage != NULL) {
				Graphics grphx( hdc );

				if (this->m_bResizeImage)
					grphx.DrawImage( this->m_pImage, 0, 0, rect.right - rect.left, rect.bottom - rect.top );
				else
					grphx.DrawImage( this->m_pImage, 0, 0);
			}
			EndPaint(this->m_Hwnd, &ps);

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

    case WM_SIZE:
      {
        InvalidateRect( this->m_Hwnd, NULL, TRUE );
      }
      break;

    case WM_CONTEXTMENU:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "rclick", this->getUserID( ) );
      }
      break;

    case WM_LBUTTONUP:
      {
				if (this->m_pParentDialog->getEventMask() & DCX_EVENT_CLICK)
					this->callAliasEx( NULL, "%s,%d", "lbup", this->getUserID( ) );
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