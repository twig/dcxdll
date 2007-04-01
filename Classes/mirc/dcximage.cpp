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
 * \param mParentHwnd Parent Window Handle
 * \param rc Window Rectangle
 * \param styles Window Style Tokenized List
 */

DcxImage::DcxImage( const UINT ID, DcxDialog * p_Dialog, const HWND mParentHwnd, const RECT * rc, TString & styles ) 
: DcxControl( ID, p_Dialog )
, m_bIsIcon(FALSE)
#ifdef DCX_USE_GDIPLUS
, m_pImage(NULL)
, m_CMode(CompositingModeSourceCopy)
, m_CQuality(CompositingQualityDefault)
, m_IMode(InterpolationModeDefault)
, m_SMode(SmoothingModeDefault)
#endif
, m_bResizeImage(true)
, m_bTileImage(false)
, m_hBitmap(NULL)
, m_clrTransColor(-1)
, m_hIcon(NULL)
, m_bBuffer(false)
, m_iXOffset(0)
, m_iYOffset(0)
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

	if (p_Dialog->getToolTip() != NULL) {
		if (styles.istok("tooltips")) {
			this->m_ToolTipHWND = p_Dialog->getToolTip();
			AddToolTipToolInfo(this->m_ToolTipHWND, this->m_Hwnd);
		}
	}

#ifdef DCX_USE_GDIPLUS
	if (p_Dialog->isExStyle(WS_EX_COMPOSITED))
		this->m_bBuffer = true;
#endif

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
	unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= SS_NOTIFY;

	while ( i <= numtok ) {
		if ( styles.gettok( i ) == "alpha" )
			this->m_bAlphaBlend = true;

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

	//int numtok = input.numtok( );

	TString prop(input.gettok( 3 ));

	// [NAME] [ID] [PROP]
	if ( prop == "fname") {
		lstrcpyn(szReturnValue,this->m_tsFilename.to_chr(), 900);
		return;
	}
  else if ( this->parseGlobalInfoRequest( input, szReturnValue ) )
    return;
  
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

#ifdef DCX_USE_GDIPLUS
	if (this->m_pImage != NULL) {
		delete this->m_pImage;
		this->m_pImage = NULL;
	}
#endif
	this->m_tsFilename = "";
}

#ifdef DCX_USE_GDIPLUS
bool DcxImage::LoadGDIPlusImage(const TString &flags, TString &filename) {
	if (!IsFile(filename)) {
		this->showError(NULL,"LoadGDIPlusImage", "Unable to open file");
		//dcxInfoError("Image",
		//	"LoadGDIPlusImage",
		//	this->m_pParentDialog->getName().to_chr(),
		//	this->getUserID(),
		//	"Unable to open file");
		return false;
	}
	this->m_pImage = new Image(filename.to_wchr(),TRUE);

	// couldnt allocate image object.
	if (this->m_pImage == NULL) {
		this->showError(NULL,"LoadGDIPlusImage", "Couldn't allocate image object.");
		//dcxInfoError("Image",
		//	"LoadGDIPlusImage",
		//	this->m_pParentDialog->getName().to_chr(),
		//	this->getUserID(),
		//	"Couldn't allocate image object.");
		return false;
	}
	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	Status status = this->m_pImage->GetLastStatus();
	if (status != Ok) {
		this->showError(NULL,"LoadGDIPlusImage", GetLastStatusStr(status));
		//dcxInfoError("Image",
		//	"LoadGDIPlusImage",
		//	this->m_pParentDialog->getName().to_chr(),
		//	this->getUserID(),
		//	GetLastStatusStr(status));
		PreloadData();
		return false;
	}

	if (flags.find('h',0)) { // High Quality
		this->m_CQuality = CompositingQualityHighQuality;
		this->m_IMode = InterpolationModeHighQualityBicubic;
	}
	else {
		this->m_CQuality = CompositingQualityDefault;
		this->m_IMode = InterpolationModeDefault;
	}

	if (flags.find('b',0)) // Blend Image
		this->m_CMode = CompositingModeSourceOver;
	else
		this->m_CMode = CompositingModeSourceCopy;

	if (flags.find('a',0)) // Anti-Aliased
		this->m_SMode = SmoothingModeAntiAlias;
	else
		this->m_SMode = SmoothingModeDefault;

	if (flags.find('t',0)) // Tile
		this->m_bTileImage = true;
	else
		this->m_bTileImage = false;

	return true;
}
#endif

