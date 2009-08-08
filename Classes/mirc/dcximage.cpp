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
#include "defines.h"
#include "Classes/mirc/dcximage.h"
#include "Classes/dcxdialog.h"
#include "Dcx.h"



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
, m_clrTransColor(CLR_INVALID)
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
		WS_CHILD | Styles,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top,
		mParentHwnd,
		(HMENU) ID,
		GetModuleHandle(NULL),
		NULL);

	if (!IsWindow(this->m_Hwnd))
		throw "Unable To Create Window";

	if ( bNoTheme )
		Dcx::XPPlusModule.dcxSetWindowTheme( this->m_Hwnd , L" ", L" " );

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
	//unsigned int i = 1, numtok = styles.numtok( );
	*Styles |= SS_NOTIFY;

	//while ( i <= numtok ) {

	//	i++;
	//}

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
		this->showErrorEx(NULL,"LoadGDIPlusImage", "Unable to Access File: %s", filename.to_chr());
		return false;
	}
	this->m_pImage = new Image(filename.to_wchr(),TRUE);

	// couldnt allocate image object.
	if (this->m_pImage == NULL) {
		this->showError(NULL,"LoadGDIPlusImage", "Couldn't allocate image object.");
		return false;
	}
	// for some reason this returns `OutOfMemory` when the file doesnt exist instead of `FileNotFound`
	Status status = this->m_pImage->GetLastStatus();
	if (status != Ok) {
		this->showError(NULL,"LoadGDIPlusImage", GetLastStatusStr(status));
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
	XSwitchFlags flags(input.gettok(3));
	int numtok = input.numtok( );

	// xdid -w [NAME] [ID] [SWITCH] [+FLAGS] [INDEX] [SIZE] [FILENAME]
	if (flags['w'] && numtok > 6) {
		TString flag(input.gettok( 4 ));
		int index = input.gettok( 5 ).to_int();
		int size = input.gettok( 6 ).to_int();
		TString filename(input.gettok(7, -1).trim());

		PreloadData();

		if (size > 16)
			this->m_hIcon = dcxLoadIcon(index, filename, true, flag);
		else
			this->m_hIcon = dcxLoadIcon(index, filename, false, flag);

		if (this->m_hIcon != NULL)
			this->m_tsFilename = filename;

		this->m_iIconSize = size;
		this->m_bIsIcon = TRUE;

		// resize window to size of icon
		RECT wnd;

		GetWindowRect(this->m_Hwnd, &wnd);

		MapWindowRect(NULL, GetParent(this->m_Hwnd), &wnd);
		MoveWindow(this->m_Hwnd, wnd.left, wnd.top, size, size, TRUE);
		this->redrawWindow();
	}
	//xdid -i [NAME] [ID] [SWITCH] [+FLAGS] [IMAGE]
	else if (flags['i'] && numtok > 4) {
		TString flag(input.gettok(4).trim());
		TString filename(input.gettok(5, -1).trim());

		PreloadData();

		if (flag[0] != '+') {
			this->showError(NULL,"-i", "Invalid Flags");
			return;
		}

#ifdef DCX_USE_GDIPLUS
		// using this method allows you to render BMP, ICON, GIF, JPEG, Exif, PNG, TIFF, WMF, and EMF (no animation)
		//if (Dcx::GDIModule.isUseable() && flag.find('g',0)) { // makes GDI+ the default method, bitmap is only used when GDI+ isn't supported.
		if (Dcx::GDIModule.isUseable()) {
			if (!LoadGDIPlusImage(flag,filename))
				this->showError(NULL,"-i", "Unable to load Image with GDI+");
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
		//InvalidateParentRect(this->m_Hwnd);
		InvalidateRect(this->m_Hwnd, NULL, TRUE);
	}
	// xdid -k [NAME] [ID] [SWITCH] [COLOR]
	else if (flags['k'] && numtok > 3) {
		this->m_clrTransColor = (COLORREF)input.gettok( 4 ).to_num();
		this->redrawWindow();
	}
	// xdid -o [NAME] [ID] [SWITCH] [XOFFSET] [YOFFSET]
	else if (flags['o'] && numtok > 4) {
		this->m_iXOffset = input.gettok( 4 ).to_int();
		this->m_iYOffset = input.gettok( 5 ).to_int();
		this->redrawWindow();
	}
	// xdid -S [NAME] [ID] [SWITCH] [1|0]
	else if (flags['S'] && numtok > 3) {
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
	HBITMAP oldBitmap = SelectBitmap( hdcbmp, this->m_hBitmap );

	if (this->m_clrTransColor != -1)
		TransparentBlt(hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, this->m_clrTransColor);
	else
		StretchBlt( hdc, x, y, w, h, hdcbmp, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

	SelectBitmap(hdcbmp, oldBitmap);
	DeleteDC( hdcbmp );
}

void DcxImage::toXml(TiXmlElement * xml) {
	__super::toXml(xml);
	if (this->m_tsFilename.len() > 0) xml->SetAttribute("src", m_tsFilename.to_chr());
}

/*!
 * \brief blah
 *
 * blah
 */
LRESULT DcxImage::ParentMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {
	return 0L;
}

LRESULT DcxImage::PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bParsed ) {

	switch( uMsg ) {

	case WM_ERASEBKGND:
		{
			//<#IF USER == 'hkr' COMMENT OUT, UNLESS ITS THE TIME WHEN HE WANTS THIS>
			if (this->isExStyle(WS_EX_TRANSPARENT))
				this->DrawParentsBackground((HDC)wParam);
			//if (this->isExStyle(WS_EX_TRANSPARENT))
			//	this->DrawParentsBackground((HDC)wParam);
			//else
			//	DcxControl::DrawCtrlBackground((HDC) wParam,this);
			//<END>
			bParsed = TRUE;
			return TRUE;
		}
		break;

	case WM_PRINTCLIENT:
		{
			this->DrawClientArea((HDC)wParam);
			bParsed = TRUE;
		}
		break;

	case WM_PAINT:
		{
			bParsed = TRUE;
			PAINTSTRUCT ps; 
			HDC hdc;

			hdc = BeginPaint(this->m_Hwnd, &ps);

			this->DrawClientArea(hdc);

			EndPaint(this->m_Hwnd, &ps);
		}
		break;

	case WM_SIZE:
		{
			InvalidateRect( this->m_Hwnd, NULL, TRUE );
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

void DcxImage::DrawClientArea(HDC hdc)
{
	RECT rect;
	// default paint method
	GetClientRect(this->m_Hwnd, &rect);

	int w = (rect.right - rect.left), h = (rect.bottom - rect.top), x = rect.left, y = rect.top;

	// Setup alpha blend if any.
	// Double Buffer required for GDI+ to look right in WS_EX_COMPOSITED
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
	else if ((this->m_hIcon != NULL) && (this->m_bIsIcon))
		DrawIconEx(hdc, 0, 0, this->m_hIcon, this->m_iIconSize, this->m_iIconSize, 0, this->m_hBackBrush, DI_NORMAL | DI_COMPAT);
#ifdef DCX_USE_GDIPLUS
	else if ((this->m_pImage != NULL) && (Dcx::GDIModule.isUseable()))
		this->DrawGDIImage(hdc, x, y, w, h);
#endif
	this->FinishAlphaBlend(ai);
}