/*!
 * \brief blah
 *
 * blah
 */

void DcxImage::parseCommandRequest(TString & input) {
	XSwitchFlags flags;
	ZeroMemory((void*)&flags, sizeof(XSwitchFlags));
	this->parseSwitchFlags(input.gettok( 3 ), &flags);
	int numtok = input.numtok( );

	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [SIZE] [FILENAME]
	if (flags.switch_flags[22] && numtok > 6) {
		TString flag(input.gettok( 4 ));
		int index = input.gettok( 5 ).to_int();
		int size = input.gettok( 6 ).to_int();
		TString filename(input.gettok( 7, -1 ));

		filename.trim();
		PreloadData();

		if (size > 16)
			this->m_hIcon = dcxLoadIcon(index, filename, TRUE, flag);
		else
			this->m_hIcon = dcxLoadIcon(index, filename, FALSE, flag);

		if (this->m_hIcon != NULL)
			this->m_tsFilename = filename;

		this->m_iIconSize = size;
		this->m_bIsIcon = TRUE;

		// resize window to size of icon
		RECT wnd;

		GetWindowRect(this->m_Hwnd, &wnd);

		MapWindowPoints(NULL, GetParent(this->m_Hwnd), (LPPOINT) &wnd, 2);
		MoveWindow(this->m_Hwnd, wnd.left, wnd.top, size, size, TRUE);
		this->redrawWindow();
	}
	//xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [IMAGE]
	else if (flags.switch_flags[8] && numtok > 4) {
		TString flag(input.gettok(4));
		TString filename(input.gettok(5, -1));

		flag.trim();
		filename.trim();
		PreloadData();

		if (flag[0] != '+') {
			this->showError(NULL,"-i", "Invalid Flags");
			//DCXError("/xdid -i", "Invalid Flags");
			return;
		}

#ifdef DCX_USE_GDIPLUS
		// using this method allows you to render BMP, ICON, GIF, JPEG, Exif, PNG, TIFF, WMF, and EMF (no animation)
		//if (mIRCLink.m_bUseGDIPlus && flag.find('g',0)) {
		if (mIRCLink.m_bUseGDIPlus) {
			if (!LoadGDIPlusImage(flag,filename))
				this->showError(NULL,"-i", "Unable to load Image with GDI+");
				//DCXError("/xdid -i", "Unable to load Image with GDI+");
		}
		else
			this->m_hBitmap = dcxLoadBitmap(this->m_hBitmap, filename);
		if ((this->m_hBitmap != NULL) || (this->m_pImage != NULL))
			this->m_tsFilename = filename;
#else
		this->m_hBitmap = dcxLoadBitmap(this->m_hBitmap, filename);
		if (this->m_hBitmap != NULL)
			this->m_tsFilename = filename;
#endif
		this->m_bIsIcon = FALSE;
		//{	// Invalidate controls area in parent.
		//	RECT rc;
		//	HWND parent = GetParent(this->m_Hwnd);
		//	GetWindowRect(this->m_Hwnd, &rc);
		//	MapWindowPoints(NULL,parent, (LPPOINT) &rc, 2);
		//	InvalidateRect(parent, &rc, TRUE);
		//}
		InvalidateRect(this->m_Hwnd, NULL, TRUE);
	}
	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	else if (flags.switch_flags[10] && numtok > 3) {
		this->m_clrTransColor = (COLORREF)input.gettok( 4 ).to_num();
		this->redrawWindow();
	}
	// xdid -o [NAME] [ID] [SWITCH] [XOFFSET] [YOFFSET]
	else if (flags.switch_flags[14] && numtok > 4) {
		this->m_iXOffset = input.gettok( 4 ).to_int();
		this->m_iYOffset = input.gettok( 5 ).to_int();
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [1|0]
	else if (flags.switch_cap_flags[18] && numtok > 3) {
		if (input.gettok( 4 ).to_int() > 0)
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

#ifdef DCX_USE_GDIPLUS
void DcxImage::DrawGDIImage(HDC hdc, int x, int y, int w, int h)
{
	Graphics grphx( hdc );

	grphx.SetCompositingQuality(this->m_CQuality);
	grphx.SetCompositingMode(this->m_CMode);
	grphx.SetSmoothingMode(this->m_SMode);

	if (((this->m_pImage->GetWidth() == 1) || (this->m_pImage->GetHeight() == 1)) && this->m_bResizeImage) {
		// This fixes a GDI+ bug when resizing 1 px images
		// http://www.devnewsgroups.net/group/microsoft.public.dotnet.framework.windowsforms/topic11515.aspx
		grphx.SetInterpolationMode(InterpolationModeNearestNeighbor);
		grphx.SetPixelOffsetMode(PixelOffsetModeHalf);
	}
	else
		grphx.SetInterpolationMode(this->m_IMode);

	if (this->m_bTileImage) {
		ImageAttributes imAtt;
		imAtt.SetWrapMode(WrapModeTile);

		grphx.DrawImage(this->m_pImage,
			Rect(x + this->m_iXOffset, y + this->m_iYOffset, w, h),  // dest rect
			0, 0, w, h,       // source rect
			UnitPixel,
			&imAtt);
	}
	else if (this->m_bResizeImage)
		grphx.DrawImage( this->m_pImage, this->m_iXOffset, this->m_iYOffset, w, h );
	else
		grphx.DrawImage( this->m_pImage, this->m_iXOffset, this->m_iYOffset);
}
#endif

void DcxImage::DrawBMPImage(HDC hdc, int x, int y, int w, int h)
{
	HDC hdcbmp = CreateCompatibleDC(hdc);

	if (hdcbmp == NULL)
		return;

	BITMAP bmp;

	GetObject( this->m_hBitmap, sizeof(BITMAP), &bmp );
	HBITMAP oldBitmap = (HBITMAP)SelectObject( hdcbmp, this->m_hBitmap );

	if (this->m_clrTransColor != -1)
		TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_clrTransColor);
	else
		StretchBlt( hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

	SelectObject(hdcbmp, oldBitmap);
	DeleteDC( hdcbmp );
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
				bParsed = TRUE;
				return TRUE;
      }
      break;

    case WM_ERASEBKGND:
			{
				if (this->m_bAlphaBlend || this->isExStyle(WS_EX_TRANSPARENT))
					this->DrawParentsBackground((HDC)wParam);
				bParsed = TRUE;
				return TRUE;
      }
      break;

		case WM_PAINT:
		{
			// default paint method
#ifdef DCX_USE_GDIPLUS
			if ((this->m_hBitmap == NULL) && (this->m_hIcon == NULL) && (this->m_pImage == NULL))
				break;
#else
			if ((this->m_hBitmap == NULL) && (this->m_hIcon == NULL))
				break;
#endif
			bParsed = TRUE;
			PAINTSTRUCT ps; 
			HDC hdc;
			RECT rect;

			hdc = BeginPaint(this->m_Hwnd, &ps);
			GetClientRect(this->m_Hwnd, &rect);

			int w = (rect.right - rect.left), h = (rect.bottom - rect.top), x = rect.left, y = rect.top;

			// Setup alpha blend if any.
			LPALPHAINFO ai = this->SetupAlphaBlend(&hdc, this->m_bBuffer);

			DcxControl::DrawCtrlBackground(hdc,this,&rect);

			// draw bitmap
#ifdef DCX_USE_GDIPLUS
			if ((this->m_hBitmap != NULL) && (!this->m_bIsIcon) && (this->m_pImage == NULL)) {
#else
			if ((this->m_hBitmap != NULL) && (!this->m_bIsIcon)) {
#endif
				this->DrawBMPImage(hdc, x, y, w, h);
			}
			// draw icon
			else if ((this->m_hIcon != NULL) && (this->m_bIsIcon)) {
				DrawIconEx(hdc, 0, 0, this->m_hIcon, this->m_iIconSize, this->m_iIconSize, 0, this->m_hBackBrush, DI_NORMAL | DI_COMPAT); 
			}
#ifdef DCX_USE_GDIPLUS
			else if ((this->m_pImage != NULL) && (mIRCLink.m_bUseGDIPlus)) {
				this->DrawGDIImage(hdc, x, y, w, h);
			}
#endif
			this->FinishAlphaBlend(ai);

			EndPaint(this->m_Hwnd, &ps);

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
					if (GetCursor() != this->m_hCursor)
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